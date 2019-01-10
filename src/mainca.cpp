
#include "utils.h"
#include <lemon/arg_parser.h>
#include "clonetree.h"
#include "parentchildgraph.h"
#include "multipleconsensustrees.h"
#include "mctsolverca.h"

int main(int argc, char** argv){
  
  int k = 0;
  int restarts = 50;
  int seed = 0;
  std::string resultspath;
  
  lemon::ArgParser ap (argc, argv);
  ap.other("trees", "Input trees");
  ap.refOption("k", "Number of clusters", k, true);
  ap.refOption("r", "Number of restarts (default: 50)", restarts);
  ap.refOption("s", "Seed for random number generator (default:0)", seed);
  ap.refOption("p", "Path to results (make it unique)", resultspath, true);
  ap.parse();
  
  if (ap.files().size() != 1){
    std::cerr << "Error <trees> must be specified" << std::endl;
    return 1;
  }
  
  std::string filenameTrees = ap.files()[0];
  
  CloneTreeVector ctv;
  try{
    if (filenameTrees != "-"){
      std::ifstream inTrees(filenameTrees.c_str());
      if (!inTrees.good()){
        std::cerr << "Could not open '" << filenameTrees << "' for reading" << std::endl;
        return 1;
      }
      inTrees >> ctv;
      inTrees.close();
      
    }
    else {
      std::cin >> ctv;
    }
  }
  catch (std::runtime_error& e){
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  MCTSolverCA solver(ctv, k, restarts, seed);
  MCTSolver::run(solver, resultspath);
}
