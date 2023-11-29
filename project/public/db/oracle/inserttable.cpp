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
    
    // Static SQL statements, suitable for one-time execution. 1) Less efficient than dynamic SQL; 2) Difficult to handle special characters; 3) Security concerns (SQL injection).
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

    // 1) If the field is string type, the bound variable can be char[] or string, char[] is recommended.
    // 2) If the field is string type, the third parameter of bindin() should be the field length, too small might be problematic, default value of 2000 is not recommended.
    // 3) Dynamic SQL statement fields can also be filled with static values.
    // 4) Bound variables are generally structures.
    struct st_girl
    {
        long id;               // Super girl ID, long data type corresponds to Oracle's number(10) without decimal.
        char name[31];   // Super girl name, char[31] corresponds to Oracle's varchar2(30).
        double weight;   // Super girl weight, double data type corresponds to Oracle's number(8,2) with decimal.
        char btime[20];   // Registration time, char corresponds to Oracle's date, format: 'yyyy-mm-dd hh24:mi:ss'.
        char memo[301];  // Note, char[301] corresponds to Oracle's varchar2(300).
    } stgirl;

    // Dynamic SQL statement, suitable for SQL executed multiple times.
    stmt.prepare("insert into girls(id,name,weight,btime,memo) \
                             values(:1,:2,:3,to_date(:4,'yyyy-mm-dd hh24:mi:ss'),:5)");   // :1,:2,...,:n can be understood as input parameters.
    stmt.bindin(1,stgirl.id);
    stmt.bindin(2,stgirl.name,30);
    stmt.bindin(3,stgirl.weight);
    stmt.bindin(4,stgirl.btime,19);
    stmt.bindin(5,stgirl.memo,300);          // The length of the string can be unspecified, the default is 2000, this usage is not rigorous and not recommended.

    // Assign values to variables and execute the SQL statement.
    for (int ii=10; ii<15; ii++)
    {
        // Initialize variables.
        memset(&stgirl, 0, sizeof(struct st_girl));

        // Assign values to variables.
        stgirl.id=ii;                                                                // Super girl ID.
        sprintf(stgirl.name,"Xishi%05dgirl",ii);                        // Super girl name.
        stgirl.weight=45.35+ii;                                              // Super girl weight.
        sprintf(stgirl.btime,"2021-08-25 10:33:%02d",ii);            // Registration time.
        sprintf(stgirl.memo,"This is the note for the %05dth super girl.",ii); // Note.

        // Execute the SQL statement, must check the return value: 0-success, others-failure.
        // The failure code is in stmt.m_cda.rc, the failure description is in stmt.m_cda.message.
        if (stmt.execute()!=0)
        {
            printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message()); return -1;
        }

        printf("Successfully inserted %ld records.\n",stmt.rpc());    // stmt.m_cda.rpc is the number of records affected by this SQL execution.
    }


    conn.commit();                // Commit the transaction.

    return 0;
}
