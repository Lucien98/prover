# run `bash experiment/run.sh` in root folder of the project
find test -name *.nl > experiment/verify/benchs.txt

while read line;
do
	echo $line >> experiment/verify/original.txt
	./bin/verify --insfile ${line} --improve_varorder 0 --verbose 0 >> experiment/verify/original.txt

	echo >> experiment/verify/original.txt
	echo >> experiment/verify/original.txt
	echo "****************" >> experiment/verify/original.txt
done < experiment/verify/benchs.txt
