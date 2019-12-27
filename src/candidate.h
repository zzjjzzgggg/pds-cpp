/**
 * Copyright (C) by J.Z. 2019-03-28 10:47
 * Distributed under terms of the MIT license.
 */

#ifndef __CANDIDATE_H__
#define __CANDIDATE_H__

#include "stdafx.h"
#include "obj_fun.h"

/**
 * The candidate items corresponding to a threshold.
 */
class Candidate {
public:
    int num_samples_;
    std::unordered_set<int> S_;            // S_theta
    std::vector<std::vector<int>> S_vec_;  // { S_theta^i: i=1,...,n }

public:
    Candidate() {}
    Candidate(const int num_samples) : num_samples_(num_samples) {
        S_vec_.resize(num_samples_);
    }

    void init(const int num_samples) {
        num_samples_ = num_samples;
        S_vec_.resize(num_samples_);
    }

    inline bool isMember(const int e) const { return S_.find(e) != S_.end(); }

    inline int size() const { return S_.size(); }

    void clear() {
        S_.clear();
        S_vec_.clear();
        S_vec_.resize(num_samples_);
    }

    void insert(const int e, const BernoulliSet& bs) {
        S_.insert(e);
        for (int i : bs) S_vec_[i].push_back(e);
    }

    std::vector<int> getMembers() const {
        return std::vector<int>(S_.begin(), S_.end());
    }

    void info() const {
        printf("S: ");
        ioutils::printSet(S_);
        int i = 0;
        for (auto& vec : S_vec_) {
            printf("S[%d]: ", i++);
            ioutils::printVec(vec);
        }
    }

    double value(const ObjFun* obj_ptr) const {
        double val = 0;
        for (auto& vec : S_vec_) val += obj_ptr->getVal(vec);
        return val / num_samples_;
    }

    double gain(const int e, const BernoulliSet& bs,
                const ObjFun* obj_ptr) const {
        double g = 0;
        for (int trial : bs) g += obj_ptr->getGain(e, S_vec_[trial]);
        return g / num_samples_;
    }

}; /* Candidate */

#endif /* __CANDIDATE_H__ */
