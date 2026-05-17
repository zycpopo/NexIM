#include "../common/icsearch.hpp"
#include <gflags/gflags.h>
#include <iostream>

DEFINE_bool (run_mode,false,"程序运行模式,false为调试模式,true为发布模式");
DEFINE_string(log_file,"","发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level,0,"发布模式下,用于指定日志的输出级别,0为trace,1为debug,2为info,3为warn,4为error,5为critical");

int main(int argc,char *argv[]) 
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode,FLAGS_log_file, FLAGS_log_level);

    std::vector<std::string> host_lists = {"http://127.0.0.1:9200/"};

    auto client  = std::make_shared<elasticlient::Client>(host_lists);
    std::cout << (uint64_t)client.get() <<std::endl;
    bool ret = ESIndex(client,"test_user","_doc").append("nickname")
        .append("phonenumber","keyword","standard",true)
        .create();
    if(ret == true){
        LOG_DEBUG("索引创建成功");
    }
    return 0;
}