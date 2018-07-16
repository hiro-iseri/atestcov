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

TEST(atestcov, ATestCovManager_metrics)
{
    ATestCovConfig config;
    config.filepath_testcase_ = "testdata/MetricsTC.txt";
    config.filepath_fl_list_ = "testdata/MetricsFL.txt";
    config.add_metrics_ = true;
    config.infolog_enable_ = true;

    ATestCovManager atcm;
    atcm.run(config);

    ASSERT_EQ(5, atcm.results_.size());



    for (auto result : atcm.results_) {
        EXPECT_EQ(5, result.nfactor_);
        EXPECT_EQ(3, result.ntestcase_);
    }
    EXPECT_EQ(12, atcm.results_[0].cov_.allnum_);
    EXPECT_EQ(10, atcm.results_[0].cov_.hitnum_);
    EXPECT_EQ(1, atcm.results_[0].cov_.nwise_);
    EXPECT_EQ(10, atcm.results_[0].combi_metrics_.n_combi_type_);
    EXPECT_EQ(15, atcm.results_[0].combi_metrics_.n_combi_);
    EXPECT_EQ(15.0/10.0, atcm.results_[0].redundancy_);
    EXPECT_EQ(0.82915619758885, atcm.results_[0].standard_deviation_);
    
    EXPECT_EQ(55, atcm.results_[1].cov_.allnum_);
    EXPECT_EQ(26, atcm.results_[1].cov_.hitnum_);
    EXPECT_EQ(2, atcm.results_[1].cov_.nwise_);
    EXPECT_EQ(26, atcm.results_[1].combi_metrics_.n_combi_type_);
    EXPECT_EQ(30, atcm.results_[1].combi_metrics_.n_combi_);
    EXPECT_EQ(30.0/26.0, atcm.results_[1].redundancy_);

    EXPECT_EQ(120, atcm.results_[2].cov_.allnum_);
    EXPECT_EQ(26, atcm.results_[2].cov_.hitnum_);
    //EXPECT_EQ(2, atcm.results_[2].cov_.nwise_);
    //EXPECT_EQ(26, atcm.results_[2].combi_metrics_.n_combi_type_);
    //EXPECT_EQ(30, atcm.results_[2].combi_metrics_.n_combi_);
    //EXPECT_EQ(30.0/26.0, atcm.results_[2].redundancy_);

    EXPECT_EQ(124, atcm.results_[3].cov_.allnum_);
    EXPECT_EQ(48, atcm.results_[4].cov_.allnum_);
}



GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
