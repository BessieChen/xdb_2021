#include "_public.h"
#include "_ftp.h"

using namespace idc;

// Function to handle program exit and signals 2 and 15.
void EXIT(int sig);

// Structure for program running parameters.
struct st_arg
{
    char host[31];                        // IP and port of the remote server.
    int  mode;                           // Transfer mode, 1-passive mode, 2-active mode, default is passive mode.
    char username[31];               // Username for remote server's ftp.
    char password[31];                // Password for remote server's ftp.
    char remotepath[256];          // Directory on the remote server to store files.
    char localpath[256];              // Directory to store local files.
    char matchname[256];          // Matching rules for files to be downloaded.
    int  ptype;                            // Processing method for server files after download: 1-do nothing; 2-delete; 3-backup.
    char remotepathbak[256];   // Backup directory for server files after download.
    char okfilename[256];          // File to store information of successfully downloaded files.
    bool checkmtime;                // Whether to check the server file's time, true-needed, false-not needed, default is false. Some businesses will only add files, not modify them. Only adding files does not require checktime. Avoid checkmtime if possible, as it requires a TCP connection and consumes resources.
    int  timeout;                         // Timeout for process heartbeat.
    char pname[51];                  // Process name, recommended format is "ftpgetfiles_suffix".
} starg;

bool _xmltoarg(const char *strxmlbuffer);  // Parse xml to starg structure.

clogfile logfile;     // Log file object.
cftpclient ftp;       // FTP client object.
cpactive pactive;  // Process heartbeat object.

void _help();        // Show help documentation.

struct st_fileinfo              // Structure for file information.
{
    string filename;           // File name.
    string mtime;              // File time.
    st_fileinfo()=default;
    st_fileinfo(const string &in_filename,const string &in_mtime):filename(in_filename),mtime(in_mtime) {}
    void clear() { filename.clear(); mtime.clear(); }
}; 

map<string,string> mfromok;               // Container one: stores successfully downloaded files, loaded from the file specified by starg.okfilename.
list<struct st_fileinfo> vfromnlist;      // Container two: stores server file names before downloading, loaded from the nlist file. Previously was vector<>
list<struct st_fileinfo> vtook;           // Container three: stores files not to be downloaded this time.
list<struct st_fileinfo> vdownload;       // Container four: stores files to be downloaded this time.

bool loadokfile();            // Load content from the starg.okfilename file into the mfromok container.
bool loadlistfile();          // Load the list file obtained by ftpclient.nlist() method into the vfromnlist container.
bool compmap();               // Compare vfromnlist and vfromok, get vtook and vdownload.
bool writetookfile();         // Write data in vtook container to the starg.okfilename file, overwrite the old starg.okfilename file.
bool appendtookfile(struct st_fileinfo &stfileinfo); // Append records of successfully downloaded files to the starg.okfilename file.

