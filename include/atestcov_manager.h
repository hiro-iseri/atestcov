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

    bool check_config(const ATestCovConfig &config) const
    {
        if (config.nwise_min_ != 0) {
            if (config.nwise_min_ < 0 || config.nwise_min_ > ATestCovRange::MAX_NWISE) {
                cerr << "error:nwisemin is out of range(0-100)" << endl;
                return false;
            }
        }
        if (config.nwise_max_ != 0) {
            if (config.nwise_max_ < 0 || config.nwise_max_ > ATestCovRange::MAX_NWISE) {
                cerr << "error:nwisemax is out of range(0-100)" << endl;
                return false;
            }
        }
        if (config.nwise_min_ > config.nwise_max_) {
            cerr << "error:nwisemin > nwisemax" << endl;
            return false;
        }
        return true;
    }

    int run(const ATestCovConfig &input_config)
    {
        results_.clear();

        if (!check_config(input_config)) {
            exit(1);
        }
        auto config = input_config;

        TestCase tc;
        ATestCovFileManager::readTestCaseFile(config.filepath_testcase_, tc);
        FactorLevelSet fl;
        ATestCovFileManager::readFLFile(config.filepath_fl_list_, fl);

        if (config.nwise_min_ == 0) {
            cout << "auto set nwisemin to 1" << endl;
            config.nwise_min_ = 1;
        }
        
        TestCaseVal tcv;
        FactorLevelSetVal fls;
        fl.toNum(fls);
        tc.textToNum(fl, tcv);

        CombinatorialCoverageMeasurer mr;
        for (auto nwise = config.nwise_min_; nwise <= config.nwise_max_; nwise++) {
            results_.push_back(mr.measureCoverage(tcv, fls, nwise));
        }
        return 0;
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
