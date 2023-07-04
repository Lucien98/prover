# run `bash experiment/run.sh` in root folder of the project
# find test -name *.nl > experiment/count_node/benchs.txt

while read line;
do
	echo -n $line, >> experiment/count_node/original.csv
	./bin/verify --insfile ${line} --count_node 1 --improve_varorder 0 --verbose 0 --memory 8589934592 >> experiment/count_node/original.csv

done < experiment/count_node/benchs.txt
