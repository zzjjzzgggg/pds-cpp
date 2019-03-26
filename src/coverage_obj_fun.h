/**
 * Copyright (C) by J.Z. 2018-08-24 21:30
 * Distributed under terms of the MIT license.
 */

#ifndef __COVERAGE_OBJ_FUN_H__
#define __COVERAGE_OBJ_FUN_H__

#include "obj_fun.h"

class CoverageObjFun : public ObjFun {
private:
    graph::bi::BGraph graph_;  // L: user, R: venue

private:
    template <class Iter>
    double getVal(const Iter first, const Iter last) const;

    template <class Iter>
    double getGain(const int v, const Iter first, const Iter last) const;

public:
    CoverageObjFun(const std::string& bin_file) : ObjFun() {
        graph_ = graph::loadBinary<graph::bi::BGraph>(bin_file);
        printf("LNodes: %d, RNodes: %d, Edges: %d\n", graph_.getNodesL(),
               graph_.getNodesR(), graph_.getEdges());
    }

    double getVal(const int v) const override {
        return graph_.getNodeL(v).getDeg();
    }

    double getVal(const std::vector<int>& S) const override {
        return getVal(S.begin(), S.end());
    }

    double getVal(const std::unordered_set<int>& S) const override {
        return getVal(S.begin(), S.end());
    }

    double getGain(const int u, const std::vector<int>& S) const override {
        return getGain(u, S.begin(), S.end());
    }

    double getGain(const int u,
                   const std::unordered_set<int>& S) const override {
        return getGain(u, S.begin(), S.end());
    }

}; /* CoverageObjFun */

template <class Iter>
double CoverageObjFun::getVal(const Iter first, const Iter last) const {
    if (first == last) return 0;
    std::unordered_set<int> covered;
    for (auto it = first; it != last; ++it) {
        if (graph_.isNodeL(*it)) {
            const auto& nd_v = graph_.getNodeL(*it);
            covered.insert(nd_v.beginNbr(), nd_v.endNbr());
        }
    }
    return covered.size();
}

template <class Iter>
double CoverageObjFun::getGain(const int v, const Iter first,
                               const Iter last) const {
    if (!graph_.isNodeL(v)) return 0;
    if (first == last)
        return getVal(v);
    else {
        std::unordered_set<int> covered;
        // covered users by S
        for (auto it = first; it != last; ++it) {
            if (graph_.isNodeL(*it)) {
                const auto& nd = graph_.getNodeL(*it);
                covered.insert(nd.beginNbr(), nd.endNbr());
            }
        }
        int rwd_S = covered.size();

        // covered users by v
        if (graph_.isNodeL(v)) {
            const auto& nd = graph_.getNodeL(v);
            covered.insert(nd.beginNbr(), nd.endNbr());
        }
        return covered.size() - rwd_S;
    }
}

#endif /* __COVERAGE_OBJ_FUN_H__ */
