// Copyright (C) 2021 Google Inc
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
// Tests for PDDL requirements.

#include "pddl-requirements.h"

#include "gtest/gtest.h"

namespace {

TEST(PddlRequirementsTest, DefaultRequirements) {
  PddlRequirements requirements;
  EXPECT_FALSE(requirements.typing());
  EXPECT_FALSE(requirements.negative_preconditions());
  EXPECT_FALSE(requirements.disjunctive_preconditions());
  EXPECT_FALSE(requirements.equality());
  EXPECT_FALSE(requirements.existential_preconditions());
  EXPECT_FALSE(requirements.universal_preconditions());
  EXPECT_FALSE(requirements.conditional_effects());
  EXPECT_FALSE(requirements.fluents());
  EXPECT_FALSE(requirements.durative_actions());
  EXPECT_FALSE(requirements.duration_inequalities());
  EXPECT_FALSE(requirements.continuous_effects());
  EXPECT_FALSE(requirements.timed_initial_literals());
}

TEST(PddlRequirementsTest, Typing) {
  PddlRequirements requirements;
  requirements.EnableTyping();
  EXPECT_TRUE(requirements.typing());
}

TEST(PddlRequirementsTest, NegativePreconditions) {
  PddlRequirements requirements;
  requirements.EnableNegativePreconditions();
  EXPECT_TRUE(requirements.negative_preconditions());
}

TEST(PddlRequirementsTest, DisjuntivePreconditions) {
  PddlRequirements requirements;
  requirements.EnableDisjunctivePreconditions();
  EXPECT_TRUE(requirements.disjunctive_preconditions());
}

TEST(PddlRequirementsTest, Equality) {
  PddlRequirements requirements;
  requirements.EnableEquality();
  EXPECT_TRUE(requirements.equality());
}

TEST(PddlRequirementsTest, ExistentialPreconditions) {
  PddlRequirements requirements;
  requirements.EnableExistentialPreconditions();
  EXPECT_TRUE(requirements.existential_preconditions());
}

TEST(PddlRequirementsTest, UniversalPreconditions) {
  PddlRequirements requirements;
  requirements.EnableUniversalPreconditions();
  EXPECT_TRUE(requirements.universal_preconditions());
}

TEST(PddlRequirementsTest, QuantifiedPreconditions) {
  PddlRequirements requirements;
  requirements.EnableQuantifiedPreconditions();
  EXPECT_TRUE(requirements.existential_preconditions());
  EXPECT_TRUE(requirements.universal_preconditions());
}

TEST(PddlRequirementsTest, ConditionalEffects) {
  PddlRequirements requirements;
  requirements.EnableConditionalEffects();
  EXPECT_TRUE(requirements.conditional_effects());
}

TEST(PddlRequirementsTest, Fluents) {
  PddlRequirements requirements;
  requirements.EnableFluents();
  EXPECT_TRUE(requirements.fluents());
}

TEST(PddlRequirementsTest, Adl) {
  PddlRequirements requirements;
  requirements.EnableAdl();
  EXPECT_TRUE(requirements.typing());
  EXPECT_TRUE(requirements.negative_preconditions());
  EXPECT_TRUE(requirements.disjunctive_preconditions());
  EXPECT_TRUE(requirements.equality());
  EXPECT_TRUE(requirements.existential_preconditions());
  EXPECT_TRUE(requirements.universal_preconditions());
  EXPECT_TRUE(requirements.conditional_effects());
}

TEST(PddlRequirementsTest, DurativeActions) {
  PddlRequirements requirements;
  requirements.EnableDurativeActions();
  EXPECT_TRUE(requirements.durative_actions());
}

TEST(PddlRequirementsTest, DurationInequalities) {
  PddlRequirements requirements;
  requirements.EnableDurationInequalities();
  EXPECT_TRUE(requirements.duration_inequalities());
}

TEST(PddlRequirementsTest, ContinuousEffects) {
  PddlRequirements requirements;
  requirements.EnableContinuousEffects();
  EXPECT_TRUE(requirements.continuous_effects());
}

TEST(PddlRequirementsTest, TimedInitialLiterals) {
  PddlRequirements requirements;
  requirements.EnableTimedInitialLiterals();
  EXPECT_TRUE(requirements.timed_initial_literals());
  EXPECT_TRUE(requirements.durative_actions());
}

}  // namespace
