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
    cout << "--param <p>     filepath of parameter:value table file" << endl;
    cout << "--lower <l>     lower limit of nwise" << endl;
    cout << "--upper <u>     upper limit of nwise" << endl;
    cout << "--addmetrics <a>  add metrics for combination" << endl; 
    cout << "--info <i>      view infomation log" << endl;
    cout << "--help <h>      show help" << endl;
}

//コマンドライン引数からコンフィグ情報を読み出す
//引数入力有無・引数重複のみに限定してコンフィグチェックを行う（他のチェックはATestCovManager内で実施）
int readConfigFromArg(const int argc, char *argv[], ATestCovConfig &config)
{
    const struct option longopts[] = {
            {"addmetrics", no_argument, nullptr, 'a'},
            {"help", no_argument, nullptr, 'h'},
            {"info", no_argument, nullptr, 'i'},
            {"lower", required_argument, nullptr, 'l'},
            {"param", required_argument, nullptr, 'p'},
            {"testcase", required_argument, nullptr, 't'},
            {"upper", required_argument, nullptr, 'u'},
            {0, 0, 0, 0},
    };
    try {
        while (true) {
            const auto opt = getopt_long(argc, argv, "ahil:p:t:u:", longopts, nullptr);
            if (opt < 0) {
                break;
            }

            int param;
            switch (opt) {
            case 'u':
                param = stoi(optarg);
                if (param >= 0) {
                    config.nwise_max_ =static_cast<TcInt>(param);
                } else {
                    cerr << "[error]must be nwise_max >= 0" << endl;
                    exit(0);
                }
                break;
            case 'l':
                param = stoi(optarg);
                if (param >= 0) {
                    config.nwise_min_ =static_cast<TcInt>(param);
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
            case 'i':
                config.infolog_enable_ = true;
                break;
            case 'a':
                config.add_metrics_ = true;
                break;
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
        cerr << "error:empty argument. specify testcase and parameter filepath" << endl;
        exit(1);
    }
    return 0;
}

int ATestCovMain(int argc, char *argv[])
{
    cout << "atestcov(ver." << ATESTCOV_VERSION << ") ***" << endl;
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
