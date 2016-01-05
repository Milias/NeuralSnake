#pragma once
#include <random>
#include <chrono>
#include <cstdint>
#include <iostream>

typedef std::chrono::high_resolution_clock myclock;

struct GameResult
{
  uint32_t Height, Width, Turn, FoodCount, Seed;
  uint32_t *SnakeLocation, *Food, *Moves;

  /** Result:
    0 - No errors.
    1 - Out of bounds.
    2 - Self-walking.
    3 - No more room.
  **/
  uint32_t Result;

  GameResult(uint32_t h, uint32_t w, uint32_t t, uint32_t fc, uint32_t s, uint32_t *sl, uint32_t *f, uint32_t *mvs) :
    Height(h), Width(w), Turn(t), FoodCount(fc), Seed(s)
  {
    SnakeLocation = new uint32_t[FoodCount];
    Food = new uint32_t[FoodCount];
    Moves = new uint32_t[Turn];
    for (uint32_t i = 0; i < 2*FoodCount; i++) {
      SnakeLocation[i] = sl[i];
      Food[i] = f[i];
    }
    for (uint32_t i = 0; i < Turn; i++) {
      Moves[i] = mvs[i];
    }
  }

  ~GameResult() {
    delete[] SnakeLocation;
    delete[] Food;
    delete[] Moves;
  }
};

class SnakeGame
{
private:
  const uint32_t Height;
  const uint32_t Width;

  uint32_t *SnakeLocation, *Food, *Moves, *temp;
  bool* Board;

  std::default_random_engine gen;
  std::uniform_int_distribution<uint32_t> randx;
  std::uniform_int_distribution<uint32_t> randy;

  /** GameState:
    0 - NotInitialized
    1 - NotStarted
    2 - Started
    3 - Ended
  **/
  uint32_t GameState;

  GameResult* GR;

public:
  uint32_t Tiles;
  uint32_t Turn;
  uint32_t FoodCount;
  uint32_t Seed;
  uint32_t Result;
  double *Repr;

  SnakeGame(uint32_t h, uint32_t w);
  ~SnakeGame();

  void Initialize(uint32_t s = 0);
  void Start();
  void End();

  void GenerateFood();

  /*** CheckInput
    Moves:  00 - 0.00:0.25 - ^
            01 - 0.25:0.50 - v
            10 - 0.50:0.75 - <
            11 - 0.75:1.00 - >
  ***/
  uint32_t CheckInput(uint32_t Count, uint32_t* Moves);
  uint32_t CheckInput(uint32_t Move);
  uint32_t CheckInput(double Move);

  void NextHeadLoc(uint32_t Move, uint32_t* Pos);
  void MoveSnake(uint32_t* NewPos);
  bool ProcessInput(uint32_t Move);

  void UpdateRepr();
  GameResult* GetGameResult() { return GR; }

  void PrintBoard();
};
