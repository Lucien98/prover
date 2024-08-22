
echo -n "test/ISW_mult/4shares.nl," >> experiment/tches2025_1/IronMask/results.csv
./bin/verify --verbose 0 --insfile test/ISW_mult/4shares.nl >> experiment/tches2025_1/IronMask/results.csv
echo -n "test/ISW_mult/5shares.nl," >> experiment/tches2025_1/IronMask/results.csv
./bin/verify --verbose 0 --insfile test/ISW_mult/5shares.nl >> experiment/tches2025_1/IronMask/results.csv
echo -n "test/ISW_mult/6shares.nl," >> experiment/tches2025_1/IronMask/results.csv
./bin/verify --verbose 0 --insfile test/ISW_mult/6shares.nl --notion 2 >> experiment/tches2025_1/IronMask/results.csv
echo -n "test/refresh_nlogn/6shares.nl," >> experiment/tches2025_1/IronMask/results.csv
./bin/verify --verbose 0 --insfile test/refresh_nlogn/6shares.nl >> experiment/tches2025_1/IronMask/results.csv
echo -n test/refresh_nlogn/7shares.nl, >> experiment/tches2025_1/IronMask/results.csv
./bin/verify --verbose 0 --insfile test/refresh_nlogn/7shares.nl >> experiment/tches2025_1/IronMask/results.csv
echo -n "test/refresh_nlogn/8shares.nl," >> experiment/tches2025_1/IronMask/results.csv
./bin/verify --verbose 0 --insfile test/refresh_nlogn/8shares.nl --notion 2 >> experiment/tches2025_1/IronMask/results.csv
