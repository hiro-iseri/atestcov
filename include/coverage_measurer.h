#pragma once

#include "atestcov_common.h"

class CombinatorialCoverage
{
public:
    int num_all_combi_ = 0;
    int num_hit_combi_ = 0;
    int nwise = 0;
};

class CombinatorialCoverageResult
{
public:
    int totalnum_ = 0;
    int hitnum_ = 0;
    int nwise_ = 0;
    int ntestcase_ = 0;
    int nfactor_ = 0;
};

/*
・処理の流れ
全因子組合せを作る 
因子組合せごとに処理：
　因子組合せの全水準組わせを作る
　水準組み合わせ毎に処理：
　　水準組合せを包含するテストケースを探す
*/
class CombinatorialCoverageMeasurer
{
protected:
    vector<vector<int>> testcase_set_;
public:
    CombinatorialCoverageResult result_;

protected:

public:
    //因子ごとの水準数をnum_listに格納
    void createNumList(const vector<vector<int>> &in, vector<int> &num_list)
    {
        num_list.assign(num_list.size(), 0);
        for (auto testcase : in) {
            for (auto i = 0; i < testcase.size(); i++) {
                num_list[i] = (num_list[i] < testcase[i]) ? testcase[i] : num_list[i];
            }
        }
    }

    void printCoverageReport()
    {
        cout << endl;
        cout << "***** report for combinatorial coverage *****" << endl;
        cout << "test info *****";
        cout << "number of testcase: " << testcase_set_.size() << endl;
    }

    // 組合せを生成しoutput格納
    void createCombination(vector<vector<int>> &output, const int n, const int r)
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
    bool coverLevelCombination(const vector<int> &comp_index, const vector<int> &comp_val)
    {
        for (auto testcase : testcase_set_) {
            Debug::p("test case", testcase);
            auto included = true;
            for (auto j = 0; j < comp_index.size(); j++) {
                if (testcase[comp_index[j]] != comp_val[j]) {
                    included = false;
                    break;
                }
            }
            if (included) {
                return true;
            }
        }
        return false;
    }

    // nスイッチカバレッジの組合せを生成し、テストケース組合せに包含されるか評価
    void create_fv_combination(const vector<int> &numlist, const vector<int> &comp_set, vector<int> &index_list, int index)
    {
        for (index_list[index] = 0; index_list[index] <= numlist[comp_set[index]]; index_list[index]++) {
            if (index + 1 >= comp_set.size()) {
                result_.totalnum_++;
                if (coverLevelCombination(comp_set, index_list)) {
                    result_.hitnum_++;
                }
            } else {
                create_fv_combination(numlist, comp_set, index_list, index + 1);
            }
        }
    }

    CombinatorialCoverageResult measureCoverage(const vector<vector<int>> &testcase_set, const vector<int> &numlevels, const int nwise)
    {
        assert(testcase_set.size() > 0);
        result_.nwise_ = nwise;
        result_.ntestcase_ = testcase_set.size();
        result_.nfactor_ = testcase_set[0].size();

        testcase_set_ = testcase_set;
        auto num_testcase = testcase_set_[0].size();

        vector<int> index_list(num_testcase, 0);

        vector<vector<int>> comp_set;
        createCombination(comp_set, num_testcase, nwise);
        for (vector<int> comp : comp_set) {
            create_fv_combination(numlevels, comp, index_list, 0);
        }
        return result_;
    }
};