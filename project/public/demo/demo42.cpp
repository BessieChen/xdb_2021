/*
 *  程序名：demo42.cpp，此程序演示采用开发框架的clogfile类记录程序的运行日志。
 *  作者：吴从周
*/
#include "../_public.h"
using namespace std;
using namespace idc;

clogfile logfile;      // 创建日志对象。
void func(){
    for (int ii=0;ii<5000000;ii++)
    {
        logfile.write("这是第%d个%s...ok.\n",ii,"超级女生");
        //logfile.write("第%d个超女开始表演...",ii);     // 表演前，写一行日志，...表示正在表演中。
        //sleep(2);                                                          // 超女在表演中。
        //logfile << "ok.\n";                                          // 表演完成后，写入ok。
    }
}

int main()
{
    // 打开日志文件。
    if (logfile.open("/tmp/log/demo42.log",ios::out,false)==false)
    //if (logfile.open("/tmp/log/demo42.log")==false)
    {
        printf("logfile.open(/tmp/log/demo42.log) failed.\n");  return -1;
    }

    logfile.write("程序开始运行。\n");
    thread t1(func);
    thread t2(func);
    t1.join();
    t2.join();
    logfile.write("程序运行结束。\n");
}