# run `bash experiment/run.sh` in root folder of the project
find test -name *.nl > experiment/verify/benchs.txt

while read line;
do
	echo $line >> experiment/verify/improved.txt
	./bin/verify --insfile ${line} --improve_varorder 1 --verbose 0 >> experiment/verify/improved.txt

	echo >> experiment/verify/improved.txt
	echo >> experiment/verify/improved.txt
	echo "****************" >> experiment/verify/improved.txt
done < experiment/verify/benchs.txt
