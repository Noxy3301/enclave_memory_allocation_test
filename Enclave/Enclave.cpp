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

#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>

#include <stdint.h>
#include <vector>
#include <string>
#include <atomic>

#include "../Include/consts.h"

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}


bool start = false;
bool quit = false;

std::vector<int> readys(MAX_THREAD_NUM);
std::vector<int> result(MAX_THREAD_NUM);
std::vector<uint64_t> newTimes(MAX_THREAD_NUM);
std::vector<uint64_t> delTimes(MAX_THREAD_NUM);

void ecall_refresh() {
    start = false;
    quit = false;
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        readys[i] = 0;
        result[i] = 0;
        newTimes[i] = 0;
        delTimes[i] = 0;
    }
}

void ecall_waitForReady(int th_num) {
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

void ecall_sendStart() {
    __atomic_store_n(&start, true, __ATOMIC_RELEASE);
}

void ecall_sendQuit() {
    __atomic_store_n(&quit, true, __ATOMIC_RELEASE);
}

void ecall_worker(int thid) {
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

int ecall_getResult(int thid) {
    return result[thid];
}

// 0: get new time
// 1: get del time
uint64_t ecall_getTime(int type) {
    uint64_t ret_value = 0;
    if (type == 0) {
        // (MAX_THREAD_NUM)回やってるけど範囲外のやつは0だからOK
        for (int i = 0; i < MAX_THREAD_NUM; i++) {
            ret_value += newTimes[i];
        }
    } else {
        for (int i = 0; i < MAX_THREAD_NUM; i++) {
            ret_value += delTimes[i];
        }
    }
    return ret_value;
}