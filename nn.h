#pragma once
#include <iostream>
#include <cmath>
#include <json/json.h>

double Sigmoid(double x);

class NeuralNetwork
{
public:
  double *W_xh, *W_hh, *W_hy, *h, *t, *y;
  uint32_t nHidden, nInputs, nOutputs;

  NeuralNetwork();
  ~NeuralNetwork() {
    delete[] h;
  }

  void Initialize(uint32_t ni, uint32_t nh, uint32_t no);
  void InitializeWeights(double *xh, double *hh, double *hy);
  double* ComputeOutput(double *x);
};

class RecurrentNeuralNetwork
{
private:
  /**
    0 - Inputs[0]
    1 - Hidden[0]
    2 - Outputs[0], Inputs[1]
    3 - Hidden[1]
    4 - Outputs[1], Inputs[2]
    ...
  **/
  uint32_t *NetworkLayout;

public:
  NeuralNetwork* NNs;
  uint32_t nNetworks;
  double *GlobalOutput;

  RecurrentNeuralNetwork();
  ~RecurrentNeuralNetwork();

  RecurrentNeuralNetwork(RecurrentNeuralNetwork&& rnn)
  : NetworkLayout(rnn.NetworkLayout),
    NNs(rnn.NNs),
    nNetworks(rnn.nNetworks),
    GlobalOutput(rnn.GlobalOutput)
  {
    rnn.NetworkLayout = NULL;
    rnn.NNs = NULL;
    rnn.nNetworks = 0;
    rnn.GlobalOutput = NULL;
  }

  RecurrentNeuralNetwork& operator=(RecurrentNeuralNetwork&& rnn) {
    std::swap(NetworkLayout,rnn.NetworkLayout);
    std::swap(NNs,rnn.NNs);
    std::swap(nNetworks,rnn.nNetworks);
    std::swap(GlobalOutput,rnn.GlobalOutput);
    return *this;
  }

  void Initialize(uint32_t nn, uint32_t *nl);
  void InitializeWeights(double *xh, double *hh, double *hy);
  double * ComputeOutput(double *x);
};
