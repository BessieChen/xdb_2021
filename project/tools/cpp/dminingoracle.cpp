/*
 *  Program Name: dminingoracle.cpp, this program is a common functionality module of the data center, used for extracting data from Oracle database source tables and generating XML files.
 *  Author: bc.
 */
#include "_public.h"
#include "_ooci.h"
using namespace idc;

// Structure for program runtime parameters.
struct st_arg
{
    char connstr[101];       // Connection parameters for the database.
    char charset[51];        // Character set of the database.
    char selectsql[1024];    // SQL statement for extracting data from the source database.
    char fieldstr[501];      // Field names of the SQL statement's result set, separated by commas.
    char fieldlen[501];      // Lengths of the fields in the SQL statement's result set, separated by commas.
    char bfilename[31];      // Prefix of the output XML file.
    char efilename[31];      // Suffix of the output XML file.
    char outpath[256];       // Directory where the output XML file is stored.
    int  maxcount;           // Maximum number of records in the output XML file, 0 means no limit. XML files will be used for import; if the file is too large, the database will generate large transactions.
    char starttime[52];      // Time interval for the program to run.
    char incfield[31];       // Incremental field name.
    char incfilename[256];   // File storing the maximum value of the incremental field of the already extracted data.
    char connstr1[101];      // Connection parameters for the database storing the maximum value of the incremental field.
    int  timeout;            // Timeout for process heartbeat.
    char pname[51];          // Process name, recommended to use "dminingoracle_suffix" format.
} starg;

// Parse XML to the starg structure parameters.
bool _xmltoarg(const char *strxmlbuffer);

ccmdstr fieldname;         // Array of result set field names.
ccmdstr fieldlen;          // Array of result set field lengths.

connection conn;           // Source database connection.
clogfile logfile;

// Function to handle exit and signals 2, 15.
void EXIT(int sig);

void _help();

// Check if the current time is within the program's runtime interval.
bool instarttime();

// Main function for data extraction.
bool _dminingoracle();

// 1) Get the maximum value of the incremental field for the last extracted data from a file or database;
// 2) Bind the input variable (maximum value of the incremental field for the last extracted data);
// 3) When retrieving the result set, save the maximum value of the incremental field in the global variable imaxincvalue;
// 4) After extracting the data, save the maximum value of the incremental field in the global variable imaxincvalue to a file or database.
long imaxincvalue;          // Maximum value of the incremental field.
int  incfieldpos=-1;        // Position of the incremental field in the result set array.
bool readincfield();        // Load the maximum value of the last extracted data from a database table or the starg.incfilename file.
bool writeincfield();       // Write the maximum value of the already extracted data to a database table or the starg.incfilename file.

cpactive pactive;           // Process heartbeat.

int main(int argc,char *argv[])
{
    if (argc!=3) { _help(); return -1; }

    // Close all signals and input/output.
    // Set signals, "kill + process number" can be used to normally terminate the process in shell status.
    // But do not use "kill -9 + process number" to forcibly terminate.
    //closeioandsignal(true); 
    signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

    // Open log file.
    if (logfile.open(argv[1])==false)
    {
      printf("Failed to open log file (%s).\n",argv[1]); return -1;
    }

    // Parse XML to get program runtime parameters.
    if (_xmltoarg(argv[2])==false) EXIT(-1);

    // Check if the current time is within the program's runtime interval.
    if (instarttime()==false) return 0;

    pactive.addpinfo(starg.timeout,starg.pname);  // Write process heartbeat information to shared memory.

    // Connect to the source database.
    if (conn.connecttodb(starg.connstr,starg.charset)!=0)
    {
        logfile.write("Failed to connect to database (%s).\n%s\n",starg.connstr,conn.message()); EXIT(-1);
    }
    logfile.write("Connected to database (%s) ok.\n",starg.connstr);

    // Get the maximum value of the already extracted data from a database table or the starg.incfilename file.
    if (readincfield()==false) EXIT(-1);

    _dminingoracle();     // Main function for data extraction.

    return 0;
}

