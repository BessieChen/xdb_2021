/*
 * obtmindtodb.cpp, this program is used to import meteorological observation data files into the T_ZHOBTMIND table, supporting both xml and csv file formats.
 * Author: bc.
 */
#include "_public.h"  // Header file of the development framework.
#include "_ooci.h"    // Header file for operating Oracle.
using namespace idc;

clogfile logfile;        // Log file.
connection conn;    // Database connection.
cpactive pactive;     // Heartbeat of the process.

struct st_zhobtmind
{
    char obtid[6];            // Station code.
    char ddatetime[21];  // Data time, accurate to the minute.
    char t[11];                 // Temperature, unit: 0.1 Celsius.
    char p[11];                // Pressure, unit: 0.1 hPa.
    char u[11];                // Relative humidity, values between 0-100.
    char wd[11];             // Wind direction, values between 0-360.
    char wf[11];              // Wind speed, unit 0.1m/s.
    char r[11];                // Rainfall, unit 0.1mm.
    char vis[11];             // Visibility, unit 0.1 meter.
} stzhobtmind;

// Main function for business processing.
bool _obtmindtodb(const char *pathname, const char *connstr, const char *charset);

void EXIT(int sig);     // Signal handling function for program exit.

int main(int argc,char *argv[])
{
    // Help document.
    if (argc!=5)
    {
        printf("\n");
        printf("Using:./obtmindtodb pathname connstr charset logfile\n");

        printf("Example:/project/tools/bin/procctl 10 /project/idc/bin/obtmindtodb /idcdata/surfdata "\
                  "\"idc/idcpwd@snorcl11g_211\" \"Simplified Chinese_China.AL32UTF8\" /log/idc/obtmindtodb.log\n\n");

        printf("This program is used to import national meteorological observation data files into the T_ZHOBTMIND table, supporting xml and csv file formats, data is only inserted, not updated.\n");
        printf("pathname Directory where national meteorological observation data files are stored.\n");
        printf("connstr  Database connection parameters: username/password@tnsname\n");
        printf("charset  Database character set.\n");
        printf("logfile  Log file name for this program.\n");
        printf("The program runs every 10 seconds, scheduled by procctl.\n\n\n");

        return -1;
    }

    // Close all signals and input/output.
    // Set signals, "kill + process number" can be used to normally terminate the process in shell status.
    // But please do not use "kill -9 + process number" to forcibly terminate.
    // clostioandsignal(true); 
    signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

    // Open the log file.
    if (logfile.open(argv[4])==false)
    {
        printf("Failed to open log file (%s).\n",argv[4]); return -1;
    }

    // Main function for business processing.
    _obtmindtodb(argv[1],argv[2],argv[3]);

    return 0;
}

void EXIT(int sig)
{
    logfile.write("Program exit, sig=%d\n\n",sig);

    // It is not necessary to write this, as the transaction will be rolled back and the connection to the database will be disconnected in the destructor.
    conn.rollback();
    conn.disconnect();   

    exit(0);
}

