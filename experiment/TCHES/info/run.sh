# run `bash experiment/run.sh` in root folder of the project
# find test -name *.nl > experiment/info/benchs.txt

while read line;
do
	echo -n $line, >> experiment/TCHES/info/info.csv
	./bin/verify --insfile ${line} --info 1 --verbose 0 --memory 8589934592 >> experiment/TCHES/info/info.csv

done < experiment/TCHES/info/benchmarks_sboxes.txt
