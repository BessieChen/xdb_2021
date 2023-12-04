/*
 *  Program Name: xmltodb.cpp, this program is a common function module of the shared platform, used for importing XML files into Oracle tables.
 *  Author: bc.
 */
#include "_tools.h"
using namespace idc;

// Structure for program runtime parameters.
struct st_arg
{
    char connstr[101];          // Database connection parameters.
    char charset[51];           // Database character set.
    char inifilename[301];      // Configuration file for data import parameters.
    char xmlpath[301];          // Directory where XML files awaiting import are stored.
    char xmlpathbak[301];       // Backup directory for XML files after import.
    char xmlpatherr[301];       // Directory for storing XML files that failed to import.
    int  timetvl;               // Time interval for running the program, the program stays in memory permanently.
    int  timeout;               // Timeout duration when running the program.
    char pname[51];             // Program name when running.
} starg;

void _help(char *argv[]);                                  // Help documentation for the program

bool _xmltoarg(const char *strxmlbuffer);      // Parse XML into the starg structure

clogfile logfile;         // Log for the program's operation.
connection conn;           // Database connection.

void EXIT(int sig);        // Signal handling function for program exit.

bool _xmltodb();           // Main function for business processing.

// Structure for data import parameters.
struct st_xmltotable
{
    char filename[101];      // Matching rules for XML files, separated by commas.
    char tname[31];          // Name of the table to be imported into.
    int  uptbz;              // Update flag: 1 - update; 2 - do not update.
    char execsql[301];       // SQL statement executed before processing XML files.
} stxmltotable;
vector<struct st_xmltotable> vxmltotable;               // Container for data import parameters.
bool loadxmltotable();                                   // Load data import parameters from starg.inifilename into the vxmltotable container.
bool findxmltotable(const string &xmlfilename);         // Find import parameters from the vxmltotable container based on file name and store in stxmltotable structure.

// Sub-function for processing XML files, return value: 0 - success, others - failure, there are many kinds of failure, not yet determined.
ctimer timer;                                          // Time consumed for processing each XML file.
int totalcount, inscount, uptcount;                    // Total, inserted, and updated record count for XML files.
int _xmltodb(const string &fullfilename, const string &filename);

ctcols tcols;                                          // Get all fields and primary key fields of the table.

string strinsertsql;                                   // SQL statement for inserting into the table.
string strupdatesql;                                   // SQL statement for updating the table.
void crtsql();                                         // Construct SQL for inserting and updating data in the table.

// <obtid>58015</obtid><ddatetime>20230508113000</ddatetime><t>141</t><p>10153</p><u>44</u><wd>67</wd><wf>106</wf><r>9</r><vis>102076</vis><keyid>6127135</keyid>
vector<string> vcolvalue;                              // Store field values parsed from each line of XML, to be used for binding variables in SQL statements for inserting and updating.
sqlstatement stmtins, stmtupt;                         // sqlstatement statements for inserting and updating the table.
void preparesql();                                     // Prepare SQL statements for insertion and update, bind input variables.

// Execute it before processing the XML file if stxmltotable.execsql is not empty.
bool execsql();

// Parse XML and store in the already bound input variable array vcolvalue.
void splitbuffer(const string &strBuffer);

// Move the XML file to the backup directory or error directory.
bool xmltobakerr(const string &fullfilename, const string &srcpath, const string &dstpath);

cpactive pactive;                                      // Heartbeat of the process.

int main(int argc, char *argv[])
{
    if (argc != 3) { _help(argv); return -1; }

    // Close all signals and I/O.
    // Set signals, in shell state can use "kill + process number" to normally terminate this process.
    // But do not use "kill -9 + process number" to forcibly terminate.
    // closeioandsignal(true); 
    signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

    if (logfile.open(argv[1]) == false)
    {
        printf("Failed to open log file (%s).\n", argv[1]); return -1;
    }

    // Parse XML into the starg structure
    if (_xmltoarg(argv[2]) == false) return -1;

    pactive.addpinfo(starg.timeout, starg.pname);  // Set the heartbeat of the process.

    _xmltodb();           // Main function for business processing.
}

