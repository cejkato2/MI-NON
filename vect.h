#ifndef __VECT_H
#define __VECT_H

#include <iostream>

using namespace std;

class Vector {
  double *data;
  uint32_t dim;
public:
  Vector();
  ~Vector();

  void setDim(uint32_t newdim);
  uint32_t getDim();
  void set(uint32_t x, double val);
  double at(uint32_t x);
  void reset();

  friend istream &operator>>(istream &is, Vector &b);
  friend ostream &operator<<(ostream &os, Vector &b);
  Vector operator*(Vector op2);

};

#endif

