/**
 * Copyright (C) by J.Z. 2019-05-01 08:48
 * Distributed under terms of the MIT license.
 */

#ifndef __RESERVOIR_SAMPLER_H__
#define __RESERVOIR_SAMPLER_H__

#include "stdafx.h"

class ReservoirSampler {
private:
    int capacity_, t_;
    rngutils::default_rng rng_;

public:
    std::vector<int> reservoir_;

public:
    //! Default constructor
    ReservoirSampler(const int capacity) : capacity_(capacity), t_(0) {
        reservoir_.reserve(capacity);
    }

    // classical reservoir sampling: process the t-th elememnt e
    void process(const int e) {
        ++t_;
        if (reservoir_.size() < capacity_)
            reservoir_.push_back(e);
        else if (rng_.uniform() < (double)capacity_ / t_) {
            reservoir_[rng_.randint(0, capacity_ - 1)] = e;
        }
    }

}; /* ReservoirSampler */

#endif /* __RESERVOIR_SAMPLER_H__ */
