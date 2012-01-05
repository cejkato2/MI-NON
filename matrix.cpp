#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

	if (this->is_cr == true) {
#pragma omp parallel
		{
#pragma omp for schedule(static, 1000) private(i)
			/* i iterates over rows */
			for (int i = 0; i < dim; ++i) {
				/* j iterates over columns */
				for (int j = this->ci[this->addr[i]]; j < dim;
				     ++j) {
					temp.set(i,
						 temp.at(i) + this->at(j,
								       i) *
						 op2.at(j));
					if (this->restIsNull(j, i) == true) {
						/* rest is 0 increment */
						break;
					}
				}
			}
		}
	} else {
		/* matrix is not compressed */
#pragma omp parallel
		{
#pragma omp for schedule(static, 1000) private(i)
			/* i iterates over rows */
			for (int i = 0; i < dim; ++i) {
				/* j iterates over columns */
				for (int j = 0; j < dim; ++j) {
					temp.set(i,
						 temp.at(i) + this->at(j,
								       i) *
						 op2.at(j));
					if (this->restIsNull(j, i) == true) {
						/* rest is 0 increment */
						break;
					}
				}
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

/**
 * @param i column
 * @param j row
 * @param val value
 */
void Matrix::set(uint32_t i, uint32_t j, double val)
{
	if (is_cr == true) {
		throw "not implemented";
	} else {
		if ((j < dim) && (i < dim) && (this->data != NULL)) {
			this->data[j * dim + i] = val;
		} else {
			throw "M set(): Bad index";
		}
	}
}

void Matrix::setCR(bool cr)
{
	this->is_cr = cr;
}

void Matrix::setNonzero(uint32_t nz)
{
	this->nonzero = nz;
}

void Matrix::genMatrix(double a, double b, double c)
{
	uint32_t coli = 0;
	uint32_t data_index = 0;
	uint32_t addr_index = 0;
	this->is_cr = true;
	// first and last row have only 2
	this->nonzero = (this->dim - 2) * 3 + 5;
	this->reset();

	for (uint32_t i = 0; i < this->dim; ++i) {
		//iterate over rows
		coli = i - 1;
		addr[addr_index++] = data_index;
		if (i > 0) {
			ci[data_index] = coli;
			data[data_index] = a;
			data_index++;
		}
		coli++;
		ci[data_index] = coli++;
		data[data_index] = b;
		data_index++;

		ci[data_index] = coli;
		data[data_index] = c;
		data_index++;
	}
	this->addr[this->dim] = nonzero;
}

/**
 * @param[in] i column
 * @param[in] j row
 * @return true if there are 0 to the end of line without column i
 */
bool Matrix::restIsNull(uint32_t i, uint32_t j)
{
	//(+1) get index of the start of next row
	uint32_t end_row = this->addr[j + 1];
	if (end_row > (nonzero + 1)) {
		cout << "end_row: " << end_row << endl;
		//throw "out of range";
	}
	uint32_t col_endrow = this->ci[end_row - 1];
	if (i > col_endrow) {
		return true;
	} else {
		return false;
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

Matrix Matrix::operator*(double op1)
{
	Matrix temp;
	if (this->is_cr == true) {
		temp.setCR(true);
		temp.copyCR(this->dim, this->nonzero, this->data, this->addr,
			    this->ci);
	} else {
		temp.setCR(false);
	}
	temp.multiply(op1);
	return temp;
}

void Matrix::copyCR(uint32_t dim, uint32_t nonzero, const double *data,
		    const uint32_t * addr, const uint32_t * ci)
{
	if (this->is_cr == true) {
		this->clean();
		this->dim = dim;
		this->nonzero = nonzero;
		this->reset();
		this->data =
		    (double *)memcpy((void *)this->data, (void *)data,
				     nonzero * sizeof(data[0]));
		this->ci =
		    (uint32_t *) memcpy((void *)this->ci, (void *)ci,
					nonzero * sizeof(*ci));
		this->addr =
		    (uint32_t *) memcpy((void *)this->addr, (void *)addr,
					(dim + 1) * sizeof(*addr));
	} else {
		throw "Target is not CR matrix";
	}

}

void Matrix::multiply(double op1)
{
	if (this->is_cr == true) {
		for (uint32_t i = 0; i < this->nonzero; ++i) {
			this->data[i] *= op1;
		}
	} else {
		for (uint32_t i = 0; i < (this->dim * this->dim); ++i) {
			this->data[i] *= op1;
		}
	}
}

Matrix Matrix::operator+(Matrix op1)
{
	Matrix temp;
	if (this->is_cr == true) {
		temp.setCR(true);
		temp.copyCR(op1.getDim(), op1.getNonzero(), op1.getData(),
			    op1.getAddr(), op1.getCi());
	} else {
		temp.setCR(false);
	}
	temp.add(op1.getData());
	return temp;
}

const double *Matrix::getData()
{
	return this->data;
}

void Matrix::add(const double *op1)
{
	if (this->is_cr == true) {
		for (uint32_t i = 0; i < this->nonzero; ++i) {
			this->data[i] += op1[i];
		}
	} else {
		for (uint32_t i = 0; i < (this->dim * this->dim); ++i) {
			this->data[i] += op1[i];
		}
	}
}

uint32_t Matrix::getNonzero()
{
	return this->nonzero;
}

const uint32_t *Matrix::getAddr()
{
	return this->addr;
}

const uint32_t *Matrix::getCi()
{
	return this->ci;
}
