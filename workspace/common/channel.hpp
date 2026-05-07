#include <brpc/channel.h>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include "logger.hpp"

//1.封装单个服务的信道管理类
class ServiceChannel{
    public:
        const std::string& get_name() const 
        {
            return _service_name;
        }
        using ptr = std::shared_ptr<ServiceChannel>;
        using ChannelPtr = std::shared_ptr<brpc::Channel>;
        ServiceChannel(const std::string  &name):
            _service_name(name), _index(0){}

        //服务上线节点，append
        void append(const std::string &host)
        {
            //初始化构造信道
            auto channel = std::make_shared<brpc::Channel>();
            brpc::ChannelOptions options;
            options.connect_timeout_ms = -1;
            options.timeout_ms = -1;
            options.max_retry = 3;
            options.protocol = "baidu_std";
            int ret = channel->Init(host.c_str(), &options);
            if(ret == -1)
            {
                LOG_ERROR("初始化{}-{}信道失败",_service_name, host);
                return;
            }
            std::unique_lock<std::mutex> lock(_mutex);

            _hosts.insert(std::make_pair(host,channel));
            _channels.push_back(channel);
        }
        //服务下线节点，remove
        void remove(const std::string &host)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _hosts.find(host);
            if(it == _hosts.end()){
                LOG_WARN( "{}-{}节点删除信道时，未找到信道信息！",_service_name, host);
                return;
            }   
            for(auto vit = _channels.begin(); vit != _channels.end(); ++vit)
            {
                if(*vit == it->second){
                    _channels.erase(vit);
                    break;
                }
            }
            _hosts.erase(it);
        }
        //通过RR轮转策略，获取一个Channel用于发起对应服务的Rpc调用
        ChannelPtr choose()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_channels.empty()){
                return nullptr;
            }
            int32_t idx = _index++ %_channels.size();
            return _channels[idx];
        }
    private:
        std::mutex _mutex;
        int32_t _index; //当前轮转下标计数器
        std::string _service_name; //服务名称
        std::vector<ChannelPtr> _channels;
        std::unordered_map<std::string, ChannelPtr> _hosts;//主机地址与信道映射关系
};

//2.总体服务信道管理类
class ServiceManager
{
    public:
        ServiceManager() {}
        using ptr = std::shared_ptr<ServiceManager>;
        ServiceChannel::ChannelPtr choose(const std::string &service_name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto sit = _services.find(service_name);
            if(sit == _services.end()){
                LOG_ERROR("当前没有能够提供{}服务的节点！",service_name);
                return ServiceChannel::ChannelPtr();
            }
            return sit->second->choose();
        }
        void declared(const std::string &service_name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _follow_services.insert(service_name);
        }

        //服务上线调用的回调接口，将服务节点管理起来
        void onServiceOnline(const std::string &service_instance , const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto fit = _follow_services.find(service_name);
                if(fit == _follow_services.end())
                {
                    LOG_DEBUG("{}-{}服务上线了,但是不在关注列表",service_name,host);
                    return ;
                }
                //先获取管理对象，没有则创建
                auto sit = _services.find(service_name);
                if(sit == _services.end())
                {
                    service = std::make_shared<ServiceChannel>(service_name);
                    _services.insert(std::make_pair(service_name, service));
                }else{
                    service = sit->second;
                }
            }
            if(!service)
            {
                LOG_ERROR("新增{}服务管理节点失败！",service->get_name());
                return ;
            }
            service->append(host);
            LOG_DEBUG("{}-{}服务上线节点,进行添加管理",service_name,host);
        }

        //服务下线调用的回调接口，从服务信道管理中，删除指定节点信道
        void onServiceOffline(const std::string &service_instance,const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto fit = _follow_services.find(service_name);
                if(fit == _follow_services.end())
                {
                    LOG_DEBUG("{}-{}服务下线了,但是不在关注列表",service_name,host);
                    return ;
                }
                auto sit = _services.find(service_name);
                if(sit == _services.end())
                {
                    LOG_WARN("删除{}服务节点时,没有找到管理对象",service_name);
                    return ;
                }
                service = sit->second;
            }
            service->remove(host);
            LOG_DEBUG("{}-{}服务下线节点,进行删除管理",service_name,host);
        }
    private:
        std::string getServiceName(std::string service_instance)
        {
            auto pos = service_instance.find_last_of('/');
            if(pos == std::string::npos) return service_instance;
            return service_instance.substr(0,pos);
        }
    private:
        std::mutex _mutex;
        std::unordered_set<std::string> _follow_services;
        std::unordered_map<std::string , ServiceChannel::ptr> _services;
};