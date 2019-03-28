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
    const double q_;  // q = 1-p where p has the same meaning as the paper

    rngutils::default_rng rng_;

public:
    /**
     * Parameter p has the same meaning as the paper.
     */
    LifespanGenerator(const int L, const double p) : L_(L), q_(1 - p) {}

    int get() { return std::min(rng_.geometric(q_) + 1, L_); }

    std::vector<int> getLifespanVec(const int n) {
        std::vector<int> vec;
        vec.reserve(n);
        for (int i = 0; i < n; ++i) vec.push_back(get());
        return vec;
    }

    std::vector<std::pair<int, int>> getTrialLifespanPairs(const int n) {
        std::vector<std::pair<int, int>> vec;
        vec.reserve(n);
        for (int i = 0; i < n; ++i) vec.emplace_back(i, get());
        return vec;
    }
};

#endif /* __LIFESPAN_GEN_H__ */
