#pragma once

#include <string>
#include "atestcov_common.h"

//起動時に指定するコンフィグデータ
class ATestCovConfig
{
public:
    TcInt nwise_min_ = 0;
    TcInt nwise_max_ = 0;
    std::string filepath_testcase_;
    std::string filepath_fl_list_;
    bool infolog_enable_ = false;
    bool add_metrics_ = false;
};
