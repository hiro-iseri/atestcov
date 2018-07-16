#include <iostream>
#include "testcase.h"

using std::cout;
using std::endl;

class LogManager
{
private:
    FactorLevelSet fl_;
    bool view_info_ = false;

public:
    LogManager(const FactorLevelSet &fl, const bool view_info) : fl_(fl), view_info_(view_info)
    {}

    LogManager()
    {
        view_info_ = false;
    }

    void printHeader(TcInt nwise) const
    {
        if (view_info_) {
            cout << "[info]measurering:";
            cout << nwise << "wise coverage" << endl;
        }
    }

    void printCombination(const string &header, const vector<TcInt> &comp_index, const vector<TcInt> &comp_val) const
    {
        if (view_info_) {
            cout << header;
            fl_.printTextByNum(comp_index, comp_val);
        }
    }
};