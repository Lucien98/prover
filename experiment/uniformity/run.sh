# while read line;
# do
# 	echo  $line >> experiment/uniformity/original.csv
# 	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 1 --verbose 1 --memory 8589934592 --prover 0 >> experiment/uniformity/original.csv
# done < experiment/count_node/benchs.txt

while read line;
do
	echo  $line >> experiment/uniformity/improved.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 1 --verbose 1 --memory 8589934592 --prover 1 >> experiment/uniformity/improved.csv
done < experiment/count_node/benchs.txt
