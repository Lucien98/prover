# run `bash experiment/run.sh` in root folder of the project
find test -name *.nl > experiment/tosc2024_4/verify/benchs.txt

while read line;
do
	echo -n $line, >> experiment/tosc2024_4/verify/improved.csv
	./bin/verify --insfile ${line} --varorder 1 --timeout 10 --onlygp 1 --verbose 0 --memory 8589934592 >> experiment/tosc2024_4/verify/improved.csv

done < experiment/tosc2024_4/verify/benchs.txt
