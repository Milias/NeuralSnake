#include "nn.h"

double Sigmoid(double x) { return 1.0/(1.0+exp(-x)); }

NeuralNetwork::NeuralNetwork() : nHidden(0), nInputs(0), nOutputs(0) {}

void NeuralNetwork::Initialize(uint32_t ni, uint32_t nh, uint32_t no)
{
  //std::cout << "Initializing NN.\n";
  nOutputs = no; nHidden = nh; nInputs = ni;
  y = new double[nOutputs];
  h = new double[nHidden];
  t = new double[nHidden];
}

void NeuralNetwork::InitializeWeights(double *xh, double *hh, double *hy)
{
  W_xh = xh; W_hh = hh; W_hy = hy;
  for (uint32_t i = 0; i < nHidden; i++) { h[i] = 0.0; }
}

double* NeuralNetwork::ComputeOutput(double *x)
{
  for (uint32_t j = 0; j < nHidden; j++) { h[j] = 0; t[j] = 0; }

  for (uint32_t i = 0; i < nHidden; i++) {
    for (uint32_t j = 0; j < nInputs; j++) {
      t[i] += W_xh[i*nInputs+j]*x[j];
    }
  }
  for (uint32_t i = 0; i < nHidden; i++) {
    for (uint32_t j = 0; j < nHidden; j++) {
      t[i] += W_hh[i*nHidden+j]*h[j];
    }
  }

  for (uint32_t j = 0; j < nHidden; j++) { h[j] = Sigmoid(t[j]); }

  for (uint32_t i = 0; i < nOutputs; i++) {
    for (uint32_t j = 0; j < nHidden; j++) {
      y[i] += W_hy[i*nOutputs+j]*h[j];
    }
    y[i] = Sigmoid(y[i]);
  }

  return y;
}

RecurrentNeuralNetwork::RecurrentNeuralNetwork() : nNetworks(0) {}

RecurrentNeuralNetwork::~RecurrentNeuralNetwork()
{
  std::cout << "Freeing NNs.\n";
  //delete[] NNs;
}

void RecurrentNeuralNetwork::Initialize(uint32_t nn, uint32_t *nl)
{
  //std::cout << "Initializing RNN.\n";
  nNetworks = nn; NetworkLayout = nl;

  NNs = std::vector<NeuralNetwork>(nNetworks);
  for (uint32_t i = 0; i < nNetworks; i++) {
    NNs[i].Initialize(NetworkLayout[2*i],NetworkLayout[2*i+1],NetworkLayout[2*i+2]);
  }
}

void RecurrentNeuralNetwork::InitializeWeights(double *xh, double *hh, double *hy)
{
  double *txh, *thh, *thy;
  txh = xh; thh = hh; thy = hy;
  for (uint32_t i = 0; i < nNetworks; i++) {
    NNs[i].InitializeWeights(txh,thh,thy);

    txh = txh + NetworkLayout[2*i  ]*NetworkLayout[2*i+1];
    thh = thh + NetworkLayout[2*i+1]*NetworkLayout[2*i+1];
    thy = thy + NetworkLayout[2*i+1]*NetworkLayout[2*i+2];
  }
}

double * RecurrentNeuralNetwork::ComputeOutput(double *x)
{
  GlobalOutput = NNs[0].ComputeOutput(x);
  for (uint32_t i = 1; i < nNetworks; i++) {
    GlobalOutput = NNs[i].ComputeOutput(GlobalOutput);
  }
  return GlobalOutput;
}
