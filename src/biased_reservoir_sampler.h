/**
 * Copyright (C) by J.Z. 2019-04-30 11:32
 * Distributed under terms of the MIT license.
 */

#ifndef __BIASED_RESERVOIR_SAMPLER_H__
#define __BIASED_RESERVOIR_SAMPLER_H__

#include "stdafx.h"

class BiasedReservoir {
public:
    int capacity_;
    double lambda_, pin_;

    rngutils::default_rng rng_;
    std::vector<int> reservoir_;

public:
    BiasedReservoir(const int capacity, const double lambda)
        : capacity_(capacity), lambda_(lambda), pin_(1) {
        reservoir_.reserve(capacity);
    }

    // process an incoming item e
    void process(const int e) {
        if (rng_.uniform() > pin_) return;

        int num = reservoir_.size();
        if (rng_.uniform() < num * lambda_ / pin_) {  // num / capacity_mx
            reservoir_[rng_.randint(0, num - 1)] = e;
        } else {  // if reservoir is full, delete one sample, and reduce pin
            if (num == capacity_) {
                pin_ *= 1 - 1.0 / capacity_;
                reservoir_[rng_.randint(0, num - 1)] = reservoir_.back();
                reservoir_.pop_back();
            }
            reservoir_.push_back(e);
        }
    }

    void echo() {
        std::sort(reservoir_.begin(), reservoir_.end());
        ioutils::printVec(reservoir_);
    }

}; /* BiasedReservoir */

#endif /* __BIASED_RESERVOIR_SAMPLER_H__ */
