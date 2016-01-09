#pragma once
#include <fstream>
#include "json.h"

#include "GA.h"
#include "snake.h"

struct CmpWeight
{
  double wTurn, wFoodCount, wCloseness, wFailed;

  CmpWeight() : wTurn(-0.01), wFoodCount(10.0), wCloseness(0.1), wFailed(-2.0) {}

  double operator()(SnakeGame *sg) {
    return wTurn*sg->Turn + wFoodCount*sg->FoodCount + wCloseness*(abs(sg->SnakeLocation[0]-sg->Food[2*sg->FoodCount])+abs(sg->SnakeLocation[1]-sg->Food[2*sg->FoodCount+1])) + wFailed*(sg->Result > 0 ? 1 : 0);
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

  void Training(uint32_t N, char const * File = "", uint32_t batch = 0);
  void ShowPlay(bool Print = true);

  void Initialize(uint32_t h, uint32_t w, uint32_t ns, uint32_t nn, uint32_t *nl, double a = -10, double b = 10);
  void SaveProgress(char const * File);
  void LoadProgress(char const * File);
};