// Main function for data extraction.
bool _dminingoracle()
{
    // 1) Prepare the SQL statement for data extraction.
    sqlstatement stmt(&conn);
    stmt.prepare(starg.selectsql);

    // 2) Bind the variables for the result set.
    string strfieldvalue[fieldname.size()];
    for (int ii=1;ii<=fieldname.size();ii++)
    {
        stmt.bindout(ii,strfieldvalue[ii-1],stoi(fieldlen[ii-1]));
    }

    // If it is incremental extraction, bind the input parameter (maximum value of the incremental field for the last extracted data).
    if (strlen(starg.incfield)!=0) stmt.bindin(1,imaxincvalue);

    // 3) Execute the SQL statement for data extraction.
    if (stmt.execute()!=0)
    {
        logfile.write("Failed to execute stmt.\n%s\n%s\n",stmt.sql(),stmt.message()); return false;
    }

    pactive.uptatime();       // Update process heartbeat.

    string strxmlfilename;  // Output XML file name, e.g., ZHOBTCODE_20230419162835_togxpt_1.xml
    int iseq=1;                    // Sequence number of the output XML file.
    cofile ofile;                  // Used to write data into the XML file.

    // 4) Get records from the result set and write to the XML file.
    while (true)
    {
        if (stmt.next()!=0) break;         // Get a row from the result set.

        if (ofile.isopen()==false)
        {
            // If the XML file is not open, open the XML file.
            sformat(strxmlfilename,"%s/%s_%s_%s_%d.xml",\
                  starg.outpath,starg.bfilename,ltime1("yyyymmddhh24miss").c_str(),starg.efilename,iseq++);
            if (ofile.open(strxmlfilename)==false)
            {
                logfile.write("Failed to open ofile (%s).\n",strxmlfilename.c_str()); return false;
            }
            ofile.writeline("<data>\n");          // Write the beginning tag of the dataset.

        }  

        // Write each field value of the result set into the XML file.
        for (int ii=1;ii<=fieldname.size();ii++)
            ofile.writeline("<%s>%s</%s>",fieldname[ii-1].c_str(),strfieldvalue[ii-1].c_str(),fieldname[ii-1].c_str());

        ofile.writeline("<endl/>\n");    // Write the end mark for each row.

        // If the number of records reaches starg.maxcount, close the current file.
        if ( (starg.maxcount>0) && (stmt.rpc()%starg.maxcount==0) )
        {
            ofile.writeline("</data>\n");   // Write the file ending tag.

            if (ofile.closeandrename()==false)  // Close the file and rename the temporary file to the formal file name.
            {
                logfile.write("Failed to closeandrename ofile (%s).\n",strxmlfilename.c_str()); return false;
            }

            logfile.write("Generated file %s (%d).\n",strxmlfilename.c_str(),starg.maxcount);     

            pactive.uptatime();    // Update process heartbeat. 
        }

        // Update the maximum value of the incremental field.
        if ( (strlen(starg.incfield)!=0) && (imaxincvalue<stol(strfieldvalue[incfieldpos])) )
           imaxincvalue=stol(strfieldvalue[incfieldpos]);
    }

    // 5) If maxcount==0 or the number of records written to the XML file is less than maxcount, close the file.
    if (ofile.isopen()==true)
    {
        ofile.writeline("</data>\n");        // Write the ending tag of the dataset.
        if (ofile.closeandrename()==false)
        {
            logfile.write("Failed to closeandrename ofile (%s).\n",strxmlfilename.c_str()); return false;
        }

        if (starg.maxcount==0)
            logfile.write("Generated file %s (%d).\n",strxmlfilename.c_str(),stmt.rpc());
        else
            logfile.write("Generated file %s (%d).\n",strxmlfilename.c_str(),stmt.rpc()%starg.maxcount);
    }

    // Write the maximum value of the already extracted data to a database table or the starg.incfilename file.
    if (stmt.rpc()>0) writeincfield(); 

    return true;
}

