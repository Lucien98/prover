in 0 3_0 # in1[3]
in 1 2_0 # in1[2]
in 2 1_0 # in1[1]
in 3 0_0 # in1[0]
in 4 3_1 # in2[3]
in 5 2_1 # in2[2]
in 6 1_1 # in2[1]
in 7 0_1 # in2[0]
in 8 3_2 # in3[3]
in 9 2_2 # in3[2]
in 10 1_2 # in3[1]
in 11 0_2 # in3[0]
ref 12 # r[23]
ref 13 # r[22]
ref 14 # r[21]
ref 15 # r[20]
ref 16 # r[19]
ref 17 # r[18]
ref 18 # r[17]
ref 19 # r[16]
ref 20 # r[15]
ref 21 # r[14]
ref 22 # r[13]
ref 23 # r[12]
ref 24 # r[11]
ref 25 # r[10]
ref 26 # r[9]
ref 27 # r[8]
ref 28 # r[7]
ref 29 # r[6]
ref 30 # r[5]
ref 31 # r[4]
ref 32 # r[3]
ref 33 # r[2]
ref 34 # r[1]
ref 35 # r[0]
xor 9 11 # \InputAffine.U1
xor 5 7 # \InputAffine.U2
xor 1 3 # \InputAffine.U3
xor 35 34 # \Q12_inst1.\Inst[1].CF_Inst.U1
xor 34 33 # \Q12_inst1.\Inst[2].CF_Inst.U1
xor 33 32 # \Q12_inst1.\Inst[4].CF_Inst.U1
xor 32 31 # \Q12_inst1.\Inst[5].CF_Inst.U1
xor 31 30 # \Q12_inst1.\Inst[7].CF_Inst.U1
xor 30 35 # \Q12_inst1.\Inst[8].CF_Inst.U1
xor 27 28 # \Q12_inst1.\Inst[11].CF_Inst.U1
xor 25 26 # \Q12_inst1.\Inst[14].CF_Inst.U1
xor 24 25 # \Q12_inst1.\Inst[16].CF_Inst.U1
xor 23 22 # \Q12_inst2.\Inst[1].CF_Inst.U1
xor 22 21 # \Q12_inst2.\Inst[2].CF_Inst.U1
xor 21 20 # \Q12_inst2.\Inst[4].CF_Inst.U1
xor 20 19 # \Q12_inst2.\Inst[5].CF_Inst.U1
xor 19 18 # \Q12_inst2.\Inst[7].CF_Inst.U1
xor 18 23 # \Q12_inst2.\Inst[8].CF_Inst.U1
xor 15 16 # \Q12_inst2.\Inst[11].CF_Inst.U1
xor 13 14 # \Q12_inst2.\Inst[14].CF_Inst.U1
xor 12 13 # \Q12_inst2.\Inst[16].CF_Inst.U1
reg 0 # \InAff_out1_reg_reg[2]
reg 3 # \InAff_out1_reg_reg[1]
reg 2 # \InAff_out1_reg_reg[0]
reg 4 # \InAff_out2_reg_reg[2]
reg 7 # \InAff_out2_reg_reg[1]
reg 6 # \InAff_out2_reg_reg[0]
reg 8 # \InAff_out3_reg_reg[2]
reg 11 # \InAff_out3_reg_reg[1]
reg 10 # \InAff_out3_reg_reg[0]
xor 60 61 # \Q12_inst1.\Inst[9].CF_Inst.U1
xor 60 61 # \Q12_inst1.\Inst[11].CF_Inst.U2
xor 63 64 # \Q12_inst1.\Inst[12].CF_Inst.U1
xor 63 64 # \Q12_inst1.\Inst[14].CF_Inst.U2
xor 57 58 # \Q12_inst1.\Inst[15].CF_Inst.U1
xor 57 58 # \Q12_inst1.\Inst[16].CF_Inst.U2
reg 59 # \Q12_inst1.areg_reg[3]
reg 65 # \Q12_inst1.areg_reg[2]
reg 62 # \Q12_inst1.areg_reg[1]
reg 38 # \InAff_out1_reg_reg[3]
reg 37 # \InAff_out2_reg_reg[3]
reg 36 # \InAff_out3_reg_reg[3]
nand 61 76 # \Q12_inst1.\Inst[0].CF_Inst.U1
nand 64 76 # \Q12_inst1.\Inst[1].CF_Inst.U2
nand 61 77 # \Q12_inst1.\Inst[2].CF_Inst.U2
nand 64 77 # \Q12_inst1.\Inst[3].CF_Inst.U1
nand 58 77 # \Q12_inst1.\Inst[4].CF_Inst.U2
nand 64 75 # \Q12_inst1.\Inst[5].CF_Inst.U2
nand 58 75 # \Q12_inst1.\Inst[6].CF_Inst.U1
nand 58 76 # \Q12_inst1.\Inst[7].CF_Inst.U2
nand 61 75 # \Q12_inst1.\Inst[8].CF_Inst.U2
and 76 66 # \Q12_inst1.\Inst[9].CF_Inst.U2
not 76 # \Q12_inst1.\Inst[10].CF_Inst.U1
nand 76 63 # \Q12_inst1.\Inst[10].CF_Inst.U3
nand 67 77 # \Q12_inst1.\Inst[11].CF_Inst.U3
and 77 68 # \Q12_inst1.\Inst[12].CF_Inst.U2
not 77 # \Q12_inst1.\Inst[13].CF_Inst.U1
nand 77 57 # \Q12_inst1.\Inst[13].CF_Inst.U3
nand 69 75 # \Q12_inst1.\Inst[14].CF_Inst.U3
and 75 70 # \Q12_inst1.\Inst[15].CF_Inst.U2
nand 71 76 # \Q12_inst1.\Inst[16].CF_Inst.U3
not 75 # \Q12_inst1.\Inst[17].CF_Inst.U1
nand 75 60 # \Q12_inst1.\Inst[17].CF_Inst.U3
reg 75 # \Q12_inst1.dreg_reg[3]
reg 77 # \Q12_inst1.dreg_reg[2]
reg 76 # \Q12_inst1.dreg_reg[1]
reg 74 # \M2_out1_reg_reg[2]
reg 73 # \M2_out2_reg_reg[2]
reg 72 # \M2_out3_reg_reg[2]
xnor 78 63 # \Q12_inst1.\Inst[0].CF_Inst.U2
xnor 39 79 # \Q12_inst1.\Inst[1].CF_Inst.U3
xnor 40 80 # \Q12_inst1.\Inst[2].CF_Inst.U3
xnor 81 57 # \Q12_inst1.\Inst[3].CF_Inst.U2
xnor 41 82 # \Q12_inst1.\Inst[4].CF_Inst.U3
xnor 42 83 # \Q12_inst1.\Inst[5].CF_Inst.U3
xnor 84 60 # \Q12_inst1.\Inst[6].CF_Inst.U2
xnor 43 85 # \Q12_inst1.\Inst[7].CF_Inst.U3
xnor 44 86 # \Q12_inst1.\Inst[8].CF_Inst.U3
nand 64 88 # \Q12_inst1.\Inst[10].CF_Inst.U2
xnor 45 90 # \Q12_inst1.\Inst[11].CF_Inst.U4
nand 58 92 # \Q12_inst1.\Inst[13].CF_Inst.U2
xnor 46 94 # \Q12_inst1.\Inst[14].CF_Inst.U4
xnor 47 96 # \Q12_inst1.\Inst[16].CF_Inst.U4
nand 61 97 # \Q12_inst1.\Inst[17].CF_Inst.U2
reg 95 # \Q12_inst1.CF_Reg_reg[15]
reg 91 # \Q12_inst1.CF_Reg_reg[12]
reg 87 # \Q12_inst1.CF_Reg_reg[9]
nand 114 89 # \Q12_inst1.\Inst[10].CF_Inst.U4
nand 116 93 # \Q12_inst1.\Inst[13].CF_Inst.U4
nand 119 98 # \Q12_inst1.\Inst[17].CF_Inst.U4
reg 118 # \Q12_inst1.CF_Reg_reg[16]
reg 117 # \Q12_inst1.CF_Reg_reg[14]
reg 115 # \Q12_inst1.CF_Reg_reg[11]
reg 113 # \Q12_inst1.CF_Reg_reg[8]
reg 112 # \Q12_inst1.CF_Reg_reg[7]
reg 111 # \Q12_inst1.CF_Reg_reg[6]
reg 110 # \Q12_inst1.CF_Reg_reg[5]
reg 109 # \Q12_inst1.CF_Reg_reg[4]
reg 108 # \Q12_inst1.CF_Reg_reg[3]
reg 107 # \Q12_inst1.CF_Reg_reg[2]
reg 106 # \Q12_inst1.CF_Reg_reg[1]
reg 105 # \Q12_inst1.CF_Reg_reg[0]
xnor 123 29 # \Q12_inst1.\Inst[10].CF_Inst.U5
xnor 124 27 # \Q12_inst1.\Inst[13].CF_Inst.U5
xnor 125 29 # \Q12_inst1.\Inst[17].CF_Inst.U5
xnor 135 137 # \Q12_inst1.\InstXOR[0].Compression1.U1
xnor 132 134 # \Q12_inst1.\InstXOR[0].Compression2.U1
xnor 129 131 # \Q12_inst1.\InstXOR[0].Compression3.U1
xnor 128 122 # \Q12_inst1.\InstXOR[1].Compression1.U1
xnor 127 121 # \Q12_inst1.\InstXOR[1].Compression2.U1
xnor 28 138 # \Q12_inst1.\Inst[10].CF_Inst.U6
xnor 26 139 # \Q12_inst1.\Inst[13].CF_Inst.U6
xnor 24 140 # \Q12_inst1.\Inst[17].CF_Inst.U6
xnor 141 136 # \Q12_inst1.\InstXOR[0].Compression1.U2
xnor 142 133 # \Q12_inst1.\InstXOR[0].Compression2.U2
xnor 143 130 # \Q12_inst1.\InstXOR[0].Compression3.U2
reg 148 # \Q12_inst1.CF_Reg_reg[17]
reg 147 # \Q12_inst1.CF_Reg_reg[13]
reg 146 # \Q12_inst1.CF_Reg_reg[10]
xnor 151 72 # \MiddleAffine.U1
xnor 150 73 # \MiddleAffine.U3
xnor 149 74 # \MiddleAffine.U5
xnor 144 154 # \Q12_inst1.\InstXOR[1].Compression1.U2
xnor 145 153 # \Q12_inst1.\InstXOR[1].Compression2.U2
xnor 152 120 # \Q12_inst1.\InstXOR[1].Compression3.U1
reg 157 # \M2_out1_reg_reg[3]
reg 156 # \M2_out2_reg_reg[3]
reg 155 # \M2_out3_reg_reg[3]
xnor 160 126 # \Q12_inst1.\InstXOR[1].Compression3.U2
xor 100 159 # \MiddleAffine.U4
xor 101 158 # \MiddleAffine.U6
not 162 # \Q12_inst2.\Inst[10].CF_Inst.U1
nand 162 104 # \Q12_inst2.\Inst[10].CF_Inst.U3
not 163 # \Q12_inst2.\Inst[13].CF_Inst.U1
nand 163 102 # \Q12_inst2.\Inst[13].CF_Inst.U3
not 161 # \Q12_inst2.\Inst[17].CF_Inst.U1
nand 161 103 # \Q12_inst2.\Inst[17].CF_Inst.U3
reg 161 # \Q12_inst2.dreg_reg[3]
reg 163 # \Q12_inst2.dreg_reg[2]
reg 162 # \Q12_inst2.dreg_reg[1]
reg 158 # \M2_out1_reg_reg[1]
reg 159 # \M2_out2_reg_reg[1]
xor 99 164 # \MiddleAffine.U2
nand 177 162 # \Q12_inst2.\Inst[0].CF_Inst.U1
nand 177 163 # \Q12_inst2.\Inst[2].CF_Inst.U2
nand 176 163 # \Q12_inst2.\Inst[4].CF_Inst.U2
nand 176 161 # \Q12_inst2.\Inst[6].CF_Inst.U1
nand 176 162 # \Q12_inst2.\Inst[7].CF_Inst.U2
nand 177 161 # \Q12_inst2.\Inst[8].CF_Inst.U2
xor 103 177 # \Q12_inst2.\Inst[9].CF_Inst.U1
xor 103 177 # \Q12_inst2.\Inst[11].CF_Inst.U2
nand 176 169 # \Q12_inst2.\Inst[13].CF_Inst.U2
xor 102 176 # \Q12_inst2.\Inst[15].CF_Inst.U1
xor 102 176 # \Q12_inst2.\Inst[16].CF_Inst.U2
nand 177 171 # \Q12_inst2.\Inst[17].CF_Inst.U2
reg 166 # \M2_out1_reg_reg[0]
reg 165 # \M2_out2_reg_reg[0]
reg 164 # \M2_out3_reg_reg[1]
xnor 179 104 # \Q12_inst2.\Inst[0].CF_Inst.U2
nand 193 162 # \Q12_inst2.\Inst[1].CF_Inst.U2
xnor 49 180 # \Q12_inst2.\Inst[2].CF_Inst.U3
nand 193 163 # \Q12_inst2.\Inst[3].CF_Inst.U1
xnor 50 181 # \Q12_inst2.\Inst[4].CF_Inst.U3
nand 193 161 # \Q12_inst2.\Inst[5].CF_Inst.U2
xnor 182 103 # \Q12_inst2.\Inst[6].CF_Inst.U2
xnor 52 183 # \Q12_inst2.\Inst[7].CF_Inst.U3
xnor 53 184 # \Q12_inst2.\Inst[8].CF_Inst.U3
and 162 185 # \Q12_inst2.\Inst[9].CF_Inst.U2
nand 193 167 # \Q12_inst2.\Inst[10].CF_Inst.U2
nand 186 163 # \Q12_inst2.\Inst[11].CF_Inst.U3
xor 104 193 # \Q12_inst2.\Inst[12].CF_Inst.U1
nand 187 170 # \Q12_inst2.\Inst[13].CF_Inst.U4
xor 104 193 # \Q12_inst2.\Inst[14].CF_Inst.U2
and 161 188 # \Q12_inst2.\Inst[15].CF_Inst.U2
nand 189 162 # \Q12_inst2.\Inst[16].CF_Inst.U3
nand 190 172 # \Q12_inst2.\Inst[17].CF_Inst.U4
reg 191 # \Q12_inst2.areg_reg[3]
reg 192 # \Q12_inst2.areg_reg[1]
reg 178 # \M2_out3_reg_reg[0]
xnor 48 195 # \Q12_inst2.\Inst[1].CF_Inst.U3
xnor 197 102 # \Q12_inst2.\Inst[3].CF_Inst.U2
xnor 51 199 # \Q12_inst2.\Inst[5].CF_Inst.U3
nand 204 168 # \Q12_inst2.\Inst[10].CF_Inst.U4
xnor 54 205 # \Q12_inst2.\Inst[11].CF_Inst.U4
and 163 206 # \Q12_inst2.\Inst[12].CF_Inst.U2
xnor 207 15 # \Q12_inst2.\Inst[13].CF_Inst.U5
nand 208 161 # \Q12_inst2.\Inst[14].CF_Inst.U3
xnor 56 210 # \Q12_inst2.\Inst[16].CF_Inst.U4
xnor 211 17 # \Q12_inst2.\Inst[17].CF_Inst.U5
reg 214 # \Q12_inst2.areg_reg[2]
reg 209 # \Q12_inst2.CF_Reg_reg[15]
reg 203 # \Q12_inst2.CF_Reg_reg[9]
reg 202 # \Q12_inst2.CF_Reg_reg[8]
reg 201 # \Q12_inst2.CF_Reg_reg[7]
reg 200 # \Q12_inst2.CF_Reg_reg[6]
reg 198 # \Q12_inst2.CF_Reg_reg[4]
reg 196 # \Q12_inst2.CF_Reg_reg[2]
reg 194 # \Q12_inst2.CF_Reg_reg[0]
xnor 218 17 # \Q12_inst2.\Inst[10].CF_Inst.U5
xnor 14 221 # \Q12_inst2.\Inst[13].CF_Inst.U6
xnor 55 222 # \Q12_inst2.\Inst[14].CF_Inst.U4
xnor 12 224 # \Q12_inst2.\Inst[17].CF_Inst.U6
xnor 232 233 # \Q12_inst2.\InstXOR[0].Compression1.U1
xnor 228 230 # \Q12_inst2.\InstXOR[0].Compression3.U1
reg 223 # \Q12_inst2.CF_Reg_reg[16]
reg 220 # \Q12_inst2.CF_Reg_reg[12]
reg 219 # \Q12_inst2.CF_Reg_reg[11]
reg 217 # \Q12_inst2.CF_Reg_reg[5]
reg 216 # \Q12_inst2.CF_Reg_reg[3]
reg 215 # \Q12_inst2.CF_Reg_reg[1]
xnor 16 234 # \Q12_inst2.\Inst[10].CF_Inst.U6
xnor 238 245 # \Q12_inst2.\InstXOR[0].Compression1.U2
xnor 243 244 # \Q12_inst2.\InstXOR[0].Compression2.U1
xnor 239 229 # \Q12_inst2.\InstXOR[0].Compression3.U2
xnor 242 227 # \Q12_inst2.\InstXOR[1].Compression1.U1
reg 237 # \Q12_inst2.CF_Reg_reg[17]
reg 236 # \Q12_inst2.CF_Reg_reg[14]
reg 235 # \Q12_inst2.CF_Reg_reg[13]
xnor 248 231 # \Q12_inst2.\InstXOR[0].Compression2.U2
xnor 252 241 # \Q12_inst2.\InstXOR[1].Compression2.U1
xnor 251 226 # \Q12_inst2.\InstXOR[1].Compression3.U1
reg 246 # \Q12_inst2.CF_Reg_reg[10]
xor 173 249 # \OutputAffine.U1
xor 175 247 # \OutputAffine.U3
xnor 250 257 # \Q12_inst2.\InstXOR[1].Compression1.U2
xnor 255 253 # \Q12_inst2.\InstXOR[1].Compression2.U2
xnor 256 240 # \Q12_inst2.\InstXOR[1].Compression3.U2
xor 174 254 # \OutputAffine.U2
not 260 # \OutputAffine.U4
out 264 3_0 # out1[3]
out 259 2_0 # out1[2]
out 213 1_0 # out1[1]
out 247 0_0 # out1[0]
out 261 3_1 # out2[3]
out 263 2_1 # out2[2]
out 225 1_1 # out2[1]
out 254 0_1 # out2[0]
out 262 3_2 # out3[3]
out 258 2_2 # out3[2]
out 212 1_2 # out3[1]
out 249 0_2 # out3[0]
