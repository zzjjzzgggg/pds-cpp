/**
 * Copyright (C) by J.Z. 2018-08-25 08:36
 * Distributed under terms of the MIT license.
 */

#include "sieve_streaming.h"

void SieveStreaming::addTheta(const int i) {
    int pos;
    if (!recycle_bin_.empty()) {  // if have available room
        pos = recycle_bin_.top();
        recycle_bin_.pop();
        S_buf_[pos].clear();  // make sure it is clean
    } else {                  // otherwise realloc room
        pos = S_buf_.size();
        S_buf_.push_back(std::unordered_set<int>());
    }
    thi_pos_[i] = pos;
}

void SieveStreaming::delTheta(const int i) {
    int pos = thi_pos_[i];
    S_buf_[pos].clear();
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

// TODO add cache
double SieveStreaming::getGain(const int v, const std::unordered_set<int>& S) {
    ++calls_;
    return obj_ptr_->getGain(v, S);
}

void SieveStreaming::feed(const int v) {
    // update maximum gain and thresholds
    double val = obj_ptr_->getVal(v);
    if (val > gain_mx_) {
        gain_mx_ = val;
        updateThresholds();
    }
    // sieve
    for (auto& pr : thi_pos_) {
        int i = pr.first;
        auto& S = getS(i);
        if (S.find(v) == S.end() && S.size() < budget_) {
            double threshold = getThreshold(i), gain = getGain(v, S);
            if (gain >= threshold) S.insert(v);
        }
    }
}

std::pair<int, double> SieveStreaming::getResult() const {
    int i_mx = -100;
    double rwd_mx = 0;
    for (auto& pr : thi_pos_) {
        int i = pr.first;
        double rwd = obj_ptr_->getVal(getS(i));
        if (rwd > rwd_mx) {
            rwd_mx = rwd;
            i_mx = i;
        }
    }
    return std::make_pair(i_mx, rwd_mx);
}

void SieveStreaming::reset() {
    calls_ = 0;
    gain_mx_ = 0;
    thi_pos_.clear();
    while (!recycle_bin_.empty()) recycle_bin_.pop();
    for (auto& S : S_buf_) S.clear();
}
