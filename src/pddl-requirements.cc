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

#include "pddl-requirements.h"

void PddlRequirements::EnableTyping() { typing_ = true; }

void PddlRequirements::EnableNegativePreconditions() {
  negative_preconditions_ = true;
}

void PddlRequirements::EnableDisjunctivePreconditions() {
  disjunctive_preconditions_ = true;
}

void PddlRequirements::EnableEquality() { equality_ = true; }

void PddlRequirements::EnableExistentialPreconditions() {
  existential_preconditions_ = true;
}

void PddlRequirements::EnableUniversalPreconditions() {
  universal_preconditions_ = true;
}

void PddlRequirements::EnableQuantifiedPreconditions() {
  EnableExistentialPreconditions();
  EnableUniversalPreconditions();
}

void PddlRequirements::EnableConditionalEffects() {
  conditional_effects_ = true;
}

void PddlRequirements::EnableFluents() { fluents_ = true; }

void PddlRequirements::EnableAdl() {
  EnableTyping();
  EnableNegativePreconditions();
  EnableDisjunctivePreconditions();
  EnableEquality();
  EnableQuantifiedPreconditions();
  EnableConditionalEffects();
}

void PddlRequirements::EnableDurativeActions() { durative_actions_ = true; }

void PddlRequirements::EnableDurationInequalities() {
  duration_inequalities_ = true;
}

void PddlRequirements::EnableContinuousEffects() { continuous_effects_ = true; }

void PddlRequirements::EnableTimedInitialLiterals() {
  EnableDurativeActions();
  timed_initial_literals_ = true;
}
