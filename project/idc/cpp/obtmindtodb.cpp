/*
 *  obtmindtodb.cpp, this program is used to import meteorological observation data files into the T_ZHOBTMIND table, supporting both xml and csv file formats.
 *  Author: bc.
 */
//#include "_public.h"  // Development framework header file.
//#include "_ooci.h"    // Header file for operating Oracle.
#include "idcapp.h"    
using namespace idc;

clogfile logfile;        // Log file.
connection conn;    // Database connection.
cpactive pactive;     // Process heartbeat.

// Main function for business processing.
bool _obtmindtodb(const char *pathname,const char *connstr,const char *charset);

void EXIT(int sig);     // Signal handling function for program exit.

int main(int argc,char *argv[])
{
    // Help documentation.
    if (argc!=5)
    {
        printf("\n");
        printf("Using:./obtmindtodb pathname connstr charset logfile\n");

        printf("Example:/project/tools/bin/procctl 10 /project/idc/bin/obtmindtodb /idcdata/surfdata "\
                  "\"idc/idcpwd@snorcl11g_128\" \"Simplified Chinese_China.AL32UTF8\" /log/idc/obtmindtodb.log\n\n");

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

    pactive.addpinfo(30,"obtmindtodb");   // Process heartbeat.

    // Main function for business processing.
    _obtmindtodb(argv[1],argv[2],argv[3]);

    return 0;
}

void EXIT(int sig)
{
    logfile.write("Program exit, sig=%d\n\n",sig);

    // No need to write this, the transaction will be rolled back and the connection to the database will be disconnected in the destructor.
    conn.rollback();
    conn.disconnect();   

    exit(0);
}

// Main function for business processing.
bool _obtmindtodb(const char *pathname,const char *connstr,const char *charset)
{
    // 1) Open the directory storing meteorological observation data files.
    cdir dir;
    if (dir.opendir(pathname,"*.xml,*.csv")==false)
    {
        logfile.write("dir.opendir(%s) failed.\n",pathname); return false;
    }

    CZHOBTMIND ZHOBTMIND(conn,logfile);  // Object for operating meteorological observation data table.

    // 2) Loop to read each file in the directory.
    while (true)
    {
        // Read a meteorological observation data file (only process *.xml and *.csv).
        if (dir.readdir()==false) break;

        // If there are files to process, check the connection status with the database, if not connected, connect.
        if (conn.isopen()==false)
        {
            if (conn.connecttodb(connstr,charset)!=0)
            {
                logfile.write("Failed to connect to database (%s).\n%s\n",connstr,conn.message()); return false;
            }
    
            logfile.write("Connected to database (%s) ok.\n",connstr);
        }

        // Open the file.
        cifile ifile;
        if (ifile.open(dir.m_ffilename)==false)
        {
            logfile.write("file.open(%s) failed.\n",dir.m_ffilename.c_str()); return false;
        }

        int  totalcount=0;     // Total record count in the file.
        int  insertcount=0;   // Successful insertion count.
        ctimer timer;            // Timer to record the processing time for each data file.
        bool bisxml=matchstr(dir.m_ffilename,"*.xml");  // File format, true-xml; false-csv.

        string strbuffer;      // Stores a line of data read from the file.

        // If it's a csv file, discard the first line.
        if (bisxml==false)  ifile.readline(strbuffer);

        // Read each line from the file and insert it into the database table.
        while(true)
        {
            // Read a line from the file.
            if (bisxml==true)
            {
                if (ifile.readline(strbuffer,"<endl/>")==false) break;     // The line ending mark for xml files is <endl/>.
            }
            else
            {
                if (ifile.readline(strbuffer)==false) break;                      // Csv files do not have a line ending mark. 
            }

            totalcount++;       // Increment total record count by 1.

            // Parse the content of the line (methods differ for *.xml and *.csv), store the data in the structure.
            ZHOBTMIND.splitbuffer(strbuffer,bisxml);

            // Insert the parsed data into the database (into the table).
            if (ZHOBTMIND.inserttable()==true) insertcount++;       // Increment successful insertion count by 1.
        }

        // Close and delete the processed file, commit the transaction.
        ifile.closeandremove();
        conn.commit();
        logfile.write("Processed file %s (totalcount=%d,insertcount=%d), taking %.2f seconds.\n",\
                              dir.m_ffilename.c_str(),totalcount,insertcount,timer.elapsed());
        pactive.uptatime();   // Process heartbeat.
    }

    return true;
}
