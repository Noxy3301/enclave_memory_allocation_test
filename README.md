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

```
=== thoughput(operation per seconds) ===
#worker normal      enclave
1       10791812    7571865
2       5963380     1069994
4       5608810     567033
8       6400805     464459
16      7088161     358555
32      17491951    241753
64      24548419    181613
128     40145095    80162

=== latency(new/delete) ===
#worker normal      enclave
1       6us/0us     8us/8us
2       11us/0us    60us/59us
4       12us/0us    113us/111us
8       10us/0us    138us/136us
16      9us/0us     179us/176us
32      3us/0us     266us/261us
64      2us/0us     354us/347us
128     1us/0us     803us/788us
```

[note] Latency calculation formula is discribed below.<br>
- (execution time)/(operation count)
    - execution time
        - Total execution time of operators(e.g., new, delete). Not thread execution time(EXTIME).
    - operation count
        - Total number of operations executed.

# Discussion
Since the dynamic memory allocation can be performed in parallel, the throughput increases with the number of worker threads. However, as the number of worker threads increased, the throughput within the enclave decreased.
We observed an overhead of up to 800 times higher with 128 worker threads.
Therefore, it can be said that parallel dynamic memory allocation inside the enclave is not desirable.