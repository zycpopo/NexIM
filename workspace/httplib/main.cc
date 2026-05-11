// 引入 cpp-httplib 头文件（只有头文件的轻量 HTTP 库）
#include "../common/httplib.h"

// 自定义 HTTP 服务类
class HelloServer {
public:
    // 构造函数：传入端口，注册路由
    HelloServer(int port) : _port(port) {
        // 注册 GET 请求：路径 /hi
        // 绑定成员函数 HelloWorld 作为处理函数
        _server.Get("/hi", std::bind(
            &HelloServer::HelloWorld, this,  // 绑定当前对象
            std::placeholders::_1,            // 占位符：Request
            std::placeholders::_2             // 占位符：Response
        ));
    }

    // 启动服务器：开始监听端口
    void run() {
        _server.listen("0.0.0.0", _port);
    }

public:
    // 处理 /hi 请求的回调函数
    void HelloWorld(const httplib::Request &req,   // 请求对象（包含请求头、参数、body）
                    httplib::Response &rsp) {      // 响应对象（设置返回内容）
        std::string body = "<h1>HelloWorld</h1>";  // 响应 HTML 内容
        rsp.set_content(body, "text/html");         // 设置响应内容 + 类型
        rsp.status = 200;                           // HTTP 状态码 200 = 成功
    }

private:
    int _port;                     // 监听端口
    httplib::Server _server;       // httplib 服务器实例
};

// 主函数：程序入口
int main() {
    HelloServer server(8080);  // 创建服务，监听 8080 端口
    server.run();              // 启动服务，阻塞运行
    return 0;
}