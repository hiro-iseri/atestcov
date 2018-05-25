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
    cout << "--pvlist <p>    filepath of parameter:value table file" << endl;
    cout << "--nwisemin <i>  lower limit of nwise" << endl;
    cout << "--nwisemax <a>  upper limit of nwise" << endl;
    cout << "--log <l>       view infomation log" << endl;
    cout << "--help <h>      show help" << endl;
    cout << "--version <v>   show software version" << endl;
}

//コマンドライン引数からコンフィグ情報を読み出す
//引数入力有無・引数重複のみに限定してコンフィグチェックを行う（他のチェックはATestCovManager内で実施）
int readConfigFromArg(const int argc, char *argv[], ATestCovConfig &config)
{
    const struct option longopts[] = {
            {"testcase", required_argument, nullptr, 't'},
            {"pvlist", required_argument, nullptr, 'p'},
            {"nwisemin", optional_argument, nullptr, 'i'},
            {"nwisemax", optional_argument, nullptr, 'a'},
            {"log", optional_argument, nullptr, 'l'},
            {"help", no_argument, nullptr, 'h'},
            {"version", no_argument, nullptr, 'v'},
            {0, 0, 0, 0},
    };
    try {
        while (true) {
            const auto opt = getopt_long(argc, argv, "p:t:i:a:lhv", longopts, nullptr);
            if (opt < 0) {
                break;
            }

            int param;
            switch (opt) {
            case 'a':
                param = stoi(optarg);
                if (param >= 0) {
                    config.nwise_max_ =static_cast<unsigned int>(param);
                } else {
                    cerr << "[error]must be nwise_max >= 0" << endl;
                    exit(0);
                }
                break;
            case 'i':
                param = stoi(optarg);
                if (param >= 0) {
                    config.nwise_min_ =static_cast<unsigned int>(param);
                } else {
                    cerr << "[error]must be nwise_min >= 0" << endl;
                    exit(0);
                }
                break;
            case 'p':
                config.filepath_fl_list_ = string(optarg);
                break;
            case 't':
                config.filepath_testcase_ = string(optarg);
                break;
            case 'l':
                config.infolog_enable_ = true;
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
