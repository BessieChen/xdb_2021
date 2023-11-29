/*
 * Program Name: filetoclob.cpp, this program demonstrates the framework for operating Oracle databases (storing a text file into the CLOB field of a database table).
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

    // Modify the structure of the girls table, add the MEMO_2 field for testing. alter table girls add memo1 clob;
    sqlstatement stmt(&conn); 
    stmt.prepare("insert into girls(id,name,MEMO_2) values(1,'Bingbing', \
                                                                empty_clob())");  // Note: empty_clob() cannot be replaced with null.
    if (stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    // Use a cursor to extract the MEMO_2 field of a record from the girls table
    stmt.prepare("select MEMO_2 from girls where id=1 \
                                                    for update");
    stmt.bindclob();

    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    // Fetch a record, must check the return value: 0-success, 1403-no record, others-failure.
    if (stmt.next() != 0) return 0; // Because where id = 1 will only return one record

    // Write the contents of the disk file memo_in.txt into the CLOB field, must check the return value: 0-success, others-failure.
    if (stmt.filetolob("/home/hw_bftp/project/public/db/oracle/memo_in.txt") != 0)
    {
        printf("stmt.filetolob() failed.\n%s\n",stmt.message()); return -1;
    }

    printf("The text file has been stored in the CLOB field of the database.\n");

    conn.commit();

    return 0;
}
