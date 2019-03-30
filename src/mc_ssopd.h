/**
 * Copyright (C) by J.Z. 2018-08-25 09:45
 * Distributed under terms of the MIT license.
 */

#ifndef __MC_SSOPD_H__
#define __MC_SSOPD_H__

#include "bernoulli_segment.h"
#include "mc_sieve_streaming.h"

class MCSSOPD {
private:
    int L_, cur_ = 0;  // cur_ points to the instance with l = 0

    std::vector<MCSieveStreaming*> sieve_ptrs_;

public:
    MCSSOPD(const int L, const int num_samples, const int budget,
            const double eps, const ObjFun* obj_ptr)
        : L_(L) {
        sieve_ptrs_.resize(L);
        for (int l = 0; l < L_; ++l)
            sieve_ptrs_[l] =
                new MCSieveStreaming(num_samples, budget, eps, obj_ptr);
    }

    virtual ~MCSSOPD() {
        for (int l = 0; l < L_; ++l) delete sieve_ptrs_[l];
    }

    void feed(const int e, const BernoulliSegments& segs) {
        for (auto& seg : segs.segments_)
            for (int i = seg.start_; i < seg.end_; ++i)
                sieve_ptrs_[(i + cur_) % L_]->feed(e, seg.bs_);
    }

    std::pair<int, double> getResult() const {
        return sieve_ptrs_[cur_]->getResult();
    }

    /**
     * Clear states and prepare for next round processing.
     */
    void next() {
        sieve_ptrs_[cur_]->clear();
        cur_ = (cur_ + 1) % L_;
    }

    int getCost() const {
        int cost = 0;
        for (int i = 0; i < L_; ++i) cost += sieve_ptrs_[i]->getCost();
        return cost;
    }

}; /* MCSSOPD */

#endif /* __MC_SSOPD_H__ */