int main(int argc,char *argv[])
{
    // Step one plan: Download files from a directory on the server, can specify file name matching rules.
    if (argc!=3) { _help();    return -1; }

    // Set signals, in shell state "kill + process number" can be used to terminate this process normally.
    // But please do not use "kill -9 + process number" to forcibly terminate.
    // closeioandsignal(true);       // Close 0, 1, 2 and ignore all signals, during the debugging stage, this line of code can be disabled.
    signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

    // Open the log file.
    if (logfile.open(argv[1])==false)
    {
        printf("Failed to open log file (%s).\n",argv[1]); return -1;
    }

    // Parse xml to get the program running parameters.
    if (_xmltoarg(argv[2])==false) return -1;

    pactive.addpinfo(starg.timeout,starg.pname);  // Write the process's heartbeat information to shared memory. Write the heartbeat information before logging in to ftp.login(), because login() might timeout.

    // Log in to the FTP server.
    if (ftp.login(starg.host,starg.username,starg.password,starg.mode)==false)
    {
        logfile.write("ftp.login(%s,%s,%s) failed.\n%s\n",starg.host,starg.username,starg.password,ftp.response()); return -1;
    }

    // Enter the directory on the FTP server where files are stored.
    if (ftp.chdir(starg.remotepath)==false)
    {
        logfile.write("ftp.chdir(%s) failed.\n%s\n",starg.remotepath,ftp.response()); return -1;
    }

    // Call the ftpclient.nlist() method to list file names in the server directory, save them in a local file.
    if (ftp.nlist(".",sformat("/tmp/nlist/ftpgetfiles_%d.nlist",getpid())) == false)
    {
        logfile.write("ftp.nlist(%s) failed.\n%s\n",starg.remotepath,ftp.response()); return -1;
    }
    // logfile.write("nlist(%s) ok.\n",sformat("/tmp/nlist/ftpgetfiles_%d.nlist",getpid()).c_str());

    pactive.uptatime();   // Update the process's heartbeat.

    // Load the list file obtained by the ftpclient.nlist() method into the vfromnlist container.
    if (loadlistfile()==false)
    {
      logfile.write("loadlistfile() failed.\n");  return -1;
    }

    if (starg.ptype==1)
    {
        // Load data from the starg.okfilename file into the mfromok container.
        loadokfile();

        // Compare vfromnlist and mfromok, get vtook and vdownload.
        compmap();

        // Write data in the vtook container to the starg.okfilename file, overwrite the old starg.okfilename file.
        writetookfile(); // Overwrite: from 1.txt,2.txt -> 1.txt. Because only 1.txt is not modified.
    }
    else
        vfromnlist.swap(vdownload);   // To unify the file downloading code, swap container two and four.

    pactive.uptatime();   // Update the process's heartbeat. Don't worry, it won't consume too much resource.

    string strremotefilename,strlocalfilename;

    // Traverse the vdownload container.
    for (auto & aa : vdownload) 
    {
        sformat(strremotefilename,"%s/%s",starg.remotepath,aa.filename.c_str());         // Concatenate the full path of the server file.
        sformat(strlocalfilename,"%s/%s",starg.localpath,aa.filename.c_str());           // Concatenate the full path of the local file.

        logfile.write("get %s ...",strremotefilename.c_str());
        // Call the ftpclient.get() method to download the file.
        if (ftp.get(strremotefilename,strlocalfilename,
            										starg.checkmtime) // Add this parameter
            										==false) 
        {
            logfile << "failed.\n" << ftp.response() << "\n"; return -1;
        }

        logfile << "ok.\n"; 

        pactive.uptatime();   // Update the process's heartbeat. Don't worry, it won't consume too much resource.

        // If ptype==1, append the record of successfully downloaded file to the starg.okfilename file.
        if (starg.ptype==1) appendtookfile(aa); // Append: from 1.txt,2.txt --overwrite--> 1.txt --append--> 1.txt.2.txt[modified],3.txt[new]

        // ptype==2, delete the file on the server.
        if (starg.ptype==2)
        {
            if (ftp.ftpdelete(strremotefilename)==false)
            {
                logfile.write("ftp.ftpdelete(%s) failed.\n%s\n",strremotefilename.c_str(),ftp.response()); return -1;
            }
        }

        // ptype==3, move the server file to the backup directory.
        if (starg.ptype==3)
        {
            string strremotefilenamebak=sformat("%s/%s",starg.remotepathbak,aa.filename.c_str());  // Generate full path of the backup file.
            if (ftp.ftprename(strremotefilename,strremotefilenamebak)==false)
            {
                logfile.write("ftp.ftprename(%s,%s) failed.\n%s\n",strremotefilename.c_str(),strremotefilenamebak.c_str(),ftp.response()); return -1;
            }
        }
    }

    return 0;
}


void _help()        // Display help documentation.
{
    printf("\n");
    printf("Using:/project/tools/bin/ftpgetfiles logfilename xmlbuffer\n\n");

    printf("Sample:/project/tools/bin/procctl 30 /project/tools/bin/ftpgetfiles /log/idc/ftpgetfiles_test.log " \
              "\"<host>192.168.150.128:21</host><mode>1</mode>"\
              "<username>xdb</username><password>oracle</password>"\
              "<remotepath>/tmp/ftp/server</remotepath><localpath>/tmp/ftp/client</localpath>"\
              "<matchname>*.TXT</matchname>"\
              "<ptype>1</ptype><okfilename>/idcdata/ftplist/ftpgetfiles_test.xml</okfilename>"\
              "<checkmtime>true</checkmtime>"\
              "<timeout>30</timeout><pname>ftpgetfiles_test</pname>\"\n\n\n");

    printf("This program is a general functional module used to download files from a remote ftp server to a local directory.\n");
    printf("logfilename is the log file of this program.\n");
    printf("xmlbuffer is the parameter for file downloading, as follows:\n");
    printf("<host>192.168.150.128:21</host> IP and port of the remote server.\n");
    printf("<mode>1</mode> Transfer mode, 1-passive mode, 2-active mode, default is passive mode.\n");
    printf("<username>xdb</username> Username for remote server's ftp.\n");
    printf("<password>oraccle</password> Password for remote server's ftp.\n");
    printf("<remotepath>/tmp/idc/surfdata</remotepath> Directory on the remote server to store files.\n");
    printf("<localpath>/idcdata/surfdata</localpath> Directory to store local files.\n");
    printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> Matching rules for files to be downloaded. "\
              "Files not matching will not be downloaded. This field should be set as precisely as possible, not recommended to use * to match all files.\n");
    printf("<ptype>1</ptype> Processing method for server files after download: "\
              "1-do nothing; 2-delete; 3-backup, if 3, then also specify the backup directory.\n");
    printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> Backup directory for server files after download, "\
              "this parameter is only effective when ptype=3.\n");
    printf("<okfilename>/idcdata/ftplist/ftpgetfiles_test.xml</okfilename> List of successfully downloaded filenames, "\
              "this parameter is only effective when ptype=1.\n");
    printf("<checkmtime>true</checkmtime> Whether to check the server file's time, true-needed, false-not needed, "\
              "this parameter is only effective when ptype=1, default is false.\n");
    printf("<timeout>30</timeout> File download timeout, in seconds, depending on file size and network bandwidth.\n");
    printf("<pname>ftpgetfiles_test</pname> Process name, should be as understandable and different from other processes as possible for troubleshooting.\n\n\n");
}

