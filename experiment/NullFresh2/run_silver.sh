
while read line;
do
	echo -n $line, >> experiment/NullFresh2/silver.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --timeout 20 --verbose 0 --memory 8589934592 >> experiment/NullFresh2/silver.csv

done < experiment/NullFresh2/benchs.txt
