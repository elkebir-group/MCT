
#include "utils.h"
#include <lemon/arg_parser.h>
#include "clonetree.h"
#include "parentchildgraph.h"
#include "mctsolverca.h"

int main(int argc, char** argv){
  
  int k = 0;
  int restarts = 50;
  int seed = 0;
  int option = 0;
  int timelimit = 10;
  std::string resultspath;
  
  lemon::ArgParser ap (argc, argv);
  ap.other("trees", "Input trees");
  ap.refOption("k", "Number of clusters", k, true);
  ap.refOption("o", "Specify termination method. 0 for no. of restarts, 1 for time limit. (default: 0)", option, true );
  ap.refOption("r", "Number of restarts (default: 50)", restarts);
  ap.refOption("t", "Time limit in seconds (default: 10)", timelimit);
  ap.refOption("s", "Seed for random number generator (default:0)", seed);
  ap.refOption("p", "Path to results (make it unique)", resultspath, true);
  ap.parse();
  
  if (ap.files().size() != 1){
    std::cerr << "Error: <trees> must be specified" << std::endl;
    return 1;
  }
  
  std::string filenameTrees = ap.files()[0];
  
  CloneTreeVector ctv;
  try{
    if (filenameTrees != "-"){
      std::ifstream inTrees(filenameTrees.c_str());
      if (!inTrees.good()){
        std::cerr << "Error: could not open '" << filenameTrees << "' for reading" << std::endl;
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
  if (option){
    MCTSolverCA solver(ctv, k, 0, seed, timelimit);
    MCTSolver::run(solver, resultspath);
  }
  else{
    MCTSolverCA solver(ctv, k, restarts, seed, 0);
    MCTSolver::run(solver, resultspath);
  }
  
  
}
