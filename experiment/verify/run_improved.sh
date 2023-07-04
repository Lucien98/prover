# run `bash experiment/run.sh` in root folder of the project
find test -name *.nl > experiment/verify/benchs.txt

while read line;
do
	echo -n $line, >> experiment/verify/improved.csv
	./bin/verify --insfile ${line} --improve_varorder 1 --verbose 0 --memory 8589934592 >> experiment/verify/improved.csv

done < experiment/verify/benchs.txt
