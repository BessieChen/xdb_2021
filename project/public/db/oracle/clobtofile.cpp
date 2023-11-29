/*
 * Program Name: clobtofile.cpp, this program demonstrates the framework for operating Oracle databases (extracting a CLOB field from the database to a file).
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
    stmt.prepare("select MEMO_2 from girls where id=1");
    stmt.bindclob();

    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    if (stmt.execute() != 0)
    {
         printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    // Fetch a record, must check the return value: 0-success, 1403-no record, others-failure.
    if (stmt.next() != 0) return 0;

    // Write the contents of the CLOB field to a disk file, must check the return value: 0-success, others-failure.
    if (stmt.lobtofile("/home/hw_bftp/project/public/db/oracle/memo_out_bessie.txt") != 0)
    {
        printf("stmt.lobtofile() failed.\n%s\n",stmt.message()); return -1;
    }

     printf("The CLOB field from the database has been extracted to a file.\n");

    return 0;
}
