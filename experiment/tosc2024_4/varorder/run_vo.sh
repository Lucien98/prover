# run `bash experiment/run.sh` in root folder of the project
# find test -name *.nl > experiment/tosc2024_4/varorder/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tosc2024_4/varorder/vo2.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 2 --verbose 0 --memory 8589934592 >> experiment/tosc2024_4/varorder/vo2.csv

done < experiment/tosc2024_4/varorder/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tosc2024_4/varorder/vo1+rule.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 1 --varorder 1 --verbose 0 --memory 8589934592 >> experiment/tosc2024_4/varorder/vo1+rule.csv

done < experiment/tosc2024_4/varorder/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tosc2024_4/varorder/vo1.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 1 --verbose 0 --memory 8589934592 >> experiment/tosc2024_4/varorder/vo1.csv

done < experiment/tosc2024_4/varorder/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tosc2024_4/varorder/vo0.csv
	./bin/verify --insfile ${line} --count_node 0 --userule 0 --varorder 0 --verbose 0 --memory 8589934592 >> experiment/tosc2024_4/varorder/vo0.csv

done < experiment/tosc2024_4/varorder/benchs.txt
