# LPRules: Rule Induction in Knowledge Graphs Using Linear Programming

This repository contains the code and the data used for the experiments in the paper
"Rule Induction in Knowledge Graphs Using Linear Programming" by Sanjeeb Dash and Joao Goncalves, AAAI-23.

## Requirements:
* The code was tested only on Linux.
* The code is written in C++ and requires a C++ compiler.
* The code uses the commercial solver [IBM ILOG CPLEX](https://www.ibm.com/products/ilog-cplex-optimization-studio).

## Folders description:
* `code`: contains the code and the Makefile.
* `data`: contains the 5 datasets used in the paper.
* `runs`: contains the parameters files and scripts to run the 5 datasets.

## How to install the code:
* Install [IBM ILOG CPLEX](https://www.ibm.com/products/ilog-cplex-optimization-studio). 
* Edit the file `Makefile` in the directory `code` and add the paths to the directory `cplex` and `concert` in the lines:
	* `CPLEXDIR      = path_to_cplex/cplex`
	* `CONCERTDIR    = path_to_cplex/concert`
* In the directory `code` type `make`.

## How to run the code without using previous known rules:
The following instructions show how to run LPRules with the dataset UMLS.

* Go to the directory `run/UMLS`.
* Execute the command:
`./run_lprules_in_parallel.sh p_UMLS.txt outUMLS 46`
where `p_UMLS.txt` is the parameter file residing in the directory `run/UMLS`, 
outUMLS is the name to be used in output files and
46 is the number of relations in the UMLS dataset.
* The results are presented at the end of the file
`results_outUMLS.txt`.

The number of relations in each dataset is:
UMLS 46, 
Kinship 25, 
WN18RR 11, 
FB15k-237 237, 
YAGO3-10  37.

## How to run the code using previous known rules:
The following instructions show how to run LPRules with the dataset UMLS
assuming the existence of a set of rules in the file `input_rules.txt`.

* Go to the directory `run/UMLS`.
* Edit the file `p_UMLS.txt` and set the parameter `run_mode` to
either 1, 2, or 3. The meaning of each of these values is:
1 - scenario B in the paper, 
2 - scenario C in the paper, 
3 - scenario D in the paper.
* Execute the command:
`./run_lprules_in_parallel_read_rules.sh p_UMLS.txt outUMLS 46 input_rules.txt`
where `p_UMLS.txt` is the parameter file residing in the directory `run/UMLS`, 
outUMLS is the name to be used in output files and
46 is the number of relations in the UMLS dataset.
* The results are presented at the end of the file
`results_outUMLS.txt`.

