# © Copyright IBM Corporation 2022. All Rights Reserved.
# LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
# SPDX-License-Identifier: EPL-2.0

#!/bin/bash

# to run type: 
# ./run_lprules_in_parallel.sh parameterFile nameForOutputFiles numberOfRelations nameForInputRulesFile 
# The results are written in the file results_nameForOutputFiles.txt

# Parameter filename
PARAMS=$1
# Output name
OUTNAME=$2
# Number of Relations
NRELATIONS=$3
# Input rules file name
INPUTRULES=$4
# Scores filename
SCORES=scores_$OUTNAME
# Rules filename
RULES=rules_$OUTNAME
# Log filename
LOG=log_$OUTNAME
# Results filename
RESULTS=results_$OUTNAME.txt
# Executable
EXEC=../../code/lprules

NRELATIONS=$((NRELATIONS-1))
for p in $(seq 0 $NRELATIONS)
do
    $EXEC -p $PARAMS -s $SCORES-p$p.txt -r $RULES-p$p.txt -i $p -v $INPUTRULES > $LOG-p$p.txt &
done

wait

grep rd_mr_mrr_1_3_10_n $SCORES* > $RESULTS
grep rd_mr_mrr_1_3_10_n $SCORES* | awk -v OFMT=%.17g '{split($0,a," "); sum += a[2]; sum1+=a[7]} END {print "MR " sum/sum1}' >> $RESULTS
grep rd_mr_mrr_1_3_10_n $SCORES* | awk -v OFMT=%.17g '{split($0,a," "); sum += a[3]; sum1+=a[7]} END {print "MRR " sum/sum1}' >> $RESULTS
grep rd_mr_mrr_1_3_10_n $SCORES* | awk -v OFMT=%.17g '{split($0,a," "); sum += a[4]; sum1+=a[7]} END {print "HITS@1 " sum/sum1}' >> $RESULTS
grep rd_mr_mrr_1_3_10_n $SCORES* | awk -v OFMT=%.17g '{split($0,a," "); sum += a[5]; sum1+=a[7]} END {print "HITS@3 " sum/sum1}' >> $RESULTS
grep rd_mr_mrr_1_3_10_n $SCORES* | awk -v OFMT=%.17g '{split($0,a," "); sum += a[6]; sum1+=a[7]} END {print "HITS@10 " sum/sum1}' >> $RESULTS
