
while read line;
do
	echo -n $line, >> experiment/tches2025_1/NullFresh2/prover.csv
	./bin/verify --insfile ${line} --timeout 20 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/NullFresh2/prover.csv

done < experiment/tches2025_1/NullFresh2/benchs.txt
