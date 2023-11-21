/*
 * Program name: demo52.cpp, this program demonstrates using the development framework's cftpclient class to download files.
 * Author: bc
 */
#include "../_ftp.h"

using namespace idc;

int main(int argc,char *argv[])
{
    cftpclient ftp;

    // Log in to the remote FTP server, please change to your own server's IP address.
    if (ftp.login("43.136.45.148:21","bftp","092121") == false)
    {
        printf("ftp.login(43.136.45.148:21,wucz/oracle) failed.\n"); return -1;
    }

    // Download the file /home/wucz/tmp/demo51.cpp from the server to the local machine, save it as /tmp/test/demo51.cpp.
    // If the local directory /tmp/test does not exist, create it.
    if (ftp.get("/home/bftp/tmp/demo51.cpp","/tmp/test/demo51.cpp") == false)
    { 
        printf("ftp.get() failed.\n"); return -1; 
    }

    printf("get /home/bftp/tmp/demo51.cpp ok.\n");  

    /*
    // Delete the file /home/wucz/tmp/demo51.cpp on the server.
    if (ftp.ftpdelete("/home/wucz/tmp/demo51.cpp") == false) { printf("ftp.ftpdelete() failed.\n"); return -1; }

    printf("delete /home/wucz/tmp/demo51.cpp ok.\n");  

    // Delete the directory /home/wucz/tmp on the server, if the directory is not empty, the deletion will fail.
    if (ftp.rmdir("/home/wucz/tmp") == false) { printf("ftp.rmdir() failed.\n"); return -1; }
    */
}
