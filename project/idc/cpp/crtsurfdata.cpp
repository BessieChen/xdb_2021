/*
 *  程序名：crtsurfdata.cpp  本程序用于生成气象站点观测的分钟数据。
 *  作者：bc。
*/
#include "_public.h"
using namespace idc;

// 省   站号  站名 纬度   经度  海拔高度
// 安徽,58015,砀山,34.27,116.2,44.2
struct st_stcode    // 站点参数的结构体。
{
    char provname[31];     // 省。    string  操作方便、自动扩容  操作数据库时，string没有优势。
    char obtid[11];             // 站号。
    char obtname[31];       // 站名，同城市名。
    double lat;                   // 纬度：度。
    double lon;                  // 经度：度。
    double height;            // 海拔高度：米。
};
list<struct st_stcode> stlist;        // 存放全部的站点参数。
bool loadstcode(const string &inifile);   // 把站点参数文件加载到stlist容器中。

// 气象站观测数据的结构体
struct st_surfdata
{
    char obtid[11];          // 站点代码。
    char ddatetime[15];  // 数据时间：格式yyyymmddhh24miss，精确到分钟，秒固定填00。
    int  t;                         // 气温：单位，0.1摄氏度。
    int  p;                        // 气压：0.1百帕。
    int  u;                        // 相对湿度，0-100之间的值。
    int  wd;                     // 风向，0-360之间的值。
    int  wf;                      // 风速：单位0.1m/s
    int  r;                        // 降雨量：0.1mm。
    int  vis;                     // 能见度：0.1米。
};
list<struct st_surfdata>  datalist;           // 存放观测数据的容器。
void crtsurfdata();                                  // 根据stlist容器中的站点参数，生成站点观测数据，存放在datalist中。

char strddatetime[15];

clogfile logfile;          // 本程序运行的日志。

void EXIT(int sig);      // 程序退出和信号2、15的处理函数。

int main(int argc,char *argv[])
{
    // 站点参数文件  生成的测试数据存放的目录 本程序运行的日志
    if (argc!=4)
    {
        // 如果参数非法，给出帮助文档。
        cout << "Using:./crtsurfdata inifile outpath logfile\n";
        cout << "Examples:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log\n\n";

        cout << "inifile  气象站点参数文件名。\n";
        cout << "outpath  气象站点数据文件存放的目录。\n";
        cout << "logfile  本程序运行的日志文件名。\n";

        return -1;  
    }

    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
    // 但请不要用 "kill -9 +进程号" 强行终止。
    closeioandsignal(true);       // 关闭0、1、2和忽略全部的信号。
    signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

    if (logfile.open(argv[3])==false)
    {
        cout << "logfile.open(" << argv[3] << ") failed.\n";  return -1;
    }

    logfile.write("crtsurfdata 开始运行。\n");

    // 在这里编写处理业务的代码。
    // 1）从站点参数文件中加载站点参数，存放于stlist容器中；
    if (loadstcode(argv[1]) ==  false) EXIT(-1);   

    // 获取观测数据的时间。
    memset(strddatetime,0,sizeof(strddatetime));
    ltime(strddatetime,"yyyymmddhh24miss");   // 获取系统当前时间。
    strncpy(strddatetime+12,"00",2);                   // 把数据时间中的秒固定填00。

    // 2）根据stlist容器中的站点参数，生成站点观测数据（随机数），生成后的数据存放于容器中；
    crtsurfdata();


    // 3）把站点观测数据存保到文件中。


    logfile.write("crtsurfdata 运行结束。\n");

    return 0;
}

// 程序退出和信号2、15的处理函数。
void EXIT(int sig)
{
    logfile.write("程序退出，sig=%d\n\n",sig);

    exit(0);
}

// 把站点参数文件加载到stlist容器中。
bool loadstcode(const string &inifile)   
{
    cifile ifile;       // 读取文件的对象。
    if (ifile.open(inifile)==false) 
    {
        logfile.write("ifile.open(%s) failed.\n",inifile.c_str()); return false;
    }

    string strbuffer;        // 存放从文件中读取的每一行。

    ifile.readline(strbuffer);         // 读取站点参数文件的第一行，它是标题，扔掉。

    ccmdstr cmdstr;                    // 用于拆分从文件中读取的行。
    st_stcode stcode;                  // 站点参数的结构体。

    while(ifile.readline(strbuffer))
    {
        // logfile.write("strbuffer=%s\n",strbuffer.c_str());

        // 拆分从文件中读取的行，例如：安徽,58015,砀山,34.27,116.2,44.2
        cmdstr.splittocmd(strbuffer,",");         // 拆分字符串。

        memset(&stcode,0,sizeof(st_stcode));

        cmdstr.getvalue(0,stcode.provname,30);   // 省
        cmdstr.getvalue(1,stcode.obtid,10);           // 站点代码
        cmdstr.getvalue(2,stcode.obtname,30);     // 站名
        cmdstr.getvalue(3,stcode.lat);                    // 纬度
        cmdstr.getvalue(4,stcode.lon);                   // 经度
        cmdstr.getvalue(5,stcode.height);              // 海拔高度

        stlist.push_back(stcode);                            // 把站点参数存入stlist容器中。
    }

    // 这里不需要手工关闭文件，cifile类的析构函数会关闭文件。

    // 把容器中全部的数据写入日志。
    //for (auto &aa:stlist)
    //{
    //    logfile.write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
    //                         aa.provname,aa.obtid,aa.obtname,aa.lat,aa.lon,aa.height);
    //}

    return true;
}

// 模拟生成气象分钟观测数据，存放在datalist容器中。
void crtsurfdata()
{
    srand(time(0));              // 播随机数种子。

    st_surfdata stsurfdata;   // 观测数据的结构体。

    // 遍历气象站点参数的stlist容器，生成每个站点的观测数据。
    for (auto &aa : stlist)
    {
        memset(&stsurfdata,0,sizeof(st_surfdata));

        // 填充观测数据的结构体的成员。
        strcpy(stsurfdata.obtid,aa.obtid);                        // 站点代码。
        strcpy(stsurfdata.ddatetime,strddatetime);        // 数据时间。
        stsurfdata.t=rand()%350;                                    // 气温：单位，0.1摄氏度。   0-350之间。 可犯可不犯的错误不要犯。
        stsurfdata.p=rand()%265+10000;                       // 气压：0.1百帕
        stsurfdata.u=rand()%101;                                    // 相对湿度， 0-100之间的值。
        stsurfdata.wd=rand()%360;                                 // 风向，0-360之间的值。
        stsurfdata.wf=rand()%150;                                  // 风速：单位0.1m/s。
        stsurfdata.r=rand()%16;                                      // 降雨>量：0.1mm。
        stsurfdata.vis=rand()%5001+100000;                 // 能见度：0.1米。

        datalist.push_back(stsurfdata);                            // 把观测数据的结构体放入datalist容器。
    }

    for (auto &aa:datalist)
    {
        logfile.write("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n", \
                              aa.obtid,aa.ddatetime,aa.t/10.0,aa.p/10.0,aa.u,aa.wd,aa.wf/10.0,aa.r/10.0,aa.vis/10.0);
    }
}