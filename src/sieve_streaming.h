/**
 * Copyright (C) by J.Z. 2018-08-24 19:47
 * Distributed under terms of the MIT license.
 */

#ifndef __SIEVE_STREAMING_H__
#define __SIEVE_STREAMING_H__

#include "obj_fun.h"
#include "candidate.h"

class SieveStreaming {
private:
    int num_samples_, budget_;
    double eps_, gain_mx_ = 0;

    const ObjFun* obj_ptr_;

    std::vector<Candidate> candidate_buf_;
    std::map<int, int> thi_pos_;  // theta_index --> buf_position
    std::stack<int> recycle_bin_;

private:
    // theta = (1+\epsilon)^i / (2k)
    inline double getThreshold(const int i) const {
        return std::pow(1 + eps_, i) / (2 * budget_);
    }

    // Given a threshold index i, return candidate c.
    inline const Candidate& getCandidate(const int i) const {
        return candidate_buf_.at(thi_pos_.at(i));
    }

    inline Candidate& getCandidate(const int i) {
        return candidate_buf_[thi_pos_.at(i)];
    }

    // Calculate the gain of item e w.r.t. the candidate indexed by i.
    double getGain(const int i, const int e, const BernoulliSet& bs) const;

    // Add a new threshold indexed by i.
    void addTheta(const int i);
    void delTheta(const int i);
    void updateThresholds();

public:
    SieveStreaming(const int num_samples, const int budget, const double eps,
                   const ObjFun* obj_ptr)
        : num_samples_(num_samples), budget_(budget), eps_(eps),
          obj_ptr_(obj_ptr) {
        // |\Theta| = O(\epsilon^{-1}\log 2k)
        candidate_buf_.reserve((int)(std::log2(2 * budget_) / eps_));
    }

    /**
     * Copy constructor. NOTE: calls_ is not overwrite.
     */
    SieveStreaming(const SieveStreaming& o)
        : budget_(o.budget_), eps_(o.eps_), gain_mx_(o.gain_mx_),
          obj_ptr_(o.obj_ptr_), candidate_buf_(o.candidate_buf_),
          thi_pos_(o.thi_pos_), recycle_bin_(o.recycle_bin_) {}

    /**
     * Copy assignment.
     */
    SieveStreaming& operator=(const SieveStreaming& o) {
        budget_ = o.budget_;
        eps_ = o.eps_;
        gain_mx_ = o.gain_mx_;
        obj_ptr_ = o.obj_ptr_;
        candidate_buf_ = o.candidate_buf_;
        thi_pos_ = o.thi_pos_;
        recycle_bin_ = o.recycle_bin_;
        return *this;
    }

    void feed(const int e, const BernoulliSet& bs);

    void clear();

    /**
     * Get current maximum reward
     */
    std::pair<int, double> getResult() const;
    int getNumThresholds() const { return thi_pos_.size(); }

}; /* SieveStreaming */

#endif /* __SIEVE_STREAMING_H__ */
