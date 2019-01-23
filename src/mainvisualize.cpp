/*
 * mainvisualize.cpp
 *
 *  Created on: 17-jan-2019
 *      Author: M. El-Kebir
 */

#include <lemon/arg_parser.h>
#include "utils.h"
#include "clonetree.h"
#include "parentchildgraph.h"
#include "visualize.h"

int main(int argc, char** argv)
{
  bool allArcs = false;
  lemon::ArgParser ap (argc, argv);
  ap.refOption("a", "Show all arcs", allArcs);
  ap.other("trees", "Input trees");
  ap.other("clustering", "Clustering");
  ap.parse();
  
  if (ap.files().size() != 2)
  {
    std::cerr << "Error: <trees> and <clustering> must be specified"
              << std::endl;
    return 1;
  }
  
  std::string filenameTrees = ap.files()[0];
  
  CloneTreeVector ctv;
  std::ifstream inTrees(filenameTrees.c_str());
  if (!inTrees.good())
  {
    std::cerr << "Error: could not open '" << filenameTrees << "' for reading" << std::endl;
    return 1;
  }
  try
  {
    inTrees >> ctv;
    inTrees.close();
  }
  catch (std::runtime_error& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  std::string filenameClustering = ap.files()[1];
  std::ifstream inClustering(filenameClustering.c_str());
  if (!inClustering.good())
  {
    std::cerr << "Error: could not open '" << filenameClustering << "' for reading" << std::endl;
    return 1;
  }
  
  IntVector clustering;
  int k = -1;
  g_lineNumber = 0;
  while (inClustering.good())
  {
    std::string line;
    getline(inClustering, line);
    
    if (line.empty()) continue;
    
    int cluster = -1;
    std::stringstream ss(line);
    ss >> cluster;
    
    if (cluster < 0)
    {
      std::cerr << "Error: invalid cluster index at line " << getLineNumber() << std::endl;
      return 1;
    }
    else
    {
      clustering.push_back(cluster);
      k = std::max(k, cluster + 1);
    }
  }
  inClustering.close();
  
  Visualize viz(ctv, clustering, k);
  viz.writeDOT(std::cout, allArcs);
  
  return 0;
}
