
while read line;
do
	echo -n $line, >> experiment/uniformity/and.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --notion 2 --uniform 2 --verbose 0 --memory 8589934592 >> experiment/uniformity/and.csv

done < experiment/uniformity/benchs.txt
