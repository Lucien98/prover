# run `bash experiment/run.sh` in root folder of the project
find test -name *.nl > experiment/tosc2024_4/verify/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tosc2024_4/verify/original.csv
	./bin/verify --insfile ${line} --varorder 0 --userule 0 --timeout 10 --verbose 0 --memory 8589934592 >> experiment/tosc2024_4/verify/original.csv

done < experiment/tosc2024_4/verify/benchs.txt
