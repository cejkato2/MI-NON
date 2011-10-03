#include <iostream>
#include "matrix.h"
#include "vect.h"

using namespace std;

void solve_steepest_descend(Matrix A, Vector b, double epsilon)
{
/*
 * 1) r0 = b - A * x0
 *  1.1) r0 = A * x0
 *  1.2) rk = b - r0
 * 2) alpha = (rT * rk) / (rk * A * rk)
 *  2.1) double al1 = rk * rk
 *  2.2) Vector v = A * rk
 *  2.3) double al2 = rk * v
 *  2.4) alpha = al1 / al2
 * 3) xk+1 = xk + alpha * rk
 *  3.1) xk+1 = alpha * rk
 *  3.2) xk+1 +=  xk
 * 4) rk+1 = rk - alpha * A * rk
 *  4.1) tmpv = A * rk
 *  4.2) tmpv *= alpha
 *  4.3) rk+1 = rk - tmpv
 * 5) if (|rk+1| / |b| < E) break
 */

  Vector r0;
  Vector rk;
  Vector rk2;

  Vector x0;
  Vector xk;
  Vector xk2;

  double al1, al2, alpha;
  double bnorm = b.norm();

  // 1)
  xk.setDim(b.getDim());
  xk.reset(); // alloc xk = (0, ..., 0)

  r0 = A * xk;
  rk = b - r0;
  r0.clean();

  while (true) {
    // 2)
    al1 = rk * rk;
    r0 = A * rk;
    al2 = rk * r0;
    r0.clean();
    alpha = al1 / al2;

    // 3)
    x0 = rk * alpha;
    xk2 = x0 + xk;
    xk.clean();
    x0.clean();
    xk = xk2;

    // 4)
    rk2 = A * rk;
    r0 = rk2 * alpha;
    rk2.clean();
    rk2 = rk - r0;
    r0.clean();
    rk.clean();
    rk = rk2;
    cout << "Iterate Result: " << xk << " residuum: " << rk << endl;
    if ((rk.norm() / bnorm) < epsilon) {
      break;
    }
  }

  cout << "Result: " << xk << " residuum: " << rk << endl;
  xk.clean();
  rk.clean();
}

int main(int argc, char **argv)
{
	Vector va, vb;
	Matrix ma, mb, mc;
	//cout << "enter matrix a" << endl;
	//cin >> ma;

	//cout << "End of input" << endl;
	//cout << "enter matrix b" << endl;
	//cin >> mb;

	//cout << "End of input" << endl;
	//cout << "enter vecor a" << endl;
	//cin >> va;

	//cout << ma << endl;
	//cout << mb << endl;
	//cout << va << endl;
	//cout << "mc = ma * mb" << endl;
	//mc = ma * mb;
	//cout << mc << endl;

	//vb = va * ma;

	//cout << vb << endl;

        //cout << "scalar mult: " << va << "*" << vb << " = " << va * vb << endl;
        //vb.clean();
        //vb = va * 2.0;
        //cout << "scalar mult: " << vb << endl;

	//mb.clean();
	//mc.clean();
	//va.clean();
        //

        double epsilon;
        cin >> epsilon;
        cin >> ma;
        cin >> va;

        try {
          solve_steepest_descend(ma, va, epsilon);
        } catch (const char *e) {
          cout << e << endl;
        }
	ma.clean();
	va.clean();
	return 0;
}
