// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// A timer to measure passage of time.

#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

// A timer to measure passage of time.
template <typename Clock = std::chrono::steady_clock>
class Timer {
 public:
  // Constructs a timer and starts it.
  Timer() : start_(Clock::now()) {}

  // Returns the time elapsed since this timer was created.
  std::chrono::nanoseconds ElapsedTime() const {
    return Clock::now() - start_;
  }

 private:
  // The start time for this timer.
  typename Clock::time_point start_;
};

#endif  // TIMER_H_
