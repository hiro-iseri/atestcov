#pragma once

#include <numeric>
#include <complex>
#include "atestcov_common.h"

class CombinatorialCoverage
{
public:
    TcInt allnum_ = 0;
    TcInt hitnum_ = 0;
    TcInt nwise_ = 0;

    void clear()
    {
        allnum_ = 0;
        hitnum_ = 0;
        nwise_ = 0;
    }
};

class CombinationMetrics
{
public:
    TcInt n_combi_type_ = 0;
    TcInt n_combi_ = 0;

    void clear()
    {
        n_combi_type_ = 0;
        n_combi_ = 0;
    }
};

class CombinatorialCoverageResult
{
public:
    CombinatorialCoverage cov_;
    CombinationMetrics combi_metrics_;
    TcInt ntestcase_ = 0;
    TcInt nfactor_ = 0;
    double standard_deviation_ = 0.0;
    double redundancy_ = 0.0;

    void clear()
    {
        cov_.clear();
        combi_metrics_.clear();
        ntestcase_ = 0;
        nfactor_ = 0;
        standard_deviation_ = 0.0;
        redundancy_ = 0.0;
    }
};

class AdMetrics
{
protected:
    vector<int> combi_counter_;

public:
    void push_back(const TcInt counter)
    {
        combi_counter_.push_back(counter);
    }

    void clear()
    {
        combi_counter_.clear();
    }

    void getAdMetrics(CombinatorialCoverageResult &result) const
    {
        int ncombi_type = 0;
        int ncombi = 0;
        for (auto count : combi_counter_) {
            if (count > 0) {
                ncombi_type++;
                ncombi += count;
            }
        }
        result.combi_metrics_.n_combi_type_ = ncombi_type;
        result.combi_metrics_.n_combi_ = ncombi;
        result.redundancy_ = getRedundancy();
        result.standard_deviation_ = getStandardDeviation();
    }

    double getRedundancy() const
    {
        CombinatorialCoverageResult result;
        getAdMetrics(result);
        if (result.combi_metrics_.n_combi_type_ == 0) {
            return -1.0;
        }
        return static_cast<double>(result.combi_metrics_.n_combi_ / result.combi_metrics_.n_combi_type_);
    }

    double getStandardDeviation() const
    {
        if (combi_counter_.size() == 0) {
            return -1.0;
        }
        const auto average = std::accumulate(combi_counter_.begin(), combi_counter_.end(), 0.0) / combi_counter_.size();
        const auto deviation = std::accumulate(
            combi_counter_.begin(), combi_counter_.end(), 0.0,
        	[average](double sum, double input){
                const auto diff = input - average;
                return sum + diff * diff;
            }) / combi_counter_.size();
        return std::sqrt(deviation);
    }
};

/*
[memo]処理の流れ
全因子組合せを作る 
因子組合せごとに処理：
　因子組合せの全水準組わせを作る
　水準組み合わせ毎に処理：
　　水準組合せを包含するテストケースを探す
*/
class CombinatorialCoverageMeasurer
{
protected:
    TestCaseSetVal testcase_set_;
    CombinatorialCoverageResult result_;
    MutexSetVal mutexs_;
    LogManager lm_;
    AdMetrics admetrics_;
    bool calc_admetrics_ = false;

public:
    // for debug
    void print() const
    {
        for (auto mutex : mutexs_) {
            mutex.print();
        }
    }

    void set_calc_admetrics(bool calc)
    {
        calc_admetrics_ = calc;
    }

    //因子ごとの水準数をnum_listに格納
    void createNumList(const TestCaseSetVal &in, vector<TcInt> &num_list) const
    {
        num_list.assign(num_list.size(), 0);
        for (auto testcase : in) {
            for (auto i = 0; i < testcase.size(); i++) {
                num_list[i] = (num_list[i] < testcase[i]) ? testcase[i] : num_list[i];
            }
        }
    }

    CombinatorialCoverageResult getResult() const
    {
        return result_;
    }

