/****************************************************************************************/
/*   Program Name: idcapp.h, this program is the declaration file for common functions 
     and classes in the shared platform project.                                         */
/*   Author: bc                                                                         */
/****************************************************************************************/

#ifndef IDCAPP_H
#define IDCAPP_H

#include "_public.h"
#include "_ooci.h"
using namespace idc;

// National meteorological observation data operation class.
class CZHOBTMIND
{
private:
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
    };

    connection  &m_conn;     // Database connection.
    clogfile    &m_logfile;       // Log file.

    sqlstatement m_stmt;       // SQL for table insertion.

    string m_buffer;                // A line read from the file.

    struct st_zhobtmind m_zhobtmind;   // National meteorological observation data structure.
public:
    CZHOBTMIND(connection &conn, clogfile &logfile): m_conn(conn), m_logfile(logfile) {}

    ~CZHOBTMIND() {}

    // Split a line read from the file into the m_zhobtmind structure.
    bool splitbuffer(const string &strbuffer, const bool bisxml);

    // Insert the data in the m_zhobtmind structure into the T_ZHOBTMIND table.
    bool inserttable();  
};

#endif
