#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <regex>

using namespace std;

class ATestCovFileManager
{
public:
    static void readTestCaseFile(const string &file_path, TestCase & output)
    {
        ifstream ifs(file_path);
        string str;
        regex sep_testcondition{"[\\s,\\t]+"};
        if (ifs.fail()) {
            cerr << "file open error:" << file_path << endl;
            return;
        }
        vector<string> v = {};
        
        // 一行目のテスト入力テキストを取得
        while (getline(ifs, str)) {
            if (str.length() == 0) {
                continue;
            }
            if (str[0] == '#') {
                continue;
            }
            auto ite = sregex_token_iterator(str.begin(), str.end(), sep_testcondition, -1);
            auto end = sregex_token_iterator();
            while (ite != end) {
                v.push_back(*ite++);
            }
            output.addItemText(v);
            //テスト入力部
            break;
        }

        while (getline(ifs, str)) {
            v.clear();
            if (str.length() == 0) {
                continue;
            }
            if (str[0] == '#') {
                continue;
            }
            auto ite = sregex_token_iterator(str.begin(), str.end(), sep_testcondition, -1);
            auto end = sregex_token_iterator();
            while (ite != end) {
                v.push_back(*ite++);
            }
            if (v.empty()) {
                cerr << "Invalid Format:" << file_path << endl;
                return;
            }
            output.addTestcaseText(v);
        }
    }

    static void readFLFile(const string &file_path, FactorLevelSet &output)
    {
        ifstream ifs(file_path);
        if (ifs.fail()) {
            cerr << "file open error:" << file_path << endl;
            return;
        }
        string str;
        regex sep_fl{":"};
        regex sep_level{"[,\\t]"};
        vector<string> v = {};
        while (getline(ifs, str)) {
            v.clear();
            if (str.length() == 0) {
                continue;
            }
            if (str[0] == '#') {
                continue;
            }
            auto ite = sregex_token_iterator(str.begin(), str.end(), sep_fl, -1);
            auto end = sregex_token_iterator();
            while (ite != end) {
                v.push_back(*ite++);
            }
            if (v.empty() || v.size() != 2) {
                cerr << "Invalid Format:" << file_path << endl;
                return;
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