    // 組合せを生成しoutput格納
    void createCombination(TestCaseSetVal &output, const TcInt n, const TcInt r) const
    {
        vector<bool> valid(n);
        fill(valid.end() - r, valid.end(), true);
        do {
            vector<TcInt> combination;
            for (auto i = 0; i < n; i++) {
                if (valid[i]) {
                    combination.push_back(i);
                }
            }
            output.push_back(combination);
        } while (next_permutation(valid.begin(), valid.end()));
    }

    //テストケース組合せが、指定の因子水準組合せを網羅していることを確認
    bool coverLevelCombination(const TestCaseSetVal &testcase_set, 
                            const vector<TcInt> &comp_index, const vector<TcInt> &comp_val,
                            TcInt &combi_count) const
    {
        combi_count = 0;
        for (auto testcase : testcase_set) {
            vector<bool> hit_list(comp_index.size(), false);
            for (auto j = 0; j < comp_index.size(); j++) {
                if (testcase[comp_index[j]] == comp_val[j]) {
                    hit_list[j] = true;
                }
            }
            if (std::find(hit_list.begin(), hit_list.end(), false) != hit_list.end()) {
                continue;
            } else {
                if (calc_admetrics_) {
                    combi_count++;
                } else {
                    return true;
                }
            }
        }
        if (calc_admetrics_) {
            return (combi_count > 0);
        }
        return false;
    }

    // nスイッチカバレッジの組合せを生成し、テストケース組合せに包含されるか評価
    void countCoverage(const vector<TcInt> &numlist, 
                    const vector<TcInt> &comp_set, vector<TcInt> &index_list, TcInt index)
    {
        for (index_list[index] = 0; index_list[index] <= numlist[comp_set[index]]; index_list[index]++) {
            auto skiped = false;

            if (index + 1 >= comp_set.size() && ! skiped) {
                for (auto mutex: mutexs_) {
                    if (mutex.enable(comp_set, index_list)) {
                        lm_.printCombination(" [info]mutex:", comp_set, index_list);
                        skiped = true;
                    }
                }
                if (skiped) {
                    continue;
                }
                result_.cov_.allnum_++;
                TcInt counter;
                if (coverLevelCombination(testcase_set_, comp_set, index_list, counter)) {
                    result_.cov_.hitnum_++;
                    if (calc_admetrics_) {
                        admetrics_.push_back(counter);
                    }
                } else {
                    lm_.printCombination(" [info]uncover:", comp_set, index_list);
                }
            } else {
                countCoverage(numlist, comp_set, index_list, index + 1);
            }
        }
    }
    
    void setMutex(const MutexSetVal &mutexs)
    {
        mutexs_ = mutexs;
    }

    CombinatorialCoverageResult measureCoverage(const TestCaseSetVal &testcase_set, 
                    const vector<TcInt> &numlevels, const TcInt nwise, const LogManager &lm)
    {
        assert(testcase_set.size() > 0);
        lm_ = lm;

        lm_.printHeader(nwise);
        result_.clear();
        result_.cov_.nwise_ = nwise;
        result_.ntestcase_ = testcase_set.size();
        result_.nfactor_ = numlevels.size();
        admetrics_.clear();

        if (testcase_set[0].size() != numlevels.size()) {
            cerr << "[warning] num of specified parameter != num of parameter in testcase" << endl;
        }

        testcase_set_ = testcase_set;

        vector<TcInt> index_list(result_.nfactor_, 0);

        TestCaseSetVal comp_set;
        createCombination(comp_set, result_.nfactor_, nwise);

        if (comp_set.size() > ATestCovRange::MAX_FACTOR_COMB) {
            cerr << "[error]the number of parameters combinations is too large(MAX:" << ATestCovRange::MAX_FACTOR_COMB << ")" << endl;
            exit(1);
        }
        for (auto comp : comp_set) {
            countCoverage(numlevels, comp, index_list, 0);
        }

        if (calc_admetrics_)
        {
            admetrics_.getAdMetrics(result_);
        }
        return result_;
    }
};