// Display program help
void _help(char *argv[])
{
    printf("Usage: /project/tools/bin/xmltodb logfilename xmlbuffer\n\n");

    printf("Example: /project/tools/bin/procctl 10 /project/tools/bin/xmltodb /log/idc/xmltodb_vip.log "\
              "\"<connstr>idc/idcpwd@snorcl11g_128</connstr><charset>Simplified Chinese_China.AL32UTF8</charset>"\
              "<inifilename>/project/idc/ini/xmltodb.xml</inifilename>"\
              "<xmlpath>/idcdata/xmltodb/vip</xmlpath><xmlpathbak>/idcdata/xmltodb/vipbak</xmlpathbak>"\
              "<xmlpatherr>/idcdata/xmltodb/viperr</xmlpatherr>"\
              "<timetvl>5</timetvl><timeout>50</timeout><pname>xmltodb_vip</pname>\"\n\n");

    printf("This program is a common function module of the shared platform for importing XML files into Oracle tables.\n");
    printf("logfilename   The log file used by this program.\n");
    printf("xmlbuffer     The parameters for running this program, represented in XML, are as follows:\n\n");

    printf("connstr     Database connection parameters, format: username/passwd@tnsname.\n");
    printf("charset     The character set of the database, this parameter must be consistent with the source database to avoid Chinese character garbling.\n");
    printf("inifilename Configuration file for data import parameters.\n");
    printf("xmlpath     Directory for storing XML files awaiting import.\n");
    printf("xmlpathbak  Backup directory for XML files after import.\n");
    printf("xmlpatherr  Directory for storing XML files that failed to import.\n");
    printf("timetvl     Interval for scanning the xmlpath directory (interval for performing import tasks), in seconds, depending on business needs, between 2-30.\n");
    printf("timeout     Timeout of this program, in seconds, depending on the size of the XML file, it is recommended to set it to more than 30.\n");
    printf("pname       Process name, try to use an easy-to-understand name, different from other processes, to facilitate troubleshooting.\n\n");
}

