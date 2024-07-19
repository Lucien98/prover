
while read line;
do
	echo -n $line, >> experiment/tches2025_1/uniformity/and.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --notion 2 --uniform 2 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/uniformity/and.csv

done < experiment/tches2025_1/uniformity/benchs.txt
