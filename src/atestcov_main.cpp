#include <iostream>
#include <getopt.h>

#include "version.h"
#include "atestcov_config.h"
#include "atestcov_manager.h"

using std::cout;
using std::cerr;
using std::endl;

void printHelp()
{
    cout << "--testcase <t>  filepath of testcase file" << endl;
    cout << "--fllist <f>    filepath of factor-level table file" << endl;
    cout << "--nwisemin <f>  lower limit of nwise" << endl;
    cout << "--nwisemax <f>  upper limit of nwise" << endl;
    cout << "--help <h>      show help" << endl;
    cout << "--version <v>   show software version" << endl;
}

//コマンドライン引数からコンフィグ情報を読み出す
//引数入力有無・引数重複のみに限定してコンフィグチェックを行う（他のチェックはATestCovManager内で実施）
int readConfigFromArg(const int argc, char *argv[], ATestCovConfig &config)
{
    const struct option longopts[] = {
            {"testcase", required_argument, nullptr, 't'},
            {"fllist", required_argument, nullptr, 'f'},
            {"nwisemin", optional_argument, nullptr, 'i'},
            {"nwisemax", optional_argument, nullptr, 'a'},
            {"help", no_argument, nullptr, 'h'},
            {"version", no_argument, nullptr, 'v'},
            {0, 0, 0, 0},
    };
    try {
        while (true) {
            const auto opt = getopt_long(argc, argv, "f:t:i:a:hv", longopts, nullptr);
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
            case 'v':
                cout << "atestcov ver.:" << ATESTCOV_VERSION << endl;
                exit(0);
            case 'h':
                printHelp();
                exit(0);
            default:
                cerr << "error:invalid argument" << endl;
                printHelp();
                exit(1);
            }
        }
    } catch (...) {
        cerr << "error:invalid argument" << endl;
        exit(1);
    }

    if (config.filepath_testcase_.empty() || config.filepath_fl_list_.empty()) {
        cerr << "error:filepath is empty" << endl;
        exit(1);
    }

    return 0;
}

//引数に基づいて解析処理実行
int ATestCovMain(int argc, char *argv[])
{
    cout << "*** atestcov(ver." << ATESTCOV_VERSION << ") ***" << endl;
    ATestCovConfig config;
    if (0 == readConfigFromArg(argc, argv, config)) {
        ATestCovManager manager;
        manager.run(config);
        exit(0);
    }
    exit(1);
}

int main(int argc, char **argv)
{
    return ATestCovMain(argc, argv);
}
