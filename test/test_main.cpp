#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>

#include "../include/atestcov_common.h"
#include "../include/testcase.h"
#include "../include/atestcov_config.h"
#include "../include/atestcov_file.h"
#include "../include/coverage_measurer.h"
#include "../include/atestcov_manager.h"

using namespace std;


TEST(atestcov, calculate_coverage_1wise)
{
    CombinatorialCoverageMeasurer mr;
    TestCaseSetVal testcase_set;
    testcase_set.push_back(vector<TcInt>{0, 0});
    testcase_set.push_back(vector<TcInt>{1, 2});
    testcase_set.push_back(vector<TcInt>{0, 1});
    vector<TcInt> numlevel(testcase_set[0].size());
    LogManager lm(FactorLevelSet(), false);
    mr.createNumList(testcase_set, numlevel);
    mr.measureCoverage(testcase_set, numlevel, 1, lm);

    const auto result = mr.getResult();
    EXPECT_EQ(5, result.cov_.allnum_);
    EXPECT_EQ(5, result.cov_.hitnum_);
}

TEST(atestcov, calculate_coverage_2wise)
{
    CombinatorialCoverageMeasurer mr;
    TestCaseSetVal testcase_set;
    testcase_set.push_back(vector<TcInt>{0, 0, 0});
    testcase_set.push_back(vector<TcInt>{0, 1, 1});
    testcase_set.push_back(vector<TcInt>{1, 0, 0});

    vector<TcInt> numlevel(testcase_set[0].size());
    mr.createNumList(testcase_set, numlevel);
    mr.measureCoverage(testcase_set, numlevel, 2, LogManager());

    const auto result = mr.getResult();
    EXPECT_EQ(12, result.cov_.allnum_);
}



TEST(atestcov, calculate_coverage_2wise_uncover)
{
    CombinatorialCoverageMeasurer mr;
    TestCaseSetVal testcase_set;
    testcase_set.push_back(vector<TcInt>{0, 0});
    testcase_set.push_back(vector<TcInt>{1, 1});
    testcase_set.push_back(vector<TcInt>{0, 2});

    vector<TcInt> numlevel(testcase_set[0].size());
    mr.createNumList(testcase_set, numlevel);
    mr.measureCoverage(testcase_set, numlevel, 2, LogManager());

    const auto result = mr.getResult();
    
    EXPECT_EQ(6, result.cov_.allnum_);
    EXPECT_EQ(3, result.cov_.hitnum_);
}


TEST(atestcov, calculate_coverage_3wise_uncover)
{
    CombinatorialCoverageMeasurer mr;
    TestCaseSetVal testcase_set;
    testcase_set.push_back(vector<TcInt>{0, 0, 0, 0});
    testcase_set.push_back(vector<TcInt>{1, 1, 1, 1});
    testcase_set.push_back(vector<TcInt>{0, 0, 0, 0});

    vector<TcInt> numlevel(testcase_set[0].size());
    mr.createNumList(testcase_set, numlevel);
    mr.measureCoverage(testcase_set, numlevel, 3, LogManager());

    const auto result = mr.getResult();
    
    EXPECT_EQ(32, result.cov_.allnum_);
    EXPECT_EQ(8, result.cov_.hitnum_);
}

TEST(atestcov, createCombination)
{
    CombinatorialCoverageMeasurer mr;

    TestCaseSetVal comp;
    mr.createCombination(comp, 5, 3);
    EXPECT_EQ(10, comp.size());
}

TEST(atestcov, testcase_TextToNum)
{
    TestCase tc;
    ATestCovFileManager::readTestCaseFile("testdata/SimpleTestCase.txt", tc);
    FactorLevelSet fl;
    ATestCovFileManager::readFLFile("testdata/SimpleFL.txt", fl);

    TestCaseSetVal tcv;
    FactorLevelSetVal fls;

    fl.toNum(fls);
    tc.textToNum(fl, tcv);

    EXPECT_EQ(2, fls.size());
    EXPECT_EQ(1, fls[0]);
    EXPECT_EQ(2, fls[1]);


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

TEST(atestcov, readFLFile_withMutex)
{
    FactorLevelSet fl;
    ATestCovFileManager::readFLFile("testdata/FLwith2Mutex.txt", fl);
    EXPECT_EQ(2, fl.size());
    EXPECT_EQ(2, fl.mutexSize());
    fl.print();
}

TEST(atestcov, ATestCovManager_run)
{
    ATestCovConfig config;
    config.filepath_testcase_ = "testdata/SimpleTestCase.txt";
    config.filepath_fl_list_ = "testdata/SimpleFL.txt";
    config.nwise_min_ = 1;
    config.nwise_max_ = 2;

    ATestCovManager atcm;
    atcm.run(config);

    ASSERT_EQ(2, atcm.results_.size());

    ASSERT_EQ(3, atcm.results_[0].ntestcase_);
    ASSERT_EQ(1, atcm.results_[0].cov_.nwise_);
    ASSERT_EQ(5, atcm.results_[0].cov_.allnum_);
    ASSERT_EQ(5, atcm.results_[0].cov_.hitnum_);
}



GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
