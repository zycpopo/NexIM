#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <iostream>


int main()
{   
    //设置全局刷新策略
    spdlog::flush_every(std::chrono::seconds(1));//每秒刷新
    //设置特定日志级别的刷新策略
    spdlog::flush_on(spdlog::level::level_enum::debug);//当日志级别为debug或更高时刷新
    //设置全局日志输出等级
    spdlog::set_level(spdlog::level::level_enum::debug);//设置全局日志输出等级为debug

    //创建异步日志器
    auto logger = spdlog::stdout_color_mt<spdlog::async_factory>("popo-logger");
    //设置输出格式
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%-8l%$] %v");
    logger->debug("这是一个debug级别的日志");
    logger->info("这是一个info级别的日志");
    logger->warn("这是一个warn级别的日志"); 
    logger->error("这是一个error级别的日志");
    logger->critical("这是一个critical级别的日志");

    std::cout<<"日志输出完成！"<<std::endl;
    return 0;
}