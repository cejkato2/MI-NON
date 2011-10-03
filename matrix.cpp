#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include "matrix.h"

using namespace std;

ostream & operator<<(ostream & os, Matrix & ia)
{
	os << "Matrix" << endl;
	os << "dim = " << ia.dim << endl;
	if (ia.data == NULL) {
		return os;
	}

	uint32_t dim = ia.getDim();

	for (int j = 0; j < dim; ++j) {
		for (int i = 0; i < dim; ++i) {
			os << ia.at(i, j);
			if (i < (dim - 1)) {
				os << " ";
			}
		}
		os << endl;
	}
	return os;
}

istream & operator>>(istream & is, Matrix & ia)
{
	uint32_t dim;
	is >> dim;

	double val;

	ia.setDim(dim);
	ia.reset();
	for (uint32_t j = 0; j < dim; ++j) {
		for (uint32_t i = 0; i < dim; ++i) {
			is >> val;
			ia.set(i, j, val);
		}
	}

	return is;
}

Matrix Matrix::operator*(Matrix b)
{
	Matrix temp;
	uint32_t dim = this->getDim();

	if (dim != b.getDim()) {
		throw "Not implemented";
	}

	temp.setDim(dim);
	temp.reset();

	for (int a = 0; a < dim; ++a) {
		for (int j = 0; j < dim; ++j) {
			for (int i = 0; i < dim; ++i) {
				temp.set(a, j, temp.at(a, j)
					 + b.at(a, i) * this->at(i, j));
			}
		}
	}

	return temp;
}

void Matrix::setDim(uint32_t newdim)
{
	this->dim = newdim;
	delete[]this->data;
	this->data = NULL;
}

uint32_t Matrix::getDim()
{
	return this->dim;
}

void Matrix::set(uint32_t i, uint32_t j, double val)
{
	if ((j < dim) && (i < dim) && (this->data != NULL)) {
		this->data[j * dim + i] = val;
	} else {
		throw "Bad index";
	}
}

double Matrix::at(uint32_t i, uint32_t j)
{
	if ((j < dim) && (i < dim) && (this->data != NULL)) {
		return this->data[j * dim + i];
	} else {
		throw "Bad index";
	}
}

void Matrix::reset()
{
	if (this->data != NULL) {
		delete[]this->data;
		this->data = NULL;
	}
	if (this->dim > 0) {
		uint64_t count = this->dim * this->dim;
		this->data = new double[count + 1];
		for (uint64_t i = 0; i < count; ++i) {
			this->data[i] = 0;
		}
	}
}

Matrix::Matrix()
{
	this->dim = 0;
	this->data = NULL;
}

Matrix::~Matrix()
{
}

void Matrix::clean()
{
	if (this->data != NULL) {
		delete[]this->data;
		this->data = NULL;
	}
}
