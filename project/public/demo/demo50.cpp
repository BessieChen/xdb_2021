/*
 * Program name: demo50.cpp, this program demonstrates using the development framework's cftpclient class to get the file list, time, and size from the FTP server.
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
        printf("ftp.login(43.136.45.148:21,bftp) failed.\n"); return -1;
    }


    // Get the file list of /project/public/*.h on the server, save it in the local /tmp/list/tmp.list file.
    // If the /tmp/list directory does not exist, it will be created automatically.
    if (ftp.nlist("public/*.h","/tmp/list/tmp.list") == false) 
    { 
        printf("ftp.nlist() failed.\n"); return -1; 
    }
    cout << "ret=" << ftp.response() << endl;

    cifile ifile;    // Use the cifile class of the development framework to operate on the list file.
    string strFileName;

    ifile.open("/tmp/list/tmp.list");  // Open the list file.

    while(true)    // Get the time and size of each file.
    {
        if (ifile.readline(strFileName) == false) break;

        ftp.mtime(strFileName); // Get the file time.
        ftp.size(strFileName);  // Get the file size.
  
        printf("filename=%s,mtime=%s,size=%d\n",strFileName.c_str(),ftp.m_mtime.c_str(),ftp.m_size);   
    }
}
