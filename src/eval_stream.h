/**
 * Copyright (C) by J.Z. (04/09/2018 16:46)
 * Distributed under terms of the MIT license.
 */

#ifndef __EVAL_STREAM_H__
#define __EVAL_STREAM_H__

#include "stdafx.h"

class EvalStream {
private:
    int L_, cur_ = 0;
    std::vector<std::vector<int>> buf_;

public:
    EvalStream(const int L) : L_(L) { buf_.resize(L); }

    void add(const int v, const int l) {
        buf_[(cur_ + l - 1) % L_].push_back(v);
    }

    void next() {
        buf_[cur_].clear();
        cur_ = (cur_ + 1) % L_;
    }

    std::unordered_set<int> getSet() const {
        std::unordered_set<int> U;
        for (auto& vec : buf_) U.insert(vec.begin(), vec.end());
        return U;
    }

    std::vector<int> getVec() const {
        std::vector<int> U;
        for (auto& vec : buf_) U.insert(U.end(), vec.begin(), vec.end());
        return U;
    }

}; /* EvalStream */

#endif /* __EVAL_STREAM_H__ */
