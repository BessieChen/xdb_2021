/*
 * Program Name: createtable.cpp, this program demonstrates the framework for operating Oracle databases (creating tables).
 * Author: bc.
 */
#include "_ooci.h"   // Header file for the development framework operating Oracle.
using namespace idc;

int main(int argc,char *argv[])
{
    connection conn; // Create an object of the database connection class.

    // Log in to the database, return value: 0-success, others-failure.
    // The failure code is in conn.m_cda.rc, the failure description is in conn.m_cda.message.
    //if (conn.connecttodb("scott/tiger@snorcl11g_128","Simplified Chinese_China.AL32UTF8") != 0)
    if (conn.connecttodb("scott/tiger@snorcl11g_211","Simplified Chinese_China.AL32UTF8") != 0)
    {
        printf("connect database failed.\n%s\n",conn.message()); return -1;
    }

    printf("connect database ok.\n");

    sqlstatement stmt;       // Object for operating SQL statements.
    stmt.connect(&conn);  // Specify the database connection used by the stmt object.
    // Prepare the SQL statement for creating a table.
    // If there is an error in the SQL statement, prepare() will not return a failure, so there is no need to check the return value of prepare().
    // Table 'girls', ID 'id', name 'name', weight 'weight', registration time 'btime', description 'memo', picture 'pic'.
    stmt.prepare("\
        create table girls(id    number(10),\
                           name  varchar2(30),\
                           weight   number(8,2),\
                           btime date,\
                           memo  varchar2(300),\
                           pic   blob,\
                           primary key (id))");

    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    // The failure code is in stmt.m_cda.rc, the failure description is in stmt.m_cda.message.
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    printf("create table girls ok.\n");

    // conn.disconnect();              // The disconnect() method is automatically called in the destructor of the connection class.

    return 0;
}
