#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>
#include "atestcov_common.h"
#include "atestcov_file.h"
#include "atestcov_config.h"
#include "coverage_measurer.h"
#include "testcase.h"


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
            cerr << "error:lower is out of range(0-" << ATestCovRange::MAX_NWISE << ")" << endl;
            return false;
        }
        if (config.nwise_max_ > ATestCovRange::MAX_NWISE) {
            cerr << "error:upper is out of range(0-" << ATestCovRange::MAX_NWISE << ")" << endl;
            return false;
        }
        if (config.nwise_min_ != 0 && config.nwise_max_!= 0 && config.nwise_min_ > config.nwise_max_) {
            cerr << "error:lower > upper" << endl;
            return false;
        }
        return true;
    }

    //ファイル読み出しと読み出し結果に基づくコンフィグ補正
    void readFile(FactorLevelSet &fl, TestCaseSetVal &tcv, FactorLevelSetVal &fls, MutexSetVal &mutex, ATestCovConfig &config)
    {
        TestCase tc;
        try {
            ATestCovFileManager::readTestCaseFile(config.filepath_testcase_, tc);
            ATestCovFileManager::readFLFile(config.filepath_fl_list_, fl);
        } catch (...) {
            cerr << "file read error" << endl;
            exit(1);
        }

        if (!fl.check() || !tc.check(fl)) {
            exit(1);
        }
        if (config.nwise_min_ == 0) {
            cout << "[info]set lower to 1" << endl;
            config.nwise_min_ = 1;
        }
        const auto nwise_max = fl.size();
        if (config.nwise_max_ == 0 || config.nwise_max_ > nwise_max) {
            cout << "[info]set upper to " << nwise_max << endl;
            config.nwise_max_ = nwise_max;
        }

        fl.toNum(fls);
        fl.toMutexNum(mutex);
        tc.textToNum(fl, tcv);
    }

    int run(const ATestCovConfig &input_config)
    {
        if (!check_config(input_config)) {
            exit(1);
        }
        auto config = input_config;
        results_.clear();

        TestCaseSetVal tcv;
        FactorLevelSetVal fls;
        MutexSetVal mutex;
        FactorLevelSet fl;
        readFile(fl, tcv, fls, mutex, config);
        
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
            if (result.cov_.allnum_ == 0) {
                cerr << "error:invalid result" << endl;
                return;
            }

            cout << result.cov_.nwise_ << "wise coverage: ";
            cout << std::fixed << std::setprecision(2);
            cout << 100.0 * result.cov_.hitnum_ / result.cov_.allnum_ << "%";
            cout << "(" << result.cov_.hitnum_ << "/" << result.cov_.allnum_ << ")" << endl;

            if (print_admetrics) {
                cout << " " << result.cov_.nwise_ << "wise ";
                cout << std::fixed << std::setprecision(2);
                cout << "rate of combination redundancy: " << result.redundancy_;
                cout << "(" << result.combi_metrics_.n_combi_ << "/" << result.combi_metrics_.n_combi_type_ << ")" << endl; 
                cout << " " << result.cov_.nwise_ << "wise ";
                cout << "standard deviation of combination: " << result.standard_deviation_ << endl;
            }
        }
    }
};
