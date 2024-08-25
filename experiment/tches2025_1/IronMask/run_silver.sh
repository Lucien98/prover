
while read line;
do
	echo -n $line, >> experiment/tches2025_1/IronMask/silver.csv
	./bin/verify --varorder 0 --userule 0 --insfile ${line} --timeout 20 --verbose 0 --memory 8589934592 >> experiment/tches2025_1/IronMask/silver.csv

done < experiment/tches2025_1/IronMask/benchs.txt

echo -n "test/ISW_mult/6shares.nl," >> experiment/tches2025_1/IronMask/silver.csv
./bin/verify --varorder 0 --userule 0 --verbose 0 --insfile test/ISW_mult/6shares.nl --notion 0 >> experiment/tches2025_1/IronMask/silver.csv
./bin/verify --varorder 0 --userule 0 --verbose 0 --insfile test/ISW_mult/6shares.nl --notion 1 >> experiment/tches2025_1/IronMask/silver.csv
./bin/verify --varorder 0 --userule 0 --verbose 0 --insfile test/ISW_mult/6shares.nl --notion 2 >> experiment/tches2025_1/IronMask/silver.csv

echo -n "test/refresh_nlogn/7shares.nl," >> experiment/tches2025_1/IronMask/silver.csv
./bin/verify --varorder 0 --userule 0 --verbose 0 --insfile test/refresh_nlogn/7shares.nl --notion 0 >> experiment/tches2025_1/IronMask/silver.csv
./bin/verify --varorder 0 --userule 0 --verbose 0 --insfile test/refresh_nlogn/7shares.nl --notion 1 >> experiment/tches2025_1/IronMask/silver.csv
./bin/verify --varorder 0 --userule 0 --verbose 0 --insfile test/refresh_nlogn/7shares.nl --notion 2 >> experiment/tches2025_1/IronMask/silver.csv