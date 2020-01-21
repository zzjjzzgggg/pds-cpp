/**
 * Copyright (C) by J.Z. 2019-03-27 14:16
 * Distributed under terms of the MIT license.
 */

#ifndef __LIFESPAN_GEN_H__
#define __LIFESPAN_GEN_H__

#include "stdafx.h"

/**
 * Lifespan generator.
 */
class LifespanGenerator {
private:
    const int L_;     // lifespan upper bound
    const double q_;  // decaying rate: smaller means decaying slower q = 1 - p

    mutable rngutils::default_rng rng_;

public:
    LifespanGenerator(const int L, const double q) : L_(L), q_(q) {}

    /**
     * Return a sampled lifespan.
     * Geo(i;q) = q(1-q)^i, i = 0,1,2,...
     */
    int get() const { return std::min(rng_.geometric(q_) + 1, L_); }

    /**
     * Return a vector of lifespans.
     */
    std::vector<int> getLifespans(const int n) const {
        std::vector<int> vec;
        vec.reserve(n);
        for (int i = 0; i < n; ++i) vec.push_back(get());
        return vec;
    }

    void getLifespans(const int n, std::vector<int>& vec) const {
        vec.reserve(n);
        for (int i = 0; i < n; ++i) vec.push_back(get());
    }
};

#endif /* __LIFESPAN_GEN_H__ */
