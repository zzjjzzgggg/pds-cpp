/**
 * Copyright (C) by J.Z. 2019-03-28 10:47
 * Distributed under terms of the MIT license.
 */

#ifndef __CANDIDATE_H__
#define __CANDIDATE_H__

#include "stdafx.h"

/**
 * The candidate items corresponding to a threshold.
 */
class Candidate {
public:
    std::unordered_set<int> S_;            // S_\theta
    std::vector<std::vector<int>> S_vec_;  // { S_\theta^i: i=1,...,n }

public:
    Candidate(const int num_samples) { S_vec_.reserve(num_samples); }

    inline bool isMember(const int e) const { return S_.find(e) != S_.end(); }

    inline int size() const { return S_.size(); }

    void clear() {
        S_.clear();
        S_vec_.clear();
    }

    void insert(const int e, const BernoulliSet& bs) {
        S_.insert(e);
        for (int i : bs) S_vec_[i].push_back(e);
    }

    std::vector<int> getMembers() const {
        return std::vector<int>(S_.begin(), S_.end());
    }

}; /* Candidate */

#endif /* __CANDIDATE_H__ */
