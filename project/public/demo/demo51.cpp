/*
 * Program name: demo51.cpp, this program demonstrates using the development framework's cftpclient class to upload files.
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
        printf("ftp.login(43.136.45.148:21,bftp/092121) failed.\n"); return -1;
    }

    // Create /home/bftp/tmp on the FTP server, note, if the directory already exists, it will return failure.
    if (ftp.mkdir("/home/bftp/tmp") == false) { printf("ftp.mkdir() failed.\n"); return -1; }
  
    // Change the working directory on the FTP server to /home/bftp/tmp
    if (ftp.chdir("/home/bftp/tmp") == false) { printf("ftp.chdir() failed.\n"); return -1; }

    // Upload the local file demo51.cpp to the current working directory on the FTP server.
    if (ftp.put("demo51.cpp","demo51.cpp") == true)
        printf("put demo51.cpp ok.\n");  
    else
        printf("put demo51.cpp failed.\n");  

    // If you do not use chdir to change the working directory, the following code uses the full path to upload the file.
    // ftp.put("/project/public/demo/demo51.cpp","/home/bftp/tmp/demo51.cpp");
}
