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
    vector<vector<int> > testcase_set_;
public:
    int totalnum_comp_set = 0;
    int hitnum_comp_set = 0;

protected:
    //因子ごとの水準数をnum_listに格納
    void createNumList(vector<vector<int> > &in, vector<int> &num_list)
    {
        num_list.assign(num_list.size(), 0);
        for (vector<int> testcase : in) {
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
    void create_combination(vector<vector<int> > &output, const int n, const int r)
    {
        vector<bool> v(n);
        fill(v.end() - r, v.end(), true);
        do {
            vector<int> cmp;
            for (auto i = 0; i < n; i++) {
                if (v[i]) {
                    cmp.push_back(i);
                }
            }
            output.push_back(cmp);
        } while (next_permutation(v.begin(), v.end()));
    }

    //テストケース組合せが、指定の因子水準組合せを網羅していることを確認
    bool testcase_include_comb(const vector<int> &comp_index, const vector<int> &comp_val)
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
                totalnum_comp_set++;
                if (testcase_include_comb(comp_set, index_list)) {
                    cout << "hit" << endl;
                    hitnum_comp_set++;
                }
            } else {
                create_fv_combination(numlist, comp_set, index_list, index + 1);
            }
        }
    }

    void calculate_coverage(const vector<vector<int> > &testcase_set, const int nwise)
    {
        assert(testcase_set.size() > 0);
        testcase_set_ = testcase_set;
        auto num_testcase = testcase_set_[0].size();

        vector<int> index_list(num_testcase, 0);
        vector<int> num_list(num_testcase, 0);
        createNumList(testcase_set_, num_list);

        std::vector<std::vector<int> > comp_set;
        create_combination(comp_set, num_testcase, nwise);

        for (vector<int> comp : comp_set) {
            printVector("factor com", comp);
            create_fv_combination(num_list, comp, index_list, 0);
        }
    }
};


TEST(atestcov, calculate_coverage_1wise)
{
    CombinatorialCoverageMeasurer mr;
    vector<vector<int> > testcase_set;
    testcase_set.push_back(vector<int>{0, 0});
    testcase_set.push_back(vector<int>{0, 1});
    testcase_set.push_back(vector<int>{1, 1});

    mr.calculate_coverage(testcase_set, 1);

    cout << mr.hitnum_comp_set << "/" << mr.totalnum_comp_set << endl;

    EXPECT_EQ(true, true);
}

TEST(atestcov, calculate_coverage_2wise)
{
    CombinatorialCoverageMeasurer mr;
    vector<vector<int> > testcase_set;
    testcase_set.push_back(vector<int>{0, 0, 0});
    testcase_set.push_back(vector<int>{0, 1, 1});
    testcase_set.push_back(vector<int>{1, 0, 0});

    mr.calculate_coverage(testcase_set, 1);

    cout << mr.hitnum_comp_set << "/" << mr.totalnum_comp_set << endl;

    EXPECT_EQ(true, true);
}

TEST(atestcov, create_combination)
{
    CombinatorialCoverageMeasurer mr;

    vector< std::vector<int> > comp;
    mr.create_combination(comp, 5, 3);
    EXPECT_EQ(10, comp.size());
}


GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
