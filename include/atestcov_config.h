#pragma once

#include <string>

//起動時に指定するコンフィグデータ
class AtestCovConfig
{
public:
    int nwise_min_ = 0;
    int nwise_max_ = 0;
    std::string filepath_testcase_;
    std::string filepath_fl_list_;
};
