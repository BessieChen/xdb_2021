/*
 * Program Name: deletetable.cpp, this program demonstrates the framework for operating Oracle databases (deleting data from a table).
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
    stmt.prepare("delete from girls where id=11");
    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    // The failure code is in stmt.m_cda.rc, the failure description is in stmt.m_cda.message.
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    printf("Successfully deleted %ld records.\n",stmt.rpc()); // stmt.m_cda.rpc is the number of records affected by this SQL execution.

    int minid=22, maxid=23;
    
    // Dynamic SQL statement.
    stmt.prepare("delete from girls where id>=:1 and id<=:2");  // :1, :2, ..., :n can be understood as input parameters.
    stmt.bindin(1, minid);
    stmt.bindin(2, maxid);

    // Execute the SQL statement, must check the return value: 0-success, others-failure.
    // The failure code is in stmt.m_cda.rc, the failure description is in stmt.m_cda.message.
    if (stmt.execute() != 0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
    }

    printf("Successfully deleted %ld records.\n",stmt.rpc()); // stmt.m_cda.rpc is the number of records affected by this SQL execution.

    conn.commit();       // Commit the transaction.

    return 0;
}
