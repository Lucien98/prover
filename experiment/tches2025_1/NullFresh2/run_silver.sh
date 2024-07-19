
while read line;
do
	echo -n $line, >> experiment/tches2025_1/NullFresh2/silver.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --timeout 1440 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/NullFresh2/silver.csv

done < experiment/tches2025_1/NullFresh2/benchs.txt
