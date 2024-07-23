
while read line;
do
	echo -n $line, >> experiment/tches2025_1/varorder/silver_vo0.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --notion 0 --timeout 1440 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/varorder/silver_vo0.csv

done < experiment/tches2025_1/varorder/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tches2025_1/varorder/silver_vo1.csv
	./bin/verify --insfile ${line} --varorder 1 --userule 0 --notion 0 --timeout 1440 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/varorder/silver_vo1.csv

done < experiment/tches2025_1/varorder/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tches2025_1/varorder/silver_vo2.csv
	./bin/verify --insfile ${line} --varorder 2 --userule 0 --notion 0 --timeout 1440 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/varorder/silver_vo2.csv

done < experiment/tches2025_1/varorder/benchs.txt
