/**
 * Copyright (C) by J.Z. (04/06/2018 09:14)
 * Distributed under terms of the MIT license.
 */

#ifndef __GREEDY_ALG_H__
#define __GREEDY_ALG_H__

#include "obj_fun.h"
#include "candidate.h"

class GreedyAlg {
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

private:
    int num_samples_, budget_, cost_ = 0;

    const ObjFun* obj_ptr_;
    Candidate chosen_;

private:
    double getGain(const int u, const BernoulliSet& bs) const {
        double gain = 0;
        for (int i : bs) gain += obj_ptr_->getGain(u, chosen_.S_vec_[i]);
        return gain / num_samples_;
    }

public:
    GreedyAlg(const int num_samples, const int budget, const ObjFun* obj)
        : num_samples_(num_samples), budget_(budget), obj_ptr_(obj) {}

    /**
     * Greedy on a population, where population = {e -> I(e): e\in V}.
     * Return final reward.
     */
    double run(const std::unordered_map<int, BernoulliSet>& population) {
        // clear and re-initialize chosen_
        chosen_.clear();
        chosen_.init(num_samples_);

        // initialize the priority queue
        auto cmp = [](Elem& a, Elem& b) { return a.gain < b.gain; };
        std::priority_queue<Elem, std::vector<Elem>, decltype(cmp)> pq(cmp);

        // fill the queue with population
        for (auto& pr : population) pq.emplace(pr.first);

        // greedily chose elements
        double rwd = 0;
        int t = 1;
        while (!pq.empty() && t <= budget_) {
            Elem e = pq.top();
            pq.pop();
            const BernoulliSet& bs = population.at(e.node);
            if (e.t == t) {
                chosen_.insert(e.node, bs);
                ++t;
                rwd += e.gain;
            } else {
                e.gain = getGain(e.node, bs);
                ++cost_;
                e.t = t;
                pq.push(std::move(e));
            }
        }
        return rwd;
    }

    int getCost() const { return cost_; }

    void saveSolution(const std::string& filename) const {
        ioutils::saveVec(chosen_.getMembers(), filename);
    }

}; /* GreedyAlg */

#endif /* __GREEDY_ALG_H__ */
