#pragma once

#include "atestcov_common.h"

class CombinatorialCoverage
{
public:
    int allnum_ = 0;
    int hitnum_ = 0;
    int nwise_ = 0;
};

class CombinatorialCoverageResult
{
public:
    CombinatorialCoverage cov;
    int ntestcase_ = 0;
    int nfactor_ = 0;
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

public:
    //因子ごとの水準数をnum_listに格納
    void createNumList(const TestCaseSetVal &in, vector<int> &num_list) const
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
    void createCombination(TestCaseSetVal &output, const int n, const int r) const
    {
        vector<bool> valid(n);
        fill(valid.end() - r, valid.end(), true);
        do {
            vector<int> combination;
            for (auto i = 0; i < n; i++) {
                if (valid[i]) {
                    combination.push_back(i);
                }
            }
            output.push_back(combination);
        } while (next_permutation(valid.begin(), valid.end()));
    }

    //テストケース組合せが、指定の因子水準組合せを網羅していることを確認
    bool coverLevelCombination(const vector<int> &comp_index, const vector<int> &comp_val) const
    {
        for (auto testcase : testcase_set_) {
            for (auto j = 0; j < comp_index.size(); j++) {
                if (testcase[comp_index[j]] != comp_val[j]) {
                    continue;
                }
            }
            return true;
        }
        return false;
    }

    // nスイッチカバレッジの組合せを生成し、テストケース組合せに包含されるか評価
    void create_fv_combination(const vector<int> &numlist, const vector<int> &comp_set, vector<int> &index_list, int index)
    {
        for (index_list[index] = 0; index_list[index] <= numlist[comp_set[index]]; index_list[index]++) {
            if (index + 1 >= comp_set.size()) {
                result_.cov.allnum_++;
                if (coverLevelCombination(comp_set, index_list)) {
                    result_.cov.hitnum_++;
                }
            } else {
                create_fv_combination(numlist, comp_set, index_list, index + 1);
            }
        }
    }

    CombinatorialCoverageResult measureCoverage(const TestCaseSetVal &testcase_set, const vector<int> &numlevels, const int nwise)
    {
        assert(testcase_set.size() > 0);
        result_.cov.nwise_ = nwise;
        result_.ntestcase_ = testcase_set.size();
        result_.nfactor_ = testcase_set[0].size();

        testcase_set_ = testcase_set;
        auto num_testcase = testcase_set_[0].size();

        vector<int> index_list(num_testcase, 0);

        TestCaseSetVal comp_set;
        createCombination(comp_set, num_testcase, nwise);
        for (auto comp : comp_set) {
            create_fv_combination(numlevels, comp, index_list, 0);
        }
        return result_;
    }
};