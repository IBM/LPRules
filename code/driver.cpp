// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#include "Parameters.hpp"
#include "Solver.hpp"

#include <iomanip>

#include <ctime>
#if !defined(_MSC_VER)
#include <sys/resource.h>
#endif

double getTime()
{
  double cpuTime;
#ifdef _MSC_VER
  cpuTime=static_cast<double> (clock()) /static_cast<double>(CLOCKS_PER_SEC);
#else
  struct rusage usage;
  getrusage(RUSAGE_SELF,&usage);
  cpuTime = usage.ru_utime.tv_sec;
  cpuTime += 1.0e-6*(static_cast<double> (usage.ru_utime.tv_usec));
#endif
  return cpuTime;
}


int
main (int argc, char* argv[])
{
  double startTime = getTime();

  string paramsFileName = "run_parameters.txt";
  string scoresFileName = "scores.txt";
  string rulesFileName = "rules.txt";
  string inputRulesFileName = "";
  int relationId = -1;

  for (int i = 1; i < argc; i++) {
    string arg = argv[i];
    if (arg == "-p") {
      if (i+1 < argc) {
	i++;
	paramsFileName = argv[i];
      }
    }
    else if (arg == "-s") {
      if (i+1 < argc) {
	i++;
	scoresFileName = argv[i];
      }
    }
    else if (arg == "-r") {
      if (i+1 < argc) {
	i++;
	rulesFileName = argv[i];
      }
    }
    else if (arg == "-i") {
      if (i+1 < argc) {
	i++;
	relationId = atoi(argv[i]);
      }
    }
    else if (arg == "-v") {
      if (i+1 < argc) {
	i++;
	inputRulesFileName = argv[i];
      }
    }
    else if (arg == "-h") {
      cerr<<"Usage: "<<argv[0]<<" -p parameters_file_name -s scores_file_name -r rules_file_name -i relation_id -v input_fules_file_name"<<endl;
      return 1;
    }
  }

  cout<<"paramsFileName = "<<paramsFileName<<endl;
  cout<<"scoresFileName = "<<scoresFileName<<endl;
  cout<<"rulesFileName = "<<rulesFileName<<endl;
  cout<<"inputRulesFileName = "<<inputRulesFileName<<endl;

  Parameters params = Parameters();
  params.readParamsFile(paramsFileName);
  if(relationId >= 0) {
    params.addRelationId(relationId);
    params.addRunOnlyWithRelationId(true);
  }
  params.printParams();

  Solver solver = Solver(params);
  remove(scoresFileName.c_str());
  remove(rulesFileName.c_str());
  solver.run(scoresFileName, rulesFileName, inputRulesFileName);
  //  cout<<"Time after CPLEX: "<<getTime()-startTime<<endl;
  //  solver.writeScoresToFile("scores.txt");

  cout<<"Total Time: "<<getTime()-startTime<<endl;
}
