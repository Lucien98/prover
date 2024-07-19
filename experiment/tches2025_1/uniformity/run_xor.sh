
while read line;
do
	echo -n $line, >> experiment/tches2025_1/uniformity/xor.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --notion 2 --uniform 0 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/uniformity/xor.csv

done < experiment/tches2025_1/uniformity/benchs.txt
