/**
 * Copyright (C) by J.Z. 2021-02-01 11:18
 * Distributed under terms of the MIT license.
 */

#ifndef __TWEET_OBJ_FUN_H__
#define __TWEET_OBJ_FUN_H__

#include "obj_fun.h"

class TweetObjFun : public ObjFun {
public:
    class Tweet {
    public:
        int num_rt_ = 0, num_like_ = 0;
        std::vector<int> words_;

    public:
        Tweet() {}
        Tweet(const int nr, const int nl, const int nw)
            : num_rt_(nr), num_like_(nl) {
            words_.reserve(nw);
        }
        double val() const { return (num_rt_ + num_like_) / 2.0; }
        int size() const { return words_.size(); }

        void add(const int word) { words_.push_back(word); }
        void sort() { std::sort(words_.begin(), words_.end()); }

        void save(std::unique_ptr<ioutils::IOOut>& po) const {
            po->save(num_rt_);
            po->save(num_like_);
            po->save(words_);
        }

        void load(std::unique_ptr<ioutils::IOIn>& pi) {
            pi->load(num_rt_);
            pi->load(num_like_);
            pi->load(words_);
        }
    }; /* Tweet */

private:
    std::vector<Tweet> tweets_;

public:
    TweetObjFun(const std::string& input_file, const bool isBinary = false) {
        if (isBinary)
            load(input_file);
        else {
            ioutils::TSVParser ss(input_file);
            while (ss.next()) {
                std::vector<std::string> words =
                    strutils::split(ss.get<std::string>(4), ',');
                Tweet tweet(ss.get<int>(2), ss.get<int>(3), words.size());
                for (auto word : words) tweet.add(std::stoi(word));
                tweet.sort();
                tweets_.push_back(std::move(tweet));
            }
        }
    }

    void save(const std::string& filename) const {
        auto po = ioutils::getIOOut(filename);
        po->save((int)tweets_.size());
        for (auto tweet : tweets_) tweet.save(po);
    }

    void load(const std::string& filename) {
        auto pi = ioutils::getIOIn(filename);
        int total;
        pi->load(total);
        for (int i = 0; i < total; ++i) {
            Tweet tweet;
            tweet.load(pi);
            tweets_.push_back(std::move(tweet));
        }
    }

    // Given a tweet id, return its value
    double getVal(const int tid) const override {
        if (tid < tweets_.size())
            return std::log(1 + tweets_[tid].size() * tweets_[tid].val());
        return 0;
    }

    double getVal(const std::vector<int>& S) const override {
        std::unordered_map<int, double> word_score;
        for (int tid : S) {
            if (tid < tweets_.size()) {
                for (int w : tweets_[tid].words_)
                    word_score[w] += tweets_[tid].val();
            }
        }
        double score = 0;
        for (auto const& [k, v] : word_score) score += std::log(1 + v);
        return score;
    }

    double getGain(const int u, const std::vector<int>& S) const override {
        std::unordered_map<int, double> word_score;
        for (int tid : S) {
            if (tid < tweets_.size()) {
                for (int w : tweets_[tid].words_)
                    word_score[w] += tweets_[tid].val();
            }
        }
        double score = 0;
        for (auto const& [k, v] : word_score) score += std::log(1 + v);

        if (u < tweets_.size()) {
            for (int w : tweets_[u].words_) word_score[w] += tweets_[u].val();
        }
        double score_all = 0;
        for (auto const& [k, v] : word_score) score_all += std::log(1 + v);

        return score_all - score;
    }
}; /* TweetObjFun */

#endif /* __TWEET_OBJ_FUN_H__ */
