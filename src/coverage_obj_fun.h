/**
 * Copyright (C) by J.Z. 2019-03-27 09:46
 * Distributed under terms of the MIT license.
 */

#ifndef __COVERAGE_OBJ_FUN_H__
#define __COVERAGE_OBJ_FUN_H__

#include "obj_fun.h"

class CoverageObjFun : public ObjFun {
private:
    std::unordered_map<int, std::vector<int>> user_venues_;

private:
    template <class Iter>
    double getVal(const Iter first, const Iter last) const;

    template <class Iter>
    double getGain(const int u, const Iter first, const Iter last) const;

public:
    // "input_file" should contain two columns in the form {(user, venue)}. Then
    // a map {user -> venue_vector} will be constructed.
    CoverageObjFun(const std::string& input_file) : ObjFun() {
        ioutils::TSVParser ss(input_file);
        while (ss.next()) {
            int u = ss.get<int>(0), v = ss.get<int>(1);
            user_venues_[u].push_back(v);
        }
    }

    double getVal(const int u) const override {
        if (user_venues_.find(u) == user_venues_.end()) return 0;
        return user_venues_.at(u).size();
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
        auto search = user_venues_.find(*it);
        if (search != user_venues_.end())
            covered.insert(search->second.begin(), search->second.end());
    }
    return covered.size();
}

template <class Iter>
double CoverageObjFun::getGain(const int u, const Iter first,
                               const Iter last) const {
    if (user_venues_.find(u) == user_venues_.end()) return 0;
    if (first == last) return getVal(u);

    std::unordered_set<int> covered;
    for (auto it = first; it != last; ++it) {
        auto search = user_venues_.find(*it);
        if (search != user_venues_.end())
            covered.insert(search->second.begin(), search->second.end());
    }

    int rwd_S = covered.size();
    auto vec = user_venues_.at(u);
    covered.insert(vec.begin(), vec.end());

    return covered.size() - rwd_S;
}

#endif /* __COVERAGE_OBJ_FUN_H__ */
