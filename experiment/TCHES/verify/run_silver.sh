# run `bash experiment/run.sh` in root folder of the project

while read line;
do
	echo -n $line, >> experiment/TCHES/verify/silver.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --timeout 20 --verbose 0 --memory 8589934592 >> experiment/TCHES/verify/silver.csv

done < experiment/TCHES/verify/benchmarks_sboxes.txt
