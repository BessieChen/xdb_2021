#include <iostream>
using namespace std;

void show(const char *name1,const char *name2,const char *message)
{
  cout << name1 << "开始表白。\n";
  cout << name2 << ":" << message << endl;
}

int main(int argc,char *argv[],char *envp[])
{
  if (argc!=4)
  {
    cout << "表白神器程序的使用方法：./demo 追求者姓名 被追求者姓名 表白内容\n"; return -1;
  }
  cout << "表白前的准备工作一。\n";
  cout << "表白前的准备工作二。\n";
  cout << "表白前的准备工作三。\n";
  cout << "表白前的准备工作四。\n";
  cout << "表白前的准备工作五。\n";

  show(argv[1],argv[2],argv[3]);

  cout << "表白完成。\n";

  for (int ii=0;ii<2;ii++)
  {
    string str="这是第"+to_string(ii)+"个超级女生。";
    cout << str << endl;
  }

  return 0;
}