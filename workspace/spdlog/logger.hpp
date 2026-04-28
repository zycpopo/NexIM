#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <iostream>


std::shared_ptr<spdlog::logger> g_default_logger;
//mode -运行模式， true为发布模式，false为调试模式
void init_logger(bool mode, const std::string& file, int level)
{
    if (mode == false){
        g_default_logger = spdlog::stdout_color_mt("default-logger");
        g_default_logger->set_level(spdlog::level::trace);
        g_default_logger->flush_on(spdlog::level::level_enum::trace);
    }else{
        g_default_logger = spdlog::basic_logger_mt("default-logger", file);
        g_default_logger->set_level((spdlog::level::level_enum)level);
        g_default_logger->flush_on((spdlog::level::level_enum)level);
    }
    g_default_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%t] [%-8l] %v");
}

#define LOG_TRACE(format, ...) g_default_logger->trace("[{}:{}]" format, __FILE__,__LINE__, ##__VA_ARGS__);
#define LOG_DEBUG(format, ...) g_default_logger->debug("[{}:{}]" format, __FILE__,__LINE__, ##__VA_ARGS__);
#define LOG_INFO(format, ...) g_default_logger->info("[{}:{}]" format, __FILE__,__LINE__, ##__VA_ARGS__);
#define LOG_WARN(format, ...) g_default_logger->warn("[{}:{}]" format, __FILE__,__LINE__, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) g_default_logger->error("[{}:{}]" format, __FILE__,__LINE__, ##__VA_ARGS__);
#define LOG_FATAL(format, ...) g_default_logger->critical("[{}:{}]" format, __FILE__,__LINE__, ##__VA_ARGS__);