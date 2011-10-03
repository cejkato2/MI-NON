#ifndef __VECT_H
#define __VECT_H

#include <iostream>
#include "matrix.h"

class Matrix;

using namespace std;

class Vector
{
  double *data;
  uint32_t dim;
public:
    Vector();
   ~Vector();

  void setDim(uint32_t newdim);
  uint32_t getDim();
  void set(uint32_t x, double val);
  double at(uint32_t x);
  double norm();
  void reset();
  void clean();

  friend istream & operator>>(istream & is, Vector & b);
  friend ostream & operator<<(ostream & os, Vector & b);

  Vector operator*(Matrix op2);
  double operator*(Vector op2);
  Vector operator*(double op2);
  Vector operator+(Vector op2);
  Vector operator-(Vector op2);

};

#endif