// Main function for business processing.
bool _obtmindtodb(const char *pathname,const char *connstr,const char *charset)
{
    // 1) Open the directory where the meteorological observation data files are stored.
    cdir dir;
    //if (dir.opendir(pathname,"*.xml,*.csv")==false)
    if (dir.opendir(pathname,"*.xml")==false)
    {
        logfile.write("dir.opendir(%s) failed.\n",pathname); return false;
    }

    sqlstatement stmt;

    // 2) Use a loop to read each file in the directory.
    while (true)
    {
        // Read a meteorological observation data file (only process *.xml and *.csv).
        if (dir.readdir()==false) break;

        // If there are files to process, check the connection status with the database, if it is not connected, then connect.
        if (conn.isopen()==false)
        {
            if (conn.connecttodb(connstr,charset)!=0)
            {
                logfile.write("Failed to connect to database (%s).\n%s\n",connstr,conn.message()); return false;
            }
    
            logfile.write("Connected to database (%s) ok.\n",connstr);

            // Prepare the sql statement to operate the table, bind input parameters.
            stmt.connect(&conn);
            stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis,keyid) "\
                                   "values(:1,to_date(:2,'yyyymmddhh24miss'),:3,:4,:5,:6,:7,:8,:9,SEQ_ZHOBTMIND.nextval)");
            stmt.bindin(1,stzhobtmind.obtid,5);
            stmt.bindin(2,stzhobtmind.ddatetime,14);
            stmt.bindin(3,stzhobtmind.t,10);
            stmt.bindin(4,stzhobtmind.p,10);
            stmt.bindin(5,stzhobtmind.u,10);
            stmt.bindin(6,stzhobtmind.wd,10);
            stmt.bindin(7,stzhobtmind.wf,10);
            stmt.bindin(8,stzhobtmind.r,10);
            stmt.bindin(9,stzhobtmind.vis,10);
        }

        // Open the file.
        cifile ifile;
        if (ifile.open(dir.m_ffilename)==false)
        {
            logfile.write("file.open(%s) failed.\n",dir.m_ffilename.c_str()); return false;
        }

        int  totalcount=0;     // Total record count in the file.
        int  insertcount=0;   // Successful insertion count.
        ctimer timer;            // Timer, to record the processing time for each data file.

        string strbuffer;      // Store a line of data read from the file. 

        // Read each line of the file and insert it into the table in the database.
        while(true)
        {
            // Read a line from the file.
            if (ifile.readline(strbuffer,"<endl/>")==false) break;

            totalcount++;       // Total record count incremented by 1.

            // Parse the content of the line (*.xml and *.csv are different), store the data in the structure.
            getxmlbuffer(strbuffer,"obtid",stzhobtmind.obtid,5);
            getxmlbuffer(strbuffer,"ddatetime",stzhobtmind.ddatetime,14); 
            char tmp[11];
            getxmlbuffer(strbuffer,"t",tmp,10);     if (strlen(tmp)>0) snprintf(stzhobtmind.t,10,"%d",(int)(atof(tmp)*10));
            getxmlbuffer(strbuffer,"p",tmp,10);    if (strlen(tmp)>0) snprintf(stzhobtmind.p,10,"%d",(int)(atof(tmp)*10));
            getxmlbuffer(strbuffer,"u",stzhobtmind.u,10);
            getxmlbuffer(strbuffer,"wd",stzhobtmind.wd,10);
            getxmlbuffer(strbuffer,"wf",tmp,10);  if (strlen(tmp)>0) snprintf(stzhobtmind.wf,10,"%d",(int)(atof(tmp)*10));
            getxmlbuffer(strbuffer,"r",tmp,10);     if (strlen(tmp)>0) snprintf(stzhobtmind.r,10,"%d",(int)(atof(tmp)*10));
            getxmlbuffer(strbuffer,"vis",tmp,10);  if (strlen(tmp)>0) snprintf(stzhobtmind.vis,10,"%d",(int)(atof(tmp)*10));

            // Insert the parsed data into the database (insert into the table).
            if (stmt.execute()!=0)
            {
                // The main reasons for failure are two: one is duplicate records, the other is illegal data content.
                // If the failure is due to illegal data content, log it and continue; if it's due to duplicate records, no need to log, and continue.
                if (stmt.rc()!=1)
                {
                    logfile.write("strbuffer=%s\n",strbuffer.c_str());
                    logfile.write("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message());
                }
            }
            else
                insertcount++;       // Successful insertion count incremented by 1.
        }

        // Close and delete the processed file, commit the transaction.
        ifile.closeandremove();
        conn.commit();
        logfile.write("Processed file %s (totalcount=%d,insertcount=%d), taking %.2f seconds.\n",\
                              dir.m_ffilename.c_str(),totalcount,insertcount,timer.elapsed());
    }


    return true;
}
