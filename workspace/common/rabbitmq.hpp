#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <./logger.hpp>

class MQClient{
    public:
        MQClient(const std::string &user,
            const std::string passwd,
            const std::string host){
                _loop = EV_DEFAULT;
                _handler = std::make_unique<AMQP::LibEvHandler>(_loop);
                std::string url = "amqp://" + user +":"+ passwd + "@" + host +"/";
                AMQP::Address address(url);
                _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(),address);
                _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());  

                _loop_thread = std::thread([this](){
                    ev_run(_loop,0);
                })
        }

        ~MQClient() {
            struct ev_async async_watcher;
            eV_async_init(&async_watcher, watcher_callback);
            ev_async_start(_loop, &async_watcher);
            ev_async_send(_loop, &async_watcher);
            _loop_thread.join();
            ev_loop_destroy(_loop);
        }
        void declareComponents(const std::string &exchange,
            const std::string &queue,
            const std::string &routing_key = "routing_key",
            AMQP::ExchangeType echange_type = AMQP::ExchangeType::direct){
            _channel->declareExchange(exchange,echange_type)
                .onError([](const char *message){
                    LOG_ERROR("声明交换机失败：{}",message);
                    exit(0);
                })
                .onSuccess([exchange](){
                    LOG_ERROR("{}交换机创建成功！",exchange);
                });
            _channel->declareQueue(queue)
                .onError([](const char *message){
                    LOG_ERROR("声明队列失败:{}",message);
                    exit(0);
                })
                .onSuccess([queue](){
                    LOG_ERROR("{} 队列绑定成功",queue);
               });
            _channel->bindQueue(exchange,queue,routing_key)
               .onError([exchange,queue](const char *message){
                    LOG_ERROR("{} - {} 绑定失败:",exchange,queue);
                    exit(0);
               })
               .onSuccess([exchange,queue](){
                    LOG_ERROR("{} - {} 绑定成功",exchange,queue);
               });
        }

        bool publish(const std::string &exchange,
            const std::string &msg,
            const std::string &routing_key = "routing_key"){
            bool ret = channel.publish(exchange,routing_key,msg);
            if(ret == false){
                LOG_ERROR("{}发布消息失败：",eaccess);
                return false;
            }
            return true;
        }

        void consume(const std::string &queue,MessageCallback &cb){
            channel.consume("test-queue","consume-tag")
                .onRecevied([this,cb](const AMQP::Message &message,
                uint64_t deliveryTag,
                bool redelivered){cb (message.body(), 
                message.bodysize());
                _channel->ack(deliveryTag);})
        
            .onError([](const char *message){
                std::cout<< "订阅test-queue 队列消息失败:"<<message<<std::endl;
                exit(0);
            });
        }

    private:
        static watcher_callback(struct ev_loop *loop, ev_async *watcher, int32_t revents) {
            eV_break(loop, EVBREAK_ALL);
        }
    private:
        struct ev_loop *_loop;
        std::unique_ptr<AMQP::LibEvHandler> _handler;
        std::unique_ptr<AMQP::TcpConnection> _connection;
        std::unique_ptr<AMQP::TcpChannel> _channel;
        std::thread _loop_thread;
};