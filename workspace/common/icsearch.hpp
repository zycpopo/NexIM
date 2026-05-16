#include <iostream>
#include <cpr/cpr.h>
#include <json/json.h>
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
        ESIndex(const std::string ,const std::string &type)
        {
            Json::Value analysis;
            Json::Value analyzer;
            Json::Value ik;
            Json::Value tokenizer;
            tokenizer["tokenizer"] = "ik_max_word";
            ik["ik"] = tokenizer;
            analyzer["analyzer"] = ik;
            analysis["analyzer"] = analyzer;
            _index["settings"] = analysis;

            Json::Value mappings;
            mappings["dynamic"] = true;

            std::string str;
            Serialize(_index,str);
            std::cout << str << std::endl;
        }
        void append(const std::string &key,
            const std::string &type = "test",
            const std::string &ayalyzer = "ik_max_word",
            bool enabled = true){

        }

    private:
        std::string _name;
        std::string _type;
        Json::Value _properties;
        Json::Value _index;
};