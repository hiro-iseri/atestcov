#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>
#include "atestcov_common.h"
#include "atestcov_file.h"
#include "atestcov_config.h"
#include "coverage_measurer.h"

using std::cout;
using std::endl;
using std::setprecision;

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
