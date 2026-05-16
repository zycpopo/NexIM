#include "../common/icsearch.hpp"
#include <gflags/gflags.h>

DEFINE_bool (run_mode,false,"程序运行模式,false为调试模式,true为发布模式");
DEFINE_string(log_file,"","发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level,0,"发布模式下,用于指定日志的输出级别,0为trace,1为debug,2为info,3为warn,4为error,5为critical");

int main(int argc,char *argv[]) 
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode,FLAGS_log_file, FLAGS_log_level);

    ESIndex index("user","_doc");
    return 0;
}