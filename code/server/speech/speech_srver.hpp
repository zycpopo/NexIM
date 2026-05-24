//语音识别子服务
#include <brpc/server.h>
#include <butil/logging.h>

#include "asr.hpp"
#include "etcd.hpp"
#include "logger.hpp"
#include "speech.pb.h"

namespace popo{
class SpeechServiceImpl: public popo::SpeechService{
    public:
        SpeechServiceImpl(const ASRClient::ptr &asr_client):
            _asr_client(asr_client){}
        ~SpeechServiceImpl(){}

        void SpeechRecognition(google::protobuf::RpcController* controller,
                    const  ::popo::SpeechRecognitionReq* request,
                    ::popo::SpeechRecognitionRsp* response,
                    ::google::protobuf::Closure* done){
            brpc::CloosureGuard rpc_guard(done);
            
            //1.取出请求中的语音数据
            //2.调用语音SDK模块进行语音识别，得到响应
            std::string err;
            std::string res = _asr_client->recognize(requst->speech_content(),err);
            if(res.empty()){
                LOG_ERROR("{}语音识别失败！",request->request_id());
                response->set_request_id(request->request_id());
                response->set_success(false);
                response->set_errmsg("语音识别失败"+err);
                return ;
            }
            //3.组织响应
            response->set_request_id(request->request_id());
            response->set_success(true);
            response->set->recognition_result(res);
        }
    private:
        ASRClient::ptr _asr_client;
};

class SpeechServer{
    public:
    private:
};
}