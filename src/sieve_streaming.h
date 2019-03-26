/**
 * Copyright (C) by J.Z. 2018-08-24 19:47
 * Distributed under terms of the MIT license.
 */

#ifndef __SIEVE_STREAMING_H__
#define __SIEVE_STREAMING_H__

#include "obj_fun.h"

class SieveStreaming {
private:
    int budget_, calls_ = 0;
    double eps_, gain_mx_ = 0;

    const ObjFun* obj_ptr_;

    std::vector<std::unordered_set<int>> S_buf_;
    std::map<int, int> thi_pos_;  // theta_index --> set_position
    std::stack<int> recycle_bin_;

private:
    // theta = (1+\epsilon)^i / (2k)
    inline double getThreshold(const int i) const {
        return std::pow(1 + eps_, i) / (2 * budget_);
    }

    // Given a threshold index i, return set S.
    inline std::unordered_set<int>& getS(const int i) {
        return S_buf_[thi_pos_.at(i)];
    }

    inline const std::unordered_set<int>& getS(const int i) const {
        return S_buf_[thi_pos_.at(i)];
    }

    // Add a new threshold with index i.
    void addTheta(const int i);
    void delTheta(const int i);

    void updateThresholds();

    double getGain(const int v, const std::unordered_set<int>& S);

public:
    SieveStreaming(const int budget, const double eps, const ObjFun* obj_ptr)
        : budget_(budget), eps_(eps), obj_ptr_(obj_ptr) {
        // |\Theta| = O(\epsilon^{-1}\log 2k)
        S_buf_.reserve((int)(std::log2(2 * budget_) / eps_));
    }

    /**
     * Copy constructor. NOTE: calls_ is not overwrite.
     */
    SieveStreaming(const SieveStreaming& o)
        : budget_(o.budget_), eps_(o.eps_), gain_mx_(o.gain_mx_),
          obj_ptr_(o.obj_ptr_), S_buf_(o.S_buf_), thi_pos_(o.thi_pos_),
          recycle_bin_(o.recycle_bin_) {}

    /**
     * Copy assignment.
     */
    SieveStreaming& operator=(const SieveStreaming& o) {
        budget_ = o.budget_;
        eps_ = o.eps_;
        gain_mx_ = o.gain_mx_;
        obj_ptr_ = o.obj_ptr_;
        S_buf_ = o.S_buf_;
        thi_pos_ = o.thi_pos_;
        recycle_bin_ = o.recycle_bin_;
        return *this;
    }

    void feed(const int v);
    void feedVec(const std::vector<int>& vec) {
        for (int v : vec) feed(v);
    }

    /**
     * If deep = true, then clean everything, including obj_ptr_ and maintaned
     * graph; otherwise, only clean temporal results maintaned in obj_ptr_.
     */
    void reset();

    /**
     * Get current maximum reward
     */
    std::pair<int, double> getResult() const;

    std::vector<int> getSolution(const int i) const {
        const auto& S = getS(i);
        return std::vector<int>(S.begin(), S.end());
    }

    int getNumThresholds() const { return thi_pos_.size(); }
    int getOCalls() const { return calls_; }

}; /* SieveStreaming */

#endif /* __SIEVE_STREAMING_H__ */
