# enclave_memory_allocation_test

# About
This program evaluates the performance of new operator(dynamic memory allocation) within the enclave provided by Intel SGX.

# How to use

```
$ git clone {THIS REPO}
$ wget https://download.01.org/intel-sgx/latest/linux-latest/distro/ubuntu20.04-server/sgx_linux_x64_sdk_2.19.100.3.bin
$ chmod 777 sgx_linux_x64_sdk_2.19.100.3.bin
$ ./sgx_linux_x64_sdk_2.19.100.3.bin
[yes]
$ source {ENVIRONMENT-PATH}   // indicated by green text. 
$ cd enclave_memory_allocation_test
$ make
$ ./app
```

# Measurement condition
- Operation
    - The new operator is used to acquire the memory area for the `Tuple` object. After the allocation, the allocated memory area is released using delete operator.
    - Size of `Tuple` object is 48 byte.
        - TID(Tidword)   : 8 byte
        - Key(std::string) : 32 byte
        - Value(int) : 4 byte (padding : 4 byte)

- Number of worker thread
    - 1,2,4,8,16,32,64,128

- Execution time
    - 10sec

# Result
This is an example of the execution result.

| Specification | Detail |
|---------------------|------------------------------------------|
| **CPU** | Intel(R) Xeon(R) Platinum 8357B CPU @ 2.70GHz |
| **Physical cores in a socket** | 32 |
| **Total Physical/logical cores** | 64/128 |

```
=== throughput(operation per seconds) ===
#worker     normal      enclave
1           10157041    7206712
2           13448659    2302767
4           23705233    1700325
8           16487037    1232962
16          26666491    1023332
32          36270243    776377
64          53793907    875925
128         89355741    783025

=== latency(new/delete) ===
#worker     normal      enclave
1           47ns/19ns   79ns/22ns
2           47ns/19ns   402ns/381ns
4           48ns/19ns   697ns/1560ns
8           54ns/19ns   1512ns/4871ns
16          154ns/19ns  3373ns/12128ns
32          295ns/20ns  11693ns/29335ns
64          413ns/25ns  34140ns/38688ns
128         486ns/28ns  87312ns/75668ns
```

<details>

<summary>Full result is here</summary>

