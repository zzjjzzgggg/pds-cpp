/**
 * Copyright (C) by J.Z. 2019-12-27 09:06
 * Distributed under terms of the MIT license.
 */

#ifndef __SIMPLE_GREEDY_H__
#define __SIMPLE_GREEDY_H__

#include "stdafx.h"
#include "obj_fun.h"

class SimpleGreedy {
private:
    class Elem {
    public:
        int t, node;  // t: the t number that gain is updated
        double gain;

    public:
        Elem() : node(-1), t(-1), gain(-1) {}
        Elem(int v, int s = 0, double g = std::numeric_limits<double>::max())
            : node(v), t(s), gain(g) {}
        void echo() const { printf("i:%3d g:%.2e\n", t, gain); }
    };

public:
    int budget_, cost_ = 0;
    const ObjFun* obj_ptr_;
    std::vector<int> chosen_;

public:
    SimpleGreedy(const int budget, const ObjFun* obj_ptr)
        : budget_(budget), obj_ptr_(obj_ptr) {}

    /**
     * Greedy on a set of elements, return final reward.
     */
    double run(const std::vector<int>& population) {
        chosen_.clear();

        // initialize the priority queue
        auto cmp = [](Elem& a, Elem& b) { return a.gain < b.gain; };
        std::priority_queue<Elem, std::vector<Elem>, decltype(cmp)> pq(cmp);

        // fill the queue with population
        for (int v : population) pq.emplace(v);

        // greedily chose elements
        double rwd = 0;
        int t = 1;
        while (!pq.empty() && t <= budget_) {
            Elem e = pq.top();
            pq.pop();
            if (e.t == t) {
                chosen_.push_back(e.node);
                ++t;
                rwd += e.gain;
            } else {
                e.gain = obj_ptr_->getGain(e.node, chosen_);
                ++cost_;
                e.t = t;
                pq.push(std::move(e));
            }
        }
        return rwd;
    }

}; /* SimpleGreedy */

#endif /* __SIMPLE_GREEDY_H__ */
