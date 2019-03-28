/**
 * Copyright (C) by J.Z. 2019-03-28 15:01
 * Distributed under terms of the MIT license.
 */

#ifndef __BERNOULLI_SET_GENERATOR_H__
#define __BERNOULLI_SET_GENERATOR_H__

#include "stdafx.h"

class BernoulliSetGenerator {
private:
    const int n_;
    const double p_;
    rngutils::default_rng rng_;

public:
    BernoulliSetGenerator(const int n, const double p) : n_(n), p_(p) {}

    BernoulliSet getBernoulliSet() {
        BernoulliSet bs;
        for (int i = 0; i < n_; i++) {
            if (rng_.uniform() <= p_) bs.push_back(i);
        }
        return bs;
    }

}; /* BernoulliSetGenerator */

#endif /* __BERNOULLI_SET_GENERATOR_H__ */
