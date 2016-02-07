#pragma once
#include <fstream>
#include "json.h"

#include "GA.h"

class CurveFitting
{
private:
  GeneticAlgorithm *ga;
  Json::Value *Root;

  double FitnessFunction(RNN &c);

public:
  CurveFitting();
  ~CurveFitting();

  void Training(uint32_t N, char const * File = "", uint32_t batch = 0);

  void Initialize(uint32_t h, uint32_t w, uint32_t ns, uint32_t nn, uint32_t *nl, double a = -10, double b = 10);
  void SaveProgress(char const * File);
  void LoadProgress(char const * File);
};
