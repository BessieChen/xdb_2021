/*
 * Program Name: updatetable.cpp, this program demonstrates the framework for operating Oracle databases (modifying data in a table).
 * Author: bc.
 */
#include "_ooci.h"   // Header file for the development framework operating Oracle.
using namespace idc;

int main(int argc,char *argv[])
{
    connection conn; // Create an object of the database connection class.

    // Log in to the database, return value: 0-success, others-failure.
    // The failure code is in conn.m_cda.rc, the failure description is in conn.m_cda.message.
    if (conn.connecttodb("scott/tiger@snorcl11g_211","Simplified Chinese_China.AL32UTF8") != 0)
    {
        printf("connect database failed.\n%s\n",conn.message()); return -1;
    }

    printf("connect database ok.\n");

    sqlstatement stmt(&conn); 

    // Static SQL statement.
    /*
    stmt.prepare("\
          update girls set name='Bingbing', weight=45.2, btime=to_date('2008-01-02 12:30:22', 'yyyy-mm-dd hh24:mi:ss') where id=10");
    */

    struct st_girl
    {
        long id;                 // Super girl ID, long data type corresponds to Oracle's number(10) without decimals.
        char name[31];     // Super girl name, char[31] corresponds to Oracle's varchar2(30).
        double weight;     // Super girl weight, double data type corresponds to Oracle's number(8,2) with decimals.
        char btime[20];     // Registration time, char corresponds to Oracle's date, format: 'yyyy-mm-dd hh24:mi:ss'.
        char memo[301];  // Note, char[301] corresponds to Oracle's varchar2(300).
    } stgirl;
    
    // Dynamic SQL statement.
    stmt.prepare("\
            update girls set name=:1, weight=:2, btime=to_date(:3, 'yyyy-mm-dd hh24:mi:ss') where id=:4");  // :1, :2, ..., :n can be understood as input parameters.
    stmt.bindin(1, stgirl.name, 30);
    stmt.bindin(2, stgirl.weight);
    stmt.bindin(3, stgirl.btime, 19);
    stmt.bindin(4, stgirl.id);

    // Initialize the structure, assign values to variables.
    memset(&stgirl, 0, sizeof(struct st_girl));
    stgirl.id=11;                                                             // Super girl ID.
    sprintf(stgirl.name, "Mimi");                                      // Super girl name.
    stgirl.weight=43.85;                                                // Super girl weight.
    strcpy(stgirl.btime, "2021-08-25 10:33:35");    

    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    // The failure code is in stmt.m_cda.rc, the failure description is in stmt.m_cda.message.
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    printf("Successfully modified %ld records.\n",stmt.rpc()); // stmt.m_cda.rpc is the number of records affected by this SQL execution.

    conn.commit();       // Commit the transaction.

    return 0;
}
