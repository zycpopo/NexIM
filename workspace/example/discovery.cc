#include "../common/etcd.hpp"
#include "../common/logger.hpp"
#include "../common/channel.hpp"
#include "main.pb.h"
#include <gflags/gflags.h>
#include <thread>

DEFINE_bool (run_mode,false,"程序运行模式,false为调试模式,true为发布模式");
DEFINE_string(log_file,"","发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level,0,"发布模式下,用于指定日志的输出级别,0为trace,1为debug,2为info,3为warn,4为error,5为critical");

DEFINE_string(etcd_host,"http://127.0.0.1:2379","服务注册中心地址");
DEFINE_string(base_service,"/service","服务监控根目录");
DEFINE_string(call_service,"/service/echo","rpc调用目录");

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


    //1.先构造Rpc信道管理对象
    auto sm = std::make_shared<ServiceManager>();
    sm->declared(FLAGS_call_service);
    auto put_cb = std::bind(&ServiceManager::onServiceOnline,sm.get(),std::placeholders::_1,std::placeholders::_2);
    auto del_cb = std::bind(&ServiceManager::onServiceOffline,sm.get(),std::placeholders::_1,std::placeholders::_2);
    //2.构造服务发现对象
    Discovery::ptr dclient = std::make_shared<Discovery>(FLAGS_etcd_host,FLAGS_base_service,put_cb,del_cb);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //4.发起EchoRpc调用
    while(1){
        //3.通过Rpc信道管理对象
        auto channel = sm->choose(FLAGS_call_service);
        if(!channel)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        example::EchoService_Stub stub(channel.get()); 
        example::EchoRequest req;
        req.set_message("我是popo,请多指教!");
        brpc::Controller *cntl = new brpc::Controller();
        example::EchoResponse *rsp = new example::EchoResponse();
        stub.Echo(cntl,&req,rsp,nullptr);//最后一个参数nullptr默认是同步
        if(cntl->Failed() == true)
        {
            std::cout << "Rpc调用失败:" << cntl->ErrorText() << std::endl;
            delete cntl;
            delete rsp; 
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        std::cout << "收到响应："<< rsp->message() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } 
    return 0;
}