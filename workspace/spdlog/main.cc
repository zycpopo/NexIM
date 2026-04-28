#include "logger.hpp"
#include <gflags/gflags.h>  

DEFINE_bool (run_mode,false,"程序运行模式,false为调试模式,true为发布模式");
DEFINE_string(log_file,"","发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level,0,"发布模式下，用于指定日志的输出级别，0为trace，1为debug，2为info，3为warn，4为error，5为critical");

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode,FLAGS_log_file, FLAGS_log_level);

    LOG_DEBUG("你好：{}", "popo");
    LOG_INFO("这是一个{}[info]级别的日志", "popo");
    LOG_WARN("这是一个{}[warn]级别的日志", "popo");     
    LOG_ERROR("这是一个{}[error]级别的日志", "popo");
    LOG_FATAL("这是一个{}[fatal]级别的日志", "popo");   

    return 0;
}