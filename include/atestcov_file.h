#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <regex>

#include "atestcov_config.h"
#include "testcase.h"

using std::regex_replace;
using std::regex;
using std::vector;
using std::cerr;
using std::stoi;
using std::ifstream;
using std::sregex_token_iterator;

//ファイル読み出し処理
class ATestCovFileManager
{
public:
    static string trimLine(string input)
    {
        auto output = regex_replace(input, regex("^[\\t\\s]+"), "");
        output = regex_replace(output, regex("[\\t\\s]+$"), "");
        output = regex_replace(output, regex("[\\t\\s]*,[\\t\\s]*"), ",");
        output = regex_replace(output, regex("[\\t\\s]*&[\\t\\s]*"), "&");
        output = regex_replace(output, regex("\\s*\\t+\\s*"), ",");
        return output;
    }

    static void readTestCaseFile(const string &file_path, TestCase &output)
    {
        ifstream ifs(file_path);
        if (ifs.fail()) {
            cerr << "error:file open error-" << file_path << endl;
            throw ATestCovException("file open error");
        }
        string raw_line;
        const regex sep_testcondition{"[\\t,]"};
        vector<string> v = {};
        auto label_readed = false;
        while (getline(ifs, raw_line)) {
            if (raw_line.length() == 0) {
                continue;
            }
            auto str = trimLine(raw_line);
            if (str.length() == 0 || str[0] == '#') {
                //先頭#はコメント行、空行無視
                continue;
            }
            auto ite = sregex_token_iterator(str.begin(), str.end(), sep_testcondition, -1);
            const auto end = sregex_token_iterator();
            v.clear();
            while (ite != end) {
                if (*ite == "") {
                    cerr << "error:invalid format:" << file_path << endl;
                    throw ATestCovException("invalid format");
                }
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
            if (*ite == "") {

            }
            vlevel.push_back(*ite++);
        }
        output.add(v[0], vlevel);        
    }

    static void readMutex(const string &input, FactorLevelSet &output)
    {
        Mutex mutex;
        string param = regex_replace(input, regex("\\s*@mutex\\s*"), "");

        const regex sep_mutex{"\\s*[,]+\\s*"};
        auto ite = sregex_token_iterator(param.begin(), param.end(), sep_mutex, -1);
        const auto end = sregex_token_iterator();
        vector<string> v = {};
        while (ite != end) {
            v.push_back(*ite++);
        }
        if (v.empty() || v.size() < 2) {
            cerr << "error:invalid @mutex statement" << endl;
            throw ATestCovException("invalid format");
        }

        vector<Factor> factor_set;
        for (auto fl : v) {
            const regex sep_fl{":"};
            auto ite = sregex_token_iterator(fl.begin(), fl.end(), sep_fl, -1);
            const auto end = sregex_token_iterator();
            vector<string> vfl = {};
            while (ite != end) {
                if (*ite == "") {
                    cerr << "error:empty keyword in @mutex statement" << endl;
                    throw ATestCovException("invalid format");                
                }
                vfl.push_back(*ite++);
            }
            if (vfl.empty() || vfl.size() != 2) {
                cerr << "error:invalid @mutex statement" << endl;
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
        string raw_line;
        regex sep_fl{":"};
        regex sep_level{"\\s*[,\\t]+\\s*"};
        vector<string> v = {};
        try {
            while (getline(ifs, raw_line)) {
                v.clear();
                if (raw_line.length() == 0) {
                    continue;
                }
                auto str = trimLine(raw_line);

                if (str.length() == 0 || str[0] == '#') {
                    //先頭#はコメント行。空行無視
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

