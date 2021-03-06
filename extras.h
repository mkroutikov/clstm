// -*- C++ -*-

// Additional functions and utilities for CLSTM networks.
// These may use the array classes from "multidim.h"

#ifndef ocropus_clstm_extras_
#define ocropus_clstm_extras_

#include "clstm.h"
#include <string>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <map>
#include <stdarg.h>
#include <glob.h>
#include "pstring.h"
#include <iostream>

namespace ocropus {
using std::string;
using std::wstring;
using std::shared_ptr;
using std::vector;
using std::cout;
using std::ostream;
using std::cerr;
using std::endl;
using std::min;

// text line normalization

struct INormalizer {
  int target_height = 48;
  float smooth2d = 1.0;
  float smooth1d = 0.3;
  float range = 4.0;
  float vscale = 1.0;
  virtual ~INormalizer() {}
  virtual void getparams(bool verbose = false) {}
  virtual void measure(Tensor<float,2> &line) = 0;
  virtual void normalize(Tensor<float,2> &out, Tensor<float,2> &in) = 0;
  virtual void setPyServer(void *p) {}
};

INormalizer *make_Normalizer(const string &);
INormalizer *make_NoNormalizer();
INormalizer *make_MeanNormalizer();
INormalizer *make_CenterNormalizer();

void read_png(Tensor<unsigned char,3> &image, FILE *fp);
void write_png(FILE *fp, Tensor<float,3> &image);
void read_png(Tensor<float,2> &image, const char *name);
void write_png(const char *name, Tensor<float,2> &image);
}

#endif
