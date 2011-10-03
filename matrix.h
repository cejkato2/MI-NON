#ifndef __MATRIX_H
#define __MATRIX_H

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include "vect.h"

using namespace std;

class Vector;

class Matrix
{
  double *data;
  uint32_t dim;
public:
    Matrix ();
   ~Matrix ();

  void setDim (uint32_t newdim);
  uint32_t getDim ();
  void set (uint32_t x, uint32_t y, double val);
  double at (uint32_t x, uint32_t y);
  void reset ();
  void clean ();

  friend istream & operator>> (istream & is, Matrix & b);
  friend ostream & operator<< (ostream & os, Matrix & b);
  Matrix operator* (Matrix op2);
  Vector operator* (Vector op2);
};

#endif
