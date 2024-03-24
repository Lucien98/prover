# run `bash experiment/TCHES/varorder/run_vo.sh` in root folder of the project

while read line;
do
	echo -n $line, >> experiment/TCHES/varorder/vo0.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --timeout 20 --verbose 0 --memory 8589934592 >> experiment/TCHES/varorder/vo0.csv

done < experiment/TCHES/varorder/benchmarks_sboxes.txt

while read line;
do
	echo -n $line, >> experiment/TCHES/varorder/vo1.csv
	./bin/verify --insfile ${line} --varorder 1 --userule 0 --timeout 20 --verbose 0 --memory 8589934592 >> experiment/TCHES/varorder/vo1.csv

done < experiment/TCHES/varorder/benchmarks_sboxes.txt

while read line;
do
	echo -n $line, >> experiment/TCHES/varorder/vo2.csv
	./bin/verify --insfile ${line} --varorder 2 --userule 0 --timeout 20 --verbose 0 --memory 8589934592 >> experiment/TCHES/varorder/vo2.csv

done < experiment/TCHES/varorder/benchmarks_sboxes.txt
