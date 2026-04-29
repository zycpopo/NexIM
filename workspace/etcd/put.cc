#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <iostream>
#include <thread>

int main()
{
    std::string etcd_host = "http://127.0.0.1:2379";


    //实例化客户端对象
    etcd::Client client(etcd_host);
    //获取租约保活对象--伴随创建一个指定有效时长的租约
    auto keep_alive = client.leasekeepalive(3).get();
    //获取租约ID
    auto lease_id= keep_alive->Lease();
    //向服务端新加相数据
    auto resp=client.put("/service/user","127.0.0.1:8080", lease_id).get();    

    if(resp.is_ok() == false){
        std::cout <<"新增数据是失败的"<<resp.error_message() << std::endl;
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}