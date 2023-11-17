#include <cstring>
#include <iostream>
using namespace std;

void bb(const int bh,const string xm)
{
  char *ptr=nullptr;
  //*ptr=3;
  strcpy(ptr,xm.c_str());
}

void aa(const int no,const string name)
{
  bb(3,"冰冰");
}

int main()
{
  aa(8,"西施");

  return 0;
}