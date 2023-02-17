# © Copyright IBM Corporation 2022. All Rights Reserved.
# LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
# SPDX-License-Identifier: EPL-2.0

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = path_to_cplex/cplex
CONCERTDIR    = path_to_cplex/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O3
CC  = gcc -O0
JAVAC = javac 

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
#CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
COPT  = -m64 -fPIC -fno-strict-aliasing
JOPT  = -classpath $(CPLEXDIR)/lib/cplex.jar -O

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXJARDIR   = $(CPLEXDIR)/lib/cplex.jar
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

# For dynamic linking
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIB      = cplex$(dynamic:yes=12100)
run           = $(dynamic:yes=LD_LIBRARY_PATH=$(CPLEXBINDIR))

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CLNDIRS   = -L$(CPLEXLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CCLNFLAGS = -lconcert -lilocplex -l$(CPLEXLIB) -lm -lpthread -ldl
CLNFLAGS  = -l$(CPLEXLIB) -lm -lpthread -ldl
JAVA      = java  -d64 -Djava.library.path=$(CPLEXDIR)/bin/x86-64_linux -classpath $(CPLEXJARDIR):


all:
	make all_cpp

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CFLAGS  = $(COPT)  -I$(CPLEXINCDIR)
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 
JCFLAGS = $(JOPT)


#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------

CPP_EX = lprules

all_cpp: $(CPP_EX)

# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf $(CPP_EX)
#	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

# ------------------------------------------------------------
#
# The examples
#
lprules: driver.o Data.o Model2MasterLP.o Solver.o SolverNew3.o Parameters.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o lprules driver.o Data.o Model2MasterLP.o Solver.o SolverNew3.o Parameters.o $(CCLNFLAGS)
driver.o: driver.cpp
	$(CCC) -c $(CCFLAGS) driver.cpp -o driver.o
Data.o: Data.cpp
	$(CCC) -c $(CCFLAGS) Data.cpp -o Data.o
Model2MasterLP.o: Model2MasterLP.cpp
	$(CCC) -c $(CCFLAGS) Model2MasterLP.cpp -o Model2MasterLP.o
Solver.o: Solver.cpp
	$(CCC) -c $(CCFLAGS) Solver.cpp -o Solver.o
SolverNew3.o: SolverNew3.cpp
	$(CCC) -c $(CCFLAGS) SolverNew3.cpp -o SolverNew3.o
Parameters.o: Parameters.cpp
	$(CCC) -c $(CCFLAGS) Parameters.cpp -o Parameters.o

# Local Variables:
# mode: makefile
# End:
