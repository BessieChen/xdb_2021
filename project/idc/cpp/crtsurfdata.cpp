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
    char provname[31];     // Province. String for convenience and auto-expansion. When operating databases, string has no advantage.
    char obtid[11];             // Station number.
    char obtname[31];       // Station name, same as the city name.
    double lat;                   // Latitude: degrees.
    double lon;                  // Longitude: degrees.
    double height;            // Altitude: meters.
};
list<struct st_stcode> stlist;        // Stores all site parameters.
bool loadstcode(const string &inifile);   // Loads site parameter file into the stlist container.

// Structure for meteorological station observation data
struct st_surfdata
{
    char obtid[11];          // Station code.
    char ddatetime[15];  // Data time: format yyyymmddhh24miss, accurate to the minute, seconds fixed at 00.
    int  t;                         // Temperature: unit, 0.1 degrees Celsius.
    int  p;                        // Atmospheric pressure: 0.1 hPa.
    int  u;                        // Relative humidity, value between 0-100.
    int  wd;                     // Wind direction, value between 0-360.
    int  wf;                      // Wind speed: unit 0.1m/s.
    int  r;                        // Rainfall: 0.1mm.
    int  vis;                     // Visibility: 0.1 meters.
};
list<struct st_surfdata>  datalist;           // Container for storing observation data.
void crtsurfdata();                                  // Generates site observation data based on site parameters in stlist, stored in datalist.

char strddatetime[15];

// Writes the meteorological observation data from the datalist container to a file, outpath - the directory where the data files are stored; datafmt - the format of the data file, which can be csv, xml, or json.
bool crtsurffile(const string& outpath, const string& datafmt);


clogfile logfile;          // Log of the program's execution.

void EXIT(int sig);      // Function to handle exit and signals 2, 15.

