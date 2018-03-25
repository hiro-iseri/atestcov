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
    //因子ごとの水準数をnum_listに格納
    void createNumList(const vector<vector<int>> &in, vector<int> &num_list)
    {
        num_list.assign(num_list.size(), 0);
        for (auto testcase : in) {
            for (int i = 0; i < testcase.size(); i++) {
                num_list[i] = (num_list[i] < testcase[i]) ? testcase[i] : num_list[i];
            }
        }
    }

public:
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
                for (auto j = 0; j < comp_set.size(); j++) {
                    cout << comp_set[j] << ":" << index_list[j] << " ";
                }
                cout << endl;
                totalnum_levelcomb_++;
                if (coverLevelCombination(comp_set, index_list)) {
                    cout << "hit" << endl;
                    hitnum_levelcomb_++;
                }
            } else {
                create_fv_combination(numlist, comp_set, index_list, index + 1);
            }
        }
    }

    void measureCoverage(const vector<vector<int>> &testcase_set, const int nwise)
    {
        assert(testcase_set.size() > 0);
        testcase_set_ = testcase_set;
        auto num_testcase = testcase_set_[0].size();

        vector<int> index_list(num_testcase, 0);
        vector<int> num_list(num_testcase, 0);
        createNumList(testcase_set_, num_list);

        vector<vector<int>> comp_set;
        createCombination(comp_set, num_testcase, nwise);

        for (vector<int> comp : comp_set) {
            printVector("factor com", comp);
            create_fv_combination(num_list, comp, index_list, 0);
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

    mr.measureCoverage(testcase_set, 1);

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

    mr.measureCoverage(testcase_set, 1);

    cout << mr.hitnum_levelcomb_ << "/" << mr.totalnum_levelcomb_ << endl;

    EXPECT_EQ(6, mr.totalnum_levelcomb_);
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
    vector<string> factors_;
    vector<vector<string>> levels_;

    void initialize()
    {
        factors_.clear();
        levels_.clear();
    }

    void add_factorlevel(const string &factor_name, const vector<string> &levels)
    {
        factors_.push_back(factor_name);
        levels_.push_back(levels);
    }
};

class ATestCovFileManager
{
public:
    static void readFLFile(const string &filepath, FactorLevel &output)
    {
        string file_path = "testdata/SimpleFL.txt";
        ifstream ifs(file_path);
        if (ifs.fail()) {
            cerr << "file open error:" << file_path << endl;
            return;
        }
        string str;
        regex sep{"[:,]"};
        vector<std::string> v = {};
        while (getline(ifs, str)) {
            cout << str << endl;
            auto ite = sregex_token_iterator(str.begin(), str.end(), sep, -1);
            auto end = sregex_token_iterator();
            while (ite != end) {
                cout << *ite << endl;
                v.push_back(*ite++);
            }
        }        
    }
};

TEST(atestcov, fileread)
{
    FactorLevel fl;
    ATestCovFileManager::readFLFile("testdata/SimpleFL.txt", fl);
    EXPECT_EQ(true, true);
}

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
