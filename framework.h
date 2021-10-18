#pragma once
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <variant>
#include <map>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <base64/base64.cpp>
#include <boost/range/algorithm_ext/push_back.hpp> // boost::push_back
#include <boost/algorithm/string/predicate.hpp> // boost::starts_with
#include <boost/chrono.hpp> // boost::chrono
#include <boost/thread.hpp> // boost::this_thread
#include <type_traits>

typedef std::mt19937 rng_type;

#endif