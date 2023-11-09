#pragma once
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

#define COMPRESS_BLOCK_SIZE 64
#define PFOR_THRESHOLD 0.9

struct PForBlock {
    int aLen, bLen;
    unsigned a, b;
    vector<unsigned> data;
};

class PForCompressor {
public:
    PForCompressor() {
        bsets_[0] = 1; bsets_[1] = 2; bsets_[2] = 3; bsets_[3] = 4; bsets_[4] = 5; bsets_[5] = 6;
        bsets_[6] = 8; bsets_[7] = 10; bsets_[8] = 16;
    }
    PForBlock compress(const vector<unsigned>& v);

protected:
    void getAB(const vector<unsigned>& v);
    unsigned bsets_[9];
    unsigned a_, b_;
};

void PForCompressor::getAB(const vector<unsigned>& v) {
    vector<unsigned> u = v;
    if (u.size() <= 0)return;
    sort(u.begin(), u.end());
    unsigned threshold = u[((unsigned)(double)u.size() * PFOR_THRESHOLD - 1)];
    unsigned max_num = u[u.size() - 1];
    // Get b
    unsigned bn = 0;
    for (; bn < 8; ++bn) {
        if ((threshold >> bsets_[bn]) <= 0)
            break;
    }
    b_ = bsets_[bn];
    // Get a
    max_num >>= b_;
    a_ = 1;
    for (; a_ < 4; ++a_) {
        if ((1 << (a_ * 8)) > max_num)
            break;
    }
}

PForBlock PForCompressor::compress(const vector<unsigned>& v) {
    getAB(v);
    unsigned threshold = 1 << b_;
    vector<unsigned> tail;
    vector<unsigned>::iterator it;
    PForBlock block;
    block.a = a_;
    block.b = b_;

    // 90% fit numbers and the low b-bit of exceptions are stored at the head
    unsigned head_size = (v.size() + 32 / b_ + 1) / (32 / b_);
    block.bLen = head_size;

    for (unsigned i = 0; i < head_size; ++i)
        block.data.push_back(0);
    for (unsigned i = 0; i < v.size(); ++i) {
        unsigned low = v[i] & (threshold - 1);
        block.data[i / (32 / b_)] |= low << i % (32 / b_) * b_;
        if (v[i] >= threshold) {
            tail.push_back(i);
            unsigned high = v[i] >> b_;
            for (unsigned l = 0; l < a_; ++l) {
                tail.push_back((high >> (l * 8)) & 255);
            }
        }
    }

    // high-bit of exceptions are stored at the end using a-bytes each.
    unsigned temp = 0;
    unsigned i;
    for (i = 0; i < tail.size(); ++i) {
        temp |= tail[i] << (i * 8 % 32);
        if (i % 4 == 3) {
            block.data.push_back(temp);
            temp = 0;
        }
    }
    block.aLen = tail.size();
    if (i % 4 != 0) {
        block.data.push_back(temp);
        block.aLen++;
    }

    return block;
}