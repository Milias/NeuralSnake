#include "snake.h"

uint64_t SplitBy2(uint64_t x)
{
  x &= 0xffffffff;
  x = (x | (x << 16)) & 0xffff0000ffff;
  x = (x | (x << 8)) & 0xff00ff00ff00ff;
  x = (x | (x << 4)) & 0xf0f0f0f0f0f0f0f;
  x = (x | (x << 2)) & 0x3333333333333333;
  x = (x | (x << 1)) & 0x5555555555555555;
  return x;
}

uint64_t Morton2D(uint64_t x, uint64_t y)
{
  return SplitBy2(x) | (SplitBy2(y) << 1);
}

SnakeGame::SnakeGame(uint32_t h, uint32_t w) : Height(h), Width(w), GameState(0), Tiles(h*w), Turn(0), FoodCount(0), Result(0)
{
  SnakeLocation = new uint32_t[2*Tiles];
  Food = new uint32_t[2*Tiles];
  Board = new bool[Tiles];
  Moves = new uint32_t[Tiles];
  randx = std::uniform_int_distribution<uint32_t>(0,Width-1);
  randy = std::uniform_int_distribution<uint32_t>(0,Height-1);
  Repr = new double[Tiles];
  temp = new uint32_t[2];
}

SnakeGame::~SnakeGame()
{
  delete[] SnakeLocation;
  delete[] Food;
  delete[] Moves;
  delete[] Board;
  delete[] Repr;
  delete[] temp;
}

void SnakeGame::Initialize(uint32_t s)
{
  if(GameState == 2) return;
  if (s) { Seed = s; } else { Seed = myclock::now().time_since_epoch().count(); }
  gen.seed(Seed);
  //std::cout << "Seed: " << Seed << std::endl << std::endl;
  GameState = 1;
}

void SnakeGame::Start()
{
  if(GameState != 1 && GameState != 3) return;
  FoodCount = 0; Turn = 0; Result = 0;
  SnakeLocation[0] = randx(gen); SnakeLocation[1] = randy(gen);
  for (uint32_t i = 0; i < Tiles; i++) { Board[i] = false; }
  Board[SnakeLocation[0]*Width+SnakeLocation[1]] = true;
  GenerateFood();
  GameState = 2;
}

void SnakeGame::End()
{
  if(GameState != 2) return;
  GameState = 3;
}

void SnakeGame::GenerateFood()
{
  if (FoodCount < Tiles) {
    std::uniform_int_distribution<uint32_t> tdist(0,Tiles-FoodCount);
    uint32_t tint = tdist(gen);
    for (uint32_t i = 0, j = 0; i < Tiles; i++) {
      if (Board[i])  { continue; }
      else {
        if (j==tint) {
          Food[2*FoodCount] = i/Width;
          Food[2*FoodCount+1] = i-Food[2*FoodCount]*Width;
          break;
        } else { j++; }
      }
    }
  }
}

uint32_t SnakeGame::CheckInput(uint32_t Count, uint32_t* mvs)
{
  if(GameState != 2) return 0;
  for (uint32_t t = 0; t < Count; t++, Turn++) {
    if (mvs[t] > 4) { Turn--; continue; }
    if (!ProcessInput(mvs[t])) break;
  }
  return Result;
}

uint32_t SnakeGame::CheckInput(uint32_t Move)
{
  if(GameState != 2) return 0;
  if (Move > 4) { return 0; }
  ProcessInput(Move);
  Turn++;
  return Result;
}

uint32_t SnakeGame::CheckInput(double *Move)
{
  if(GameState != 2) return 0;
  return CheckInput(std::max_element(Move,Move+4)-Move);
}

void SnakeGame::NextHeadLoc(uint32_t Move, uint32_t* Pos)
{
  if (Move == 0) {
    Pos[0] = SnakeLocation[0]-1; Pos[1] = SnakeLocation[1];
  } else if (Move == 1) {
    Pos[0] = SnakeLocation[0]+1; Pos[1] = SnakeLocation[1];
  } else if (Move == 2) {
    Pos[0] = SnakeLocation[0]; Pos[1] = SnakeLocation[1]-1;
  } else if (Move == 3) {
    Pos[0] = SnakeLocation[0]; Pos[1] = SnakeLocation[1]+1;
  }
}

void SnakeGame::MoveSnake(uint32_t* NewPos)
{
  for (uint32_t i = 0; i <= FoodCount; i++) {
    Board[SnakeLocation[2*i]*Width+SnakeLocation[2*i+1]] = false;
  }

  for (uint32_t i = FoodCount; i != 0; i--) {
    SnakeLocation[2*(i+1)] = SnakeLocation[2*i];
    SnakeLocation[2*(i+1)+1] = SnakeLocation[2*i+1];
  }

  SnakeLocation[2] = SnakeLocation[0];
  SnakeLocation[3] = SnakeLocation[1];
  SnakeLocation[0] = NewPos[0];
  SnakeLocation[1] = NewPos[1];

  for (uint32_t i = 0; i <= FoodCount; i++) {
    Board[SnakeLocation[2*i]*Width+SnakeLocation[2*i+1]] = true;
  }
}

bool SnakeGame::ProcessInput(uint32_t Move)
{
  if (Result) return false;
  NextHeadLoc(Move,temp);
  Moves[Turn] = Move;

  if (temp[0] >= Width || temp[1] >= Height) {
    //std::cout << "Out of bounds.\n";
    Result = 1;
    return false;
  }

  if (Board[temp[0]*Width+temp[1]]) {
    //std::cout << "Self-walking.\n";
    Result = 2;
    return false;
  }

  MoveSnake(temp);
  if (Board[Food[2*FoodCount]*Width+Food[2*FoodCount+1]]) {
    MoveSnake(temp);
    FoodCount++;
    GenerateFood();
  }

  UpdateRepr();
  return true;
}

void BinaryToChar(uint64_t x, char * msg, uint32_t N = 64)
{
  for (uint32_t i = 0; i < N; i++) { msg[i] = (x & (1ULL << (N-i-1)) ? '1' : '0'); }
  msg[N] = '\0';
}

void SnakeGame::UpdateRepr()
{
  for (uint32_t i = 0; i < Tiles; i++) { Repr[i] = 0; }
  for (uint32_t i = 1; i < Tiles && i <= FoodCount; i++) {
    Repr[Morton2D(SnakeLocation[2*i],SnakeLocation[2*i+1])] = 5;
  }
  Repr[Morton2D(SnakeLocation[0],SnakeLocation[1])] = 10;
  Repr[Morton2D(Food[2*FoodCount],Food[2*FoodCount+1])] = 100;
}

void SnakeGame::PrintBoard()
{
  char* board = new char[Tiles];
  for (uint32_t i = 0; i < Tiles; i++) { board[i] = '.'; }
  for (uint32_t i = 1; i < Tiles && i <= FoodCount; i++) {
    board[SnakeLocation[2*i]*Width+SnakeLocation[2*i+1]] = '#';
  }
  board[SnakeLocation[0]*Width+SnakeLocation[1]] = 'O';
  board[Food[2*FoodCount]*Width+Food[2*FoodCount+1]] = '@';
  for (uint32_t i = 0; i < Height; i++) {
    for (uint32_t j = 0; j < Width; j++) {
      if (j==0) { std::cout << i << "\t\t";}
      std::cout << board[i*Width+j] << " ";
    }
    std::cout << std::endl;
  }
  delete[] board;
}
