/**
 * Copyright (C) by J.Z. 2018-08-25 10:50
 * Distributed under terms of the MIT license.
 */

#include "hist_streaming.h"

void HistStreaming::newEndIfNeed(const int idx) {
    if (algs_.empty() || algs_.back()->idx_ < idx)
        algs_.push_back(new Alg(idx, num_samples_, budget_, eps_, obj_ptr_));
}

void HistStreaming::insertHead(const int idx, const int e,
                               const BernoulliSet& bs,
                               const std::list<Alg*>::iterator& it) {
    Alg* alg = new Alg(*(*it));
    alg->idx_ = idx;
    alg->delta_ = alg->uncertainty_ = (*it)->uncertainty_;
    alg->feed(e, bs);
    algs_.insert(it, alg);
}

void HistStreaming::insertBefore(const int idx, const int e,
                                 const BernoulliSet& bs,
                                 const std::list<Alg*>::iterator& it) {
    Alg* alg = new Alg(*(*it));
    alg->idx_ = idx;
    alg->feed(e, bs);
    algs_.insert(it, alg);
    alg->delta_ = alg->uncertainty_ = (*it)->uncertainty_;
}

void HistStreaming::feedSegment(const int e, const BernoulliSegment& seg,
                                std::list<Alg*>::iterator& alg_it) {
    auto job = [e, &seg](Alg* alg) { alg->feed(e, seg.bs_); };
    std::vector<std::future<void>> futures;
    while (alg_it != algs_.end() && (*alg_it)->idx_ < seg.end_) {
        futures.push_back(std::async(std::launch::async, job, *alg_it));
        ++alg_it;
    }
    for (auto& future : futures) future.get();
}

void HistStreaming::feed(const int e, const BernoulliSegments& segs) {
    newEndIfNeed(segs.getMxIdx());

    auto it = algs_.begin();
    for (auto& seg : segs.segments_) {
        // Update instances belonging to this segment.
        feedSegment(e, seg, it);

        // If the last updated instance has index equal to sendment end, then
        // there is no need to create new instance at the end of this segment.
        auto pre = it;
        if (it != algs_.begin()) {
            --pre;  // Now "pre" is the precessor of "it".
            if ((*pre)->idx_ == seg.end_ - 1) continue;
        }

        // Create a new instance before "it". If it = algs.begin(), then insert
        // a new head. Otherwise, the new instance has a precessor, which is
        // "pre". In this case, we need to check the new instance's redundancy.
        // If the new instance is redundant, we do not need to create it.
        if (it == algs_.begin())
            insertHead(seg.end_ - 1, e, seg.bs_, it);
        else if ((*it)->lower() < (1 - eps_) * (*pre)->upper())
            insertBefore(seg.end_ - 1, e, seg.bs_, it);
    }
}

void HistStreaming::reduce() {
    auto i = algs_.begin();
    while (i != algs_.end()) {
        // find the largest j s.t. g(j) >= (1-e)g(i)
        auto j = i, l = i;
        if (++j == algs_.end()) break;
        double bound = (*i)->upper() * (1 - eps_);
        while (j != algs_.end() && (*j)->lower() >= bound) ++j;
        if (--j == i || ++l == j) {  // j and l will be at the correct position
            ++i;
            continue;
        }
        // (*l) now points to the first instance to be deleted.
        (*i)->uncertainty_ = (*l)->uncertainty_;
        while (l != j) {
            del_cost_ += (*l)->getCost();
            delete *l;
            ++l;
        }
        (*j)->uncertainty_ = (*i)->upper() * eps_;
        algs_.erase(++i, j);
        ++i;
    }
}

void HistStreaming::next() {
    if (algs_.front()->idx_ == 0) {
        delete algs_.front();
        algs_.pop_front();
    }
    for (auto ptr : algs_) ptr->idx_--;
}
