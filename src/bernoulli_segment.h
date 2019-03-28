/**
 * Copyright (C) by J.Z. 2019-03-27 17:08
 * Distributed under terms of the MIT license.
 */

#ifndef __BERNOULLI_SEGMENT_H__
#define __BERNOULLI_SEGMENT_H__

#include "stdafx.h"

class BernoulliSegment {
public:
    int start_, end_;
    BernoulliSet bs_;

public:
    BernoulliSegment(const int start, const int end, BernoulliSet &&bs)
        : start_(start), end_(end), bs_(std::move(bs)) {
        std::sort(bs_.begin(), bs_.end());
    }
}; /* BernoulliSegment */

class BernoulliSegments {
private:
    int e_;
    std::vector<BernoulliSegment> segments_;

public:
    BernoulliSegments(const int e, std::vector<std::pair<int, int>> &&samples)
        : e_(e) {
        std::sort(samples.begin(), samples.end(),
                  [](std::pair<int, int> &a, std::pair<int, int> &b) {
                      return a.second < b.second;
                  });
        // a vector of trials
        std::vector<int> rest_trials;
        rest_trials.reserve(samples.size());
        for (const auto &pr : samples) rest_trials.push_back(pr.first);
        samples.emplace_back(-1, -1);  // append a dumy element
        // scan the pair vector
        auto first = rest_trials.begin();
        int seg_end = -1, seg_start;
        int pre_i = 0, pre_l = samples[0].second, l;
        for (int i = 1; i < samples.size(); ++i) {
            l = samples[i].second;
            if (l != pre_l) {
                seg_start = seg_end + 1;
                seg_end = pre_l - 1;
                segments_.emplace_back(seg_start, seg_end,
                                       BernoulliSet(first, rest_trials.end()));
                std::advance(first, i - pre_i);
                pre_i = i;
                pre_l = l;
            }
        }
    }

    const int getItem() const { return e_; }

    const std::vector<BernoulliSegment> &getBernoulliSegments() const {
        return segments_;
    }

}; /* BernoulliSegments */

#endif /* __BERNOULLI_SEGMENT_H__ */
