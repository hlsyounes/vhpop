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
// Tests for timer.

#include "timer.h"

#include <chrono>
#include <ratio>

#include "gtest/gtest.h"

namespace {

class FakeClock {
 public:
  using rep = int;
  using period = std::micro;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<FakeClock>;

  static time_point now() { return time_point(duration(value)); }

  static int value;
};

int FakeClock::value = 0;

TEST(TimerTest, FakeClock) {
  FakeClock::value = 0;
  Timer<FakeClock> timer1;
  EXPECT_EQ(0, timer1.ElapsedTime().count());
  FakeClock::value = 5;
  Timer<FakeClock> timer2;
  EXPECT_EQ(5000, timer1.ElapsedTime().count());
  EXPECT_EQ(0, timer2.ElapsedTime().count());
  FakeClock::value = 7;
  EXPECT_EQ(7000, timer1.ElapsedTime().count());
  EXPECT_EQ(2000, timer2.ElapsedTime().count());
}

TEST(TimerTest, RealClock) {
  Timer<> timer;
  EXPECT_LE(0, timer.ElapsedTime().count());
}

}  // namespace
