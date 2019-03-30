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
        candidate_buf_.emplace_back();
    }
    thi_pos_[i] = pos;
    candidate_buf_[pos].init(num_samples_);  // must be initialized before using
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

double MCSieveStreaming::getGain(const int e, const BernoulliSet& bs,
                                 const Candidate& ca) const {
    ++cost_;
    double gain = 0;
    for (int trial : bs) gain += obj_ptr_->getGain(e, ca.S_vec_[trial]);
    return gain / num_samples_;
}

double MCSieveStreaming::getVal(const Candidate& ca) const {
    double val = 0;
    for (auto& vec : ca.S_vec_) val += obj_ptr_->getVal(vec);
    return val / num_samples_;
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
            double threshold = getThreshold(i), gain = getGain(e, bs, ca);
            if (gain >= threshold) ca.insert(e, bs);
        }
    }
}

std::pair<int, double> MCSieveStreaming::getResult() const {
    int i_mx = -1;
    double rwd_mx = -1;
    for (auto& pr : thi_pos_) {
        int i = pr.first;
        double rwd = getVal(getCandidate(i));
        if (rwd > rwd_mx) {
            rwd_mx = rwd;
            i_mx = i;
        }
    }
    return std::make_pair(i_mx, rwd_mx);
}
