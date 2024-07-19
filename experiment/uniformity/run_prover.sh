
while read line;
do
	echo -n $line, >> experiment/uniformity/prover.csv
	./bin/verify --insfile ${line} --notion 2 --verbose 0 --memory 8589934592 >> experiment/uniformity/prover.csv

done < experiment/uniformity/benchs.txt
