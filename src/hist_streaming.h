/**
 * Copyright (C) by J.Z. 2018-08-25 10:39
 * Distributed under terms of the MIT license.
 */

#ifndef __HIST_STREAMING_H__
#define __HIST_STREAMING_H__

#include "bernoulli_segment.h"
#include "mc_sieve_streaming.h"

class HistStreaming {
private:
    /**
     * Wraper of the MC-SieveStreaming instance.
     */
    class Alg {
    private:
        MCSieveStreaming* ptr_;

    public:
        int idx_;             // instance index
        double val_ = 0,      // current utility value
            delta_ = 0,       // uncertainty of this instance
            uncertainty_ = 0; /* uncertainty of this interval (each interval's
                                 uncertainty is stored at the tail index) */

    public:
        Alg(const int idx, const int num_samples, const int budget,
            const double eps, const ObjFun* obj)
            : idx_(idx) {
            ptr_ = new MCSieveStreaming(num_samples, budget, eps, obj);
        }
        virtual ~Alg() { delete ptr_; }

        // Copy constructor.
        Alg(const Alg& o) : idx_(o.idx_), val_(o.val_) {
            ptr_ = new MCSieveStreaming(*o.ptr_);
        }

        inline void feed(const int e, const BernoulliSet& bs) {
            ptr_->feed(e, bs);
            val_ = ptr_->getResult().second;
        }

        inline double upper() const { return val_ + delta_; }
        inline double lower() const { return val_; }

        inline int getCost() const { return ptr_->getCost(); }

        // std::vector<int> getSolution() const {
        //     int i = ptr_->getResult().first;
        //     return ptr_->getSolution(i);
        // }

    }; /* Alg */

private:
    int num_samples_, budget_, del_cost_ = 0;
    double eps_;

    const ObjFun* obj_ptr_;
    std::list<Alg*> algs_;  // a list of Alg instances

private:
    /**
     * Create a new instance at the tail if need.
     * - "idx": the maximum position that an instance should exist.
     */
    void newEndIfNeed(const int idx);

    /**
     * Insert a new instance at the head.
     * - "idx": the new instance's index,
     * - "e": new element,
     * - "bs": new element's Bernoulli set,
     * - "it": insert before this position. Specially, it = algs.begin().
     */
    void insertHead(const int idx, const int e, const BernoulliSet& bs,
                    const std::list<Alg*>::iterator& it);

    /**
     * Insert a new instance before position "it".
     * - "idx": the new instance's index,
     * - "e": new element,
     * - "bs": new element's Bernoulli set,
     * - "it": insert before this position.
     */
    void insertBefore(const int idx, const int e, const BernoulliSet& bs,
                      const std::list<Alg*>::iterator& it);

    /**
     * Feed the segment's Bernoulli set to each instance belonging to this
     * segment, and alg_it points to the instance outside of the segment.
     */
    void feedSegment(const int e, const BernoulliSegment& seg,
                     std::list<Alg*>::iterator& alg_it);

public:
    HistStreaming(const int num_samples, const int budget, const double eps,
                  const ObjFun* obj)
        : num_samples_(num_samples), budget_(budget), eps_(eps), obj_ptr_(obj) {
    }

    virtual ~HistStreaming() {
        for (auto it = algs_.begin(); it != algs_.end(); ++it) delete *it;
    }

    /**
     * Process an element e with Bernoulli segments segs.
     * NOTE: Will scan the instance list from head to tail.
     *
     * - "e": new element,
     * - "segs": segments of Bernoulli sets.
     */
    void feed(const int e, const BernoulliSegments& segs);

    /**
     * Remove redundant instances. Meanwhile, maintain the uncertainty of each
     * interval. The amount of uncertainty of each interval is stored at the
     * tail index of each interval.
     */
    void reduce();

    /**
     * Prepare for next time step. Deleta the head instance if its l=1, and
     * conduct light clean for other instances.
     */
    void next();

    double getResult() const { return algs_.front()->val_; }

    // void saveSolution(const std::string& filename) const {
    //     ioutils::saveVec(algs_.front()->getSolution(), filename);
    // }

    int getCost() const {
        int cost = del_cost_;
        for (auto ptr : algs_) cost += ptr->getCost();
        return cost;
    }

    // Get the number of algorithm instances.
    int size() const { return algs_.size(); }

}; /* HistStreaming */

#endif /* __HIST_STREAMING_H__ */
