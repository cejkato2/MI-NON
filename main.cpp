#include <iostream>
#include <matrix.h>

using namespace std;

int main(int argc, char **argv)
{
  Matrix ma, mb, mc;
  cin >> ma;

  cout << "End of input" << endl;
  cin >> mb;

  cout << "End of input" << endl;

  cout << ma << endl;
  cout << mb << endl;
  mc = ma * mb;
  cout << mc << endl;
  return 0;
}

