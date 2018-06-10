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
        while (getline(ifs, str)) {
            v.clear();
            if (str.length() == 0) {
                continue;
            }
            if (str[0] == '#') {
                //先頭#はコメント行
                continue;
            }
            auto ite = sregex_token_iterator(str.begin(), str.end(), sep_fl, -1);
            auto end = sregex_token_iterator();
            while (ite != end) {
                v.push_back(*ite++);
            }
            if (v.empty() || v.size() != 2) {
                cerr << "Invalid Format:" << file_path << endl;
                throw ATestCovException("invalid format");
            }

            str = v[1];
            v.clear();
            vector<string> vlevel = {};
            ite = sregex_token_iterator(str.begin(), str.end(), sep_level, -1);
            while (ite != end) {
                vlevel.push_back(*ite++);
            }
            output.add(v[0], vlevel);
        }
    }
};

