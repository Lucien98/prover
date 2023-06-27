echo > experiment/verify/original.csv
echo > experiment/verify/improved.csv
echo > experiment/count_node/original.csv
echo > experiment/count_node/improved.csv
bash experiment/verify/run_improved.sh
bash experiment/verify/run_original.sh
bash experiment/count_node/run_improved.sh
bash experiment/count_node/run_original.sh
