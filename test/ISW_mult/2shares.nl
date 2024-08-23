in 0 0_0 #  a[0]
in 1 0_1 #  a[1]
in 2 1_0 #  b[0]
in 3 1_1 #  b[1]
ref 4 #  r01
and 0 3 #  tmp = a0 * b1 --> 5
xor 4 5 #  r10 = r01 + tmp --> 6
and 1 2 #  tmp = a1 * b0 --> 7
xor 6 7 #  r10 = r10 + tmp --> 8
and 0 2 #  tmp = a0 * b0 --> 9
xor 9 4 #  c0 = tmp + r01 --> 10
and 1 3 #  tmp = a1 * b1 --> 11
xor 11 8 #  c1 = tmp + r10 --> 12
out 10 0_0 #  c0
out 12 0_1 #  c1