// Parse xml into the starg structure.
bool _xmltoarg(const char *strxmlbuffer)
{
    memset(&starg, 0, sizeof(struct st_arg));

    getxmlbuffer(strxmlbuffer, "host", starg.host, 30);   // IP and port of the remote server.
    if (strlen(starg.host) == 0)
    { logfile.write("host is null.\n");  return false; }

    getxmlbuffer(strxmlbuffer, "mode", starg.mode);   // Transfer mode, 1-passive mode, 2-active mode, default is passive mode.
    if (starg.mode != 2)  starg.mode = 1;

    getxmlbuffer(strxmlbuffer, "username", starg.username, 30);   // Username for remote server's ftp.
    if (strlen(starg.username) == 0)
    { logfile.write("username is null.\n");  return false; }

    getxmlbuffer(strxmlbuffer, "password", starg.password, 30);   // Password for remote server's ftp.
    if (strlen(starg.password) == 0)
    { logfile.write("password is null.\n");  return false; }

    getxmlbuffer(strxmlbuffer, "remotepath", starg.remotepath, 255);   // Directory on the remote server to store files.
    if (strlen(starg.remotepath) == 0)
    { logfile.write("remotepath is null.\n");  return false; }

    getxmlbuffer(strxmlbuffer, "localpath", starg.localpath, 255);   // Directory to store local files.
    if (strlen(starg.localpath) == 0)
    { logfile.write("localpath is null.\n");  return false; }

    getxmlbuffer(strxmlbuffer, "matchname", starg.matchname, 100);   // Matching rules for files to be downloaded.
    if (strlen(starg.matchname) == 0)
    { logfile.write("matchname is null.\n");  return false; }  

    // Processing method for server files after download: 1-do nothing; 2-delete; 3-backup.
    getxmlbuffer(strxmlbuffer, "ptype", starg.ptype);   
    if ((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3))
    { logfile.write("ptype is error.\n"); return false; }

    // Backup directory for server files after download.
    if (starg.ptype == 3) 
    {
        getxmlbuffer(strxmlbuffer, "remotepathbak", starg.remotepathbak, 255); 
        if (strlen(starg.remotepathbak) == 0) { logfile.write("remotepathbak is null.\n");  return false; }
    }

    // Incremental download of files.
    if (starg.ptype == 1) 
    {
        getxmlbuffer(strxmlbuffer, "okfilename", starg.okfilename, 255); // List of successfully downloaded filenames.
        if (strlen(starg.okfilename) == 0) { logfile.write("okfilename is null.\n");  return false; }

        // Whether to check the server file's time, true-needed, false-not needed, this parameter is only effective when ptype=1, default is false.
        getxmlbuffer(strxmlbuffer, "checkmtime", starg.checkmtime);
    }

    getxmlbuffer(strxmlbuffer, "timeout", starg.timeout);   // Timeout for process heartbeat.
    if (starg.timeout == 0) { logfile.write("timeout is null.\n");  return false; }

    getxmlbuffer(strxmlbuffer, "pname", starg.pname, 50);     // Process name.
    // if (strlen(starg.pname) == 0) { logfile.write("pname is null.\n");  return false; }

    return true;
}


void EXIT(int sig)
{
    printf("Program exit, sig=%d\n\n",sig);

    exit(0);
}

