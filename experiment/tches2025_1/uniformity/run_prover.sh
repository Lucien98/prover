
while read line;
do
	echo -n $line, >> experiment/tches2025_1/uniformity/prover.csv
	./bin/verify --insfile ${line} --notion 2 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/uniformity/prover.csv

done < experiment/tches2025_1/uniformity/benchs.txt
