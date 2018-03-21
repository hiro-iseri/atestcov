#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>

using namespace std;

// 組合せを生成しoutput格納
void create_combination(vector<vector<int> > &output, int n, int r)
{
    vector<bool> v(n);
    fill(v.end() - r, v.end(), true);

    do {
        vector<int> cmp;
        for (int i = 0; i < n; i++) {
            if (v[i]) {
                cmp.push_back(i);
            }
        }
        output.push_back(cmp);
    } while (std::next_permutation(v.begin(), v.end()));
}

vector<vector<int> > testcase_set;

void print_vector(const string &name, const vector<int> &v)
{
    cout << name << ":";
    for (int c : v) {
        cout << c << " ";
    }
    cout << endl;
}
int totalnum_comp_set = 0;
int hitnum_comp_set = 0;

/*
・処理の流れ
全因子組合せを作る 
因子組合せごとに処理：
　因子組合せの全水準組わせを作る
　水準組み合わせ毎に処理：
　　水準組合せを包含するテストケースを探す
*/

//テストケース組合せが、指定の因子水準組合せを網羅していることを確認
bool testcase_include_comb(const vector<int> &comp_index, const vector<int> &comp_val)
{
    for (auto testcase : testcase_set) {
        print_vector("test case", testcase);
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
            for (int j = 0; j < comp_set.size(); j++) {
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

//因子ごとの水準数をnum_listに格納
void create_numlist(vector<vector<int> > &in, vector<int> &num_list)
{
    num_list.assign(num_list.size(), 0);
    for (vector<int> testcase : in) {
        for (int i = 0; i < testcase.size(); i++) {
            num_list[i] = (num_list[i] < testcase[i]) ? testcase[i] : num_list[i];
        }
    }
}

void calculate_coverage()
{
    assert(testcase_set.size() > 0);
    vector<int> index_list(testcase_set[0].size(), 0);
    vector<int> num_list(testcase_set[0].size(), 0);

    create_numlist(testcase_set, num_list);
    print_vector("numlist", num_list);

    std::vector<std::vector<int> > comp_set;
    create_combination(comp_set, testcase_set[0].size(), 2);

    for (vector<int> comp : comp_set) {
        cout << "factor cob:";
        for (int nnn : comp) {
            cout << nnn << " ";
        }
        cout << endl;

        create_fv_combination(num_list, comp, index_list, 0);
    }
}

TEST(atestcov, hoge)
{
    testcase_set.push_back(vector<int>{0, 0, 0});
    testcase_set.push_back(vector<int>{0, 1, 1});
    testcase_set.push_back(vector<int>{1, 0, 0});

    calculate_coverage();

    cout << hitnum_comp_set << "/" << totalnum_comp_set << endl;

    EXPECT_EQ(true, true);
}

TEST(atestcov, create_combination)
{
    std::vector< std::vector<int> > comp;
    create_combination(comp, 5, 3);
    EXPECT_EQ(10, comp.size());
}


GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
