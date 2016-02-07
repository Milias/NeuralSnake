#include "ap.h"

ArtificialPlayer::ArtificialPlayer() {}

ArtificialPlayer::~ArtificialPlayer()
{
  delete ga;
  delete sg;
  delete Root;
}

double ArtificialPlayer::RNNFitness(RNN &c)
{
  sg->Start();
  while (sg->Result == 0 && sg->Turn < sg->Tiles*sg->Tiles) {
    sg->CheckInput(c->ComputeOutput(sg->Repr));
  }
  sg->End();
  return cw(sg);
}

void ArtificialPlayer::Training(uint32_t N, char const * File, uint32_t batch)
{
  std::cout << "Starting training.\n";
  if (batch) {
    for (uint32_t i = 0; i < N; i++) {
      std::cout << "Batch #" << (i+1) << std::endl;
      ga->Simulate(batch);
      (*Root)["GeneralData"]["Last training"] = static_cast<Json::Value::UInt64>(std::chrono::high_resolution_clock::now().time_since_epoch().count() * std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den);
      (*Root)["Training"]["Generations"] = (*Root)["Training"]["Generations"].asUInt() + batch;
      std::cout << "Saving.\n";
      SaveProgress(File);
    }
  } else {
    ga->Simulate(N);
    (*Root)["GeneralData"]["Last training"] = static_cast<Json::Value::UInt64>(std::chrono::high_resolution_clock::now().time_since_epoch().count() * std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den);
    (*Root)["Training"]["Generations"] = (*Root)["Training"]["Generations"].asUInt() + N;
  }
}

void ArtificialPlayer::ShowPlay(bool Print)
{
  RNN *c = ga->GetChromosomes();

  sg->Start(); double *t;

  if (Print) {
    sg->PrintBoard();
    std::cout << std::endl;
  }
  while (sg->Result == 0 && sg->Turn < sg->Tiles*sg->Tiles) {
    t = (*c)->ComputeOutput(sg->Repr);
    std::cout << "Up: " << t[0] << " Down: " << t[1] << " Left: " << t[2] << " Right: " << t[3] << std::endl;
    sg->CheckInput(t);
    if (Print) {
      sg->PrintBoard();
      std::cout << std::endl;
    }
  }
  sg->End();
}

void ArtificialPlayer::Initialize(uint32_t h, uint32_t w, uint32_t ns, uint32_t nn, uint32_t *nl, double a, double b)
{
  if (sg != NULL) { delete sg; }
  if (ga != NULL) { delete ga; }
  if (Root != NULL) { delete Root; }

  sg = new SnakeGame(h,w);
  sg->Initialize();

  ga = new GeneticAlgorithm(ns,nn,nl,
    std::function<double(RNN&)>(
      std::bind(&ArtificialPlayer::RNNFitness, this, std::placeholders::_1)
    ),a,b);
  ga->InitializeRandom();

  Root = new Json::Value;
  (*Root)["GeneralData"]["Creation date"] = static_cast<Json::Value::UInt64>(std::chrono::high_resolution_clock::now().time_since_epoch().count() * std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den);
  (*Root)["GeneralData"]["Last training"] = (*Root)["GeneralData"]["Creation date"];

  (*Root)["BoardData"]["Width"] = w;
  (*Root)["BoardData"]["Height"] = h;

  (*Root)["NeuralData"]["Number"] = nn;
  (*Root)["NeuralData"]["Layout"] = Json::Value(Json::arrayValue);
  for (uint32_t i = 0; i < 1+2*nn; i++) {
    (*Root)["NeuralData"]["Layout"].append(nl[i]);
  }
  (*Root)["NeuralData"]["W"]["xh"] = Json::Value(Json::arrayValue);
  (*Root)["NeuralData"]["W"]["hh"] = Json::Value(Json::arrayValue);
  (*Root)["NeuralData"]["W"]["hy"] = Json::Value(Json::arrayValue);

  for (uint32_t i = 0; i < ga->nXH; i++) { (*Root)["NeuralData"]["W"]["xh"].append(ga->W_xh[i]); }
  for (uint32_t i = 0; i < ga->nHH; i++) { (*Root)["NeuralData"]["W"]["hh"].append(ga->W_hh[i]); }
  for (uint32_t i = 0; i < ga->nHY; i++) { (*Root)["NeuralData"]["W"]["hy"].append(ga->W_hy[i]); }

  (*Root)["NeuralData"]["Memory"] = Json::Value(Json::arrayValue);

  for (uint32_t j = 0; j < nn; j++) {
    for (uint32_t k = 0; k < nl[2*j+1]; k++) {
      (*Root)["NeuralData"]["Memory"].append(0.0);
    }
  }

  (*Root)["GeneticData"]["Selection"] = ns;
  (*Root)["GeneticData"]["MutRate"] = 0.1;
  (*Root)["Training"]["Generations"] = 0;
}

