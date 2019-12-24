/**
 * Copyright (C) by J.Z. (04/09/2018 16:46)
 * Distributed under terms of the MIT license.
 */

#ifndef __EVAL_STREAM_H__
#define __EVAL_STREAM_H__

#include "stdafx.h"
#include "bernoulli_segment.h"

class EvalStream {
private:
    int L_, cur_ = 0;
    std::vector<std::vector<std::pair<int, BernoulliSet>>> buf_;

public:
    EvalStream(const int L) : L_(L) { buf_.resize(L); }

    void add(const int e, const BernoulliSegments& segs) {
        for (auto& seg : segs.segments_)
            for (int i = seg.start_; i < seg.end_; ++i)
                buf_[(cur_ + i) % L_].emplace_back(e, seg.bs_);
    }

    void next() {
        buf_[cur_].clear();
        cur_ = (cur_ + 1) % L_;
    }

    /**
     * Get current Bernoulli sets
     */
    std::unordered_map<int, BernoulliSet> getPop() const {
        std::unordered_map<int, BernoulliSet> pop;
        for (auto& pr : buf_[cur_]) pop[pr.first] = std::move(pr.second);
        return pop;
    }

}; /* EvalStream */

#endif /* __EVAL_STREAM_H__ */
