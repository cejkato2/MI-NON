#include <string.h>
#include <getopt.h>
#include <iostream>
#include "matrix.h"
#include "vect.h"

using namespace std;

void solve_conjugate_gradient(Matrix A, Vector b, double epsilon)
{
/*
 * 1) x0
 * 2) r0 = b - A*x0
 *  2.1) rk = A*x0
 *  2.2) r0 = b - rk
 * 3) s0 = r0
 * 4) rk = r0
 *
 * cycle:
 * 5) alpha = (rk * rk) / (s*A*s)
 *  5.1) al1 = rk * rk
 *  5.2) r0 = A*s
 *  5.3) al2 = s * r0
 *  5.4) alpha = al1 / al2
 * 6) xk = xk + alpha * s
 * 7) rk2 = rk - alpha * A * s
 * 8) alpha = (rk2*rk2) / (rk*rk)
 * 9) s = rk2 + alpha * s
 *10) rk = rk2
 */

  Vector r0;
  Vector rk;
  Vector rk2;

  Vector x0;
  Vector xk;
  Vector xk2;
  Vector s;

  double al1, al2, alpha;
  double bnorm = b.norm();

  std::cout << "Conjugate gradient" << std::endl;

  // 1)
  xk.setDim(b.getDim());
  xk.reset(); // alloc xk = (0, ..., 0)

  r0 = A * xk;
  rk = b - r0;
  r0.clean();
  s = rk * 1.0;

  while (true) {
    // 5)
    al1 = rk * rk;
    r0 = A * s;
    al2 = s * r0;
    r0.clean();
    alpha = al1 / al2;

    // 6) xk2
    x0 = s * alpha;
    xk2 = x0 + xk;
    x0.clean();
    xk.clean();
    xk = xk2;

    // 7) rk2
    rk2 = A * s;
    r0 = rk2 * alpha;
    rk2.clean();
    rk2 = rk - r0;
    r0.clean();
   
    // 8) beta
    al1 = rk2 * rk2;
    al2 = rk * rk;
    alpha = al1 / al2;

    // 9) s2
    r0 = s * alpha;
    s.clean();
    s = rk2 + r0;
    r0.clean();

    rk.clean();
    rk = rk2;
    double rknorm = rk.norm();
    cerr << "Iterate Result  norma residuum: " << rknorm << endl;
    if ((rknorm / bnorm) < epsilon) {
      break;
    }
  }

  cout << "Result: " << xk << " residuum: " << rk << endl;
  xk.clean();
  rk.clean();
}

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

  std::cout << "Steepest descend" << std::endl;
  
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
    double rknorm = rk.norm();
    cerr << "Iterate Result  residuum: " << rknorm << endl;
    if ((rknorm / bnorm) < epsilon) {
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

        getopt

        if (argc == 1) {
          cout << "./non sd|cg" << endl;
          return 0;
        }

        double epsilon;
        cin >> epsilon;
        cin >> ma;
        cin >> va;

        try {
          if (strncmp(argv[1], "sd", sizeof("sd")) == 0) {
            solve_steepest_descend(ma, va, epsilon);
          } else {
            solve_conjugate_gradient(ma, va, epsilon);
          }
        } catch (const char *e) {
          cout << e << endl;
        }
	ma.clean();
	va.clean();
	return 0;
}
