#pragma once 

/*  
 *  readysとかの変数の長さをいちいち変えるのが面倒だからこっちでハードコーディングしているだけ
 *  128threadよりでかければなんでもいい
 */
#define MAX_THREAD_NUM 1000
#define CLOCKS_PER_US 2700
#define EXTIME 10

struct Tidword {
    union {
        uint64_t obj_;
        struct {
            bool lock : 1;
            bool latest : 1;
            bool absent : 1;
            uint64_t tid : 29;
            uint64_t epoch : 32;
        };
    };
    Tidword() : epoch(0), tid(0), absent(false), latest(true), lock(false) {};
};

class Tuple {
public:
    Tidword tidword_;
    std::string key_;
    int value_;

    Tuple(std::string key, int value) : key_(key), value_(value) {}

    void init() {
        tidword_.epoch = 1;
        tidword_.latest = true;
        tidword_.absent = false;
        tidword_.lock = false;
    }
};

static uint64_t rdtscp() {
    uint64_t rax;
    uint64_t rdx;
    uint32_t aux;
    asm volatile("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(aux)::);
    // store EDX:EAX.
    // 全ての先行命令を待機してからカウンタ値を読み取る．ただし，後続命令は
    // 同読み取り操作を追い越す可能性がある．   
    return (rdx << 32) | rax;
}
