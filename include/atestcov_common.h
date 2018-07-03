#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::string;

//テストケースやパラメータの値、個数の処理に用いる整数型。将来的なメモリ削減の検討のため独自型で定義
using TcInt = unsigned short;

using TestCaseSetVal = vector<vector<TcInt>>;
using TestCaseVal = vector<TcInt>;
using FactorLevelSetVal = vector<TcInt>;

//デバッグ用の雑多な処理
class Debug
{
public:
    static void p(const string &name, const vector<TcInt> &v)
    {
        cout << name << ":";
        for (auto c : v) {
            cout << c << " ";
        }
        cout << endl;        
    }

    static void p(const vector<vector<TcInt>> &tcv)
    {  
        for (auto testcase : tcv) {
            for (auto value : testcase) {
                cout << value << "  ";
            }
            cout << endl;
        }
    }
};

class ATestCovException
{
public:
    string message_;

    ATestCovException(const string &message) : message_(message)
    {
    }

    void p() const
    {
        cout << message_ << endl;
    }
};

class FactorLevelVal
{
public:
    TcInt index_;
    TcInt value_;
    FactorLevelVal(TcInt index, TcInt value) : index_(index), value_(value)
    {}
};

class FactorLevel
{
public:
    string factor_;
    vector<string> level_;

    FactorLevel(const string &factor, const vector<string> &level) : factor_(factor), level_(level)
    {
    }
};

class Factor
{
public:
    string factor_;
    string value_;

    Factor(const Factor &other)
    {
        factor_ = other.factor_;
        value_ = other.value_;
    }

    Factor(const string &factor, const string &value) : factor_(factor), value_(value)
    {}
};

using Mutex = vector<Factor>;


class MutexVal
{
public:
    vector<FactorLevelVal> mutexval_;

    MutexVal()
    {}

    void textToVal()
    {
        
    }

    MutexVal(const vector<FactorLevelVal> &mutex_set) : mutexval_(mutex_set)
    {
    }

    void initialize()
    {
        mutexval_.clear();
    }

    size_t size() const
    {
        return mutexval_.size();
    }

    // for debug
    void print()
    {
        cout << "* mutex list *" << endl;
        for (auto factor : mutexval_) {
            cout << factor.index_ << ":" << factor.value_ << ", ";
        }
        cout << endl;
    }

    bool enable(const vector<TcInt> &comp_index, const vector<TcInt> &comp_val)
    {
        if (mutexval_.size() < comp_index.size()) {
            return false;
        }

        vector<bool> hit_list(mutexval_.size(), false);
        for (auto i = 0; i < comp_index.size(); i++) {
            for (auto j = 0; j < mutexval_.size(); j++) {
                if (mutexval_[j].index_ == comp_index[i] && mutexval_[j].value_ == comp_val[i]) {
                    hit_list[j] = true;
                }
            }
        }
        return (std::find(hit_list.begin(), hit_list.end(), false) == hit_list.end());
    }
};

using MutexSetVal = vector<MutexVal>;

//パラメータ、制約のテキストデータとそれらの値化処理の管理
class FactorLevelSet
{
protected:
    vector<FactorLevel> factors_;
    vector<Mutex> mutex_;

public:
    size_t size() const
    {
        return factors_.size();
    }

    size_t mutexSize() const
    {
        return mutex_.size();
    }

    void initialize()
    {
        factors_.clear();
        mutex_.clear();
    }

    void addMutex(Mutex mutex)
    {
        mutex_.push_back(mutex);
    }

    void add(const string &factor_name, const vector<string> &levels)
    {
        factors_.push_back(FactorLevel(factor_name, levels));
    }

    void printTextByNum(const vector<TcInt> &comp_index, const vector<TcInt> &comp_val) const
    {
        for (auto i = 0; i < comp_index.size(); i++) {
            cout << factors_[comp_index[i]].factor_ << ":" << factors_[comp_index[i]].level_[comp_val[i]]<< "  ";
        }
        cout << endl;
    }

    //パラメータごとの値の数を算出
    void toNum(FactorLevelSetVal &numlist) const
    {
        numlist.clear();
        for (auto fl : factors_) {
            numlist.push_back(fl.level_.size() - 1);
        }
    }

    void toMutexNum(MutexSetVal &mutex_val) const
    {
        for (auto mutex_set : mutex_) {
            MutexVal mutex;
            for (auto mutex_factor : mutex_set) {
                for (auto i = 0; i < factors_.size(); i++) {
                    for (auto j = 0; j < factors_[i].level_.size(); j++) {
                        if (factors_[i].level_[j] == mutex_factor.value_ &&
                            factors_[i].factor_ == mutex_factor.factor_) {
                                mutex.mutexval_.push_back(FactorLevelVal(i, j));
                        }
                    }
                }
            }
            if (mutex.size() > 0) {
                mutex_val.push_back(mutex);
            }
        }
    }

    TcInt getLevelNum(const string &factorText, const string &levelText) const
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
        throw ATestCovException("no match");
    }

    void print() const
    {
        for (auto i = 0; i < factors_.size(); i++) {
            cout << factors_[i].factor_ << "]";
            for (auto level : factors_[i].level_) {
                cout << level << ",";
            }
            cout << endl;
        }

        for (auto mutex : mutex_) {
            for (auto factor : mutex) {
                cout << factor.factor_ << "::" << factor.value_ << "  ";
            }
            cout << endl;
        }
    }
};


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

class TestCase 
{
protected:
    vector<string> item_text_;
    vector<vector<string>> testcase_text_;

public:
    size_t itemSize() const
    {
        return item_text_.size();
    }

    size_t testcaseSize() const
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

    void textToNum(const FactorLevelSet &fl, TestCaseSetVal &tc) const
    {
        tc.clear();
        for (auto j = 0; j < testcase_text_.size(); j++) {
            vector<TcInt> tcvv(testcase_text_[j].size());
            for (auto i = 0; i < testcase_text_[j].size(); i++) {
                try {
                    auto num = fl.getLevelNum(item_text_[i], testcase_text_[j][i]);
                    tcvv[i] = num;
                } catch (ATestCovException e) {
                    //unmatch. noop
                }
            }
            tc.push_back(tcvv);
        }
    }

    bool checkWithParamList(const FactorLevelSet &fl) const 
    {
        return true;
        /*
        vector<bool> hit_list(item_text_.size(), false);
        for (auto item : item_text_) {
            for ()
            for (auto j = 0; j < comp_index.size(); j++) {
                if (testcase[comp_index[j]] == comp_val[j]) {
                    hit_list[j] = true;
                }
            }
            if (std::find(hit_list.begin(), hit_list.end(), false) != hit_list.end()) {
                continue;
            } else {
                return true;
            }
        }*/
    }

    // for debug
    void print() const
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
