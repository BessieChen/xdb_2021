/*
 * Program Name: filetoblob.cpp, this program demonstrates the framework for operating Oracle databases (storing a binary file into the BLOB field of a database).
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
    stmt.prepare("insert into girls(id,name,PIC_2) values(2,'Mimi',empty_blob())");  // Note: null cannot replace empty_blob().
    if (stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    // Use a cursor to extract the PIC_2 field of a record from the girls table
    stmt.prepare("select PIC_2 from girls where id=2 for update");
    stmt.bindblob();

    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    // Fetch a record, must check the return value: 0-success, 1403-no record, others-failure.
    if (stmt.next() != 0) return 0;

    // Write the contents of the disk file pic_in.jpeg into the BLOB field, must check the return value: 0-success, others-failure.
    if (stmt.filetolob("/home/hw_bftp/project/public/db/oracle/pic_in.jpeg") != 0)
    {
        printf("stmt.filetolob() failed.\n%s\n",stmt.message()); return -1;
    }

    printf("The binary file has been stored in the BLOB field of the database.\n");

    conn.commit();

    return 0;
}
