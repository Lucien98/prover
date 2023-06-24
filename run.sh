#!/bin/bash

# ./bin/verify --insfile /home/lucien/SILVER/test/quadratic/q12.nl --verbose=1
# ./bin/verify  --verilog=1 --verilog-design_file ~/Desktop/Sbox_39/SM4_SBOX_MASK.v --verilog-module_name=SM4_SBOX_MASK --verbose=1 --insfile ~/Desktop/Sbox_39/SM4_SBOX_MASK.nl
# ./bin/verify  --insfile /home/lucien/SILVER/test/dom/dom1.nl --verbose=1

# ./bin/verify  --verilog=1 --verilog-design_file vlog/quadratic/q12/2-Sythesized/q12_annot.v --verilog-module_name=q12 --verbose=1 --insfile vlog/quadratic/q12/3-Netlist/q12.nl
# pj_dir=/home/lucien/Desktop/DCSynthesis/$1
# design_file=${pj_dir}/$2_r.v
# insfile=${pj_dir}/$2.nl
# ./bin/verif --verilog=1 --verilog-design_file ${design_file} --verilog-module_name=$2 --insfile ${insfile} --verbose=1 --memory 8589934592
pj_dir=$1
design_file=${pj_dir}/$2.v
insfile=${pj_dir}/$2.nl
./bin/verify --verilog=1 --verilog-design_file ${design_file} --verilog-module_name=$2 --insfile ${insfile} --verbose=1 --memory 8589934592
