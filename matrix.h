#ifndef __MATRIX_H
#define __MATRIX_H

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include "vect.h"

using namespace std;

class Vector;

class Matrix {
	double *data;
	uint32_t *addr;
	uint32_t *ci;
	uint32_t dim;
	uint32_t nonzero;
	bool is_cr;
 public:
	 Matrix();
	~Matrix();

	void load_cr(istream & is);
	void setDim(uint32_t newdim);
	uint32_t getDim();
	uint32_t getNonzero();
	const uint32_t *getAddr();
	const uint32_t *getCi();
	void setCR(bool cr);
	void setNonzero(uint32_t nz);
	void set(uint32_t x, uint32_t y, double val);
	double at(uint32_t x, uint32_t y);
	bool getIsCR();
	bool restIsNull(uint32_t i, uint32_t j);
	void genMatrix(double a, double b, double c);
	void reset();
	void clean();
	void copyCR(uint32_t dim, uint32_t nonzero, const double *data,
		    const uint32_t * addr, const uint32_t * ci);
	void multiply(double op1);
	void add(const double *op1);
	const double *getData();

	friend istream & operator>>(istream & is, Matrix & b);
	friend ostream & operator<<(ostream & os, Matrix & b);
	Matrix operator*(Matrix op2);
	Vector operator*(Vector op2);
	Matrix operator*(double op1);
	Matrix operator+(Matrix op1);
};

#endif
