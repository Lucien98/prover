
while read line;
do
	echo -n $line, >> experiment/uniformity/xor.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --notion 2 --uniform 0 --verbose 0 --memory 8589934592 >> experiment/uniformity/xor.csv

done < experiment/uniformity/benchs.txt
