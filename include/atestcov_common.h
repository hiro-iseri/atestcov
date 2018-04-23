#pragma once

#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::string;

using TestCaseVal = vector<vector<int>>;
using FactorLevelSetVal = vector<int>;

class Debug
{
public:
    static void p(const string &name, const vector<int> &v)
    {
        cout << name << ":";
        for (auto c : v) {
            cout << c << " ";
        }
        cout << endl;        
    }

    static void p(const TestCaseVal &tcv)
    {  
        for (auto testcase : tcv) {
            for (auto value : testcase) {
                cout << value << "  ";
            }
            cout << endl;
        }
    }
};

class FLException
{
public:
    string message_;

    FLException(string message) : message_(message)
    {
    }
};

class FactorLevel {
public:
    string factor_;
    vector<string> level_;

    FactorLevel(const string &factor, const vector<string> &level) {
        factor_ = factor;
        level_ = level;
    }
};

class FactorLevelSet {
public:
    vector<FactorLevel> factors_;

    size_t size()
    {
        return factors_.size();
    }

    void initialize()
    {
        factors_.clear();
    }

    void add(const string &factor_name, const vector<string> &levels)
    {
        factors_.push_back(FactorLevel(factor_name, levels));
    }

    void toNum(vector<int> &numlist)
    {
        numlist.clear();
        for (auto fl : factors_) {
            numlist.push_back(fl.level_.size() - 1);
        }
    }

    int getLevelNum(const string factorText, const string levelText)
    {
        for (auto j = 0; j < factors_.size(); j++) {
            if (factors_[j].factor_ == factorText) {
                for (auto i2 = 0; i2 < factors_[j].level_.size(); i2++) {
                    if (factors_[j].level_[i2] == levelText) {
                        return i2;
                    }
                }
            }
        }
        throw FLException("no match");
    }

    void print()
    {
        for (auto i = 0; i < factors_.size(); i++) {
            cout << factors_[i].factor_ << "]";
            for (auto level : factors_[i].level_) {
                cout << level << ",";
            }
            cout << endl;
        }
    }
};

class TestCase {
private:
    vector<string> item_text_;
    vector<vector<string>> testcase_text_;

public:
    size_t itemSize()
    {
        return item_text_.size();
    }

    size_t testcaseSize()
    {
        return testcase_text_.size();
    }

    void addItemText(const vector<string> &item_text) 
    {
        item_text_ = item_text;
    }

    void addTestcaseText(const vector<string> &testcase_text)
    {
        testcase_text_.push_back(testcase_text);
    }


    void textToNum(FactorLevelSet &fl, TestCaseVal &tc)
    {
        tc.clear();
        for (auto j = 0; j < testcase_text_.size(); j++) {
            vector<int> tcvv(testcase_text_[j].size());
            for (auto i = 0; i < testcase_text_[j].size(); i++) {
                try {
                    auto num = fl.getLevelNum(item_text_[i], testcase_text_[j][i]);
                    tcvv[i] = num;
                } catch (FLException e) {
                    cout << "invalid value text" << endl;
                }
            }
            tc.push_back(tcvv);
        }
    }

    void print()
    {
        cout << "label:" << endl;
        for (auto item : item_text_) {
                cout << item << "  ";
        }
        cout << endl;

        cout << "testcase:" << endl;
        for (auto testcase : testcase_text_) {
            cout << testcase.size() << endl;
            for (auto value : testcase) {
                cout << value << "  ";
            }
            cout << endl;
        }

    }
};