// Check if the current time is within the program's operational time range.
bool instarttime()
{
    // The time range during which the program runs, for example, 02,13 means: if the program starts at 02 or 13 o'clock, it will run, otherwise, it will not run.
    if (strlen(starg.starttime) != 0)
    {
        string strhh24 = ltime1("hh24");  // Get the current hour of the time. If the current time is 2023-01-08 12:35:40, it will return 12.
        if (strstr(starg.starttime, strhh24.c_str()) == 0) return false;
    }       // Idle time: 12-14 o'clock and 00-06 o'clock.

    return true;
}

void EXIT(int sig)
{
    logfile.write("Program exiting, sig=%d\n\n", sig);

    exit(0);
}

// The help document for this program.
void _help()
{
    printf("Using:/project/tools/bin/dminingoracle logfilename xmlbuffer\n\n");

    printf("Sample:/project/tools/bin/procctl 3600 /project/tools/bin/dminingoracle /log/idc/dminingoracle_ZHOBTCODE.log "
              "\"<connstr>idc/idcpwd@snorcl11g_128</connstr><charset>Simplified Chinese_China.AL32UTF8</charset>"
              "<selectsql>select obtid,cityname,provname,lat,lon,height from T_ZHOBTCODE where obtid like '5%%%%'</selectsql>"
              "<fieldstr>obtid,cityname,provname,lat,lon,height</fieldstr><fieldlen>5,30,30,10,10,10</fieldlen>"
              "<bfilename>ZHOBTCODE</bfilename><efilename>togxpt</efilename><outpath>/idcdata/dmindata</outpath>"
              "<timeout>30</timeout><pname>dminingoracle_ZHOBTCODE</pname>\"\n\n");
    printf("       /project/tools/bin/procctl   30 /project/tools/bin/dminingoracle /log/idc/dminingoracle_ZHOBTMIND.log "
              "\"<connstr>idc/idcpwd@snorcl11g_128</connstr><charset>Simplified Chinese_China.AL32UTF8</charset>"
              "<selectsql>select obtid,to_char(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis,keyid from T_ZHOBTMIND where keyid>:1 and obtid like '5%%%%'</selectsql>"
              "<fieldstr>obtid,ddatetime,t,p,u,wd,wf,r,vis,keyid</fieldstr><fieldlen>5,19,8,8,8,8,8,8,8,15</fieldlen>"
              "<bfilename>ZHOBTMIND</bfilename><efilename>togxpt</efilename><outpath>/idcdata/dmindata</outpath>"
              "<starttime></starttime><incfield>keyid</incfield>"
              "<incfilename>/idcdata/dmining/dminingoracle_ZHOBTMIND_togxpt.keyid</incfilename>"
              "<timeout>30</timeout><pname>dminingoracle_ZHOBTMIND_togxpt</pname>"
              "<maxcount>1000</maxcount><connstr1>scott/tiger@snorcl11g_128</connstr1>\"\n\n");

    printf("This program is a common functional module of the shared platform, used to extract data from Oracle database source tables and generate XML files.\n");
    printf("logfilename The log file for this program's operation.\n");
    printf("xmlbuffer   The parameters for this program's operation, represented in XML, as follows:\n\n");

    printf("connstr     Connection parameters for the data source database, format: username/passwd@tnsname.\n");
    printf("charset     Character set of the database, this parameter must be consistent with the data source database, otherwise, there will be issues with Chinese character encoding.\n");
    printf("selectsql   SQL statement to extract data from the data source database. If it is incremental extraction, be sure to use an increasing field as the query condition, such as where keyid>:1.\n");
    printf("fieldstr    List of field names in the result set of the SQL statement for data extraction, separated by commas, which will serve as field names in the XML file.\n");
    printf("fieldlen    A list of lengths for the fields in the result set output by the SQL statement for data extraction, separated by commas. The fields in fieldstr and fieldlen must correspond one-to-one.\n");
    printf("outpath     Directory for storing output XML files.\n");
    printf("bfilename   Prefix for the output XML files.\n");
    printf("efilename   Suffix for the output XML files.\n"); 
    printf("maxcount    Maximum record count for the output XML files, default is 0, meaning no limit. If this parameter is 0, consider increasing the timeout value to prevent program timeout.\n");
    printf("starttime   Time interval for running the program, for example, 02,13 means: if the program starts at 02 or 13 o'clock, it runs, otherwise it does not. "\
             "If starttime is empty, it means it's not enabled, and the data extraction task is executed as soon as this program starts. To reduce the pressure on the data source database, "\
             "extract data when there's no requirement for timeliness, generally during the data source database's idle time.\n");
    printf("incfield    Incremental field name, it must be one of the fields in fieldstr, and can only be an integer, usually an auto-increment field. "\
              "If incfield is empty, it means the incremental extraction scheme is not used.");
    printf("incfilename File storing the maximum value of the extracted data's incremental field. If this file is lost, all data will be re-extracted.\n");
    printf("connstr1    Connection parameters for the database storing the maximum value of the extracted data's incremental field. Choose either connstr1 or incfilename, connstr1 is preferred.");
    printf("timeout     Timeout duration for this program, in seconds.\n");
    printf("pname       Process name, use a name that is easy to understand and different from other processes for easier troubleshooting.\n\n\n");
}