int main(int argc,char *argv[])
{
    // Site parameter file Directory for storing generated test data Log of the program's execution Output data file format
    if (argc!=5)
    {
        // If the parameters are illegal, provide help documentation.
        cout << "Using:./crtsurfdata inifile outpath logfile datafmt\n";
        cout << "Examples:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json\n\n";

        cout << "inifile Meteorological station parameter file name.\n";
        cout << "outpath Directory where meteorological station data files are stored.\n";
        cout << "logfile The log file name of this program's execution.\n";
        cout << "datafmt The format of the output data file, supports csv, xml, and json, separated by commas.\n\n";

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
    // 1) Load site parameters from the site parameter file, store them in the stlist container;
    if (loadstcode(argv[1]) ==  false) EXIT(-1);   

    // Get the observation data time.
    memset(strddatetime,0,sizeof(strddatetime));
    ltime(strddatetime,"yyyymmddhh24miss");   // Get the current system time.
    strncpy(strddatetime+12,"00",2);                   // Fix seconds in the data time to 00.

    // 2) Generate site observation data (random numbers) based on site parameters in stlist container, store the generated data in the container;
    crtsurfdata();


    // 3) Save the site observation data to a file.
    if (strstr(argv[4],"csv")!=0)    crtsurffile(argv[2],"csv");
    if (strstr(argv[4],"xml")!=0)   crtsurffile(argv[2],"xml");
    if (strstr(argv[4],"json")!=0)  crtsurffile(argv[2],"json");


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
    //for (auto &aa:stlist)
    //{
    //    logfile.write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
    //                         aa.provname,aa.obtid,aa.obtname,aa.lat,aa.lon,aa.height);
    //}

    return true;
}

// Simulates generating minute meteorological observation data, stored in datalist container.
void crtsurfdata()
{
    srand(time(0));              // Seed the random number generator.

    st_surfdata stsurfdata;   // Structure for observation data.

    // Traverse the stlist container of meteorological station parameters, generate observation data for each site.
    for (auto &aa : stlist)
    {
        memset(&stsurfdata,0,sizeof(st_surfdata));

        // Fill in the members of the observation data structure.
        strcpy(stsurfdata.obtid,aa.obtid);                        // Station code.
        strcpy(stsurfdata.ddatetime,strddatetime);        // Data time.
        stsurfdata.t=rand()%350;                                    // Temperature: unit, 0.1 degrees Celsius. 0-350 range. Avoidable errors should not be made.
        stsurfdata.p=rand()%265+10000;                       // Atmospheric pressure: 0.1 hPa.
        stsurfdata.u=rand()%101;                                    // Relative humidity, value between 0-100.
        stsurfdata.wd=rand()%360;                                 // Wind direction, value between 0-360.
        stsurfdata.wf=rand()%150;                                  // Wind speed: unit 0.1m/s.
        stsurfdata.r=rand()%16;                                      // Rainfall: 0.1mm.
        stsurfdata.vis=rand()%5001+100000;                 // Visibility: 0.1 meters.

        datalist.push_back(stsurfdata);                            // Put the observation data structure into datalist container.
    }

    // for (auto &aa:datalist)
    // {
    //     logfile.write("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n", \
    //                           aa.obtid,aa.ddatetime,aa.t/10.0,aa.p/10.0,aa.u,aa.wd,aa.wf/10.0,aa.r/10.0,aa.vis/10.0);
    // }
}


// Writes the meteorological observation data from the datalist container to a file, outpath - the directory where the data files are stored; datafmt - the format of the data file, which can be csv, xml, or json.
bool crtsurffile(const string& outpath, const string& datafmt)
{
    // Concatenates to create the data file name, for example: /tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
    string strfilename = outpath + "/" + "SURF_ZH_" + strddatetime + "_" + to_string(getpid()) + "." + datafmt;

    cofile ofile;         // Object for writing data files.

    if (ofile.open(strfilename) == false)
    {
        logfile.write("ofile.open(%s) failed.\n", strfilename.c_str()); return false;
    }

    // Writes the observation data from the datalist container to the file, supporting three formats: csv, xml, and json.
    if (datafmt == "csv")    ofile.writeline("Station Code,Data Time,Temperature,Atmospheric Pressure,Relative Humidity,Wind Direction,Wind Speed,Rainfall,Visibility\n");
    if (datafmt == "xml")   ofile.writeline("<data>\n");
    if (datafmt == "json")  ofile.writeline("{\"data\":[\n");

    // Iterates over the datalist container storing observation data.
    for (auto &aa : datalist)
    {
        // Writes each row of data to the file.
        if (datafmt == "csv")
            ofile.writeline("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
                                    aa.obtid, aa.ddatetime, aa.t / 10.0, aa.p / 10.0, aa.u, aa.wd, aa.wf / 10.0, aa.r / 10.0, aa.vis / 10.0);

        if (datafmt == "xml") 
            ofile.writeline("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p><u>%d</u>"\
                                   "<wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",\
                                    aa.obtid, aa.ddatetime, aa.t / 10.0, aa.p / 10.0, aa.u, aa.wd, aa.wf / 10.0, aa.r / 10.0, aa.vis / 10.0);

        if (datafmt == "json") 
        {
            ofile.writeline("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\","\
                                  "\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",\
                                   aa.obtid, aa.ddatetime, aa.t / 10.0, aa.p / 10.0, aa.u, aa.wd, aa.wf / 10.0, aa.r / 10.0, aa.vis / 10.0);
            // Note, the last record in the json file does not need a comma, specially handled with the following code.
            static int ii = 0;     // Counter for the number of data rows written.
            if (ii < datalist.size() - 1)
            {   // If it is not the last row.
                ofile.writeline(",\n");  ii++;
            }
            else
                ofile.writeline("\n");  
        }
    }

    if (datafmt == "xml")  ofile.writeline("</data>\n");
    if (datafmt == "json") ofile.writeline("]}\n");

    ofile.closeandrename();    // Closes the temporary file and renames it to the official file.

    logfile.write("Successfully generated data file %s, data time %s, record count %d.\n", strfilename.c_str(), strddatetime, datalist.size());

    return true;
}
