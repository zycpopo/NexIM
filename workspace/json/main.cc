#include <jsoncpp/json/json.h>
#include <memory>
#include <iostream>
#include <sstream>

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

int main()
{
    float score[3] = {18,36,48};

    Json::Value popo;
    popo["姓名"] = "紫洋葱";
    popo["年龄"] = 20;
    popo["成绩"].append(score[0]);
    popo["成绩"].append(score[1]);
    popo["成绩"].append(score[2]);

    std::string popo_str;
    bool ret = Serialize(popo,popo_str);
    if(ret == false) return -1;
    std::cout << popo_str << std::endl;

    Json::Value val;
    ret = UnSerialize(popo_str,val);
    std::cout << val["姓名"].asString() << std::endl;
    std::cout << val["年龄"].asInt() << std::endl;

    int sz = val["成绩"].size();
    for(int i=0;i < sz;i++)
    {
        std::cout << val["成绩"][i].asFloat() << std::endl;
    }
    return 0; 
}