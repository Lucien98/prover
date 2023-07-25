# run `bash experiment/run.sh` in root folder of the project
# find test -name *.nl > experiment/count_node/benchs.txt

while read line;
do
	echo -n $line, >> experiment/count_node/cn3.csv
	./bin/verify --insfile ${line} --count_node 1 --improve_varorder 3 --verbose 0 --memory 8589934592 >> experiment/count_node/cn3.csv

done < experiment/count_node/benchs.txt


while read line;
do
	echo -n $line, >> experiment/count_node/vo2.csv
	./bin/verify --insfile ${line} --count_node 1 --improve_varorder 2 --verbose 0 --memory 8589934592 >> experiment/count_node/cn2.csv

done < experiment/count_node/benchs.txt

while read line;
do
	echo -n $line, >> experiment/count_node/cn0.csv
	./bin/verify --insfile ${line} --count_node 1 --improve_varorder 0 --verbose 0 --memory 8589934592 >> experiment/count_node/cn0.csv

done < experiment/count_node/benchs.txt
