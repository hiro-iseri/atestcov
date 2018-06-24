#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <regex>

#include "atestcov_config.h"

using namespace std;

//ファイル読み出し処理
class ATestCovFileManager
{
public:
    static void readTestCaseFile(const string &file_path, TestCase &output)
    {
        ifstream ifs(file_path);
        if (ifs.fail()) {
            cerr << "error:file open error:" << file_path << endl;
            throw ATestCovException("file open error");
        }
        string str;
        const regex sep_testcondition{"\\s*[,\\t]+\\s*"};
        vector<string> v = {};
        auto label_readed = false;
        
        while (getline(ifs, str)) {
            v.clear();
            if (str.length() == 0) {
                continue;
            }
            if (str[0] == '#') {
                //先頭#はコメント行
                continue;
            }
            auto ite = sregex_token_iterator(str.begin(), str.end(), sep_testcondition, -1);
            auto end = sregex_token_iterator();
            while (ite != end) {
                v.push_back(*ite++);
            }
            if (v.empty()) {
                cerr << "error:invalid format:" << file_path << endl;
                throw ATestCovException("invalid format");
            }
            if (!label_readed) {
                //1行目ラベル
                output.addItemText(v);
                label_readed = true;
            } else {
                //2行目以降がテストケース
                output.addTestcaseText(v);
                if (output.itemSize() >  ATestCovRange::MAX_TESTCASE) {
                    cerr << "error:number of testcase is too large(MAX:" << ATestCovRange::MAX_TESTCASE << ")" << endl;
                    throw ATestCovException("invalid format");
                }
            }
        }
    }

    static void readParamVal(const string &input, FactorLevelSet &output)
    {
        const regex sep_fl{":"};
        auto ite = sregex_token_iterator(input.begin(), input.end(), sep_fl, -1);
        const auto end = sregex_token_iterator();
        vector<string> v = {};
        while (ite != end) {
            v.push_back(*ite++);
        }
        if (v.empty() || v.size() != 2) {
            throw ATestCovException("invalid format");
        }

        const regex sep_level{"\\s*[,\\t]+\\s*"};
        const auto value_set = v[1];
        vector<string> vlevel = {};
        ite = sregex_token_iterator(value_set.begin(), value_set.end(), sep_level, -1);
        while (ite != end) {
            vlevel.push_back(*ite++);
        }
        output.add(v[0], vlevel);        
    }

    static void readMutex(const string &input, FactorLevelSet &output)
    {
        Mutex mutex;
        string param = regex_replace(input, regex("\\s*@mutex\\s*"), "");

        const regex sep_mutex{"\\s*[&]+\\s*"};
        auto ite = sregex_token_iterator(param.begin(), param.end(), sep_mutex, -1);
        const auto end = sregex_token_iterator();
        vector<string> v = {};
        while (ite != end) {
            v.push_back(*ite++);
        }
        if (v.empty() || v.size() < 2) {
            throw ATestCovException("invalid format");
        }

        vector<Factor> factor_set;
        for (auto fl : v) {
            const regex sep_fl{":"};
            auto ite = sregex_token_iterator(fl.begin(), fl.end(), sep_fl, -1);
            const auto end = sregex_token_iterator();
            vector<string> vfl = {};
            while (ite != end) {
                vfl.push_back(*ite++);
            }
            if (vfl.empty() || vfl.size() != 2) {
                throw ATestCovException("invalid format");
            }
            factor_set.push_back(Factor(vfl[0], vfl[1]));
        }
        output.addMutex(Mutex(factor_set));
    }

    static void readFLFile(const string &file_path, FactorLevelSet &output)
    {
        
        ifstream ifs(file_path);
        if (ifs.fail()) {
            cerr << "error:file open error:" << file_path << endl;
            throw ATestCovException("file open error");
        }
        string str;
        regex sep_fl{":"};
        regex sep_level{"\\s*[,\\t]+\\s*"};
        vector<string> v = {};
        try {
            while (getline(ifs, str)) {
                v.clear();
                if (str.length() == 0) {
                    continue;
                }
                if (str[0] == '#') {
                    //先頭#はコメント行
                    continue;
                }

                if (str.find("@mutex") != string::npos) {
                    readMutex(str, output);

                } else if (str.find(':') != string::npos) {
                    readParamVal(str, output);
                }

            }
        } catch(...) {
            cerr << "error:file read error:" << file_path << endl;
            throw ATestCovException("invalid format");
        }
    }
};

