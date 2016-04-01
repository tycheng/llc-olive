#ifndef REGISTERALLOCATOR_H
#define REGISTERALLOCATOR_H

#include <iostream>
#include <map>
#include <queue>

#include "LiveRange.h"

enum Register {
    RAX, RBX, RCX, RDX,
    RSI, RDI, RBP, RSP,
    R8, R9, R10, R11,
    R12, R13, R14, R15
};

// System V X86_64 's calling convention
static Register scratch_regs [] = { RAX, RDI, RSI, RDX, RCX, R8, R9, R10, R11 };
static Register preseve_regs [] = { RBX, RSP, RBP, R12, R13, R14, R15 };
static Register params_regs  [] = { RDI, RSI, RDX, RCX, R8, R9 };

static const char* registers[] = {
    "rax", "rbx", "rcx", "rdx",
    "rsi", "rdi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", 
    "r12", "r13", "r14", "r15", 
};
static int NUM_REGS = 16;

// use naive linear search to insert elem 
static void insert_active_set (std::vector<LiveRange*>& vec, LiveRange* elem) {
    vec.push_back(elem);
    for (int i = vec.size()-1; i >= 0 ; i --) {
        if (vec[i]->endpoint <= elem->endpoint) break;
        else {
            LiveRange* tmp = vec[i];
            vec[i] = vec[i+1];
            vec[i+1] = tmp;
        }
    }
}

// use naive linear search to insert elem 
static void insert_intervals (std::vector<LiveRange*>& vec, LiveRange* elem) {
    vec.push_back(elem);
    for (int i = vec.size()-1; i >= 0 ; i --) {
        if (vec[i]->startpoint <= elem->startpoint) break;
        else {
            LiveRange* tmp = vec[i];
            vec[i] = vec[i+1];
            vec[i+1] = tmp;
        }
    }
}

/**
 * Register Allocator
 * */
class RegisterAllocator
{
public:
    RegisterAllocator(int num_regs) {
        // initialize register map
        for (int i = 0; i < NUM_REGS; i++) 
            register_map[i] = NULL;
    }
    void set_intervals (std::map<int, LiveRange*> liveness) {
         for (auto it : liveness) {
            all_intervals.push_back(it.second);
         }
         virtual2machine.resize(all_intervals.size());
    }

    virtual ~RegisterAllocator() { }

    std::string Allocate() {
        return std::string("rax");
    }
    void linearScanAllocate ();
    void expireOldIntervals (int i);
    void spillAtInterval (int i);

    void buildIntervals();
    void allocateFreeReg();
    void allocateBlockedReg();

    std::vector<int> get_virtual2machine() const {
        return virtual2machine;
    }

private:
    int num_regs;
    // restore active_set set of live interval in increasing order of endpoint
    std::vector<LiveRange*> active_set;
    // restore live all_intervals of all variables
    std::vector<LiveRange*> all_intervals;
    // restore the free registers
    std::map<int, LiveRange*> register_map;
    // gloally restore ssa form mapping
    std::vector<int> virtual2machine;
};

#endif /* end of include guard: REGISTERALLOCATOR_H */
