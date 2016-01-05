#pragma once
#include <iostream>
#include "GA.h"
#include "snake.h"

struct CmpWeight
{
  double wTurn, wFoodCount;

  CmpWeight() : wTurn(-0.1), wFoodCount(1.0) {}

  double operator()(SnakeGame *sg) {
    return wTurn*sg->Turn + wFoodCount*sg->FoodCount;
  }
};

class ArtificialPlayer
{
private:
  SnakeGame *sg;
  GeneticAlgorithm *ga;

  std::function<bool(RNN&,RNN&)> *cmp;

  CmpWeight cw;

  bool RNNCompare(RNN &c1, RNN &c2);

public:
  ArtificialPlayer(uint32_t h, uint32_t w, uint32_t ns, uint32_t nn, uint32_t *nl, double a = -10, double b = 10);
  ~ArtificialPlayer();

  void Training(uint32_t N);
  void ShowPlay(bool Print = true);
};
