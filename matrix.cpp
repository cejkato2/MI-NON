#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include "matrix.h"
#include "vect.h"
#include <omp.h>

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

bool Matrix::getIsCR()
{
	return is_cr;
}

void Matrix::load_cr(istream & is)
{
	uint32_t dim;
	is >> dim;
	is >> nonzero;

	double val;

	this->is_cr = true;
	this->setDim(dim);
	this->reset();
	uint32_t row, column, prev_row;
	uint32_t addr_index;
	uint32_t data_index;

	prev_row = addr_index = data_index = 0;
	for (data_index = 0; data_index < nonzero; ++data_index) {
		if (is.good()) {
			is >> row;
			is >> column;
			is >> val;
			if (data_index == nonzero - 1) {
				cout << "row " << row << " col " << column <<
				    " val " << val << endl;
			}
			if (prev_row != row) {
				/* beginning of the new row */
				this->addr[addr_index++] = data_index;
			}
			this->ci[data_index] = column - 1;
			this->data[data_index] = val;
			prev_row = row;
		} else {
			cerr << "could not read from file properly" << endl;
			break;
		}
	}
	this->ci[data_index] = nonzero + 1;
}

istream & operator>>(istream & is, Matrix & ia)
{
	ia.is_cr = false;
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
	if ((this->is_cr == true) || (b.is_cr == true)) {
		throw "CR multiplication not implemented";
	}
	Matrix temp;
	uint32_t dim = this->getDim();

	if (dim != b.getDim()) {
		throw "Not implemented";
	}

	temp.setDim(dim);
	temp.reset();

#pragma omp for schedule(dynamic,100) nowait
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

//Vector Matrix::operator*(Vector op2)
//{
//  Vector temp;
//  uint32_t index2 = 0;
//
//  uint32_t dim = this->getDim();
//  temp.setDim(dim);
//  temp.reset();
//  uint32_t bord1, bord2;
//
//  bord1 = dim/4;
//  bord2 = dim%4;
//
//  int i, tid;
//
//  int nthreads;
//  cout << "mult threads: " << omp_get_num_threads() << endl;
//  #pragma omp parallel
//  {
//    tid = omp_get_thread_num();
//    cout << "Hello thread = " << tid << endl;
//
//    /* Only master thread does this */
//    if (tid == 0) 
//    {
//      nthreads = omp_get_num_threads();
//      cout << "Number of threads " << nthreads << endl;
//    }
//    #pragma omp for schedule(static, 100) private(i) nowait
//    for (i=0; i<dim; ++i) {
//      for (int j=0; j<bord1; ++j) {
//        index2 = 8 * j;
//        temp.set(i, temp.at(i) + this->at(i, index2) * op2.at(index2));
//        temp.set(i, temp.at(i) + this->at(i, index2+1) * op2.at(index2+1));
//        temp.set(i, temp.at(i) + this->at(i, index2+2) * op2.at(index2+2));
//        temp.set(i, temp.at(i) + this->at(i, index2+3) * op2.at(index2+3));
//      }
//
//    }
//  }
//
//  for (i=0; i<dim; ++i) {
//    index2 = bord1 * 4;
//    for (int j=0, index2=bord1*4; j<bord2; ++j, ++index2) {
//      temp.set(i, temp.at(i) + this->at(i, index2) * op2.at(index2));
//    }
//  }
//
//  return temp;
//}
Vector Matrix::operator*(Vector op2)
{
	Vector temp;
	uint32_t index2 = 0;

	uint32_t dim = this->getDim();
	temp.setDim(dim);
	temp.reset();

	int i, tid;

	int nthreads;

#pragma omp parallel
	{
#pragma omp for schedule(static, 1000) private(i)
		for (int i = 0; i < dim; ++i) {
			for (int j = 0; j < dim; ++j) {
				temp.set(i,
					 temp.at(i) + this->at(j,
							       i) * op2.at(j));
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
	if (is_cr == true) {

	} else {
		if ((j < dim) && (i < dim) && (this->data != NULL)) {
			this->data[j * dim + i] = val;
		} else {
			throw "M set(): Bad index";
		}
	}
}

/**
 * @param[in] i column
 * @param[in] j row
 */
double Matrix::at(uint32_t i, uint32_t j)
{
	uint32_t index;
	uint32_t last_index;
	if (this->is_cr == true) {
		index = this->addr[j];
		if (j >= (this->dim - 1)) {
			last_index = this->nonzero;
		} else {
			last_index = this->addr[j + 1] + 1;
		}
		//last_index = this->addr[j+1]+1;

		for (uint32_t p = index; p < last_index; ++p) {
			if (ci[p] == i) {
				return data[p];
			}
			if (ci[p] > i) {
				break;
			}
		}
		return 0.0;

	} else {
		if ((j < dim) && (i < dim) && (this->data != NULL)) {
			return this->data[j * dim + i];
		} else {
			throw "M at(): Bad index";
		}
	}
}

void Matrix::reset()
{
	clean();
	if (this->dim > 0) {
		if (is_cr == true) {
			this->data = new double[nonzero];
			this->ci = new uint32_t[nonzero];
			this->addr = new uint32_t[dim + 1];
		} else {
			uint64_t count = this->dim * this->dim;
			this->data = new double[count + 1];
			for (uint64_t i = 0; i < count; ++i) {
				this->data[i] = 0;
			}
		}
	}
}

Matrix::Matrix()
{
	this->dim = 0;
	this->data = NULL;
	this->addr = NULL;
	this->ci = NULL;
}

Matrix::~Matrix()
{
	// cleaning has to be done manually, NOT by destructor
}

void Matrix::clean()
{
	if (this->data != NULL) {
		delete[]this->data;
		this->data = NULL;
	}

	if (this->addr != NULL) {
		delete[]this->addr;
		this->data = NULL;
	}

	if (this->ci != NULL) {
		delete[]this->ci;
		this->ci = NULL;
	}
}
