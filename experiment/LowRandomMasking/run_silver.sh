
while read line;
do
	echo -n $line, >> experiment/LowRandomMasking/silver.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --timeout 20 --verbose 0 --memory 8589934592 >> experiment/LowRandomMasking/silver.csv

done < experiment/LowRandomMasking/benchs.txt
