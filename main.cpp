#include <cstring>
#include "ap.h"

int main(int argc, char** argv)
{
  if (argc == 1) return 0;
  ArtificialPlayer *p1 = new ArtificialPlayer;

  if (strcmp(argv[1],"new") == 0) {
    uint32_t * nl = new uint32_t[1+2*std::atoi(argv[6])];
    for (int i = 0; i < 1+2*std::atoi(argv[6]); i++) {
      nl[i] = std::atoi(argv[7+i]);
    }
    p1->Initialize(std::atoi(argv[3]),std::atoi(argv[4]),std::atoi(argv[5]),std::atoi(argv[6]),nl);
    p1->SaveProgress(argv[2]);
  } else if (strcmp(argv[1],"learn") == 0) {
    p1->LoadProgress(argv[2]);
    if (argc == 4) { p1->Training(std::atoi(argv[3])); }
    else if (argc == 5) { p1->Training(std::atoi(argv[3]),argv[2],std::atoi(argv[4])); }
    p1->SaveProgress(argv[2]);
  } else if (strcmp(argv[1],"play") == 0) {
    p1->LoadProgress(argv[2]);
    p1->ShowPlay();
  }

  return 0;
}
