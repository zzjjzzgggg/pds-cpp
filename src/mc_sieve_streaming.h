/**
 * Copyright (C) by J.Z. 2018-08-24 19:47
 * Distributed under terms of the MIT license.
 */

#ifndef __MCSIEVE_STREAMING_H__
#define __MCSIEVE_STREAMING_H__

#include "obj_fun.h"
#include "candidate.h"

class MCSieveStreaming {
private:
    int num_samples_, budget_;
    mutable int cost_ = 0;

    double eps_, mx_gain_;

    const ObjFun* obj_ptr_;

    std::vector<Candidate> candidate_buf_;
    std::map<int, int> thi_pos_;  // theta_index --> buf_position
    std::vector<int> recycle_;    // a vector of positions for recycling

private:
    // theta = (1+\epsilon)^i / (2k)
    inline double getThreshold(const int i) const {
        return std::pow(1 + eps_, i) / (2 * budget_);
    }

    // Given a threshold index i, return candidate c.
    inline const Candidate& getCandidate(const int i) const {
        return candidate_buf_[thi_pos_.at(i)];
    }

    inline Candidate& getCandidate(const int i) {
        return candidate_buf_[thi_pos_.at(i)];
    }

    // Add a new threshold indexed by i.
    void addTheta(const int i);
    void delTheta(const int i);
    void updateThresholds();

public:
    MCSieveStreaming(const int num_samples, const int budget, const double eps,
                     const ObjFun* obj_ptr)
        : num_samples_(num_samples), budget_(budget), cost_(0), eps_(eps),
          mx_gain_(0), obj_ptr_(obj_ptr) {
        // |\Theta| = O(\epsilon^{-1}\log 2k)
        candidate_buf_.reserve((int)(std::log2(2 * budget_) / eps_));
    }

    /**
     * Copy constructor.
     * NOTE: cost_ is not overwrite.
     */
    MCSieveStreaming(const MCSieveStreaming& o)
        : num_samples_(o.num_samples_), budget_(o.budget_), cost_(0),
          eps_(o.eps_), mx_gain_(o.mx_gain_), obj_ptr_(o.obj_ptr_),
          candidate_buf_(o.candidate_buf_), thi_pos_(o.thi_pos_),
          recycle_(o.recycle_) {}

    /**
     * Copy assignment.
     * NOTE: cost_ is not overwrite.
     */
    MCSieveStreaming& operator=(const MCSieveStreaming& o) {
        num_samples_ = o.num_samples_;
        budget_ = o.budget_;
        cost_ = 0;
        eps_ = o.eps_;
        mx_gain_ = o.mx_gain_;
        obj_ptr_ = o.obj_ptr_;
        candidate_buf_ = o.candidate_buf_;
        thi_pos_ = o.thi_pos_;
        recycle_ = o.recycle_;
        return *this;
    }

    void clear() {
        cost_ = 0;
        mx_gain_ = 0;
        thi_pos_.clear();
        recycle_.clear();
        for (auto& ca : candidate_buf_) ca.clear();
    }

    // Process element e and its Bernoulli set.
    void feed(const int e, const BernoulliSet& bs);

    // Get current maximum reward.
    std::pair<int, double> getResult() const;

    // Get the number of thresholds maintained in MC-MCSieveStreaming.
    int size() const { return thi_pos_.size(); }

    // Get the number of oracle calls.
    int getCost() const { return cost_; }

}; /* MCSieveStreaming */

#endif /* __MCSIEVE_STREAMING_H__ */
