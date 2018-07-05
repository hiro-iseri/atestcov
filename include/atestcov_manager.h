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
        if (config.nwise_min_ > ATestCovRange::MAX_NWISE) {
            cerr << "error:nwisemin is out of range(0-100)" << endl;
            return false;
        }
        if (config.nwise_max_ > ATestCovRange::MAX_NWISE) {
            cerr << "error:nwisemax is out of range(0-100)" << endl;
            return false;
        }
        if (config.nwise_min_ != 0 && config.nwise_max_!= 0 && config.nwise_min_ > config.nwise_max_) {
            cerr << "error:nwisemin > nwisemax" << endl;
            return false;
        }
        return true;
    }

    int run(const ATestCovConfig &input_config)
    {
        if (!check_config(input_config)) {
            exit(1);
        }
        auto config = input_config;
        results_.clear();

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
            cout << "[info]set nwisemin to 1" << endl;
            config.nwise_min_ = 1;
        }
        const auto nwise_max = fl.size();
        if (config.nwise_max_ == 0 || config.nwise_max_ > nwise_max) {
            cout << "[info]set nwisemax to " << nwise_max << endl;
            config.nwise_max_ = nwise_max;
        }
        
        TestCaseSetVal tcv;
        FactorLevelSetVal fls;
        MutexSetVal mutex;
        fl.toNum(fls);
        fl.toMutexNum(mutex);
        tc.textToNum(fl, tcv);
        LogManager fm(fl, config.infolog_enable_);
        CombinatorialCoverageMeasurer mr;
        mr.set_calc_admetrics(config.add_metrics_);
        if (mutex.size() > 0) {
            mr.setMutex(mutex);
        }
        for (auto nwise = config.nwise_min_; nwise <= config.nwise_max_; nwise++) {
            results_.push_back(mr.measureCoverage(tcv, fls, nwise, fm));
        }

        print(config.add_metrics_);
        return 0;
    }
    
    void print(bool print_admetrics = false)
    {
        cout << endl << "[coverage report]" << endl;
        if (results_.empty()) {
            cerr << "empty result" << endl;
            return;
        }
        cout << "number of test case: " << results_[0].ntestcase_ << endl;
        cout << "number of parameter: " << results_[0].nfactor_ << endl;
        for (auto result : results_) {
            if (result.cov.allnum_ == 0) {
                cerr << "error:invalid result" << endl;
                return;
            }

            cout << result.cov.nwise_ << "wise coverage: ";
            cout << fixed << std::setprecision(2);
            cout << 100.0 * result.cov.hitnum_ / result.cov.allnum_ << "%";
            cout << "(" << result.cov.hitnum_ << "/" << result.cov.allnum_ << ")" << endl;

            if (print_admetrics) {
                if (result.combi_metrics_.n_combi_type_ == 0) {
                    cerr << "error:invalid result" << endl;
                    return;
                }
                cout << result.cov.nwise_ << "wise ";
                cout << fixed << std::setprecision(2);
                cout << "rate of combination redundancy: " << (double)result.combi_metrics_.n_combi_ / result.combi_metrics_.n_combi_type_;
                cout << "(" << result.combi_metrics_.n_combi_ << "/" << result.combi_metrics_.n_combi_type_ << ")" << endl; 
            }
        }
    }
};
