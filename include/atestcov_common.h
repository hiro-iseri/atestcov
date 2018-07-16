#pragma once

#include <string>
#include <vector>

using std::vector;
using std::endl;
using std::cout;
using std::string;

//テストケースやパラメータの値、個数の処理に用いる整数型。将来的なメモリ削減の検討のため独自型で定義
using TcInt = unsigned short;

class ATestCovRange
{
public:
    const static TcInt MAX_NWISE = 50;
    const static TcInt MAX_TESTCASE = 10000;
    const static TcInt MAX_FACTOR_COMB = 10000;
    const static TcInt MAX_NAME_LENGTH = 50;
    const static TcInt MAX_LEVEL = 50;
    const static TcInt MAX_PARAETER = 50;
    const static TcInt MAX_MUTEX_PARAMETER = 50;
    const static TcInt MAX_MUTEX = 20;
};

constexpr TcInt INVALID_PARAMETER_VALUE = ATestCovRange::MAX_PARAETER + 1;
constexpr TcInt INVALID_PARAMETER_INDEX = ATestCovRange::MAX_PARAETER + 1;

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
private:
    string message_;
public:
    ATestCovException(const string &message) : message_(message)
    {
    }

    void p() const
    {
        cout << message_ << endl;
    }
};
