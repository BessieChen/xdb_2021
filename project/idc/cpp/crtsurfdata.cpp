/*
 * Program name: crtsurfdata.cpp This program is used to generate minute data observed by meteorological station sites.
 * Author: bc.
 */
#include "_public.h"
using namespace idc;

// Province Station Number Station Name Latitude Longitude Altitude
// Anhui,58015,Dangshan,34.27,116.2,44.2
struct st_stcode    // Structure for site parameters. 
{
    char provname[31];     // Province. String for convenience and auto-expansion. When operating the database, string has no advantage.
    char obtid[11];             // Station number.
    char obtname[31];       // Station name, same as the city name.
    double lat;                   // Latitude: degrees.
    double lon;                  // Longitude: degrees.
    double height;            // Altitude: meters.
};
list<struct st_stcode> stlist;        // Stores all site parameters.
bool loadstcode(const string &inifile);   // Loads site parameter file into the stlist container.

clogfile logfile;          // The log of this program's execution.

void EXIT(int sig);      // Function to handle exit and signals 2, 15.

int main(int argc,char *argv[])
{
    // Site parameter file Directory where the generated test data is stored The log of this program's execution
    if (argc!=4)
    {
        // If the parameters are illegal, provide help documentation.
        cout << "Using:./crtsurfdata inifile outpath logfile\n";
        cout << "Examples:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log\n\n";

        cout << "inifile Meteorological station parameter file name.\n";
        cout << "outpath Directory where meteorological station data files are stored.\n";
        cout << "logfile The log file name of this program's execution.\n";

        return -1;  
    }

    // Set signals, in shell state "kill + process number" can be used to terminate this process normally.
    // But please do not use "kill -9 + process number" to forcibly terminate.
    closeioandsignal(true);       // Close 0, 1, 2 and ignore all signals.
    signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

    if (logfile.open(argv[3])==false)
    {
        cout << "logfile.open(" << argv[3] << ") failed.\n";  return -1;
    }

    logfile.write("crtsurfdata starts running.\n");

    // Write the business processing code here.
    // 1) Load site parameters from the site parameter file and store them in the stlist container;
    if (loadstcode(argv[1]) ==  false) EXIT(-1);   

    // 2) Generate site observation data (random numbers) based on site parameters;


    // 3) Save the site observation data to a file.


    logfile.write("crtsurfdata ends running.\n");

    return 0;
}

// Function to handle exit and signals 2, 15.
void EXIT(int sig)
{
    logfile.write("Program exit, sig=%d\n\n",sig);

    exit(0);
}

// Loads site parameter file into the stlist container.
bool loadstcode(const string &inifile)   
{
    cifile ifile;       // Object for reading the file.
    if (ifile.open(inifile)==false) 
    {
        logfile.write("ifile.open(%s) failed.\n",inifile.c_str()); return false;
    }

    string strbuffer;        // Stores each line read from the file.

    ifile.readline(strbuffer);         // Read the first line of the site parameter file, it is the title, discard it.

    ccmdstr cmdstr;                    // Used to split the lines read from the file.
    st_stcode stcode;                  // Structure for site parameters.

    while(ifile.readline(strbuffer))
    {
        // logfile.write("strbuffer=%s\n",strbuffer.c_str());

        // Split the line read from the file, for example: Anhui,58015,Dangshan,34.27,116.2,44.2
        cmdstr.splittocmd(strbuffer,",");         // Split the string.

        memset(&stcode,0,sizeof(st_stcode));

        cmdstr.getvalue(0,stcode.provname,30);   // Province
        cmdstr.getvalue(1,stcode.obtid,10);           // Station code
        cmdstr.getvalue(2,stcode.obtname,30);     // Station name
        cmdstr.getvalue(3,stcode.lat);                    // Latitude
        cmdstr.getvalue(4,stcode.lon);                   // Longitude
        cmdstr.getvalue(5,stcode.height);              // Altitude

        stlist.push_back(stcode);                            // Store the site parameters in the stlist container.
    }

    // No need to manually close the file here, the cifile class's destructor will close the file.

    // Write all the data in the container to the log.
    for (auto &aa:stlist)
    {
       logfile.write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
                            aa.provname,aa.obtid,aa.obtname,aa.lat,aa.lon,aa.height);
    }

    return true;
}
