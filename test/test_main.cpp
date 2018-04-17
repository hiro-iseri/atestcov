#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>

using namespace std;

//デバッグ用print
void printVector(const string &name, const vector<int> &v)
{
    cout << name << ":";
    for (auto c : v) {
        cout << c << " ";
    }
    cout << endl;
}


class CombinatorialCoverage
{
public:
    int num_all_combi_ = 0;
    int num_hit_combi_ = 0;
    int nwise = 0;
};

/*
・処理の流れ
全因子組合せを作る 
因子組合せごとに処理：
　因子組合せの全水準組わせを作る
　水準組み合わせ毎に処理：
　　水準組合せを包含するテストケースを探す
*/
class CombinatorialCoverageMeasurer
{
protected:
    vector<vector<int>> testcase_set_;
public:
    int totalnum_levelcomb_ = 0;
    int hitnum_levelcomb_ = 0;

protected:

public:
    //因子ごとの水準数をnum_listに格納
    void createNumList(const vector<vector<int>> &in, vector<int> &num_list)
    {
        num_list.assign(num_list.size(), 0);
        for (auto testcase : in) {
            for (auto i = 0; i < testcase.size(); i++) {
                num_list[i] = (num_list[i] < testcase[i]) ? testcase[i] : num_list[i];
            }
        }
    }

    void printCoverageReport()
    {
        cout << endl;
        cout << "***** report for combinatorial coverage *****" << endl;
        cout << "test info *****";
        cout << "number of testcase: " << testcase_set_.size() << endl;
    }

    // 組合せを生成しoutput格納
    void createCombination(vector<vector<int>> &output, const int n, const int r)
    {
        vector<bool> valid(n);
        fill(valid.end() - r, valid.end(), true);
        do {
            vector<int> combination;
            for (auto i = 0; i < n; i++) {
                if (valid[i]) {
                    combination.push_back(i);
                }
            }
            output.push_back(combination);
        } while (next_permutation(valid.begin(), valid.end()));
    }

    //テストケース組合せが、指定の因子水準組合せを網羅していることを確認
    bool coverLevelCombination(const vector<int> &comp_index, const vector<int> &comp_val)
    {
        for (auto testcase : testcase_set_) {
            printVector("test case", testcase);
            auto included = true;
            for (auto j = 0; j < comp_index.size(); j++) {
                if (testcase[comp_index[j]] != comp_val[j]) {
                    included = false;
                    break;
                }
            }
            if (included) {
                return true;
            }
        }
        return false;
    }

    // nスイッチカバレッジの組合せを生成し、テストケース組合せと包含評価
    void create_fv_combination(const vector<int> &numlist, const vector<int> &comp_set, vector<int> &index_list, int index)
    {
        for (index_list[index] = 0; index_list[index] <= numlist[comp_set[index]]; index_list[index]++) {
            if (index + 1 >= comp_set.size()) {
                totalnum_levelcomb_++;
                if (coverLevelCombination(comp_set, index_list)) {
                    hitnum_levelcomb_++;
                }
            } else {
                create_fv_combination(numlist, comp_set, index_list, index + 1);
            }
        }
    }

    void measureCoverage(const vector<vector<int>> &testcase_set, const vector<int> &numlevels, const int nwise)
    {
        assert(testcase_set.size() > 0);
        testcase_set_ = testcase_set;
        auto num_testcase = testcase_set_[0].size();

        vector<int> index_list(num_testcase, 0);

        vector<vector<int>> comp_set;
        createCombination(comp_set, num_testcase, nwise);

        for (vector<int> comp : comp_set) {
            printVector("factor com", comp);
            create_fv_combination(numlevels, comp, index_list, 0);
        }
    }
};


TEST(atestcov, calculate_coverage_1wise)
{
    CombinatorialCoverageMeasurer mr;
    vector<vector<int>> testcase_set;
    testcase_set.push_back(vector<int>{0, 0});
    testcase_set.push_back(vector<int>{0, 1});
    testcase_set.push_back(vector<int>{1, 1});
    vector<int> numlevel(testcase_set[0].size());
    mr.createNumList(testcase_set, numlevel);

    mr.measureCoverage(testcase_set, numlevel, 1);

    cout << mr.hitnum_levelcomb_ << "/" << mr.totalnum_levelcomb_ << endl;

    EXPECT_EQ(4, mr.totalnum_levelcomb_);
    EXPECT_EQ(4, mr.hitnum_levelcomb_);
}

TEST(atestcov, calculate_coverage_2wise)
{
    CombinatorialCoverageMeasurer mr;
    vector<vector<int>> testcase_set;
    testcase_set.push_back(vector<int>{0, 0, 0});
    testcase_set.push_back(vector<int>{0, 1, 1});
    testcase_set.push_back(vector<int>{1, 0, 0});

    vector<int> numlevel(testcase_set[0].size());
    mr.createNumList(testcase_set, numlevel);
    mr.measureCoverage(testcase_set, numlevel, 2);

    cout << mr.hitnum_levelcomb_ << "/" << mr.totalnum_levelcomb_ << endl;

    EXPECT_EQ(12, mr.totalnum_levelcomb_);
}

TEST(atestcov, createCombination)
{
    CombinatorialCoverageMeasurer mr;

    vector< std::vector<int> > comp;
    mr.createCombination(comp, 5, 3);
    EXPECT_EQ(10, comp.size());
}

#include <fstream>
#include <iostream>
#include <string>
#include <regex>


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
protected:
    vector<FactorLevel> factors_;

public:
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

    void print()
    {
        for (auto i = 0; i < factors_.size(); i++) {
            cout << factors_[i].factor_;
            for (auto level : factors_[i].level_) {
                cout << level;
            }
            cout << endl;
        }
    }
};

using testcase = vector<vector<int>>;

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

};


class ATestCovFileManager
{
public:
    static void readTestCaseFile(const string &file_path, TestCase & output)
    {
        ifstream ifs(file_path);
        string str;
        regex sep_testcondition{"[,¥t]+"};
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
        regex sep_level{"[,]"};
        vector<string> v = {};
        while (getline(ifs, str)) {
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
            if (v.empty()) {
                cerr << "Invalid Format:" << file_path << endl;
                return;
            }
            
            vector<string> vlevel = {};
            ite = sregex_token_iterator(str.begin(), str.end(), sep_level, -1);
            while (ite != end) {
                vlevel.push_back(*ite++);
            }
            output.add(v[0], vlevel);
        }
    }
};

TEST(atestcov, readTestCaseFile_simple)
{
    TestCase tc;
    ATestCovFileManager::readTestCaseFile("testdata/SimpleTestCase.txt", tc);
    EXPECT_EQ(3, tc.testcaseSize());
}

TEST(atestcov, readFLFile_simple)
{
    FactorLevelSet fl;
    ATestCovFileManager::readFLFile("testdata/SimpleFL.txt", fl);
    EXPECT_EQ(2, fl.size());
}

class ATestCovManager
{
public:
    void run(string fl_file_path, string testcase_file_path)
    {
        TestCase tc;
        FactorLevelSet fl;
        ATestCovFileManager::readTestCaseFile(fl_file_path, tc);
        ATestCovFileManager::readFLFile(testcase_file_path, fl);
    }
};

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
