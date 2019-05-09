/**
 * Copyright (C) by J.Z. 2019-05-01 16:23
 * Distributed under terms of the MIT license.
 */

#ifndef __MEDOID_OBJ_FUN_H__
#define __MEDOID_OBJ_FUN_H__

#include "obj_fun.h"

class MedoidObjFun : public ObjFun {
private:
    double Le0_;
    std::unordered_map<int, std::pair<double, double>> points_;
    std::vector<int> eval_points_;

private:
    // d(e1, e2)
    inline double d(const int p1, const int p2) const {
        return std::pow(points_.at(p1).first - points_.at(p2).first, 2) +
               std::pow(points_.at(p1).second - points_.at(p2).second, 2);
    }

    // d(e, S) where S is specified by [first, last)
    template <class Iter>
    double d(const int e, const Iter first, const Iter last) const {
        double deS = d(e, -1);
        for (auto it = first; it != last; ++it) {
            if (points_.find(*it) != points_.end()) {
                double des = d(e, *it);
                if (des < deS) deS = des;
            }
        }
        return deS;
    }

    template <class Iter>
    double getVal(const Iter first, const Iter last) const;

    template <class Iter>
    double getGain(const int u, const Iter first, const Iter last) const;

public:
    MedoidObjFun(const std::string& points_fnm) {
        ioutils::TSVParser ss(points_fnm);
        while (ss.next()) {
            int id = ss.get<int>(0);
            double x = ss.get<double>(1), y = ss.get<double>(2);
            points_[id] = std::pair<double, double>(x, y);
        }
        points_[-1] = std::pair<double, double>(0, 0);  // e0
    }

    void setEvalPoints(const std::vector<int>& eval_points) {
        eval_points_ = eval_points;

        // calculate L({e0})
        Le0_ = 0;
        for (int e : eval_points_) Le0_ += d(e, -1);
        Le0_ /= eval_points_.size();
    }

    double getVal(const int s) const override {
        if (points_.find(s) != points_.end()) {
            double loss = 0;
            for (int e : eval_points_) {
                double des = d(e, s), de0 = d(e, -1);
                loss += std::min(des, de0);
            }
            return Le0_ - loss / eval_points_.size();
        }
        return 0;
    }

    double getVal(const std::vector<int>& S) const override {
        return getVal(S.begin(), S.end());
    }

    double getGain(const int s, const std::vector<int>& S) const override {
        return getGain(s, S.begin(), S.end());
    }

}; /* MedoidObjFun */

template <class Iter>
double MedoidObjFun::getVal(const Iter first, const Iter last) const {
    if (first == last) return 0;
    double loss = 0;
    for (int e : eval_points_) loss += d(e, first, last);
    return Le0_ - loss / eval_points_.size();
}

template <class Iter>
double MedoidObjFun::getGain(const int s, const Iter first,
                             const Iter last) const {
    if (points_.find(s) == points_.end()) return 0;
    if (first == last) return getVal(s);

    double loss1 = 0, loss2 = 0;
    for (int e : eval_points_) {
        double deS = d(e, first, last);
        loss1 += deS;
        double des = d(e, s);
        if (des < deS) deS = des;
        loss2 += deS;
    }

    return (loss1 - loss2) / eval_points_.size();
}

#endif /* __MEDOID_OBJ_FUN_H__ */
