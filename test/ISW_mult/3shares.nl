in 0 0_0 #  a[0]
in 1 0_1 #  a[1]
in 2 0_2 #  a[2]
in 3 1_0 #  b[0]
in 4 1_1 #  b[1]
in 5 1_2 #  b[2]
ref 6 #  r01
ref 7 #  r02
ref 8 #  r12
and 0 4 #  tmp = a0 * b1 --> 9
xor 6 9 #  r10 = r01 + tmp --> 10
and 1 3 #  tmp = a1 * b0 --> 11
xor 10 11 #  r10 = r10 + tmp --> 12
and 0 5 #  tmp = a0 * b2 --> 13
xor 7 13 #  r20 = r02 + tmp --> 14
and 2 3 #  tmp = a2 * b0 --> 15
xor 14 15 #  r20 = r20 + tmp --> 16
and 1 5 #  tmp = a1 * b2 --> 17
xor 8 17 #  r21 = r12 + tmp --> 18
and 2 4 #  tmp = a2 * b1 --> 19
xor 18 19 #  r21 = r21 + tmp --> 20
and 0 3 #  tmp = a0 * b0 --> 21
xor 21 6 #  c0 = tmp + r01 --> 22
xor 22 7 #  c0 = c0 + r02 --> 23
and 1 4 #  tmp = a1 * b1 --> 24
xor 24 12 #  c1 = tmp + r10 --> 25
xor 25 8 #  c1 = c1 + r12 --> 26
and 2 5 #  tmp = a2 * b2 --> 27
xor 27 16 #  c2 = tmp + r20 --> 28
xor 28 20 #  c2 = c2 + r21 --> 29
out 23 0_0 #  c0
out 26 0_1 #  c1
out 29 0_2 #  c2
