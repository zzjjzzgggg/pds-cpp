/**
 * Copyright (C) by J.Z. 2018-08-25 08:36
 * Distributed under terms of the MIT license.
 */

#include "sieve_streaming.h"

void SieveStreaming::addTheta(const int i) {
    int pos;
    if (!recycle_bin_.empty()) {  // if recycle_bin has an unoccupied room
        pos = recycle_bin_.top();
        recycle_bin_.pop();
        candidate_buf_[pos].clear();
    } else {  // otherwise realloc room
        pos = candidate_buf_.size();
        candidate_buf_.emplace_back(num_samples_);
    }
    thi_pos_[i] = pos;
}

void SieveStreaming::delTheta(const int i) {
    int pos = thi_pos_[i];
    candidate_buf_[pos].clear();
    recycle_bin_.push(pos);
    thi_pos_.erase(i);
}

void SieveStreaming::updateThresholds() {
    int new_li = (int)std::floor(std::log((1 - eps_) * gain_mx_) /
                                 std::log(1 + eps_)),
        new_ui = (int)std::ceil(std::log(2 * budget_ * gain_mx_) /
                                std::log(1 + eps_));
    int li, ui;               // lower bound and upper bound of theata index
    if (!thi_pos_.empty()) {  // delete outdated thresholds
        li = thi_pos_.begin()->first;
        ui = thi_pos_.rbegin()->first;
        while (li <= ui && li < new_li) {
            delTheta(li);
            li++;
        }
    }
    li = thi_pos_.empty() ? new_li : ui + 1;
    for (int i = li; i <= new_ui; i++) addTheta(i);
}

double SieveStreaming::getGain(const int i, const int e,
                               const BernoulliSet& bs) const {
    const Candidate& candidate = getCandidate(i);
    double gain = 0;
    for (int trial : bs) gain += obj_ptr_->getGain(e, candidate.S_vec_[trial]);
    return gain / num_samples_;
}

void SieveStreaming::feed(const int e, const BernoulliSet& bs) {
    // update maximum gain and thresholds
    double val = obj_ptr_->getVal(e) * bs.size() / num_samples_;
    if (val > gain_mx_) {
        gain_mx_ = val;
        updateThresholds();
    }
    // sieve-streaming
    for (auto& pr : thi_pos_) {
        int i = pr.first;
        auto& candidate = getCandidate(i);
        if (!candidate.isMember(e) && candidate.size() < budget_) {
            double threshold = getThreshold(i), gain = getGain(i, e, bs);
            if (gain >= threshold) candidate.insert(e, bs);
        }
    }
}

std::pair<int, double> SieveStreaming::getResult() const {
    int i_mx = -100;
    double rwd_mx = 0;
    for (auto& pr : thi_pos_) {
        int i = pr.first;
        auto&& items = getCandidate(i).getMembers();
        double rwd = obj_ptr_->getVal(items);
        if (rwd > rwd_mx) {
            rwd_mx = rwd;
            i_mx = i;
        }
    }
    return std::make_pair(i_mx, rwd_mx);
}

void SieveStreaming::clear() {
    gain_mx_ = 0;
    thi_pos_.clear();
    while (!recycle_bin_.empty()) recycle_bin_.pop();
    for (auto& S : candidate_buf_) S.clear();
}
