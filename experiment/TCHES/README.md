# About
This folder contains the original data for paper3 submission for TCHES 2024_4, which was submitted to TCHES before 16th April 2024. This submission received `Major Revision` decision.

### Changes
Compared to the submission for TOSC, we include the improvements on standard probing security check and uniformity check, as well as three more benchmarks.

  - AES_sbox_TI$_1$
  - Keccak_sbox_NF$_1$
  - Keccak_sbox_DOM_NF$_1$

### Table 3
The folder `varorder` contains the original data for Table 3: Comparison between different variables ordering.

### Table 4

**SILVER**

The folder `verify` contains the original data of SILVER (standard probing security check and glitch-extended probing security check) in Table 4: Comparison between SILVER and Prover.

It seems the timeout is not that right? For glitch-extended probing security, we seemed to forget to rerun the experiments with 24 hour limit (while when we replace data, we changed the `360` to `864`).

**Prover**

The folder `prover` contains the original data of `Prover` (standard probing security check and glitch-extended probing security check) in Table 4: Comparison between SILVER and Prover.

