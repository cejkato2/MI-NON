#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "vect.h"
#include "matrix.h"
#include <omp.h>

using namespace std;

ostream & operator<<(ostream & os, Vector & ia)
{
	uint32_t dim = ia.getDim();
	if (ia.data == NULL) {
		return os;
	}

	for (int i = 0; i < dim; ++i) {
		os << ia.at(i) << " ";
	}
	//os << endl;
	return os;
}

istream & operator>>(istream & is, Vector & ia)
{
	uint32_t dim;
	is >> dim;

	double val;

	ia.setDim(dim);
	ia.reset();
	for (uint32_t i = 0; i < dim; ++i) {
		is >> val;
		ia.set(i, val);
	}

	return is;
}

/**
 * scalar multiplication
 */
double Vector::operator*(Vector op2)
{
	double temp = 0;
	uint32_t dim = this->getDim();
	for (int i = 0; i < dim; ++i) {
		temp += this->at(i) * op2.at(i);
	}
	return temp;
}

Vector Vector::operator*(Matrix op2)
{
	Vector temp;
	uint32_t dim = this->getDim();

	if (dim != op2.getDim()) {
		throw "Not implemented";
	}

	temp.setDim(dim);
	temp.reset();

	int j = 0;

#pragma omp parallel
	{
#pragma omp for schedule(static, 500) private (j)
		for (j = 0; j < dim; ++j) {
			for (int i = 0; i < dim; ++i) {
				temp.set(i,
					 temp.at(i) + this->at(i) * op2.at(j,
									   i));
			}
		}
	}

	return temp;
}

Vector Vector::operator-(Vector op2)
{
	Vector temp;
	uint32_t dim = this->getDim();

	temp.setDim(dim);
	temp.reset();

#pragma omp parallel for schedule(dynamic, 2)
	for (int i = 0; i < dim; ++i) {
		temp.set(i, this->at(i) - op2.at(i));
	}
	return temp;
}

Vector Vector::operator+(Vector op2)
{
	Vector temp;
	uint32_t dim = this->getDim();

	temp.setDim(dim);
	temp.reset();

#pragma omp parallel for schedule(dynamic, 2)
	for (int i = 0; i < dim; ++i) {
		temp.set(i, this->at(i) + op2.at(i));
	}
	return temp;
}

Vector Vector::operator*(double op2)
{
	Vector temp;
	uint32_t dim = this->getDim();

	temp.setDim(dim);
	temp.reset();

#pragma omp parallel for schedule(dynamic, 2)
	for (int i = 0; i < dim; ++i) {
		temp.set(i, this->at(i) * op2);
	}

	return temp;
}

void Vector::setDim(uint32_t newdim)
{
	this->dim = newdim;
	delete[]this->data;
	this->data = NULL;
}

uint32_t Vector::getDim()
{
	return this->dim;
}

void Vector::set(uint32_t i, double val)
{
	if ((i < dim) && (this->data != NULL)) {
		this->data[i] = val;
	} else {
		throw "V set() Bad index";
	}
}

double Vector::at(uint32_t i)
{
	if ((i < dim) && (this->data != NULL)) {
		return this->data[i];
	} else {
		throw "V at() Bad index";
	}
}

void Vector::reset()
{
	if (this->data != NULL) {
		delete[]this->data;
		this->data = NULL;
	}
	if (this->dim > 0) {
		this->data = new double[this->dim];
		for (uint64_t i = 0; i < this->dim; ++i) {
			this->data[i] = 0;
		}
	}
}

void Vector::reset1()
{
	reset();
	for (uint32_t i = 0; i < dim; ++i) {
		data[i] = 1;
	}
}

Vector::Vector()
{
	this->dim = 0;
	this->data = NULL;
}

Vector::~Vector()
{
}

void Vector::clean()
{
	if (this->data != NULL) {
		delete[]this->data;
		this->data = NULL;
	}
}

double Vector::norm()
{
	double normal = 0;

	for (int i = 0; i < this->dim; ++i) {
		normal += data[i] * data[i];
	}

	return sqrt(normal);
}
