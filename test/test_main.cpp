#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>

using namespace std;

int create_combination(std::vector< std::vector<int> > &output, int n, int r)
{
    std::vector<bool> v(n);
    std::fill(v.end() - r, v.end(), true);

    do {
        std::vector<int> cmp;
        for (int i = 0; i < n; ++i) {
            if (v[i]) {
                cmp.push_back(i);
                std::cout << i << " ";
            }
        }
        output.push_back(cmp);
        std::cout << "\n";
    } while (std::next_permutation(v.begin(), v.end()));

    return 0;
}

//vector<int> testcase{1, 2, 3, 4};

void create_fv_combination(vector<int> &n , vector<int> &comp_set, vector<int> &i, int index)
{
    for (i[index] = 0; i[index] < n[comp_set[index]]; i[index]++) {
        if (index + 1 >= comp_set.size()) {
            //ここで完成
            for(int temp : i) {
                cout << temp << " ";
            }
            cout << "i:" << index << endl;
        } else {
            create_fv_combination(n, comp_set, i, index + 1);
        }
    }
}

TEST(atestcov, hoge)
{
    std::vector< std::vector<int> > comp_set;
    create_combination(comp_set, 3, 2);

    vector<int> test{3, 3, 2};
    vector<int> in(2, 0);

    for (vector<int> comp : comp_set) {
        cout << "*****";
        for (int nnn : comp) {
            cout << nnn << " ";
        }
        cout << endl;

        create_fv_combination(test, comp, in, 0);
    }

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
