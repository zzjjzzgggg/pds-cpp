/**
 * Copyright (C) by J.Z. 2019-04-30 11:32
 * Distributed under terms of the MIT license.
 */

#ifndef __BIASED_RESERVOIR_SAMPLER_H__
#define __BIASED_RESERVOIR_SAMPLER_H__

#include "stdafx.h"

class BiasedReservoir {
private:
    int capacity_;
    double pin_;
    std::vector<int> reservoir_;

public:
    BiasedReservoir(const int capacity) : capacity_(capacity), pin_(1) {
        reservoir_.reserve(capacity);
    }

    // process an incoming item e
    void process(const int e) {
        rngutils::default_rng rng;
        if (rng.uniform() > pin_) return;
        int num = reservoir_.size();
        if (rng.uniform() <= num / (double)capacity_)
            reservoir_[rng.randint(0, num - 1)] = e;
        else
            reservoir_.push_back(e);
    }

    void echo() {
        std::sort(reservoir_.begin(), reservoir_.end());
        ioutils::printVec(reservoir_);
    }

}; /* BiasedReservoir */

#endif /* __BIASED_RESERVOIR_SAMPLER_H__ */
