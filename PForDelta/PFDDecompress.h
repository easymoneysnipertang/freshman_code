#pragma once
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

class PForDecompressor {
public:
    PForDecompressor();
    void Decompress(unsigned a, unsigned b, unsigned item_num, unsigned* data, unsigned data_length, unsigned* res);

private:
    typedef void(PForDecompressor::* step1Funs)();
    typedef void(PForDecompressor::* step2Funs)(unsigned);
    step1Funs step1_[17];
    step2Funs step2_[5];

    unsigned item_num_;
    unsigned data_length_;
    unsigned* data_;
    unsigned* res_;

    void step3();
    void step1B1();
    void step1B2();
    void step1B3();
    void step1B4();
    void step1B5();
    void step1B6();
    void step1B8();
    void step1B10();
    void step1B16();
    void step1Ex();

    void step2A1(unsigned b);
    void step2A2(unsigned b);
    void step2A3(unsigned b);
    void step2A4(unsigned b);
};
PForDecompressor::PForDecompressor() {
    step1_[0] = &PForDecompressor::step1Ex;
    step1_[3] = &PForDecompressor::step1B3;
    step1_[11] = &PForDecompressor::step1Ex;
    step1_[12] = &PForDecompressor::step1Ex;
    step1_[13] = &PForDecompressor::step1Ex;
    step1_[14] = &PForDecompressor::step1Ex;
    step1_[15] = &PForDecompressor::step1Ex;
    step1_[16] = &PForDecompressor::step1B16;
    step2_[1] = &PForDecompressor::step2A1;
    step2_[2] = &PForDecompressor::step2A2;

    /*step1_[1] = &PForDecompressor::step1B1;
    step1_[2] = &PForDecompressor::step1B2;
    step1_[3] = &PForDecompressor::step1B3;
    step1_[4] = &PForDecompressor::step1B4;
    step1_[5] = &PForDecompressor::step1B5;
    step1_[6] = &PForDecompressor::step1B6;
    step1_[7] = &PForDecompressor::step1Ex;
    step1_[8] = &PForDecompressor::step1B8;
    step1_[9] = &PForDecompressor::step1Ex;
    step1_[10] = &PForDecompressor::step1B10;
    step1_[11] = &PForDecompressor::step1Ex;
    step1_[12] = &PForDecompressor::step1Ex;
    step1_[13] = &PForDecompressor::step1Ex;
    step1_[14] = &PForDecompressor::step1Ex;
    step1_[15] = &PForDecompressor::step1Ex;
    step1_[16] = &PForDecompressor::step1B16;
    step2_[1] = &PForDecompressor::step2A1;
    step2_[2] = &PForDecompressor::step2A2;
    step2_[3] = &PForDecompressor::step2A3;
    step2_[4] = &PForDecompressor::step2A4;*/
}

void PForDecompressor::Decompress(unsigned a, unsigned b, unsigned item_num, unsigned* data, unsigned data_length, unsigned* res) {
    item_num_ = item_num;
    data_length_ = data_length;
    data_ = data;
    res_ = res;
    (this->*step1_[b])();
    (this->*step2_[a])(b);
}

void PForDecompressor::step1B3()
{
    unsigned l = (item_num_ + 9) / 10;
    unsigned i, block;
    unsigned* con = res_;
    for (i = 0; i < l; ++i)
    {
        block = *(data_++);
        data_length_--;
        con[0] = block & 7;
        con[1] = (block >> 3) & 7;
        con[2] = (block >> 6) & 7;
        con[3] = (block >> 9) & 7;
        con[4] = (block >> 12) & 7;
        con[5] = (block >> 15) & 7;
        con[6] = (block >> 18) & 7;
        con[7] = (block >> 21) & 7;
        con[8] = (block >> 24) & 7;
        con[9] = (block >> 27) & 7;
        con += 10;
    }
}

void PForDecompressor::step2A1(unsigned b)
{
    unsigned block;
    while (data_length_ > 0)
    {
        block = *(data_++);
        data_length_--;
        res_[block & 255] += ((block >> 8) & 255) << b;
        res_[(block >> 16) & 255] += ((block >> 24)) << b;
    }
}

void PForDecompressor::step2A2(unsigned b)
{
    unsigned block1, block2;
    while (data_length_ > 0)
    {
        block1 = *(data_++);
        data_length_--;
        res_[block1 & 255] += ((block1 >> 8) & 65535) << b;
        if (data_length_ == 0) break;
        block2 = *(data_++);
        data_length_--;
        res_[block1 >> 24] += (block2 & 65535) << b;
        if (data_length_ == 0) break;
        block1 = *(data_++);
        data_length_--;
        res_[(block2 >> 16) & 255] += ((block2 >> 24) + ((block1 & 255) << 8)) << b;
        res_[(block1 >> 8) & 255] += (block1 >> 16) << b;
    }
}


void PForDecompressor::step1B16()
{
    unsigned l = (item_num_ + 1) / 2;
    unsigned i, block;
    unsigned* con = res_;
    for (i = 0; i < l; i++)
    {
        block = *(data_++);
        data_length_--;
        con[0] = block & ((1 << 16) - 1);
        con[1] = block >> 16;
        con += 2;
     }
}
void PForDecompressor::step1Ex()
{
    cout << "Invalid b value" << endl;
}