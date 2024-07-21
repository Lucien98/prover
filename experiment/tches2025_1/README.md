# About

This folder contains the additional data for tches 2025_1 submission (comparing to tches 2024_4 submission).

For comparing four tools, we included the time for elaborating ROBDD.

The verification procedure of SILVER and Prover including:

  - Parsing
  - Elaboration
  - Verification

In our previous submissions, we only compared SILVER and Prover. And for convenience, we only compared the verification time of SILVER and Prover, excluding the Parsing time and Elaboration time.

If we do so when comparing with CocoAlma and maskVerif, it would be a little unfair since we omits the parsing and elaboration time. So in our submission for tches 2025_1, we have included the elaboration time when comparing to coco and maskVerif. We did not include the parsing time since we did not have enough time to get the timing data. So it is a little unfair comparing to CocoAlma and maskVerif, although we believe it would not affect the conclusion.

## Detailed analysis for TINU implementation of PRESENT S-box

In [The First Thorough Side-Channel Hardware Trojan](https://link.springer.com/chapter/10.1007/978-3-319-70694-8_26)<sup><a href="#ref1">[1]</a></sup>, PRESENT S-box is decomposed as $\mathcal F\circ \mathcal G$. $\mathcal G$ is not uniform without correction terms. The flawed TINU implementation is implemented with $\mathcal G$ without correction terms.

First, we changed the code of Prover a bit to get all the leaky probes of TINU implementation from <a href="#ref1">[1]</a>.
<!-- In fact, the TINU implementation is the mask -->

```
--- a/src/Silver.cpp
+++ b/src/Silver.cpp
@@ -885,7 +885,7 @@ Silver::reduce_Probing(Circuit &model, std::map<int, Probes> inputs, const int p
     int num_subset = 0;
     long total_combinations = 0;
     long number_rule = 0;
-    for (int order = 0; order < probingOrder; order++) {
+    for (int order = 0; order < 1/*probingOrder*/; order++) {
         std::vector<Node> probes(order + 1);
         std::vector<bool> bitmask(positions.size());
         std::fill(bitmask.begin(), bitmask.begin() + (order + 1), true);
@@ -1086,7 +1086,9 @@ Silver::reduce_Probing(Circuit &model, std::map<int, Probes> inputs, const int p
                 if (!independent) {
                     if (verbose == 2) printf("independent: comb = %d \n", comb);
                     if (verbose == 0) printf("%ld,%ld,%ld,", total_combinations, num_subset, number_rule);
-                    return probes;
+                    printf("%d\n", probes[0]);
+                    // return probes;
+                    break;
                 }
                 if (!robustModel) break;
             }
```

Then we remake and run Prover:

```
lucien@ubuntu:/mnt/hgfs/VMShared/SILVER/SILVER$ ./bin/verify --insfile vlog/present/PRESENT_Sbox_TI_non-uniform/3-Netlist/sharedSbox_non-uniform.nl --usesubset 0
[     0.000] Netlist: vlog/present/PRESENT_Sbox_TI_non-uniform/3-Netlist/sharedSbox_non-uniform.nl
[     0.008] Parse: 161 gate(s) / 268 signal(s)
[     1.924] Elaborate: 161 gate(s) / 268 signal(s)
137
139
140
141
142
144
145
146
147
[     1.952] probing.standard (d ≤ 2) -- PASS.	>> Probes: "4,8,12"
149
150
151
153
154
155
157
158
159
[     3.721] probing.robust   (d ≤ 2) -- PASS.	>> Probes: "4,8,12"
[     5.555] uniformity               -- FAIL.
```

We can see that the leaky probes in standard probing model are at line `137,139,140,141,142,144,145,146,147`+1 and the leaky probes in standard probing model are at line `149,150,151,153,154,155,157,158,159`+1.

In fact, they are identical. Leakage only exists on three msb of the output shares, i.e. the lsb of $\mathcal F$ is not leaky while the rest three bits of $\mathcal F$ is leaky.


| id(std) | at line | id(rob) | at line |           |
|---------|---------|---------|---------|-----------|
| 137     | 138     | 155     | 156     | share2[1] |
| 139     | 140     | 153     | 154     | share2[3] |
| 140     | 141     | 157     | 158     | share3[3] |
| 141     | 142     | 158     | 159     | share3[2] |
| 142     | 143     | 159     | 160     | share3[1] |
| 144     | 145     | 149     | 150     | share1[3] |
| 145     | 146     | 150     | 151     | share1[2] |
| 146     | 147     | 151     | 152     | share1[1] |
| 147     | 148     | 154     | 155     | share2[2] |

### results of CocoAlma

CocoAlma reports that `_15_` is leaky.

```
  assign share1[2] = _14_ ^ _15_;
  assign _15_ = \y3.q [1] & \y2.q [0];
  assign _14_ = _12_ ^ _13_;
  assign _13_ = \y2.q [1] & \y3.q [0];
  assign _12_ = \y2.q [3] ^ _11_;
  assign _11_ = \y2.q [1] & \y2.q [0];
```

In the paper <a href="#ref1">[1]</a>, `share1[2]` is denoted as $y_2^1$ in Appendix A.

$y_2^1=d^2\oplus b^2a^2\oplus b^2a^3\oplus b^3a^2$

We can infer that `_15_` $=b^3a^2$

\[
	\begin{align*}
	    &\text{_15_} =b^3a^2\\
	    &\text{_11_} =b^2a^2\\
	    &\text{_12_} =d^2 + b^2a^2\\
	    &\text{_13_} =b^2a^3\\
	    &\text{_14_} =(d^2 + b^2a^2) + b^2a^3\\
	\end{align*}
\]


From [Synthesized Netlist of TINU](https://github.com/Lucien98/prover/blob/uniformity/vlog/present/PRESENT_Sbox_TI_non-uniform/2-Synthesized/sharedSbox_non-uniform.v)<sup><a href="#ref2">[2]</a></sup>, we can obtain the following equations hold:
```
share2[1] = g2Reg[3] + f1_n11 + 1
f1_n11 = f1_n9 + f1_n10 + 1
f1_n9 = ~(g3Reg[1] & g2Reg[0])
f1_n10 = ~(f1_n8 & g2Reg[1])
f1_n8 = g2Reg[0] + g3Reg[0]
```

We can know that `g2Reg[3]` is $d^2$, `g3Reg[1]` is $b^3$, `g2Reg[0]` is $a^2$, `g2Reg[1]` is $b^2$, `g3Reg[0]` is $a^3$.


\[
	\begin{align*}
		&\text{f1_n8} =  a^3+a^2 & \\
		&\text{f1_n10} =  (a^3+a^2)b^2+1 & \\
		& \text{f1_n9} =  b^3a^2+1 & \\
		& \text{f1_n11} =  b^3a^2+(a^3+a^2)b^2 + 1 & \\
		& \text{share1[2]} = d^2 +b^3a^2+(a^3+a^2)b^2 & \\
	\end{align*}
\]


SILVER and Prover did not report leakage for `f1_n9` = $b^3a^2+1$ but did for `share1[2]`.

So it is a false positive case for cocoalma to report `_15_`  $=b^3a^2$ is insecure.

However, CocoAlma correctly report the leakage caused by `share1[2]` in glitch-extended probing model.

### results of maskVerif
maskVerif report `tmp102` (under standard probing model) and `tmp135` (under glitch-extended probing model) to be leaky <sup><a href="#ref3">[3]</a></sup>. In fact, `tmp102` and `tmp135` correspond to line 103 and 153 in the  [Netlist file of TINU](https://github.com/Lucien98/prover/blob/uniformity/vlog/present/PRESENT_Sbox_TI_non-uniform/3-Netlist/sharedSbox_non-uniform.nl)<sup><a href="#ref4">[4]</a></sup>. Prover and SILVER did not report it to be leaky so they are false positives.

## Detailed analysis for $L_2^4$ implementation of PRINCE S-box
First, we changed the code of Prover a little bit to get all the leaky probes of $L_2^4$ implementation from [Low_Random_Masking](https://github.com/Chair-for-Security-Engineering/Low_Random_Masking)<sup><a href="#ref5">[5]</a></sup>.

```
--- a/src/Silver.cpp
+++ b/src/Silver.cpp
@@ -885,7 +885,7 @@ Silver::reduce_Probing(Circuit &model, std::map<int, Probes> inputs, const int p
     int num_subset = 0;
     long total_combinations = 0;
     long number_rule = 0;
-    for (int order = 0; order < probingOrder; order++) {
+    for (int order = 0; order < 2/*probingOrder*/; order++) {
         std::vector<Node> probes(order + 1);
         std::vector<bool> bitmask(positions.size());
         std::fill(bitmask.begin(), bitmask.begin() + (order + 1), true);
@@ -1086,7 +1086,10 @@ Silver::reduce_Probing(Circuit &model, std::map<int, Probes> inputs, const int p
                 if (!independent) {
                     if (verbose == 2) printf("independent: comb = %d \n", comb);
                     if (verbose == 0) printf("%ld,%ld,%ld,", total_combinations, num_subset, number_rule);
-                    return probes;
+                    printf("%d,", probes[0]);
+                    if (order == 1) printf("%d\n", probes[1]);
+                    // return probes;
+                    break;
                 }
                 if (!robustModel) break;
             }
```

Then we remake and run Prover:
```
lucien@ubuntu:~/git/prover$ ./bin/verify --insfile test/prince/d2/PRINCE_Sbox_Inverse_4stageL2.nl --usesubset 0
[     0.000] Netlist: test/prince/d2/PRINCE_Sbox_Inverse_4stageL2.nl
[     0.015] Parse: 497 gate(s) / 737 signal(s)
[     0.755] Elaborate: 497 gate(s) / 737 signal(s)
167, 318
[    22.992] probing.standard (d ≤ 2) -- PASS.	>> Probes: "4,8,12"
214, 349
[   635.131] probing.robust   (d ≤ 2) -- PASS.	>> Probes: "4,8,12"
[   708.653] uniformity               -- PASS.

```

From the output, we can see that the leakage reported in the paper is the only leakage of this implementation. For more details about the leakage and our paper, please refer to the README of [SynLowRandomMasking](https://github.com/Lucien98/SynLowRandomMasking)<sup><a href="#ref6">[6]</a></sup>

**Leakage under standard probing model**

```
167(at line 168): \Q294_inst1.\Inst[16].CF_Inst.U3
318(at line 319): \Q294_inst2.\Inst[15].CF_Inst.U5
```

**Leakage under glitch-extended probing model**:

```
214(at line 215): \Q294_inst1.CF_Reg_reg[16]
349(at line 319): \Q294_inst2.CF_Reg_reg[15]
```

### results of CocoAlma

One can see the leakge results from \Q294_inst1 and \Q294_inst2, while leakage under standard reported by CocoAlma results from \Q294_inst3. Therefore they are all false positives. 

**Leakage under standard probing model reported by CocoAlma**
```
xor _171_[0] = \Q294_inst3.CF_Reg [6] ^ \Q294_inst3.CF_Reg [7];
dff Q294_inst3.CF_Reg[12]
```
**Leakage under glitch-extended probing model reported by CocoAlma**
```
xor Q294_inst3.CF_Out[7]
xor Q294_inst3.CF_Out[11]
```

### result of maskVerif
maskVerif reported `tmp167` and `tmp318` to be not secure under standard probing model, which is genuine.

maskVerif reported `tmp415` and `tmp418` (corresponding to line 457 and 458 in the [`.nl` file](https://github.com/Lucien98/SynLowRandomMasking/blob/main/PRINCE/4stage/nl/unflattened/PRINCE_Sbox_Inverse_4stageL2_broken.nl) <sup><a href="#ref7">[7]</a></sup>
 ) to be not secure under glitch-extended probing model, which is a false positive case.

## Reference
1. <p><a name = "ref1"></a>The First Thorough Side-Channel Hardware Trojan.https://link.springer.com/chapter/10.1007/978-3-319-70694-8_26</p>
2. <p><a name = "ref2"></a>Synthesized Netlist of TINU.https://github.com/Lucien98/prover/blob/uniformity/vlog/present/PRESENT_Sbox_TI_non-uniform/2-Synthesized/sharedSbox_non-uniform.v</p>
3. <p><a name = "ref3"></a>Results of maskVerif.https://github.com/Lucien98/maskVerif_evaluation/blob/main/experiments/tches/results_ng.txt</p>
4. <p><a name = "ref4"></a>Netlist file of TINU.https://github.com/Lucien98/prover/blob/uniformity/vlog/present/PRESENT_Sbox_TI_non-uniform/3-Netlist/sharedSbox_non-uniform.nl</p>
5. <p><a name = "ref5"></a>Low_Random_Masking.https://github.com/Chair-for-Security-Engineering/Low_Random_Masking</p>
6. <p><a name = "ref6"></a>SynLowRandomMasking.https://github.com/Lucien98/SynLowRandomMasking</p>
7. <p><a name = "ref7"></a>Netlist file of L24 impl. of PRINCE S-box.https://github.com/Lucien98/SynLowRandomMasking/blob/main/PRINCE/4stage/nl/unflattened/PRINCE_Sbox_Inverse_4stageL2_broken.nl</p>

