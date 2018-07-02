#pragma once

#include <atestcov_common.h>
#include <string>

class ATestCovRange
{
public:
    const static TcInt MAX_NWISE = 100;
    const static TcInt MAX_TESTCASE = 10000;
    const static TcInt MAX_FACTOR_COMB = 10000;
};

//起動時に指定するコンフィグデータ
class ATestCovConfig
{
public:
    TcInt nwise_min_ = 0;
    TcInt nwise_max_ = 0;
    std::string filepath_testcase_;
    std::string filepath_fl_list_;
    bool infolog_enable_ = false;
};