void ArtificialPlayer::SaveProgress(char const * File)
{
  for (uint32_t i = 0; i < ga->nXH; i++) { (*Root)["NeuralData"]["W"]["xh"][i] = ga->W_xh[i]; }
  for (uint32_t i = 0; i < ga->nHH; i++) { (*Root)["NeuralData"]["W"]["hh"][i] = ga->W_hh[i]; }
  for (uint32_t i = 0; i < ga->nHY; i++) { (*Root)["NeuralData"]["W"]["hy"][i] = ga->W_hy[i]; }

  for (uint32_t j = 0, i = 0; j < (*Root)["NeuralData"]["Number"].asUInt(); j++, i += (*Root)["NeuralData"]["Layout"][2*j+1].asUInt()) {
    for (uint32_t k = 0; k < (*Root)["NeuralData"]["Layout"][2*j+1].asUInt(); k++) {
      (*Root)["NeuralData"]["Memory"][i+k] = ga->GetChromosomes()[0]->NNs[j].h[k];
    }
  }

  Json::StyledStreamWriter writer;
  std::ofstream savefile(File);
  writer.write(savefile, *Root);
  savefile.close();
}

void ArtificialPlayer::LoadProgress(char const * File)
{
  Root = new Json::Value;
  Json::Reader reader;
  std::ifstream savefile(File, std::ifstream::in);
  savefile >> *Root;
  savefile.close();

  sg = new SnakeGame((*Root)["BoardData"]["Height"].asUInt(),(*Root)["BoardData"]["Width"].asUInt());
  sg->Initialize();

  uint32_t * nl = new uint32_t[1+2*(*Root)["NeuralData"]["Number"].asUInt()];
  for (uint32_t i = 0; i < 1+2*(*Root)["NeuralData"]["Number"].asUInt(); i++) {
    nl[i] = (*Root)["NeuralData"]["Layout"][i].asUInt();
  }

  ga = new GeneticAlgorithm(
    (*Root)["GeneticData"]["Selection"].asUInt(),
    (*Root)["NeuralData"]["Number"].asUInt(),
    nl,
    std::function<double(RNN&)>(
      std::bind(&ArtificialPlayer::RNNFitness, this, std::placeholders::_1)
    ),
    (*Root)["GeneticData"]["MutRate"].asUInt()
  );

  for (uint32_t i = 0; i < ga->nXH; i++) { ga->W_xh[i] = (*Root)["NeuralData"]["W"]["xh"][i].asDouble(); }
  for (uint32_t i = 0; i < ga->nHH; i++) { ga->W_hh[i] = (*Root)["NeuralData"]["W"]["hh"][i].asDouble(); }
  for (uint32_t i = 0; i < ga->nHY; i++) { ga->W_hy[i] = (*Root)["NeuralData"]["W"]["hy"][i].asDouble(); }

  double * mem = new double[ga->nH];

  for (uint32_t j = 0, i = 0; j < (*Root)["NeuralData"]["Number"].asUInt(); j++, i += (*Root)["NeuralData"]["Layout"][2*j+1].asUInt()) {
    for (uint32_t k = 0; k < (*Root)["NeuralData"]["Layout"][2*j+1].asUInt(); k++) {
      mem[i+k] = (*Root)["NeuralData"]["Memory"][i+k].asDouble();
    }
  }

  ga->InitializeLoad(mem);

  delete[] mem;
}
