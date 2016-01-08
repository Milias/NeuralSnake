#pragma once
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <functional>
#include <memory>

#include "nn.h"

typedef std::chrono::high_resolution_clock myclock;

struct GenAlgRate
{
  double MutRate;
  uint32_t nSelected;

  GenAlgRate() : MutRate(0.1), nSelected(10) {}
};

template <class T> struct ChromosomeWrapper
{
  T * Object;

  ChromosomeWrapper() : Object(NULL) {}
  ~ChromosomeWrapper() {}
  ChromosomeWrapper(const ChromosomeWrapper<T>& w) : Object(w.Object) {}
  ChromosomeWrapper(ChromosomeWrapper<T>&& w) : Object(w.Object) { w.Object = NULL; }

  ChromosomeWrapper<T>& operator=(const ChromosomeWrapper<T>& w) {
    Object = w.Object;
    w.Object = NULL;
    return *this;
  }

  ChromosomeWrapper<T>& operator=(ChromosomeWrapper<T>&& w) {
    std::swap(Object,w.Object);
    return *this;
  }

  T * operator->() { return Object; }
};

typedef ChromosomeWrapper<RecurrentNeuralNetwork> RNN;

class GeneticAlgorithm
{
private:
  uint32_t Population, nNetworks, *NetworkLayout;
  RNN* Chromosomes;

  std::default_random_engine gen;
  std::uniform_real_distribution<double> rand;
  std::uniform_real_distribution<double> randd;
  std::normal_distribution<double> randn;

  uint32_t nXH, nHH, nHY;
  double *W_xh, *W_hh, *W_hy;

  std::function<bool(RNN&,RNN&)> ChromosomeCmp;

public:
  GenAlgRate GAR;

  GeneticAlgorithm(uint32_t ns, uint32_t nn, uint32_t *nl, std::function<bool(RNN&,RNN&)> cmp, double a = -10, double b = 10);
  ~GeneticAlgorithm();

  void InitializeRandom();
  void Mutation(RNN &c);
  void Crossover(RNN &c1, RNN &c2, RNN &c3);
  void Selection();

  void Simulate(uint32_t N);

  RNN* GetChromosomes() { return Chromosomes; }
};
