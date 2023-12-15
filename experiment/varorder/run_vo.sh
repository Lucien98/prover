# run `bash experiment/run.sh` in root folder of the project
# find test -name *.nl > experiment/count_node/benchs.txt

while read line;
do
	echo -n $line, >> experiment/count_node/vo2.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 2 --verbose 0 --memory 8589934592 >> experiment/count_node/vo2.csv

done < experiment/count_node/benchs.txt

while read line;
do
	echo -n $line, >> experiment/count_node/vo1+rule.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 1 --varorder 1 --verbose 0 --memory 8589934592 >> experiment/count_node/vo1+rule.csv

done < experiment/count_node/benchs.txt

while read line;
do
	echo -n $line, >> experiment/count_node/vo1.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 1 --verbose 0 --memory 8589934592 >> experiment/count_node/vo1.csv

done < experiment/count_node/benchs.txt

while read line;
do
	echo -n $line, >> experiment/count_node/vo0.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 0 --verbose 0 --memory 8589934592 >> experiment/count_node/vo0.csv

done < experiment/count_node/benchs.txt
