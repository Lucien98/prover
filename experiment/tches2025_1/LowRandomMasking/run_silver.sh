
while read line;
do
	echo -n $line, >> experiment/tches2025_1/LowRandomMasking/silver.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --timeout 20 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/LowRandomMasking/silver.csv

done < experiment/tches2025_1/LowRandomMasking/benchs.txt
