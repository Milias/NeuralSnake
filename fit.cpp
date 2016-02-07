#include "fit.h"

CurveFitting::CurveFitting() {}

CurveFitting::~CurveFitting()
{
  delete ga;
  delete Root;
}

double CurveFitting::FitnessFunction(RNN &c)
{
  return 0.0;
}
