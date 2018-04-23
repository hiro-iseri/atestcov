#include <iostream>
#include <getopt.h>

using namespace std;

//起動時に指定するコンフィグデータ
class AtestCovConfig
{
public:
    int nwise_min_ = 0;
    int nwise_max_ = 0;
    string filepath_testcase_;
    string filepath_fl_list_;
};

void printHelp()
{
    cout << "TBD" << endl;
}

//コマンドライン引数からコンフィグ情報を読み出す
int readConfigFromArg(const int argc, char *argv[], AtestCovConfig &config)
{
    const struct option longopts[] = {
            {"testcase", required_argument, nullptr, 't'},
            {"fllist", required_argument, nullptr, 'f'},
            {"nwisemin", optional_argument, nullptr, 'i'},
            {"nwisemax", optional_argument, nullptr, 'a'},
            {"help", no_argument, nullptr, 'h'},
            {0, 0, 0, 0},
        };
    while (true) {
        const auto opt = getopt_long(argc, argv, "f:t:i:a:h", longopts, nullptr);
        if (opt < 0) {
            break;
        }

        switch (opt) {
        case 'a':
            config.nwise_max_ = stoi(optarg);
            break;
        case 'i':
            config.nwise_min_ = stoi(optarg);
            break;
        case 'f':
            config.filepath_fl_list_ = string(optarg);
            break;
        case 't':
            config.filepath_testcase_ = string(optarg);
            break;
        case 'h':
        default:
            printHelp();
        }
    }  
    return 0;
}

//atestcovスタートアップ
int ATestCovMain(int argc, char *argv[])
{
    AtestCovConfig config;
    readConfigFromArg(argc, argv, config);

    return 0;
}

int main(int argc, char **argv)
{
    std::cout << "atestcov" << std::endl;
    return ATestCovMain(argc, argv);
}