#include <iostream>
#include "matrix.h"
#include "vect.h"

using namespace std;

int main(int argc, char **argv)
{
	Vector va, vb;
	Matrix ma, mb, mc;
	cout << "enter matrix a" << endl;
	cin >> ma;

	cout << "End of input" << endl;
	cout << "enter matrix b" << endl;
	cin >> mb;

	cout << "End of input" << endl;
	cout << "enter vecor a" << endl;
	cin >> va;

	cout << ma << endl;
	cout << mb << endl;
	cout << va << endl;
	cout << "mc = ma * mb" << endl;
	mc = ma * mb;
	cout << mc << endl;

	vb = va * ma;

	cout << vb << endl;

	mc.clean();
	ma.clean();
	mb.clean();
	va.clean();
	vb.clean();

	return 0;
}
