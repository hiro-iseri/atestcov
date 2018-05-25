#pragma once

#include <string>

class ATestCovRange
{
public:
    const static int MAX_NWISE = 100;
    const static int MAX_TESTCASE = 10000;
    const static int MAX_FACTOR_COMB = 10000;
};

//起動時に指定するコンフィグデータ
class ATestCovConfig
{
public:
    unsigned int nwise_min_ = 0;
    unsigned int nwise_max_ = 0;
    std::string filepath_testcase_;
    std::string filepath_fl_list_;
    bool infolog_enable_ = false;
};
