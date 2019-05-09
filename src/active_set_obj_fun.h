/**
 * Copyright (C) by J.Z. 2019-05-02 20:52
 * Distributed under terms of the MIT license.
 */

#ifndef __ACTIVE_SET_OBJ_FUN_H__
#define __ACTIVE_SET_OBJ_FUN_H__

#include "obj_fun.h"
#include <Eigen/Dense>

using FeatureVector = Eigen::Matrix<double, FEATURE_DIM, 1>;
using FeatureData = std::unordered_map<int, FeatureVector>;
using FeatureDataPtr = FeatureData*;

/**
 * f(S) = log det(I + K(S,S))
 */
class ActiveSetObjFun : public ObjFun {
private:
    double lambda_;
    const FeatureDataPtr dat_ptr_;

private:
    inline double kernel(const int d1, const int d2) const {
        return std::exp((dat_ptr_->at(d1) - dat_ptr_->at(d2)).squaredNorm() /
                        (-2 * lambda_ * lambda_));
    }

    template <class Iter>
    double getVal(const Iter first, const Iter last) const;

    template <class Iter>
    double getGain(const int u, const Iter first, const Iter last) const;

public:
    ActiveSetObjFun(const double lambda, const FeatureDataPtr dat_ptr)
        : lambda_(lambda), dat_ptr_(dat_ptr) {}

    double getVal(const int s) const override { return std::log(2); }

    // NOTE: assume items in S are different with each other
    double getVal(const std::vector<int>& S) const override {
        return getVal(S.begin(), S.end());
    }

    double getGain(const int s, const std::vector<int>& S) const override {
        return getGain(s, S.begin(), S.end());
    }

}; /* ActiveSetObjFun */

template <class Iter>
double ActiveSetObjFun::getVal(const Iter first, const Iter last) const {
    if (first == last) return 0;
    int n = last - first;
    Eigen::MatrixXd K(n, n);
    for (auto it_i = first; it_i != last; ++it_i) {
        for (auto it_j = it_i; it_j != last; ++it_j) {
            K(it_i - first, it_j - first) = K(it_j - first, it_i - first) =
                it_i == it_j ? 2 : kernel(*it_i, *it_j);
        }
    }
    return std::log(K.determinant());
}

template <class Iter>
double ActiveSetObjFun::getGain(const int s, const Iter first,
                                const Iter last) const {
    if (first == last) return getVal(s);
    int n = last - first + 1;
    Eigen::MatrixXd K(n, n);
    for (auto it_i = first; it_i != last; ++it_i) {
        if (*it_i == s) return 0;
        int i = it_i - first;
        for (auto it_j = it_i; it_j != last; ++it_j) {
            int j = it_j - first;
            K(i, j) = K(j, i) = (i == j) ? 2 : kernel(*it_i, *it_j);
        }
        K(n - 1, i) = K(i, n - 1) = kernel(s, *it_i);
    }
    K(n - 1, n - 1) = 2;
    return std::log(K.determinant()) -
           std::log(K.block(0, 0, n - 1, n - 1).determinant());
}

#endif /* __ACTIVE_SET_OBJ_FUN_H__ */
