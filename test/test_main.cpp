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
