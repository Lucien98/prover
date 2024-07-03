
while read line;
do
	echo -n $line, >> experiment/NullFresh2/prover.csv
	./bin/verify --insfile ${line} --timeout 20 --verbose 0 --memory 8589934592 >> experiment/NullFresh2/prover.csv

done < experiment/NullFresh2/benchs.txt
