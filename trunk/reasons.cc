/*
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: reasons.cc,v 3.6 2003-03-01 18:54:49 lorens Exp $
 */
#include "reasons.h"
#include "plans.h"
#include "formulas.h"
#include "parameters.h"


/* ====================================================================== */
/* Reason */

struct DummyReason : public Reason {
  static const DummyReason THE_DUMMY;

  DummyReason() {
    register_use(this);
#ifdef DEBUG_MEMORY
    created_reasons--;
#endif
  }

protected:
  virtual void print(std::ostream& os) const {
    os << "DummyReason";
  }
};


/* A dummy reason. */
const DummyReason DummyReason::THE_DUMMY = DummyReason();
const Reason& Reason::DUMMY = DummyReason::THE_DUMMY;


/* Constructs a reason. */
Reason::Reason()
  : ref_count_(0) {
#ifdef DEBUG_MEMORY
  created_reasons++;
#endif
}


/* Deletes this reason. */
Reason::~Reason() {
#ifdef DEBUG_MEMORY
    deleted_reasons++;
#endif
}


/* Checks if this reason is a dummy reason. */
bool Reason::dummy() const {
  return this == &DUMMY;
}


/* Checks if this reason involves the given link. */
bool Reason::involves(const Link& link) const {
  return false;
}


/* Checks if this reason involves the given step. */
bool Reason::involves(const Step& step) const {
  return false;
}


/* Output operator for reasons. */
std::ostream& operator<<(std::ostream& os, const Reason& r) {
  r.print(os);
  return os;
}


/* ====================================================================== */
/* InitReason */


/* Returns an init reason (or a dummy reason if reasons are not
   needed. */
const Reason& InitReason::make(const Parameters& params) {
  return params.transformational ? (const Reason&) *(new InitReason()) : DUMMY;
}


/* Constructs an Init reason. */
InitReason::InitReason() {}


/* Prints this reason on the given stream. */
void InitReason::print(std::ostream& os) const {
  os << "#<InitReason>";
}


/* ====================================================================== */
/* AddStepReason */

/* Returns an AddStep reason (or a dummy reason if reasons are not
   needed. */
const Reason& AddStepReason::make(const Parameters& params, size_t step_id) {
  return (params.transformational
	  ? (const Reason&) *(new AddStepReason(step_id)) : DUMMY);
}


/* Constructs an AddStep reason. */
AddStepReason::AddStepReason(size_t step_id)
  : step_id_(step_id) {}


/* Checks if this reason involves the given step. */
bool AddStepReason::involves(const Step& step) const {
  return step_id_ == step.id();
}


/* Prints this reason on the given stream. */
void AddStepReason::print(std::ostream& os) const {
  os << "#<AddStepReason " << step_id_ << ">";
}


/* ====================================================================== */
/* EstablishReason */

/* Returns an Establish reason (or a dummy reason if reasons are not
   needed. */
const Reason& EstablishReason::make(const Parameters& params,
				    const Link& link) {
  return (params.transformational
	  ? (const Reason&) *(new EstablishReason(link)) : DUMMY);
}

/* Constructs an Establish reason. */
EstablishReason::EstablishReason(const Link& link)
  : link_(&link) {}


/* Checks if this reason involves the given link. */
bool EstablishReason::involves(const Link& link) const {
  return link_ == &link;
}


/* Prints this reason on the given stream. */
void EstablishReason::print(std::ostream& os) const {
  os << "#<EstablishReason " << link_->from_id() << ' ' << link_->condition()
     << ' ' << link_->to_id() << ">";
}


/* ====================================================================== */
/* ProtectReason */

/* Returns a Protect reason (or a dummy reason if reasons are not
   needed. */
const Reason& ProtectReason::make(const Parameters& params,
				  const Link& link, const size_t step_id) {
  return (params.transformational
	  ? (const Reason&) *(new ProtectReason(link, step_id)) : DUMMY);
}


/* Constructs a Protect reason. */
ProtectReason::ProtectReason(const Link& link, size_t step_id)
  : link_(&link), step_id_(step_id) {}


/* Checks if this reason involves the given link. */
bool ProtectReason::involves(const Link& link) const {
  return link_ == &link;
}


/* Checks if this reason involves the given step. */
bool ProtectReason::involves(const Step& step) const {
  return step_id_ == step.id();
}


/* Prints this reason on the given stream. */
void ProtectReason::print(std::ostream& os) const {
  os << "#<ProtectReason " << link_->from_id() << ' ' << link_->condition()
     << ' ' << link_->to_id() << " step " << step_id_ << ">";
}
