/*
 * Program Name: selecttable.cpp, this program demonstrates the framework for operating Oracle databases (querying data in a table).
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

    int minid=10, maxid=24;
    struct st_girl
    {
        long id;                 // Super girl ID, long data type corresponds to Oracle's number(10) without decimals.
        char name[31];     // Super girl name, char[31] corresponds to Oracle's varchar2(30).
        double weight;     // Super girl weight, double data type corresponds to Oracle's number(8,2) with decimals.
        char btime[20];     // Registration time, char corresponds to Oracle's date, format: 'yyyy-mm-dd hh24:mi:ss'.
        char memo[301];  // Note, char[301] corresponds to Oracle's varchar2(300).
    } stgirl;

    // Prepare the SQL statement for querying the table, prepare() method does not need to check the return value.
    // Static
    /* 
    stmt.prepare("select id,name,weight,to_char(btime,'yyyy-mm-dd hh24:mi:ss'),memo from girls where id>=11 and id<=13");
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }
    while (true)
    {
        memset(&stgirl, 0, sizeof(stgirl));    
        if (stmt.next() != 0) break;
        printf("id=%ld, name=%s, weight=%.02f, btime=%s, memo=%s\n", stgirl.id, stgirl.name, stgirl.weight, stgirl.btime, stgirl.memo);
    }
    printf("This query affected %ld records in the girls table.\n", stmt.rpc());    
    */
    
    stmt.prepare("select id, name, weight, to_char(btime,'yyyy-mm-dd hh24:mi:ss'), memo from girls where id>=:1 and id<=:2");
    // Bind the address of input variables to the SQL statement, bindin() method does not need to check the return value.
    stmt.bindin(1, minid);
    stmt.bindin(2, maxid);
    // Bind the result set of the query statement to the address of variables, bindout() method does not need to check the return value.
    stmt.bindout(1, stgirl.id);
    stmt.bindout(2, stgirl.name, 30);
    stmt.bindout(3, stgirl.weight);
    stmt.bindout(4, stgirl.btime, 19);
    stmt.bindout(5, stgirl.memo, 300);

    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    // The failure code is in stmt.m_cda.rc, the failure description is in stmt.m_cda.message.
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    // This program executes a query statement. After executing stmt.execute(), a result set will be generated in the database buffer.
    while (true)
    {
        memset(&stgirl, 0, sizeof(stgirl));    // First initialize the structure variable.

        // Fetch a record from the result set, must check the return value: 0-success, 1403-no record, others-failure.
        // In practical development, besides 0 and 1403, other situations rarely occur.
        if (stmt.next() != 0) break;

        // Print the values of the fetched record.
        printf("id=%ld, name=%s, weight=%.02f, btime=%s, memo=%s\n", stgirl.id, stgirl.name, stgirl.weight, stgirl.btime, stgirl.memo);
    }

    // Please note, the stmt.m_cda.rpc variable is very important, it stores the number of records affected by the executed SQL.
    printf("This query affected %ld records in the girls table.\n", stmt.rpc());

    return 0;
}