// Parse XML into the starg structure parameters
bool _xmltoarg(const char *strxmlbuffer)
{
    memset(&starg, 0, sizeof(struct st_arg));

    getxmlbuffer(strxmlbuffer, "connstr", starg.connstr, 100);
    if (strlen(starg.connstr) == 0) { logfile.write("connstr is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer, "charset", starg.charset, 50);
    if (strlen(starg.charset) == 0) { logfile.write("charset is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer, "inifilename", starg.inifilename, 300);
    if (strlen(starg.inifilename) == 0) { logfile.write("inifilename is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer, "xmlpath", starg.xmlpath, 300);
    if (strlen(starg.xmlpath) == 0) { logfile.write("xmlpath is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer, "xmlpathbak", starg.xmlpathbak, 300);
    if (strlen(starg.xmlpathbak) == 0) { logfile.write("xmlpathbak is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer, "xmlpatherr", starg.xmlpatherr, 300);
    if (strlen(starg.xmlpatherr) == 0) { logfile.write("xmlpatherr is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer, "timetvl", &starg.timetvl);
    if (starg.timetvl < 2) starg.timetvl = 2;
    if (starg.timetvl > 30) starg.timetvl = 30;

    getxmlbuffer(strxmlbuffer, "timeout", &starg.timeout);
    if (starg.timeout == 0) { logfile.write("timeout is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer, "pname", starg.pname, 50);
    if (strlen(starg.pname) == 0) { logfile.write("pname is null.\n"); return false; }

    return true;
}

void EXIT(int sig)
{
    logfile.write("Program exiting, sig=%d\n\n", sig);

    conn.disconnect();

    exit(0);
}

// Main function for business processing.
bool _xmltodb()
{
    cdir dir;

    int icout = 50;    // Counter for the loop, initialized to 50 to load parameters the first time the loop is entered.

    while (true)      // Execute an import task for each loop iteration.
    {
        // Load the data import parameter configuration file starg.inifilename into the vxmltotable container.
        if (icout > 30)
        {
            if (loadxmltotable() == false) return false;
            icout = 0;
        } else icout++;

        // Open the starg.xmlpath directory, to ensure the XML files generated first are imported first, open the directory in file name order.
        if (dir.opendir(starg.xmlpath, "*.XML", 10000, false, true) == false)
        {
            logfile.write("dir.opendir(%s) failed.\n", starg.xmlpath); return false;
        }
        
        if (conn.isopen() == false)
        {
            if (conn.connecttodb(starg.connstr, starg.charset) != 0)
            {
                logfile.write("connect database(%s) failed.\n%s\n", starg.connstr, conn.message()); return false;
            }
            logfile.write("connect database(%s) ok.\n", starg.connstr);
        }

        while (true)
        {
            // Read the directory and get an XML file.
            if (dir.readdir() == false) break;

            logfile.write("Processing file %s...", dir.m_ffilename.c_str());

            // Sub-function for processing XML files, return value: 0-success, others-failure, there are many kinds of failure, not yet determined.
            int ret = _xmltodb(dir.m_ffilename, dir.m_filename);

            pactive.uptatime();   // Update the heartbeat of the process.

            // 0-success, no error. Move the imported XML file to the backup directory.
            if (ret == 0)
            {
                logfile << "ok(" << stxmltotable.tname << ", total=" << totalcount << ", inserted=" << inscount 
                           << ", updated=" << uptcount << ", time consumed=" << timer.elapsed() <<").\n";

                // Move the XML file to the directory specified by starg.xmlpathbak parameter, usually no error occurs, if it does, the program will exit.
                if (xmltobakerr(dir.m_ffilename, starg.xmlpath, starg.xmlpathbak) == false) return false;
            }

            // 1-Incorrect import parameters; 3-The table to be imported does not exist; 4-Failed to execute the SQL statement before import. Move the XML file to the error directory.
            if ((ret == 1) || (ret == 3) || (ret == 4))
            {
                if (ret == 1) logfile << "failed, incorrect import parameters.\n";
                if (ret == 3) logfile << "failed, the table to be imported (" << stxmltotable.tname << ") does not exist.\n";
                if (ret == 4) logfile << "failed, failed to execute the SQL statement before import.\n";

                // Move the XML file to the directory specified by starg.xmlpatherr parameter, usually no error occurs, if it does, the program will exit.
                if (xmltobakerr(dir.m_ffilename, starg.xmlpath, starg.xmlpatherr) == false) return false;
            }

            // 2-Database error, function returns, program will exit.
            if (ret == 2)
            {
                logfile << "failed, database error.\n";  return false;
            }

            // 5-Failed to open XML file, function returns, program will exit.
            if (ret == 5)
            {
                logfile << "failed, failed to open file.\n";  return false;
            }
        }

        // If a file was processed just now, it means it is not idle, there might be files continuously needing to be imported, so do not sleep.
        if (dir.size() == 0)  sleep(starg.timetvl);

        pactive.uptatime();   // Update the heartbeat of the process.
    }

    return true;
}

// Load the parameter configuration file starg.ini into the vxmltotable container.
bool loadxmltotable()
{
    vxmltotable.clear();

    cifile ifile;
    if (ifile.open(starg.inifilename) == false)
    {
        logfile.write("ifile.open(%s) failed.\n", starg.inifilename); return false;
    }

    string strbuffer;

    while (true)
    {
        if (ifile.readline(strbuffer, "<endl/>") == false) break;

        memset(&stxmltotable, 0, sizeof(struct st_xmltotable));

        getxmlbuffer(strbuffer, "filename", stxmltotable.filename, 100);   // The matching rule for the XML file, separated by commas.
        getxmlbuffer(strbuffer, "tname", stxmltotable.tname, 30);          // The name of the table to be imported.
        getxmlbuffer(strbuffer, "uptbz", stxmltotable.uptbz);              // Update flag: 1-update; 2-do not update.
        getxmlbuffer(strbuffer, "execsql", stxmltotable.execsql, 300);     // The SQL statement executed before processing the XML file.

        vxmltotable.push_back(stxmltotable);
    }

    logfile.write("loadxmltotable(%s) ok.\n", starg.inifilename);

    return true;
}

// Based on the file name, find the import parameters from the vxmltotable container and store them in the stxmltotable structure.
bool findxmltotable(const string &xmlfilename)
{
    for (auto &aa : vxmltotable)
    {
        if (matchstr(xmlfilename, aa.filename) == true)
        {
            stxmltotable = aa;  
            return true;
        }
    }

    return false;
}

// Subfunction for processing XML files. Return value: 0 for success, others for failure. There are many kinds of failure scenarios, which are currently uncertain.
int _xmltodb(const string &fullfilename, const string &filename)
{
    timer.start();          // Start timing.
    totalcount = inscount = uptcount = 0;

    // 1) Find the import parameters based on the file name to be imported, and get the corresponding table name.
    if (findxmltotable(filename) == false) return 1;  // 1 - Import parameter configuration incorrect.

    // 2) Read the data dictionary based on the table name to get the table's field names and primary key.
    if (tcols.allcols(conn, stxmltotable.tname) == false) return 2; // 2 - Database system issues, or network disconnection, or timeout.
    if (tcols.pkcols(conn, stxmltotable.tname) == false) return 2;  // 2 - Database system issues, or network disconnection, or timeout.

    // 3) Based on the table's field names and primary key, concatenate SQL statements for inserting and updating, and bind input variables.
    // If tcols.m_allcols.size() is 0, it means the table does not exist (either wrong parameters were set or the table wasn't created), return 3.
    if (tcols.m_allcols.size() == 0) return 3;  // 3 - The table to be imported does not exist.

    // Concatenate SQL statements for inserting and updating the table.
    crtsql();

    // Prepare SQL statements and bind input variables.
    preparesql();

    // Before processing the XML file, execute it if stxmltotable.execsql is not empty.
    if (execsql() == false) return 4;  // 4 - Failed to execute SQL statement before import.

    // 4) Open the XML file.
    cifile ifile;
    if (ifile.open(fullfilename) == false) { conn.rollback(); return 5; }  // 5 - Failed to open file.

    string strbuffer;  // Stores a line read from the XML file.

    while (true)
    {
        // 5) Read a line of data from the XML file.
        if (ifile.readline(strbuffer, "<endl/>") == false) break;

        totalcount++;  // Increment total record count of the XML file.

        // 6) Parse each field value from the read line based on the table's field names.
        splitbuffer(strbuffer);

        // 7) Execute SQL statements for insertion or update.
        if (stmtins.execute() != 0)
        {
            if (stmtins.rc() == 1)  // Violation of uniqueness constraint, indicating the record already exists.
            {
                if (stxmltotable.uptbz == 1)  // Judge the update flag of import parameters.
                {
                    if (stmtupt.execute() != 0)
                    {
                        // If update fails, log the erroneous line and reason, but the function continues processing data, i.e., ignoring this line.
                        // Failure is mainly due to data issues, e.g., incorrect format of time, invalid or oversized values.
                        logfile.write("%s", strbuffer.c_str());
                        logfile.write("stmtupt.execute() failed.\n%s\n%s\n", stmtupt.sql(), stmtupt.message());
                    }
                    else uptcount++;  // Increment count of updated records.
                }
            }
            else
            {
                // If insert fails, log the erroneous line and reason, but the function continues processing data, i.e., ignoring this line.
                // Failure is mainly due to data issues, e.g., incorrect format of time, invalid or oversized values.
                logfile.write("%s", strbuffer.c_str());
                logfile.write("stmtins.execute() failed.\n%s\n%s\n", stmtins.sql(), stmtins.message());

                // If the database system has issues, common errors include, but are not limited to, the following. Add more errors as they occur.
                // ORA-03113: end-of-file on communication channel; ORA-03114: not connected to ORACLE; ORA-03135: connection lost contact; ORA-16014: archiver error.
                if ((stmtins.rc() == 3113) || (stmtins.rc() == 3114) || (stmtins.rc() == 3135) || (stmtins.rc() == 16014)) return 2;
            }
        }
        else inscount++;  // Increment count of inserted records.
    }

    // 8) Commit the transaction.
    conn.commit();

    return 0;
}

// Concatenates SQL for inserting and updating table data.
void crtsql()
{
    // Concatenate the SQL statement for inserting into the table.
    // insert into T_ZHOBTMIND1(obtid,ddatetime,t,p,u,wd,wf,r,vis,keyid) 
    // values(:1,to_date(:2,'yyyymmddhh24miss'),:3,:4,:5,:6,:7,:8,:9,SEQ_ZHOBTMIND1.nextval)
    string strinsertp1;  // Field list for the insert statement.
    string strinsertp2;  // Contents after values in the insert statement.
    int colseq = 1;      // Sequence number of fields in the values part.

    for (auto &aa : tcols.m_vallcols)  // Traverse the container of all fields of the table.
    {
        // The default value of the upttime field is sysdate, no need to process.
        if (strcmp(aa.colname, "upttime") == 0) continue;

        // Concatenate field list for the insert statement, strinsertp1.
        strinsertp1 = strinsertp1 + aa.colname + ",";

        // Concatenate strinsertp2, need to differentiate between keyid field, date and non-date fields.
        if (strcmp(aa.colname, "keyid") == 0)  // The keyid field needs special handling, obtaining the value of keyid from the sequence generator named after the table.
        {
            strinsertp2 = strinsertp2 + sformat("SEQ_%s.nextval", stxmltotable.tname + 2) + ",";
        }
        else
        {
            if (strcmp(aa.datatype, "date") == 0)  // Date-time fields need special handling.
                strinsertp2 = strinsertp2 + sformat("to_date(:%d,'yyyymmddhh24miss')", colseq) + ",";  // Date-time field.
            else
                strinsertp2 = strinsertp2 + sformat(":%d", colseq) + ",";  // Non-date-time field.

            colseq++;  // If the field name is keyid, colseq does not increase, other fields do.
        }
    }

    deleterchr(strinsertp1, ',');  deleterchr(strinsertp2, ',');  // Remove the last redundant comma.

    // Concatenate the complete SQL statement for inserting into the table.
    sformat(strinsertsql, "insert into %s(%s) values(%s)", stxmltotable.tname, strinsertp1.c_str(), strinsertp2.c_str());

    // logfile << "strinsertsql=" << strinsertsql << "\n";  // Log the SQL statement for inserting into the table for debugging.

    // If the import parameters specify that table data does not need to be updated, do not concatenate the update statement and return.
    if (stxmltotable.uptbz != 1) return;

    // Concatenate the SQL statement for updating the table.
    // update T_ZHOBTMIND1 set t=:1,p=:2,u=:3,wd=:4,wf=:5,r=:6,vis=:7 
    // where obtid=:8 and ddatetime=to_date(:9,'yyyymmddhh24miss')
    // a) Concatenate the beginning part of the update statement.
    strupdatesql = sformat("update %s set ", stxmltotable.tname);

    // b) Concatenate the part after set in the update statement.
    colseq = 1;  // The sequence number of the binding variable starts from 1.
    for (auto &aa : tcols.m_vallcols)  // Traverse the container of all fields of the table.
    {
        // If it is a primary key field, it does not need to be concatenated after set.
        if (aa.pkseq != 0) continue;

        // If the field name is keyid, it does not need updating, skip it.
        if (strcmp(aa.colname, "keyid") == 0) continue;

        // If the field name is upttime, the field is directly assigned sysdate.
        if (strcmp(aa.colname, "upttime") == 0)
        {
            strupdatesql = strupdatesql + "upttime=sysdate,"; continue;
        }

        // Concatenate other fields after set, need to differentiate between date fields and non-date fields.
        if (strcmp(aa.datatype, "date") != 0)  // Non-date fields.
            strupdatesql = strupdatesql + sformat("%s=:%d,", aa.colname, colseq);
        else  // Date fields.
            strupdatesql = strupdatesql + sformat("%s=to_date(:%d,'yyyymmddhh24miss'),", aa.colname, colseq);

        colseq++;  // Increment the sequence number of the binding variable.
    }

    deleterchr(strupdatesql, ',');  // Remove the last redundant comma.

    // c) Concatenate the part after where in the update statement.
    strupdatesql = strupdatesql + " where 1=1 ";  // Use 1=1 for easier concatenation later, this is a common approach.
    // where obtid=:8 and ddatetime=to_date(:9,'yyyymmddhh24miss')
    // where 1=1 and obtid=:8 and ddatetime=to_date(:9,'yyyymmddhh24miss')

    for (auto &aa : tcols.m_vallcols)  // Traverse the container of all fields of the table.
    {
        if (aa.pkseq == 0) continue;  // Skip if it's not a primary key field.

        // Concatenate primary key fields into the update statement, need to differentiate between date fields and non-date fields.
        if (strcmp(aa.datatype, "date") != 0)
             strupdatesql = strupdatesql + sformat(" and %s=:%d", aa.colname, colseq);
        else
             strupdatesql = strupdatesql + sformat(" and %s=to_date(:%d,'yyyymmddhh24miss')", aa.colname, colseq);

        colseq++;  // Increment the sequence number of the binding variable.
    }

    // logfile.write("strupdatesql=%s\n", strupdatesql.c_str());  // Log the SQL statement for updating the table for debugging.
    
    return;
}

// Prepare and update the SQL statement, bind input variables.
void preparesql()
{
    // Allocate memory for the array vcolvalue of input variables.
    vcolvalue.resize(tcols.m_allcols.size());

    // Prepare the SQL statement for inserting into the table, bind input variables.
    stmtins.connect(&conn);
    stmtins.prepare(strinsertsql);

    int colseq=1;        // Input variable (parameter), sequence number of fields in the values section.

    for (int ii=0; ii<tcols.m_vallcols.size(); ii++)    // Traverse the container of all fields.
    {
        // The fields 'upttime' and 'keyid' do not need to bind parameters.
        if ((strcmp(tcols.m_vallcols[ii].colname,"upttime")==0) ||
            (strcmp(tcols.m_vallcols[ii].colname,"keyid")==0)) continue;

        stmtins.bindin(colseq, vcolvalue[ii], tcols.m_vallcols[ii].collen);     // Bind input parameters.
        // logfile.write("stmtins.bindin(%d, vcolvalue[%d], %d);\n", colseq, ii, tcols.m_vallcols[ii].collen);

        colseq++;     // Increment the sequence number of the input parameter.
    }

    // Prepare the SQL statement for updating the table, bind input variables.
    // If the parameters for storage specify that the table's data does not need updating, then do not process the update statement and return from the function.
    if (stxmltotable.uptbz != 1) return;

    stmtupt.connect(&conn);
    stmtupt.prepare(strupdatesql);

    colseq=1;        // Input variable (parameter), sequence number of fields in the set and where sections.

    // Bind input parameters for the set part.
    for (int ii=0; ii<tcols.m_vallcols.size(); ii++)     // Traverse the container of all fields.
    {
        // If it is a primary key field, it is not needed in the set part.
        if (tcols.m_vallcols[ii].pkseq != 0) continue;

        // The fields 'upttime' and 'keyid' do not need to be processed.
        if ((strcmp(tcols.m_vallcols[ii].colname,"upttime")==0) ||
            (strcmp(tcols.m_vallcols[ii].colname,"keyid")==0)) continue;

        stmtupt.bindin(colseq, vcolvalue[ii], tcols.m_vallcols[ii].collen);
        // logfile.write("stmtupt.bindin(%d, vcolvalue[%d], %d);\n", colseq, ii, tcols.m_vallcols[ii].collen);

        colseq++;
    }

    // Bind input parameters for the where part.
    for (int ii=0; ii<tcols.m_vallcols.size(); ii++)     // Traverse the container of all fields.
    {
        // Skip if it is not a primary key field; only primary key fields are concatenated in the where part.
        if (tcols.m_vallcols[ii].pkseq == 0) continue;

        stmtupt.bindin(colseq, vcolvalue[ii], tcols.m_vallcols[ii].collen);
        // logfile.write("stmtupt.bindin(%d, vcolvalue[%d], %d);\n", colseq, ii, tcols.m_vallcols[ii].collen);

        colseq++;
    }

    return;
}

// Before processing the XML file, execute stxmltotable.execsql if it is not empty.
bool execsql()
{
    if (strlen(stxmltotable.execsql) == 0) return true;

    sqlstatement stmt;
    stmt.connect(&conn);
    stmt.prepare(stxmltotable.execsql);
    if (stmt.execute() != 0)
    {
        logfile.write("stmt.execute() failed.\n%s\n%s\n", stmt.sql(), stmt.message()); return false;
    }

    return true;
}

// Parses XML and stores it in the vcolvalue array of already bound input variables.
void splitbuffer(const string &strBuffer)
{
    string strtemp;  // Temporary variable to store the value of the field parsed from XML.

    for (int ii = 0; ii < tcols.m_vallcols.size(); ii++)  // Traverse the container of all fields.
    {
        // Parse the value of the data item from XML based on the field name and store it in the temporary variable strtemp.
        // Using a temporary variable prevents the call to the move constructor and move assignment operator from changing the internal address of the string in the vcolvalue array.
        getxmlbuffer(strBuffer, tcols.m_vallcols[ii].colname, strtemp, tcols.m_vallcols[ii].collen);

        // If it's a date-time field 'date', just extracting the numbers is sufficient.
        // That is, the date-time in the XML file only needs to include yyyymmddhh24miss, any separator will do.
        if (strcmp(tcols.m_vallcols[ii].datatype, "date") == 0)
        {
            picknumber(strtemp, strtemp, false, false);
        }
        else if (strcmp(tcols.m_vallcols[ii].datatype, "number") == 0)
        {
            // If it's a numerical field 'number', extract digits, +/- signs, and decimal points.
            picknumber(strtemp, strtemp, true, true);
        }

        // If it's a character field 'char', no processing is needed.

        // vcolvalue[ii] = strtemp;  // Cannot use this line of code, it would call the move assignment operator.
        vcolvalue[ii] = strtemp.c_str();
    }

    return;
}

// Move the XML file to the backup directory or error directory.
bool xmltobakerr(const string &fullfilename, const string &srcpath, const string &dstpath)
{
    string dstfilename = fullfilename;   // Target file name.

    replacestr(dstfilename, srcpath, dstpath, false);    // Be careful with the fourth parameter, it must be false.

    if (renamefile(fullfilename, dstfilename.c_str()) == false)
    {
        logfile.write("renamefile(%s, %s) failed.\n", fullfilename.c_str(), dstfilename.c_str()); return false;
    }

    return true;
}