```
=== non-enclave : thread = 1 ===
thread #0:      101570410 times
total:  101570410 times

=== enclave     : thread = 1 ===
thread #0:      72067120 times
total:  72067120 times

=== non-enclave : thread = 2 ===
thread #0:      67268128 times
thread #1:      67218467 times
total:  134486595 times

=== enclave     : thread = 2 ===
thread #0:      11508219 times
thread #1:      11519451 times
total:  23027670 times

=== non-enclave : thread = 4 ===
thread #0:      59373763 times
thread #1:      59223835 times
thread #2:      59225764 times
thread #3:      59228975 times
total:  237052337 times

=== enclave     : thread = 4 ===
thread #0:      4346715 times
thread #1:      4618849 times
thread #2:      3491192 times
thread #3:      4546501 times
total:  17003257 times

=== non-enclave : thread = 8 ===
thread #0:      19335444 times
thread #1:      21673669 times
thread #2:      20672489 times
thread #3:      19943175 times
thread #4:      20308794 times
thread #5:      21826947 times
thread #6:      20506965 times
thread #7:      20602895 times
total:  164870378 times

=== enclave     : thread = 8 ===
thread #0:      641365 times
thread #1:      767422 times
thread #2:      1841569 times
thread #3:      2050295 times
thread #4:      2105367 times
thread #5:      2038159 times
thread #6:      1937903 times
thread #7:      947547 times
total:  12329627 times

=== non-enclave : thread = 16 ===
thread #0:      20483217 times
thread #1:      18713429 times
thread #2:      18330826 times
thread #3:      20373571 times
thread #4:      18233416 times
thread #5:      18190077 times
thread #6:      18183009 times
thread #7:      18062475 times
thread #8:      14709497 times
thread #9:      14714874 times
thread #10:     14587428 times
thread #11:     14674871 times
thread #12:     13446406 times
thread #13:     14692891 times
thread #14:     14638138 times
thread #15:     14630786 times
total:  266664911 times

=== enclave     : thread = 16 ===
thread #0:      334781 times
thread #1:      361490 times
thread #2:      361243 times
thread #3:      389300 times
thread #4:      398689 times
thread #5:      418603 times
thread #6:      838759 times
thread #7:      905255 times
thread #8:      501359 times
thread #9:      821934 times
thread #10:     620475 times
thread #11:     730521 times
thread #12:     893430 times
thread #13:     886219 times
thread #14:     892823 times
thread #15:     878442 times
total:  10233323 times

=== non-enclave : thread = 32 ===
thread #0:      22529648 times
thread #1:      23087713 times
thread #2:      22941894 times
thread #3:      22653248 times
thread #4:      23048843 times
thread #5:      22873585 times
thread #6:      22990198 times
thread #7:      23037745 times
thread #8:      7775179 times
thread #9:      7745440 times
thread #10:     7771966 times
thread #11:     7789773 times
thread #12:     7792874 times
thread #13:     7761160 times
thread #14:     7766475 times
thread #15:     7701689 times
thread #16:     7743226 times
thread #17:     7721199 times
thread #18:     7689851 times
thread #19:     7175040 times
thread #20:     7738672 times
thread #21:     7730312 times
thread #22:     7514884 times
thread #23:     7748828 times
thread #24:     6878774 times
thread #25:     6848386 times
thread #26:     6868729 times
thread #27:     8379337 times
thread #28:     6827729 times
thread #29:     6852162 times
thread #30:     6886873 times
thread #31:     6831000 times
total:  362702432 times

=== enclave     : thread = 32 ===
thread #0:      202783 times
thread #1:      230204 times
thread #2:      201058 times
thread #3:      180439 times
thread #4:      197975 times
thread #5:      168811 times
thread #6:      311794 times
thread #7:      338389 times
thread #8:      309790 times
thread #9:      218495 times
thread #10:     400771 times
thread #11:     229228 times
thread #12:     230043 times
thread #13:     352935 times
thread #14:     311564 times
thread #15:     322290 times
thread #16:     350398 times
thread #17:     347432 times
thread #18:     311069 times
thread #19:     189276 times
thread #20:     256055 times
thread #21:     248289 times
thread #22:     201350 times
thread #23:     222458 times
thread #24:     201811 times
thread #25:     207041 times
thread #26:     209110 times
thread #27:     203586 times
thread #28:     123739 times
thread #29:     130144 times
thread #30:     131395 times
thread #31:     224049 times
total:  7763771 times

=== non-enclave : thread = 64 ===
thread #0:      19840210 times
thread #1:      19344669 times
thread #2:      22138098 times
thread #3:      22509959 times
thread #4:      19737388 times
thread #5:      19775765 times
thread #6:      19571147 times
thread #7:      19621337 times
thread #8:      5853671 times
thread #9:      5867151 times
thread #10:     5863591 times
thread #11:     6988527 times
thread #12:     5864549 times
thread #13:     5885961 times
thread #14:     5868701 times
thread #15:     5836399 times
thread #16:     6637257 times
thread #17:     5840235 times
thread #18:     5635075 times
thread #19:     6716217 times
thread #20:     5849375 times
thread #21:     5874482 times
thread #22:     5850905 times
thread #23:     5846332 times
thread #24:     7161091 times
thread #25:     7162984 times
thread #26:     7198922 times
thread #27:     7185153 times
thread #28:     3559278 times
thread #29:     3801452 times
thread #30:     4015446 times
thread #31:     7481147 times
thread #32:     4250490 times
thread #33:     7406787 times
thread #34:     5896733 times
thread #35:     7498910 times
thread #36:     5935579 times
thread #37:     6450964 times
thread #38:     6208735 times
thread #39:     7317547 times
thread #40:     6458423 times
thread #41:     6762451 times
thread #42:     6642936 times
thread #43:     6791303 times
thread #44:     6588118 times
thread #45:     6750205 times
thread #46:     6743142 times
thread #47:     6756628 times
thread #48:     6711135 times
thread #49:     6746418 times
thread #50:     6735000 times
thread #51:     6832830 times
thread #52:     6673668 times
thread #53:     6936417 times
thread #54:     6763773 times
thread #55:     6626642 times
thread #56:     10785240 times
thread #57:     4566341 times
thread #58:     10802476 times
thread #59:     11481150 times
thread #60:     10143177 times
thread #61:     10808259 times
thread #62:     10987617 times
thread #63:     3497503 times
total:  537939071 times

=== enclave     : thread = 64 ===
thread #0:      63011 times
thread #1:      122753 times
thread #2:      108455 times
thread #3:      84165 times
thread #4:      139660 times
thread #5:      39166 times
thread #6:      186525 times
thread #7:      126126 times
thread #8:      113167 times
thread #9:      49261 times
thread #10:     180356 times
thread #11:     31079 times
thread #12:     31127 times
thread #13:     208017 times
thread #14:     180209 times
thread #15:     209507 times
thread #16:     177788 times
thread #17:     206753 times
thread #18:     186151 times
thread #19:     155030 times
thread #20:     170897 times
thread #21:     205861 times
thread #22:     117267 times
thread #23:     170296 times
thread #24:     168920 times
thread #25:     89344 times
thread #26:     213281 times
thread #27:     330960 times
thread #28:     145997 times
thread #29:     232797 times
thread #30:     40741 times
thread #31:     76730 times
thread #32:     52271 times
thread #33:     101065 times
thread #34:     171008 times
thread #35:     330896 times
thread #36:     50886 times
thread #37:     50565 times
thread #38:     98231 times
thread #39:     63748 times
thread #40:     103684 times
thread #41:     207572 times
thread #42:     177857 times
thread #43:     207026 times
thread #44:     155734 times
thread #45:     205975 times
thread #46:     117143 times
thread #47:     170161 times
thread #48:     49922 times
thread #49:     212164 times
thread #50:     145651 times
thread #51:     233041 times
thread #52:     30585 times
thread #53:     210230 times
thread #54:     31632 times
thread #55:     169952 times
thread #56:     63904 times
thread #57:     181585 times
thread #58:     48229 times
thread #59:     104921 times
thread #60:     98812 times
thread #61:     181123 times
thread #62:     142429 times
thread #63:     29859 times
total:  8759258 times

=== non-enclave : thread = 128 ===
thread #0:      14268565 times
thread #1:      13860603 times
thread #2:      16616841 times
thread #3:      14576180 times
thread #4:      15822211 times
thread #5:      16689686 times
thread #6:      16746898 times
thread #7:      16457246 times
thread #8:      4326698 times
thread #9:      4353726 times
thread #10:     6515127 times
thread #11:     4402886 times
thread #12:     4383003 times
thread #13:     6523546 times
thread #14:     7173573 times
thread #15:     6930449 times
thread #16:     4351085 times
thread #17:     4280598 times
thread #18:     4366353 times
thread #19:     6355124 times
thread #20:     4280794 times
thread #21:     4337946 times
thread #22:     4398459 times
thread #23:     4420895 times
thread #24:     6405755 times
thread #25:     8849542 times
thread #26:     5894385 times
thread #27:     5907316 times
thread #28:     5767357 times
thread #29:     8923415 times
thread #30:     5929182 times
thread #31:     8900683 times
thread #32:     8680016 times
thread #33:     5903964 times
thread #34:     9144960 times
thread #35:     8925266 times
thread #36:     9348274 times
thread #37:     9149596 times
thread #38:     8932269 times
thread #39:     9228353 times
thread #40:     5005115 times
thread #41:     7175734 times
thread #42:     7189898 times
thread #43:     7145687 times
thread #44:     5069602 times
thread #45:     4976246 times
thread #46:     7130581 times
thread #47:     5001656 times
thread #48:     4911627 times
thread #49:     5065577 times
thread #50:     7183012 times
thread #51:     7390022 times
thread #52:     5147782 times
thread #53:     4946466 times
thread #54:     7219461 times
thread #55:     7173068 times
thread #56:     7344469 times
thread #57:     3612396 times
thread #58:     3551577 times
thread #59:     7199445 times
thread #60:     7244448 times
thread #61:     3416388 times
thread #62:     7329958 times
thread #63:     3516711 times
thread #64:     3435456 times
thread #65:     3566247 times
thread #66:     3407330 times
thread #67:     7413363 times
thread #68:     3590350 times
thread #69:     7431583 times
thread #70:     7169889 times
thread #71:     3521718 times
thread #72:     8280938 times
thread #73:     7814491 times
thread #74:     8600467 times
thread #75:     4444137 times
thread #76:     4334140 times
thread #77:     4258610 times
thread #78:     7923899 times
thread #79:     4262406 times
thread #80:     7871524 times
thread #81:     4229475 times
thread #82:     4329047 times
thread #83:     8183649 times
thread #84:     4272044 times
thread #85:     4277208 times
thread #86:     4235410 times
thread #87:     7726258 times
thread #88:     5532666 times
thread #89:     4993962 times
thread #90:     7660372 times
thread #91:     7475470 times
thread #92:     5527401 times
thread #93:     5525684 times
thread #94:     5623327 times
thread #95:     7908315 times
thread #96:     7819807 times
thread #97:     5552730 times
thread #98:     5117022 times
thread #99:     5576992 times
thread #100:    7977230 times
thread #101:    5129811 times
thread #102:    7610911 times
thread #103:    5546041 times
thread #104:    8568331 times
thread #105:    7717400 times
thread #106:    7294690 times
thread #107:    7158570 times
thread #108:    8987271 times
thread #109:    8628763 times
thread #110:    9006797 times
thread #111:    9010506 times
thread #112:    8985435 times
thread #113:    7188051 times
thread #114:    8648290 times
thread #115:    6855811 times
thread #116:    8217317 times
thread #117:    7005525 times
thread #118:    7490466 times
thread #119:    7005046 times
thread #120:    7081816 times
thread #121:    7574007 times
thread #122:    6810425 times
thread #123:    6731439 times
thread #124:    7379093 times
thread #125:    6928208 times
thread #126:    7623381 times
thread #127:    7227645 times
total:  893557414 times

=== enclave     : thread = 128 ===
thread #0:      124240 times
thread #1:      18710 times
thread #2:      21404 times
thread #3:      122128 times
thread #4:      103753 times
thread #5:      15465 times
thread #6:      21699 times
thread #7:      20714 times
thread #8:      16386 times
thread #9:      16738 times
thread #10:     26159 times
thread #11:     97964 times
thread #12:     99333 times
thread #13:     126063 times
thread #14:     130586 times
thread #15:     98236 times
thread #16:     108228 times
thread #17:     126867 times
thread #18:     144385 times
thread #19:     18512 times
thread #20:     87537 times
thread #21:     105442 times
thread #22:     88757 times
thread #23:     136476 times
thread #24:     150133 times
thread #25:     135456 times
thread #26:     26537 times
thread #27:     17294 times
thread #28:     108420 times
thread #29:     18466 times
thread #30:     111756 times
thread #31:     16667 times
thread #32:     116263 times
thread #33:     16881 times
thread #34:     149476 times
thread #35:     88366 times
thread #36:     16277 times
thread #37:     133790 times
thread #38:     151125 times
thread #39:     116831 times
thread #40:     105909 times
thread #41:     26294 times
thread #42:     146333 times
thread #43:     134188 times
thread #44:     79324 times
thread #45:     111021 times
thread #46:     16041 times
thread #47:     14134 times
thread #48:     19223 times
thread #49:     16628 times
thread #50:     126441 times
thread #51:     6289 times
thread #52:     14808 times
thread #53:     150096 times
thread #54:     152641 times
thread #55:     21273 times
thread #56:     144250 times
thread #57:     12622 times
thread #58:     83395 times
thread #59:     122619 times
thread #60:     103153 times
thread #61:     14422 times
thread #62:     12329 times
thread #63:     130135 times
thread #64:     17342 times
thread #65:     18549 times
thread #66:     19650 times
thread #67:     18142 times
thread #68:     20275 times
thread #69:     87360 times
thread #70:     13112 times
thread #71:     12579 times
thread #72:     45075 times
thread #73:     14493 times
thread #74:     16602 times
thread #75:     16652 times
thread #76:     134210 times
thread #77:     136897 times
thread #78:     19256 times
thread #79:     124397 times
thread #80:     105285 times
thread #81:     17611 times
thread #82:     4328 times
thread #83:     18940 times
thread #84:     16835 times
thread #85:     99714 times
thread #86:     18618 times
thread #87:     5662 times
thread #88:     79401 times
thread #89:     4310 times
thread #90:     97500 times
thread #91:     12436 times
thread #92:     14672 times
thread #93:     16827 times
thread #94:     27496 times
thread #95:     96926 times
thread #96:     117203 times
thread #97:     45706 times
thread #98:     115123 times
thread #99:     19580 times
thread #100:    152781 times
thread #101:    21586 times
thread #102:    19308 times
thread #103:    127509 times
thread #104:    19102 times
thread #105:    4113 times
thread #106:    18142 times
thread #107:    13252 times
thread #108:    14940 times
thread #109:    20557 times
thread #110:    104913 times
thread #111:    19076 times
thread #112:    4069 times
thread #113:    145773 times
thread #114:    19629 times
thread #115:    18766 times
thread #116:    26048 times
thread #117:    14487 times
thread #118:    17799 times
thread #119:    96714 times
thread #120:    14377 times
thread #121:    14153 times
thread #122:    27505 times
thread #123:    83303 times
thread #124:    20762 times
thread #125:    97274 times
thread #126:    19201 times
thread #127:    13268 times
total:  7830259 times

=== throughput(operation per seconds) ===
#worker     normal      enclave
1           10157041    7206712
2           13448659    2302767
4           23705233    1700325
8           16487037    1232962
16          26666491    1023332
32          36270243    776377
64          53793907    875925
128         89355741    783025

=== latency(new/delete) ===
#worker     normal      enclave
1           47ns/19ns   79ns/22ns
2           47ns/19ns   402ns/381ns
4           48ns/19ns   697ns/1560ns
8           54ns/19ns   1512ns/4871ns
16          154ns/19ns  3373ns/12128ns
32          295ns/20ns  11693ns/29335ns
64          413ns/25ns  34140ns/38688ns
128         486ns/28ns  87312ns/75668ns
```
</details>


[note] The latency is calculated using the following formula:<br>
- (Total execution time of operations)/(Total number of executed operations)
    - Total execution time of operations
        - Total execution time of operations (e.g., new, delete), not the overall thread execution time (EXTIME).
    - Total number of executed operations
        - Total number of operations executed.

# Discussion
Our observations indicate that the throughput of dynamic memory allocation increases with the number of worker threads due to its ability to execute in parallel.
However, an increase in the number of worker threads leads to a reduction in throughput within the enclave.
Specifically, with 128 worker threads, we observed an overhead up to 1100 times higher compared to fewer threads.
Consequently, performing parallel dynamic memory allocation inside the enclave proves to be inefficient and is not recommended.
