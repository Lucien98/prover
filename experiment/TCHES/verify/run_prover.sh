# run `bash experiment/run.sh` in root folder of the project

while read line;
do
	echo -n $line, >> experiment/TCHES/verify/prover.csv
	./bin/verify --insfile ${line} --timeout 20 --verbose 0 --memory 8589934592 >> experiment/TCHES/verify/prover.csv

done < experiment/TCHES/verify/benchmarks_sboxes.txt
