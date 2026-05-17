#include <iostream>
#include <cpr/cpr.h>
#include <json/json.h>
#include <elasticlient/client.h>
#include <memory>
#include "logger.hpp"

bool Serialize(const Json::Value &val,std::string &dst)
{
    //定义Json::StreamWriter 工厂类
    Json::StreamWriterBuilder swb;
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

    std::stringstream ss;
    int ret =sw ->write(val, &ss);
    if(ret != 0)
    {
        std:: cout << "Json序列化失败!\n";
        return false;
    }
    dst = ss.str();
    return true;
}

bool UnSerialize(const std::string &src, Json::Value &val)
{
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
    std::string err;
    bool ret = cr->parse(src.c_str(),src.c_str() + src.size(),&val,&err);
    if(ret == false)
    {
        std::cout << "json反序列化失败" << err << std::endl;
        return false; 
    }
    return true;
}

class ESIndex{
    public:
        ESIndex(std::shared_ptr<elasticlient::Client> &client,
            const std::string & name,const std::string &type):
            _name(name),_type(type),_client(client){
            Json::Value analysis;
            Json::Value analyzer;
            Json::Value ik;
            Json::Value tokenizer;
            tokenizer["tokenizer"] = "ik_max_word";
            ik["ik"] = tokenizer;
            analyzer["analyzer"] = ik;
            analysis["analysis"] = analyzer;
            _index["settings"] = analysis;

            
        }
        ESIndex& append(const std::string &key,
            const std::string &type = "text",
            const std::string &analyzer = "ik_max_word",
            bool enabled = true){
                Json::Value fields;
                
                fields["type"] = type;
                fields["analyzer"] = analyzer;
                if (enabled == false) fields["enabled"] = enabled;
                _properties[key] = fields;
                return *this;
        }

        bool create()
        {
            Json::Value mappings;
            mappings["dynamic"] = true;
            mappings["properties"] = _properties;
            _index["mappings"] = mappings;

            std::string body;
            bool ret = Serialize(_index,body);
            if(ret == false){
                LOG_ERROR("索引序列化失败");
                return false;
            }
            LOG_INFO("请求正文:[{}]",body);

            //2.发起搜索请求
            try{
                std::cout << (uint64_t)_client.get() <<std::endl;
                auto rsp = _client->index(_name,_type,"",body);
                //3.打印响应状态码和响应正文
                if(rsp.status_code < 200 || rsp.status_code>=300)
                {
                    LOG_ERROR("创建ES索引{}失败,响应状态码异常{}",_name,rsp.status_code);
                    return false;
                }
                std::cout << rsp.status_code << std::endl;
                std::cout << rsp.text << std::endl;
            }catch(std::exception &e){
                std::cout << "请求失败" << e.what() <<std::endl;
                LOG_ERROR("创建ES索引{}失败：{}",_name,e.what());
                return false;
            }
            LOG_DEBUG("测试索引创建成功！")
            return true;
        }

    private:
        std::string _name;
        std::string _type;
        Json::Value _properties;
        Json::Value _index;
        std::shared_ptr<elasticlient::Client> _client;
};