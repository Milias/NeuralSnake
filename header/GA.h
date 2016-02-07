#pragma once
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <functional>
#include <memory>

#include "nn.h"

typedef std::chrono::high_resolution_clock myclock;

template <class T> struct ChromosomeWrapper
{
  T * Object;
  double *Fitness, Average;
  uint32_t np;

  ChromosomeWrapper() : Object(NULL), Average(0.0), np(0) {
    Fitness = new double[5];
    for (uint32_t i = 0; i < 5; i++) { Fitness[i] = 0.0; }
  }
  ~ChromosomeWrapper() { delete[] Fitness; }
  ChromosomeWrapper(const ChromosomeWrapper<T>& w) : Object(w.Object), Average(w.Average), np(w.np) {
    Fitness = new double[5];
    for (uint32_t i = 0; i < 5; i++) { Fitness[i] = w.Fitness[i]; }
  }

  ChromosomeWrapper(ChromosomeWrapper<T>&& w) : Object(w.Object), Average(w.Average), np(w.np) {
    w.Object = NULL; w.Average = 0.0; w.np = 0;
    Fitness = new double[5];
    for (uint32_t i = 0; i < 5; i++) { Fitness[i] = w.Fitness[i]; }
  }

  ChromosomeWrapper<T>& operator=(const ChromosomeWrapper<T>& w) {
    Object = w.Object;
    Average = w.Average;
    np = w.np;
    for (uint32_t i = 0; i < 5; i++) { Fitness[i] = w.Fitness[i]; }
    return *this;
  }

  ChromosomeWrapper<T>& operator=(ChromosomeWrapper<T>&& w) {
    std::swap(Object,w.Object);
    for (uint32_t i = 0; i < 5; i++) { std::swap(Fitness[i],w.Fitness[i]); }
    std::swap(Average,w.Average);
    std::swap(np,w.np);
    return *this;
  }

  double ComputeAverage() {
    double t = 0.0;
    for (uint32_t i = 0; i < np; i++) {
      t += Fitness[i];
    } return t/np;
  }

  void AddFitness(double nf) {
    if (np < 5) { Fitness[np++] = nf; }
    else {
      for (uint32_t i = 1; i < 5; i++) {
        Fitness[i-1] = Fitness[i];
      } Fitness[4] = nf;
    }
    Average = ComputeAverage();
  }

  T * operator->() { return Object; }
  bool operator<(const ChromosomeWrapper<T>& w) {
    return Average > w.Average;
  }
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

  std::function<double(RNN&)> Fitness;

public:
  uint32_t nXH, nHH, nHY, nH, nSelected;
  double *W_xh, *W_hh, *W_hy, MutRate;

  GeneticAlgorithm(uint32_t ns, uint32_t nn, uint32_t *nl, std::function<double(RNN&)> fit, double mr = 0.1, double a = -1, double b = 1);
  ~GeneticAlgorithm();

  void InitializeRandom();
  void InitializeLoad(double *mem);
  void Mutation(RNN &c);
  void Crossover(RNN &c1, RNN &c2, RNN &c3);
  void Selection();

  void Simulate(uint32_t N);

  RNN* GetChromosomes() { return Chromosomes; }
};
