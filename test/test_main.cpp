#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>

#include "../include/atestcov_common.h"
#include "../include/atestcov_config.h"
#include "../include/atestcov_file.h"

using namespace std;

using TestCaseVal = vector<vector<int>>;
using FactorLevelSetVal = vector<int>;

void printTestCaseVal(const TestCaseVal &tcv)
{
    for (auto testcase : tcv) {
        for (auto value : testcase) {
            cout << value << "  ";
        }
        cout << endl;
    }
}

//デバッグ用print
void printVector(const string &name, const vector<int> &v)
{
    cout << name << ":";
    for (auto c : v) {
        cout << c << " ";
    }
    cout << endl;
}


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
            printVector("test case", testcase);
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
            printVector("factor com", comp);
            create_fv_combination(numlevels, comp, index_list, 0);
        }
        return result_;
    }
};


TEST(atestcov, calculate_coverage_1wise)
{
    CombinatorialCoverageMeasurer mr;
    vector<vector<int>> testcase_set;
    testcase_set.push_back(vector<int>{0, 0});
    testcase_set.push_back(vector<int>{1, 2});
    testcase_set.push_back(vector<int>{0, 1});
    vector<int> numlevel(testcase_set[0].size());
    mr.createNumList(testcase_set, numlevel);

    mr.measureCoverage(testcase_set, numlevel, 1);

    EXPECT_EQ(5, mr.result_.totalnum_);
    EXPECT_EQ(5, mr.result_.hitnum_);
}

TEST(atestcov, calculate_coverage_2wise)
{
    CombinatorialCoverageMeasurer mr;
    vector<vector<int>> testcase_set;
    testcase_set.push_back(vector<int>{0, 0, 0});
    testcase_set.push_back(vector<int>{0, 1, 1});
    testcase_set.push_back(vector<int>{1, 0, 0});

    vector<int> numlevel(testcase_set[0].size());
    mr.createNumList(testcase_set, numlevel);
    mr.measureCoverage(testcase_set, numlevel, 2);

    EXPECT_EQ(12, mr.result_.totalnum_);
}

TEST(atestcov, createCombination)
{
    CombinatorialCoverageMeasurer mr;

    vector< std::vector<int> > comp;
    mr.createCombination(comp, 5, 3);
    EXPECT_EQ(10, comp.size());
}

#include <fstream>
#include <iostream>
#include <string>
#include <regex>

TEST(atestcov, testcase_TextToNum)
{
    TestCase tc;
    ATestCovFileManager::readTestCaseFile("testdata/SimpleTestCase.txt", tc);
    FactorLevelSet fl;
    ATestCovFileManager::readFLFile("testdata/SimpleFL.txt", fl);

    TestCaseVal tcv;
    FactorLevelSetVal fls;

    fl.toNum(fls);
    tc.textToNum(fl, tcv);

    EXPECT_EQ(2, fls.size());
    EXPECT_EQ(1, fls[0]);
    EXPECT_EQ(2, fls[1]);

    printTestCaseVal(tcv);
    EXPECT_EQ(3, tcv.size());
    EXPECT_EQ(0, tcv[0][0]);
    EXPECT_EQ(1, tcv[1][0]);
    EXPECT_EQ(2, tcv[1][1]);
}

TEST(atestcov, readTestCaseFile_simple)
{
    TestCase tc;
    ATestCovFileManager::readTestCaseFile("testdata/SimpleTestCase.txt", tc);
    EXPECT_EQ(3, tc.testcaseSize());
}

TEST(atestcov, readFLFile_simple)
{
    FactorLevelSet fl;
    ATestCovFileManager::readFLFile("testdata/SimpleFL.txt", fl);
    EXPECT_EQ(2, fl.size());
}

class ATestCovManager
{
public:
    vector<CombinatorialCoverageResult> results_;
    //void run(string fl_file_path, string testcase_file_path, vector<int> nwise_list)
    void run(const AtestCovConfig &config)
    {
        results_.clear();
        TestCase tc;
        FactorLevelSet fl;
        ATestCovFileManager::readTestCaseFile(config.filepath_testcase_, tc);
        ATestCovFileManager::readFLFile(config.filepath_fl_list_, fl);
        
        TestCaseVal tcv;
        FactorLevelSetVal fls;

        fl.toNum(fls);
        tc.textToNum(fl, tcv);

        CombinatorialCoverageMeasurer mr;
        for (auto nwise = config.nwise_min_; nwise <= config.nwise_max_; nwise++) {
            results_.push_back(mr.measureCoverage(tcv, fls, nwise));
        }
    }
    
    void print()
    {
        if (results_.empty()) {
            cout << "empty result" << endl;
            return;
        }
        cout << "num of testcase: " << results_[0].ntestcase_ << endl;
        cout << "num of factor: " << results_[0].nfactor_ << endl;
        for (auto result : results_) {
            cout << result.nwise_ << "wise coverage: ";
            cout << fixed << std::setprecision(2);
            cout << 100.0 * result.hitnum_ / result.totalnum_ << "%";
            cout << "(" << result.hitnum_ << "/" << result.totalnum_ << ")" << endl;
        }
    }
};


TEST(atestcov, ATestCovManager_run)
{
    AtestCovConfig config;
    config.filepath_testcase_ = "testdata/SimpleTestCase.txt";
    config.filepath_fl_list_ = "testdata/SimpleFL.txt";
    config.nwise_min_ = 1;
    config.nwise_max_ = 2;

    ATestCovManager atcm;
    atcm.run(config);
    atcm.print();

    ASSERT_EQ(2, atcm.results_.size());

    ASSERT_EQ(3, atcm.results_[0].ntestcase_);
    ASSERT_EQ(1, atcm.results_[0].nwise_);
    ASSERT_EQ(5, atcm.results_[0].totalnum_);
    ASSERT_EQ(5, atcm.results_[0].hitnum_);
}

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
