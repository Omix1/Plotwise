#include "solver/tspmanager.h"

int main()
{

   SOLVER::TSPManager manager("benchmark/C1_2_1.TXT");
   manager.runSolvers();
   return 0;
}