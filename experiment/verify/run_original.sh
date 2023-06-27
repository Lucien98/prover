# run `bash experiment/run.sh` in root folder of the project
find test -name *.nl > experiment/verify/benchs.txt

while read line;
do
	echo -n $line, >> experiment/verify/original.csv
	./bin/verify --insfile ${line} --improve_varorder 0 --verbose 0 >> experiment/verify/original.csv

done < experiment/verify/benchs.txt