// Load the list file obtained by the ftp.nlist() method into {Container 2} vfromnlist.
bool loadlistfile()
{
    vfromnlist.clear();

    cifile ifile;
    if (ifile.open(sformat("/tmp/nlist/ftpgetfiles_%d.nlist",getpid())) == false) // This nlist is generated by us
    {
        logfile.write("ifile.open(%s) failed.\n", sformat("/tmp/nlist/ftpgetfiles_%d.nlist", getpid())); return false;
    }

    string strfilename;

    while (true)
    {
        if (ifile.readline(strfilename) == false) break;

        if (matchstr(strfilename, starg.matchname) == false) continue;

        if ((starg.ptype == 1) && (starg.checkmtime == true))
        {
            // Get the file time from the FTP server.
            if (ftp.mtime(strfilename) == false)
            {
                logfile.write("ftp.mtime(%s) failed.\n", strfilename.c_str()); return false;
            }
        }

        vfromnlist.emplace_back(strfilename, ftp.m_mtime);
    }

    ifile.closeandremove();

    // for (auto &aa : vfromnlist)
    //     logfile.write("filename=%s, mtime=%s\n", aa.filename.c_str(), aa.mtime.c_str());

    return true;
}

// Load the contents of the starg.okfilename file into the container mfromok.
bool loadokfile()
{
    if (starg.ptype != 1) return true;

    mfromok.clear();

    cifile ifile;

    // Note: If the program is run for the first time, starg.okfilename does not exist, which is not an error, so it also returns true.
    if ((ifile.open(starg.okfilename)) == false) return true;
        // okfilename list of successfully downloaded filenames: /idcdata/ftplist/ftpgetfiles_test.xml
        // I did not create /home/hw_bftp/project/bessie_test/idcdata/ftplist/ftpgetfiles_test.xml that contains okfilename
        // But the cmd did not report an error, because the first time the program is run, it will create this file for me {/home/hw_bftp/project/bessie_test/idcdata/ftplist/ftpgetfiles_test.xml}!

    string strbuffer;

    struct st_fileinfo stfileinfo;

    while (true)
    {
        stfileinfo.clear();

        if (ifile.readline(strbuffer) == false) break;

        getxmlbuffer(strbuffer, "filename", stfileinfo.filename);
        getxmlbuffer(strbuffer, "mtime", stfileinfo.mtime);

        mfromok[stfileinfo.filename] = stfileinfo.mtime;
    }

    // for (auto &aa : mfromok)
    //     logfile.write("filename=%s, mtime=%s\n", aa.first.c_str(), aa.second.c_str());

    return true;
}

// Compare vfromnlist and mfromok to get vtook and vdownload.
bool compmap()
{
    vtook.clear(); 
    vdownload.clear();

    // Traverse vfromnlist.
    for (auto &aa : vfromnlist)
    {
        auto it = mfromok.find(aa.filename);           // Find by filename in container one.
        if (it != mfromok.end())
        {   // If found, then judge by file time.
            if (starg.checkmtime == true)
			{
				// If the time is also the same, no need to download; otherwise, need to re-download.
				if (it->second == aa.mtime) vtook.push_back(aa);    // File time has not changed, no need to download.
				else vdownload.push_back(aa);     // Need to re-download.
			}
			else
			{
				vtook.push_back(aa);   // No need to re-download.
			}
        }
        else
        {   // If not found, put the record into the vdownload container.
            vdownload.push_back(aa);
        }
    }

    return true;
}

// Write the contents of the vtook container to the starg.okfilename file, overwriting the old starg.okfilename file.
bool writetookfile()
{
    cofile ofile;    

    if (ofile.open(starg.okfilename) == false) // Since it's not append, it's in default mode: directly overwrite
    {
      logfile.write("file.open(%s) failed.\n", starg.okfilename); return false;
    }

    for (auto &aa : vtook)
        ofile.writeline("<filename>%s</filename><mtime>%s</mtime>\n", aa.filename.c_str(), aa.mtime.c_str());

    ofile.closeandrename();

    return true;
}

// Append the record of successfully downloaded files to the starg.okfilename file.
bool appendtookfile(struct st_fileinfo &stfileinfo)
{
    cofile ofile;

    // Open the file in append mode, note that the second parameter (whether to use a temporary file) must be false.
    if (ofile.open(starg.okfilename, false, ios::app) == false)
    {
      logfile.write("file.open(%s) failed.\n", starg.okfilename); return false;
    }

    ofile.writeline("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo.filename.c_str(), stfileinfo.mtime.c_str());

    return true;
}
