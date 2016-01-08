#include "GA.h"

GeneticAlgorithm::GeneticAlgorithm(uint32_t ns, uint32_t nn, uint32_t *nl, std::function<bool(RNN&,RNN&)> cmp, double a, double b) : Population(ns*ns), nNetworks(nn), nXH(0), nHH(0), nHY(0), ChromosomeCmp(cmp)
{
  gen.seed(myclock::now().time_since_epoch().count());
  rand = std::uniform_real_distribution<double>(0.0,1.0);
  randd = std::uniform_real_distribution<double>(a,b);
  randn = std::normal_distribution<double>(0.0,1.0);

  NetworkLayout = new uint32_t[1+2*nn];
  for (uint32_t i = 0; i < 1+2*nn; i++) { NetworkLayout[i] = nl[i]; }

  Chromosomes = new RNN[Population];
  for (uint32_t i = 0; i < Population; i++) {
    Chromosomes[i].Object = new RecurrentNeuralNetwork();
    Chromosomes[i]->Initialize(nn,nl);
  }

  for (uint32_t i = 0; i < nNetworks; i++) {
    nXH += NetworkLayout[2*i  ]*NetworkLayout[2*i+1];
    nHH += NetworkLayout[2*i+1]*NetworkLayout[2*i+1];
    nHY += NetworkLayout[2*i+1]*NetworkLayout[2*i+2];
  }

  W_xh = new double[Population*nXH];
  W_hh = new double[Population*nHH];
  W_hy = new double[Population*nHY];

  GAR.nSelected = ns;
}

GeneticAlgorithm::~GeneticAlgorithm()
{
  delete[] Chromosomes;
  delete[] W_xh;
  delete[] W_hh;
  delete[] W_hy;
}

void GeneticAlgorithm::InitializeRandom()
{
  for (uint32_t i = 0; i < Population*nXH; i++) { W_xh[i] = randd(gen); }
  for (uint32_t i = 0; i < Population*nHH; i++) { W_hh[i] = randd(gen); }
  for (uint32_t i = 0; i < Population*nHY; i++) { W_hy[i] = randd(gen); }

  for (uint32_t i = 0; i < Population; i++) {
    Chromosomes[i]->InitializeWeights(W_xh+i*nXH,W_hh+i*nHH,W_hy+i*nHY);
  }
}

void GeneticAlgorithm::Mutation(RNN &c)
{
  for (uint32_t i = 0; i < c->nNetworks; i++) {
    for (uint32_t j = 0; j < c->NNs[i].nInputs*c->NNs[i].nHidden; j++) {
      if (rand(gen) < GAR.MutRate) {
        c->NNs[i].W_xh[j] += randn(gen);
      }
    }

    for (uint32_t j = 0; j < c->NNs[i].nHidden*c->NNs[i].nHidden; j++) {
      if (rand(gen) < GAR.MutRate) {
        c->NNs[i].W_hh[j] += randn(gen);
      }
    }

    for (uint32_t j = 0; j < c->NNs[i].nHidden*c->NNs[i].nOutputs; j++) {
      if (rand(gen) < GAR.MutRate) {
        c->NNs[i].W_hy[j] += randn(gen);
      }
    }
  }
}

void GeneticAlgorithm::Crossover(RNN &c1, RNN &c2, RNN &c3)
{
  for (uint32_t i = 0; i < c3->nNetworks; i++) {
    for (uint32_t j = 0; j < c3->NNs[i].nInputs*c3->NNs[i].nHidden; j++) {
      if (rand(gen) < GAR.MutRate) {
        c3->NNs[i].W_xh[j] = c1->NNs[i].W_xh[j];
      } else {
        c3->NNs[i].W_xh[j] = c2->NNs[i].W_xh[j];
      }
    }

    for (uint32_t j = 0; j < c3->NNs[i].nHidden*c3->NNs[i].nHidden; j++) {
      if (rand(gen) < GAR.MutRate) {
        c3->NNs[i].W_hh[j] = c1->NNs[i].W_hh[j];
      } else {
        c3->NNs[i].W_hh[j] = c2->NNs[i].W_hh[j];
      }
    }

    for (uint32_t j = 0; j < c3->NNs[i].nHidden*c3->NNs[i].nOutputs; j++) {
      if (rand(gen) < GAR.MutRate) {
        c3->NNs[i].W_hy[j] = c1->NNs[i].W_hy[j];
      } else {
        c3->NNs[i].W_hy[j] = c2->NNs[i].W_hy[j];
      }
    }
  }
}

void GeneticAlgorithm::Selection()
{
  std::sort(Chromosomes, Chromosomes+Population, ChromosomeCmp);
  for (uint32_t i = 0; i < GAR.nSelected; i++) {
    for (uint32_t j = 0; j < GAR.nSelected; j++) {
      Crossover(Chromosomes[i],Chromosomes[j],Chromosomes[i*GAR.nSelected+j]);
      if (i!=j) Mutation(Chromosomes[i*GAR.nSelected+j]);
    }
  }
}

void GeneticAlgorithm::Simulate(uint32_t N)
{
  for (uint32_t i = 0; i < N; i++) {
    std::cout << "Generation " << i << " of " << N << ".\n";
    Selection();
  }
  std::sort(Chromosomes, Chromosomes+Population, ChromosomeCmp);
}