// Parse XML to the starg structure.
bool _xmltoarg(const char *strxmlbuffer)
{
    memset(&starg,0,sizeof(struct st_arg));

    getxmlbuffer(strxmlbuffer,"connstr",starg.connstr,100);       // Connection parameters for the data source database.
    if (strlen(starg.connstr)==0) { logfile.write("connstr is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"charset",starg.charset,50);         // Character set of the database.
    if (strlen(starg.charset)==0) { logfile.write("charset is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"selectsql",starg.selectsql,1000);  // SQL statement for extracting data from the data source database.
    if (strlen(starg.selectsql)==0) { logfile.write("selectsql is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"fieldstr",starg.fieldstr,500);          // List of field names in the result set.
    if (strlen(starg.fieldstr)==0) { logfile.write("fieldstr is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"fieldlen",starg.fieldlen,500);         // List of field lengths in the result set.
    if (strlen(starg.fieldlen)==0) { logfile.write("fieldlen is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"bfilename",starg.bfilename,30);   // Directory for storing output XML files.
    if (strlen(starg.bfilename)==0) { logfile.write("bfilename is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"efilename",starg.efilename,30);    // Prefix for the output XML files.
    if (strlen(starg.efilename)==0) { logfile.write("efilename is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"outpath",starg.outpath,255);        // Suffix for the output XML files.
    if (strlen(starg.outpath)==0) { logfile.write("outpath is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"maxcount",starg.maxcount);       // Maximum record count for output XML files, optional parameter.

    getxmlbuffer(strxmlbuffer,"starttime",starg.starttime,50);     // Time interval for running the program, optional parameter.

    getxmlbuffer(strxmlbuffer,"incfield",starg.incfield,30);          // Incremental field name, optional parameter.

    getxmlbuffer(strxmlbuffer,"incfilename",starg.incfilename,255);  // File storing the maximum value of the extracted data's incremental field, optional parameter.

    getxmlbuffer(strxmlbuffer,"connstr1",starg.connstr1,100);          // Connection parameters for the database storing the maximum value of the extracted data's incremental field, optional parameter.

    getxmlbuffer(strxmlbuffer,"timeout",starg.timeout);       // Timeout duration for process heartbeat.
    if (starg.timeout==0) { logfile.write("timeout is null.\n");  return false; }

    getxmlbuffer(strxmlbuffer,"pname",starg.pname,50);     // Process name.
    if (strlen(starg.pname)==0) { logfile.write("pname is null.\n");  return false; }

    // Split starg.fieldstr into fieldname array.
    fieldname.splittocmd(starg.fieldstr,",");

    // Split starg.fieldlen into fieldlen array.
    fieldlen.splittocmd(starg.fieldlen,",");

    // Check if the sizes of fieldname and fieldlen arrays are equal.
    if (fieldlen.size()!=fieldname.size())
    {
        logfile.write("The number of elements in fieldstr and fieldlen does not match.\n"); return false;
    }

    // For incremental extraction, choose either incfilename or connstr1.
    if (strlen(starg.incfield)>0)
    {
        if ( (strlen(starg.incfilename)==0) && (strlen(starg.connstr1)==0) )
        {
            logfile.write("For incremental extraction, choose either incfilename or connstr1, both cannot be empty.\n"); return false;
        }
    }

    return true;
}

// Load the maximum value of the last extracted data from the database table or starg.incfilename file.
bool readincfield()
{
    imaxincvalue=0;    // Initialize the maximum value of the incremental field.

    // If starg.incfield is empty, it is not incremental extraction.
    if (strlen(starg.incfield)==0) return true;

    // Find the position of the incremental field in the result set.
    for (int ii=0;ii<fieldname.size();ii++)
        if (fieldname[ii]==starg.incfield) { incfieldpos=ii; break; }

    if (incfieldpos==-1)
    {
        logfile.write("The incremental field name %s is not in the list %s.\n",starg.incfield,starg.fieldstr); return false;
    }

    if (strlen(starg.connstr1)!=0)
    {
        // Load the maximum value of the incremental field from the database table.
        connection conn1;
        if (conn1.connecttodb(starg.connstr1,starg.charset)!=0)
        {
            logfile.write("connect database(%s) failed.\n%s\n",starg.connstr1,conn1.message()); return false;
        }
        sqlstatement stmt(&conn1);
        stmt.prepare("select maxincvalue from T_MAXINCVALUE where pname=:1");
        stmt.bindin(1,starg.pname);
        stmt.bindout(1,imaxincvalue);
        stmt.execute();
        stmt.next();
    }
    else
    {
        // Load the maximum value of the incremental field from a file.
        cifile ifile; 

        // If opening starg.incfilename file fails, it indicates the program is running for the first time, and it's not necessary to return failure.
        // It could also mean the file is lost, in which case there's no choice but to re-extract.
        if (ifile.open(starg.incfilename)==false) return true;

        // Read the maximum value of the already extracted data from the file.
        string strtemp;
        ifile.readline(strtemp);
        imaxincvalue=stol(strtemp);
    }

    logfile.write("The position of the last extracted data (%s=%ld).\n",starg.incfield,imaxincvalue);

    return true;
}

// Write the maximum value of the extracted data into the database table or starg.incfilename file.
bool writeincfield()
{
    // If starg.incfield is empty, it is not incremental extraction.
    if (strlen(starg.incfield)==0) return true;

     if (strlen(starg.connstr1)!=0)
    {
        // Write the maximum value of the incremental field into a table in the database.
        connection conn1;
        if (conn1.connecttodb(starg.connstr1,starg.charset)!=0)
        {
            logfile.write("connect database(%s) failed.\n%s\n",starg.connstr1,conn1.message()); return false;
        }
        sqlstatement stmt(&conn1);
        stmt.prepare("update T_MAXINCVALUE set maxincvalue=:1 where pname=:2");
        stmt.bindin(1,imaxincvalue);
        stmt.bindin(2,starg.pname);
        iCertainly! Here's the translation of the provided code snippet while maintaining its format:

    if (stmt.execute() != 0)
    {
        if (stmt.rc() == 942)  // If the table does not exist, stmt.execute() will return the ORA-00942 error.
        {
            // If the table does not exist, create the table and then insert records.
            conn1.execute("create table T_MAXINCVALUE(pname varchar2(50), maxincvalue number(15), primary key(pname))");
            conn1.execute("insert into T_MAXINCVALUE values('%s', %ld)", starg.pname, imaxincvalue);
            conn1.commit();
            return true;
        }
        else
        {
            logfile.write("stmt.execute() failed.\n%s\n%s\n", stmt.sql(), stmt.message()); return false;
        }
    }
    else
    {
        if (stmt.rpc() == 0)
        {
            // If the record does not exist, insert a new record.
            conn1.execute("insert into T_MAXINCVALUE values('%s', %ld)", starg.pname, imaxincvalue);
        }
        conn1.commit();
    }
    }
    else
    {
        // Write the maximum value of the incremental field to a file.
        cofile ofile;
    
        if (ofile.open(starg.incfilename, false) == false)
        {
            logfile.write("ofile.open(%s) failed.\n", starg.incfilename); return false;
        }
    
        // Write the maximum ID of the extracted data to the file.
        ofile.writeline("%ld", imaxincvalue);
    }
    
    return true;
}
