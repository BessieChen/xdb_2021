/*
 * Program Name: inserttable.cpp, this program demonstrates the framework for operating Oracle databases (inserting data into a table).
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

    // sqlstatement stmt;       // Object for operating SQL statements.
    // stmt.connect(&conn);  // Specify the database connection used by the stmt object.
    sqlstatement stmt(&conn); 

    // Prepare the SQL statement for inserting into the table.
    
    // Static SQL statements, suitable for one-time execution. 1) Less efficient than dynamic SQL; 2) Difficult to handle special characters; 3) Security (SQL injection) issues.
    for (int ii=10; ii<15; ii++)
    {
        stmt.prepare("\
                insert into girls(id,name,weight,btime,memo) \
                    values(%d,'Xishi%05dgirl',%.1f,to_date('2000-01-01 12:30:%02d','yyyy-mm-dd hh24:mi:ss'),\
                               'This is the note for the %05dth super girl.')",ii,ii,45.35+ii,ii,ii);

        // Execute the SQL statement, must check the return value: 0-success, others-failure.
        // The failure code is in stmt.m_cda.rc, the failure description is in stmt.m_cda.message.
        if (stmt.execute() != 0)
        {
            printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
        }

        printf("Successfully inserted %ld records.\n",stmt.rpc()); // stmt.m_cda.rpc is the number of records affected by this SQL execution.
    }

    conn.commit();                // Commit the transaction.

    return 0;
}
