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
        FactorLevelSet fl;
        try {
            ATestCovFileManager::readTestCaseFile(config.filepath_testcase_, tc);
            ATestCovFileManager::readFLFile(config.filepath_fl_list_, fl);
        } catch (...) {
            cerr << "file read error" << endl;
            exit(1);
        }

        if (config.nwise_min_ == 0) {
            cout << "auto set nwisemin to 1" << endl;
            config.nwise_min_ = 1;
        }
        const auto nwise_max = filepath_fl_list_.size();
        if (config.nwise_max_ == 0 || config.nwise_max_ < nwise_max) {
            cout << "auto set nwisemax to nwise_max" << endl;
        }
        
        TestCaseSetVal tcv;
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
            cout << result.cov.nwise_ << "wise coverage: ";
            cout << fixed << std::setprecision(2);
            cout << 100.0 * result.cov.hitnum_ / result.cov.allnum_ << "%";
            cout << "(" << result.cov.hitnum_ << "/" << result.cov.allnum_ << ")" << endl;
        }
    }
};
