/**
 * Copyright (C) by J.Z. 2019-03-27 17:08
 * Distributed under terms of the MIT license.
 */

#ifndef __BERNOULLI_SEGMENT_H__
#define __BERNOULLI_SEGMENT_H__

#include "stdafx.h"

/**
 * A Bernoulli segment.
 * Bernoulli sets belonging to the same segment are the same.
 * That is, I_l(e) for l in [start, end) are same.
 */
class BernoulliSegment {
public:
    int start_, end_;
    BernoulliSet bs_;

public:
    BernoulliSegment(const int start, const int end, BernoulliSet &&bs)
        : start_(start), end_(end), bs_(std::move(bs)) {
        // std::sort(bs_.begin(), bs_.end());
    }

}; /* BernoulliSegment */

/**
 * A vector of Bernoulli segments.
 */
class BernoulliSegments {
public:
    std::vector<BernoulliSegment> segments_;

private:
    void init(std::vector<std::pair<int, int>> &&samples) {}

public:
    /**
     * Constructor.
     * "lifespans" is a vector of lifespans.
     */
    BernoulliSegments(const std::vector<int> &lifespans) {
        std::vector<std::pair<int, int>> samples;
        samples.reserve(lifespans.size());
        int i = 0;
        for (int l : lifespans) samples.emplace_back(i++, l);

        // sort in ascending order of lifespan
        std::sort(samples.begin(), samples.end(),
                  [](std::pair<int, int> &a, std::pair<int, int> &b) {
                      return a.second < b.second;
                  });

        // a vector of trials
        std::vector<int> rest_trials;
        rest_trials.reserve(samples.size());
        for (const auto &pr : samples) rest_trials.push_back(pr.first);

        // add a dumy element at the end
        samples.emplace_back(-1, -1);

        // scan the pair vector
        auto first = rest_trials.begin();
        int p_i = 0, pp_l = 0, p_l = samples[0].second, l;
        for (int i = 1; i < samples.size(); ++i) {
            l = samples[i].second;
            if (l != p_l) {
                segments_.emplace_back(pp_l, p_l,
                                       BernoulliSet(first, rest_trials.end()));
                std::advance(first, i - p_i);
                p_i = i;
                pp_l = p_l;
                p_l = l;
            }
        }
    }

    // Return the minimum trial ID in the Bernoulli segments.
    int getMnIdx() const { return segments_.front().end_ - 1; }

    // Return the maximum trial ID in the Bernoulli segments.
    int getMxIdx() const { return segments_.back().end_ - 1; }

}; /* BernoulliSegments */

#endif /* __BERNOULLI_SEGMENT_H__ */
