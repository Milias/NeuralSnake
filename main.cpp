#include "ap.h"

int main(int argc, char** argv)
{
  /*
  SnakeGame snek(16,16);
  snek.Initialize();
  snek.Start();

  uint32_t m; uint32_t err = 0;
  while (std::cin >> m) {
    err = snek.CheckInput(m-1);
    if (err) break;
    snek.PrintBoard();
  }

  snek.End();
  snek.PrintBoard();
  */

  uint32_t w,h,ns,nn; w = 10; h = 10, ns = 10; nn = 1;
  uint32_t nl[] = {w*h,32,1};

  ArtificialPlayer *p1 = new ArtificialPlayer(w,h,ns,nn,nl);
  //p1.Training(10);
  for (uint32_t i = 0; i < 10000; i++) {
    std::cout << i << std::endl;
    p1->ShowPlay(false);
  }

  return 0;
}
