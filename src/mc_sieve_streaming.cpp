/**
 * Copyright (C) by J.Z. 2018-08-25 08:36
 * Distributed under terms of the MIT license.
 */

#include "mc_sieve_streaming.h"

void MCSieveStreaming::addTheta(const int i) {
    int pos;
    if (!recycle_.empty()) {  // if recycle_bin has an unoccupied room
        pos = recycle_.back();
        recycle_.pop_back();
    } else {  // otherwise realloc room
        pos = candidate_buf_.size();
        candidate_buf_.emplace_back(num_samples_);
    }
    thi_pos_[i] = pos;
}

void MCSieveStreaming::delTheta(const int i) {
    int pos = thi_pos_[i];
    candidate_buf_[pos].clear();
    recycle_.push_back(pos);
    thi_pos_.erase(i);
}

void MCSieveStreaming::updateThresholds() {
    int new_li =
            std::floor(std::log((1 - eps_) * mx_gain_) / std::log(1 + eps_)),
        new_ui =
            std::ceil(std::log(2 * budget_ * mx_gain_) / std::log(1 + eps_));
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

void MCSieveStreaming::feed(const int e, const BernoulliSet& bs) {
    // update maximum gain and thresholds
    double val = obj_ptr_->getVal(e) * bs.size() / num_samples_;
    if (val > mx_gain_) {
        mx_gain_ = val;
        updateThresholds();
    }
    // sieve-streaming
    for (auto& pr : thi_pos_) {
        int i = pr.first;
        auto& ca = getCandidate(i);
        if (!ca.isMember(e) && ca.size() < budget_) {
            double threshold = getThreshold(i), gain = ca.gain(e, bs, obj_ptr_);
            if (gain >= threshold) ca.insert(e, bs);
            ++cost_;
        }
    }
}

std::pair<int, double> MCSieveStreaming::getResult() const {
    int i_mx = -1;
    double rwd_mx = 0;
    for (auto& pr : thi_pos_) {
        int i = pr.first;
        double rwd = getCandidate(i).value(obj_ptr_);
        if (rwd > rwd_mx) {
            rwd_mx = rwd;
            i_mx = i;
        }
    }
    return std::make_pair(i_mx, rwd_mx);
}
