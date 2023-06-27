# run `bash experiment/run.sh` in root folder of the project
find test -name *.nl > experiment/count_node/benchs.txt

while read line;
do
	echo $line >> experiment/count_node/original.txt
	./bin/verify --insfile ${line} --count_node 1 --improve_varorder 0 --verbose 0 >> experiment/count_node/original.txt

	echo >> experiment/count_node/original.txt
	echo >> experiment/count_node/original.txt
	echo "****************" >> experiment/count_node/original.txt
done < experiment/count_node/benchs.txt
