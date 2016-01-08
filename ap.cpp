#include "ap.h"

ArtificialPlayer::ArtificialPlayer(uint32_t h, uint32_t w, uint32_t ns, uint32_t nn, uint32_t *nl, double a, double b)
{
  sg = new SnakeGame(h,w);
  sg->Initialize();

  //cmp = new std::function<bool(RNN&,RNN&)>(std::bind(&ArtificialPlayer::RNNCompare, this, std::placeholders::_1, std::placeholders::_2));

  ga = new GeneticAlgorithm(ns,nn,nl,std::function<bool(RNN&,RNN&)>(std::bind(&ArtificialPlayer::RNNCompare, this, std::placeholders::_1, std::placeholders::_2)),a,b);
  ga->InitializeRandom();
}

ArtificialPlayer::~ArtificialPlayer()
{
  delete ga;
  delete sg;
}

bool ArtificialPlayer::RNNCompare(RNN &c1, RNN &c2)
{
  double t1, t2;

  if (&c1 < ga->GetChromosomes() || &c2 < ga->GetChromosomes()) return false;

  sg->Start();
  while (sg->Result == 0 && sg->Turn < sg->Tiles*sg->Tiles) {
    //std::cout << "&c1: " << &c1 << " c1.Object: " << c1.Object << std::endl;
    sg->CheckInput(*c1->ComputeOutput(sg->Repr));
  }
  sg->End();
  t1 = cw(sg);

  sg->Start();
  while (sg->Result == 0 && sg->Turn < sg->Tiles*sg->Tiles) {
    //std::cout << "&c2: " << &c2 << " c2.Object: " << c2.Object << std::endl;
    sg->CheckInput(*c2->ComputeOutput(sg->Repr));
  }
  //std::cout << std::endl;
  sg->End();
  t2 = cw(sg);

  return t1>t2;
}

void ArtificialPlayer::Training(uint32_t N)
{
  std::cout << "Starting training.\n";
  ga->Simulate(N);
}

void ArtificialPlayer::ShowPlay(bool Print)
{
  RNN *c = ga->GetChromosomes();

  sg->Start();
  while (sg->Result == 0 && sg->Turn < sg->Tiles*sg->Tiles) {
    //std::cout << &c->NNs[0] << std::endl;
    sg->CheckInput(*(*c)->ComputeOutput(sg->Repr));
    if (Print) {
      sg->PrintBoard();
      std::cout << std::endl;
    }
  }
  sg->End();
}
