/*
 * Copyright (C) 2011-2021 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

#include <vector>
#include <thread>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <ios>

#include "../Include/consts.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret) {
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void) {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    
    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }

    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str) {
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

uint64_t convert2ms(uint64_t time) { return time / (CLOCKS_PER_US * 1000); }
uint64_t convert2us(uint64_t time) { return time / (CLOCKS_PER_US); }
uint64_t convert2ns(uint64_t time) { return time * 1000 / (CLOCKS_PER_US);}

void worker(int thid) {
    ecall_worker(global_eid, thid);
}

bool start = false;
bool quit = false;
std::vector<int> readys(MAX_THREAD_NUM);
std::vector<int> result(MAX_THREAD_NUM);
std::vector<uint64_t> newTimes(MAX_THREAD_NUM);
std::vector<uint64_t> delTimes(MAX_THREAD_NUM);

void refresh() {
    start = false;
    quit = false;
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        readys[i] = 0;
        result[i] = 0;
        newTimes[i] = 0;
        delTimes[i] = 0;
    }
}

void waitForReady(int th_num) {
    while (true) {
        bool failed = false;
        for (int i = 0; i < th_num; i++) {
            if (!__atomic_load_n(&readys[i], __ATOMIC_ACQUIRE)) {
                failed = true;
                break;
            }
        }
        if (!failed) break;
    }
}

void sendStart() {
    __atomic_store_n(&start, true, __ATOMIC_RELEASE);
}

void sendQuit() {
    __atomic_store_n(&quit, true, __ATOMIC_RELEASE);
}

void normal_worker(int thid) {
    uint64_t newTime = 0;
    uint64_t delTime = 0;
    __atomic_store_n(&readys[thid], 1, __ATOMIC_RELEASE);
    while (!__atomic_load_n(&start, __ATOMIC_ACQUIRE)) continue;
    
    while (!__atomic_load_n(&quit, __ATOMIC_ACQUIRE)) {
        // memory allocation using new
        uint64_t startTime1 = rdtscp();
        Tuple *temp = new Tuple("test", 0);
        uint64_t endTime1 = rdtscp();

        // some process
        temp->init();

        // memory deallocation using delete
        uint64_t startTime2 = rdtscp();
        delete temp;
        uint64_t endTime2 = rdtscp();

        // result
        result[thid]++;
        newTime += (endTime1 - startTime1);
        delTime += (endTime2 - startTime2);
    }
    newTimes[thid] = newTime;
    delTimes[thid] = delTime;
}

/* Application entry */
int SGX_CDECL main() {
    std::cout << "size of Tuple is " << sizeof(Tuple) << "Byte" << std::endl;
    std::cout << "- size of Tidword is " << sizeof(Tidword) << "Byte" << std::endl;
    std::cout << "- size of std::string is " << sizeof(std::string) << "Byte" << std::endl;
    std::cout << "- size of int is " << sizeof(int) << "Byte (padding: 4Byte)" << std::endl;

    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1;
    }
 
    std::vector<int> normal_iteration_result;     // Number of iterations for each thread number
    std::vector<int> enclave_iteration_result;    // Number of iterations for each thread number
    std::vector<std::vector<uint64_t>> normal_latency_result;
    std::vector<std::vector<uint64_t>> encalve_latency_result;

    for (int i = 1; i <= 128; i *= 2) {
        // non-enclave
        {
            std::cout << "=== non-enclave\t: thread = " << i << " ===" << std::endl;

            // initialize
            refresh();

            // create thread
            std::vector<std::thread> th;
            for (int j = 0; j < i; j++) {
                th.emplace_back(normal_worker, j);
            }

            // execution
            waitForReady(i);
            sendStart();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 * EXTIME));
            sendQuit();
            for (auto &t : th) t.join();

            // show result
            int normal_total_iteration_counts = 0;
            for (int j = 0; j < i; j++) {
                std::cout << "thread #" << j << ":\t" << result[j] << " times"<< std::endl;
                normal_total_iteration_counts += result[j];
            }
            std::cout << "total:\t" << normal_total_iteration_counts << " times" << std::endl << std::endl;

            // store result
            normal_iteration_result.emplace_back(normal_total_iteration_counts);


            // calculate latency (the latency is calculated as the average execution time per operation)
            uint64_t normal_total_new_time = 0;
            uint64_t normal_total_del_time = 0;

            for (int i = 0; i < MAX_THREAD_NUM; i++) {
                normal_total_new_time += newTimes[i];
                normal_total_del_time += delTimes[i];
            }

            // Each iteration involves both a 'new' and a 'delete' operation. 
            // Therefore, calculating the average latency by dividing the total time spent on 'new'/'delete' operations by the total number of iterations is a valid approach.
            normal_latency_result.emplace_back(
                std::vector<uint64_t> {normal_total_new_time / normal_total_iteration_counts, 
                                       normal_total_del_time / normal_total_iteration_counts});
        }

        // enclave
        {
            std::cout << "=== enclave\t: thread = " << i << " ===" << std::endl;

            // initialize
            ecall_refresh(global_eid);

            // create thread
            std::vector<std::thread> th;
            for (int j = 0; j < i; j++) {
                th.emplace_back(worker, j);
            }

            // execution
            ecall_waitForReady(global_eid, i);
            ecall_sendStart(global_eid);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 * EXTIME));
            ecall_sendQuit(global_eid);
            for (auto &t : th) t.join();

            // show result
            int enclave_total = 0;
            for (int j = 0; j < i; j++) {
                // get ops(operation per second)
                int ret;
                ecall_getResult(global_eid, &ret, j);
                std::cout << "thread #" << j << ":\t" << ret << " times" << std::endl;
                enclave_total += ret;
            }
            std::cout << "total:\t" << enclave_total << " times" << std::endl << std::endl;

            enclave_iteration_result.emplace_back(enclave_total);

            // Get the execution times for 'new' and 'delete' operations from the enclave.
            // NOTE: The latency is calculated as the average execution time per operation.
            // It is obtained by dividing the total execution time of memory allocation (or deallocation) 
            // by the total number of iterations.
            uint64_t enclave_total_new_time = 0;
            uint64_t enclave_total_del_time = 0;
            ecall_getTime(global_eid, &enclave_total_new_time, 0);
            ecall_getTime(global_eid, &enclave_total_del_time, 1);

            encalve_latency_result.emplace_back(
                std::vector<uint64_t> {enclave_total_new_time / enclave_total, 
                                       enclave_total_del_time / enclave_total});
        }
    }

    // show thoughput
    int width = 12;
    std::cout << "=== thoughput(operation per seconds) ===" << std::endl;
    std::cout << std::left << std::setw(width) << "#worker" << std::left << std::setw(width) << "normal" << std::left << std::setw(width) << "enclave" << std::endl;
    int count = 0;
    
    for (int i = 1; i <= 128; i *= 2) {
        std::cout << std::left << std::setw(width) << i << std::left << std::setw(width) << normal_iteration_result[count]/EXTIME << std::left << std::setw(width) << enclave_iteration_result[count]/EXTIME << std::endl;
        count++;
    }

    std::cout << std::endl;

    // show latency
    std::cout << "=== latency(new/delete) ===" << std::endl;
    std::cout << std::left << std::setw(width) << "#worker" << std::left << std::setw(width) << "normal" << std::left << std::setw(width) << "enclave" << std::endl;
    count = 0;

    for (int i = 1; i <= 128; i *= 2) {
        std::cout << std::left << std::setw(width) << i <<
        std::left << std::setw(width) << std::to_string(convert2ns(normal_latency_result[count][0])) + "ns/" + 
                                         std::to_string(convert2ns(normal_latency_result[count][1])) + "ns" << 
        std::left << std::setw(width) << std::to_string(convert2ns(encalve_latency_result[count][0])) + "ns/"+ 
                                         std::to_string(convert2ns(encalve_latency_result[count][1])) + "ns" << std::endl;
        count++;
    }

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    return 0;
}

