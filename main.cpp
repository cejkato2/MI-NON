#include <string.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include "matrix.h"
#include "vect.h"
#include <omp.h>

using namespace std;

Vector solve_conjugate_gradient(Matrix A, Vector b, double epsilon)
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

	std::cerr << "Conjugate gradient" << std::endl;

	// 1)
	xk.setDim(b.getDim());
	xk.reset();		// alloc xk = (0, ..., 0)

	r0 = A * xk;
	rk = b - r0;
	r0.clean();
	s = rk * 1.0;
	uint32_t count = 1;

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
		cerr.width(6);
		cerr.fill(' ');
		cerr << count << ". Iteration |residuum|: " << rknorm << endl;
		count++;

		if ((rknorm / bnorm) < epsilon) {
			break;
		}
		if (count > rk.getDim()) {
			break;
		}
	}

	rk2.clean();
	//rk.clean();
	return xk;
}

Vector solve_steepest_descend(Matrix A, Vector b, double epsilon)
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
	xk.reset();		// alloc xk = (0, ..., 0)

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

	rk.clean();
	return xk;
}

#define TIMESOLVER
#ifdef TIMESOLVER
//time solver
int main(int argc, char **argv)
{
	double k, c, l, n, alpha, t;
	Vector d0, d1, f, v0, v1;
	Vector pom, pom2, vpom;
	Matrix mc, mk, ma, ma2;
	uint32_t countiteration = 20;

	cerr << "Amount of parts n: ";
	cin >> n;
	cerr << "Time step: ";
	cin >> t;
	cerr << "Alpha: ";
	cin >> alpha;

	mc.setDim(n - 1);
	mk.setDim(n - 1);

	mk.genMatrix(1, 2, 1);
	mc.genMatrix(1 / 6.0, 4 / 6.0, 1 / 6.0);

	f.setDim(n - 1);
	d0.setDim(n - 1);
	v0.setDim(n - 1);
	f.reset();
	d0.reset1();
	v0.reset();

	for (uint32_t itk = 0; itk < countiteration; ++itk) {
		/* pom = f - K * (d + t * (1 - alpha) * v) */
		v1 = v0 * (t * (1 - alpha));
		vpom = d0 + v1;
		pom2 = mk * vpom;
		if (itk == 4) {
			f.set(10, 10E29);
		} else if (itk == 5) {
			f.set(10, 0);
		} else {
			cerr << f.at(1) << " ";
		}
		pom = f - pom2;
		pom2.clean();
		vpom.clean();
		v1.clean();

		/* A = C + t * alpha * K */

		ma2 = mk * (t * alpha);
		ma = mc + ma2;
		ma2.clean();

		//cout << ma;
		//cout << pom;
		v1 = solve_conjugate_gradient(ma, pom, 0.0001);
		cout << v1;
		if (itk < countiteration) {
			cout << endl;
		}
		pom.clean();
		ma.clean();

		/* vk+a = (1 - alpha) * vk + alpha * vk+1 */
		pom2 = v0 * (1 - alpha);
		vpom = v1 * alpha;
		pom = pom2 + vpom;
		vpom.clean();
		pom2.clean();

		/* dk+1 = dk + t * vk+a */
		vpom = pom * t;
		d1 = d0 + vpom;
		vpom.clean();
		pom.clean();

		/* move to next iteration */
		d0.clean();
		d0 = d1;
		v0.clean();
		v0 = v1;

	}

	f.clean();
	d0.clean();
	v0.clean();
	mk.clean();
	mc.clean();

	return 0;
}
#else
//CR
int main(int argc, char **argv)
{
	Vector va, vb, vres;
	Matrix ma, mb, mc;

	if (argc != 5) {
		cout << "./non sd|cg <matrix_file> <vector_file> <result_file>"
		    << endl;
		return 0;
	}

	double epsilon = 0.0000000001;
	cout << "Epsilon: ";
	cin >> epsilon;
	ifstream inputfile;
	//inputfile.open("../data/mat_cr_3630.txt");
	inputfile.open(argv[2]);
	ma.load_cr(inputfile);
	inputfile.close();

	//inputfile.open("../data/vektor3630.txt");
	inputfile.open(argv[3]);
	inputfile >> va;
	inputfile.close();

	try {
		if (strncmp(argv[1], "sd", sizeof("sd")) == 0) {
			vres = solve_steepest_descend(ma, va, epsilon);
		} else {
			vres = solve_conjugate_gradient(ma, va, epsilon);
		}

		cerr << "Result: " << endl;
		cerr << vres << endl;	//<< " residuum: " << rk << endl;

		vb.clean();
		va.clean();
		ifstream inputfile;
		inputfile.open(argv[4]);
		if (inputfile.good() == true) {
			inputfile >> vb;
			inputfile.close();
			va = vb - vres;
			cout << "Deviation from result: " << va.norm() << endl;
			va.clean();
			vb.clean();
		}
		vres.clean();
	}
	catch(const char *e) {
		cout << e << endl;
	}
	ma.clean();
	va.clean();
	return 0;
}
#endif

//int main(int argc, char **argv)
//{
//      Vector va, vb;
//      Matrix ma, mb, mc;
//
//        if (argc == 1) {
//          cout << "./non sd|cg" << endl;
//          return 0;
//        }
//
//        double epsilon;
//        cin >> epsilon;
//        cin >> ma;
//        cin >> va;
//
//        try {
//          if (strncmp(argv[1], "sd", sizeof("sd")) == 0) {
//            solve_steepest_descend(ma, va, epsilon);
//          } else {
//            solve_conjugate_gradient(ma, va, epsilon);
//          }
//        } catch (const char *e) {
//          cout << e << endl;
//        }
//      ma.clean();
//      va.clean();
//      return 0;
//}
