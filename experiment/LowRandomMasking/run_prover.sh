
while read line;
do
	echo -n $line, >> experiment/LowRandomMasking/prover.csv
	./bin/verify --insfile ${line} --timeout 20 --verbose 0 --memory 8589934592 >> experiment/LowRandomMasking/prover.csv

done < experiment/LowRandomMasking/benchs.txt
