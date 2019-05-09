#pragma once

#include <cstdio>
#include <cassert>
#include <cmath>

#include <limits>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <queue>
#include <array>

#include <mutex>
#include <future>
#include <thread>

#include <os/osutils.h>
#include <io/ioutils.h>
#include <adv/rngutils.h>

using namespace fmt::literals;
using BernoulliSet = std::vector<int>;

// NYC-taxi: 6, Yahoo-click: 5
// #define FEATURE_DIM 6
