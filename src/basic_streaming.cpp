/**
 * Copyright (C) by J.Z. 2018-08-25 09:55
 * Distributed under terms of the MIT license.
 */

#include "basic_streaming.h"

void BasicStreaming::feed(const int v, const int l) {
    int i = cur_;
    for (int j = 0; j < l; j++) {
        sieve_ptrs_[i]->feed(v);
        i = (i + 1) % L_;
    }
}

void BasicStreaming::next() {
    sieve_ptrs_[cur_]->reset();
    cur_ = (cur_ + 1) % L_;
}

int BasicStreaming::statOCalls() const {
    int n_calls = 0;
    for (int i = 0; i < L_; ++i) n_calls += sieve_ptrs_[i]->getOCalls();
    return n_calls;
}
