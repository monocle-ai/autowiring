// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

// Internal build flag for namespace discrimination
#define AUTOWIRING_IS_BEING_BUILT

#include <autowiring/C++11/cpp11.h>
#include <gtest/gtest.h>
#include <autowiring/autowiring.h>

#ifdef _MSC_VER
  #include <thread>
  #include <mutex>
#endif

// Very unusual syntax -- function taking an array of fixed size, and returning
// a character array of that same size
template<class T, int n>
const char(&ArraySize(const T(&vals)[n]))[n];

#define ARRAYCOUNT(x) sizeof(ArraySize(x))
