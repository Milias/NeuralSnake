#pragma once
#include <fstream>
#include <json/json.h>

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
  CmpWeight cw;
  Json::Value *Root;

  bool RNNCompare(RNN &c1, RNN &c2);

public:
  ArtificialPlayer();
  ~ArtificialPlayer();

  void Training(uint32_t N, uint32_t batch = 0);
  void ShowPlay(bool Print = true);

  void Initialize(uint32_t h, uint32_t w, uint32_t ns, uint32_t nn, uint32_t *nl, double a = -10, double b = 10);
  void SaveProgress(char const * File);
  void LoadProgress(char const * File);
};
