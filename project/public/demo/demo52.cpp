/*
 * Program name: demo52.cpp, this program demonstrates using the development framework's cftpclient class to download files.
 * Author: Wu Congzhou
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
    if (
