#include <unistd.h>
#include <iostream>
using namespace std;

void bb(const int bh,const string xm)
{
  for (int ii=0;ii<1000000;ii++)
  {
    sleep(1);
    cout << "ii=" << ii << endl;
  }
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