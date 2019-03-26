/**
 * Copyright (C) by J.Z. 2018-08-25 09:45
 * Distributed under terms of the MIT license.
 */

#ifndef __BASIC_STREAMING_H__
#define __BASIC_STREAMING_H__

#include "sieve_streaming.h"

class BasicStreaming {
private:
    int L_, cur_ = 0;  // cur_ points to the instance with l = 1

    std::vector<SieveStreaming*> sieve_ptrs_;

public:
    BasicStreaming(const int L, const int budget, const double eps,
                   const ObjFun* obj_ptr)
        : L_(L) {
        sieve_ptrs_.resize(L);
        for (int l = 0; l < L_; ++l)
            sieve_ptrs_[l] = new SieveStreaming(budget, eps, obj_ptr);
    }

    virtual ~BasicStreaming() {
        for (int l = 0; l < L_; l++) delete sieve_ptrs_[l];
    }

    /**
     * Data with lifespan l will be fed to instances with i <= l.
     * Alg. instances in range [cur_, cur_ + l - 1] will be updated.
     */
    void feed(const int v, const int l);

    std::pair<int, double> getResult() const {
        return sieve_ptrs_[cur_]->getResult();
    }

    std::vector<int> getSolution(const int i) const {
        return sieve_ptrs_[cur_]->getSolution(i);
    }

    /**
     * Clear and prepare for next time step.
     */
    void next();

    int statOCalls() const;

}; /* BasicStreaming */

#endif /* __BASIC_STREAMING_H__ */
