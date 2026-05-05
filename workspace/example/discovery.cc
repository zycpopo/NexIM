#include "../common/etcd.hpp"
#include "../common/logger.hpp"
#include <gflags/gflags.h>
#include <thread>

DEFINE_bool (run_mode,false,"程序运行模式,false为调试模式,true为发布模式");
DEFINE_string(log_file,"","发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level,0,"发布模式下,用于指定日志的输出级别,0为trace,1为debug,2为info,3为warn,4为error,5为critical");

DEFINE_string(etcd_host,"http://127.0.0.1:2379","服务注册中心地址");
DEFINE_string(base_service,"/service","服务监控根目录");

void online(const std::string & service_name,const std::string &service_host)
{
    LOG_DEBUG("上线服务：{}-{}",service_name,service_host);
}
void offline(const std::string & service_name,const std::string &service_host)
{
    LOG_DEBUG("下线服务：{}-{}",service_name,service_host);
}

int main(int argc, char * argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode,FLAGS_log_file, FLAGS_log_level);

    Discovery::ptr dclient = std::make_shared<Discovery>(FLAGS_etcd_host,FLAGS_base_service,online,offline);

    std::this_thread::sleep_for(std::chrono::seconds(600));
    return 0;
}