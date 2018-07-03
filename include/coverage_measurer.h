#pragma once

#include "atestcov_common.h"

class CombinatorialCoverage
{
public:
    TcInt allnum_ = 0;
    TcInt hitnum_ = 0;
    TcInt nwise_ = 0;
};

class CombinatorialCoverageResult
{
public:
    CombinatorialCoverage cov;
    TcInt ntestcase_ = 0;
    TcInt nfactor_ = 0;

    void clear()
    {
        cov.allnum_ = 0;
        cov.hitnum_ = 0;
        cov.nwise_ = 0;
        ntestcase_ = 0;
        nfactor_ = 0;
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

public:
    // for debug
    void print() const
    {
        for (auto mutex : mutexs_) {
            mutex.print();
        }
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
                            const vector<TcInt> &comp_index, const vector<TcInt> &comp_val) const
    {
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
                return true;
            }
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
                result_.cov.allnum_++;
                if (coverLevelCombination(testcase_set_, comp_set, index_list)) {
                    result_.cov.hitnum_++;
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
        result_.cov.nwise_ = nwise;
        result_.ntestcase_ = testcase_set.size();
        result_.nfactor_ = numlevels.size();

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
        return result_;
    }
};