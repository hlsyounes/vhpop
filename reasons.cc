/*
 * Copyright (C) 2002 Carnegie Mellon University
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
 * $Id: reasons.cc,v 1.2 2002-01-25 18:23:13 lorens Exp $
 */
#include "reasons.h"
#include "plans.h"
#include "parameters.h"


/* ====================================================================== */
/* Reason */

struct DummyReason : public Reason {
protected:
  virtual void print(ostream& os) const {
    os << "DummyReason";
  }
};


/* A dummy reason. */
const Reason& Reason::DUMMY = *(new DummyReason());


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


/* ====================================================================== */
/* InitReason */


/* Returns an init reason (or a dummy reason if reasons are not
   needed. */
const Reason& InitReason::make(const Parameters& params) {
  return params.transformational ? *(new InitReason()) : DUMMY;
}


/* Constructs an Init reason. */
InitReason::InitReason() {}


/* Prints this reason on the given stream. */
void InitReason::print(ostream& os) const {
  os << "#<InitReason>";
}


/* ====================================================================== */
/* AddStepReason */

/* Returns an AddStep reason (or a dummy reason if reasons are not
   needed. */
const Reason& AddStepReason::make(const Parameters& params, size_t step_id) {
  return params.transformational ? *(new AddStepReason(step_id)) : DUMMY;
}


/* Constructs an AddStep reason. */
AddStepReason::AddStepReason(size_t step_id)
  : step_id(step_id) {}


/* Checks if this reason involves the given step. */
bool AddStepReason::involves(const Step& step) const {
  return step_id == step.id;
}


/* Prints this reason on the given stream. */
void AddStepReason::print(ostream& os) const {
  os << "#<AddStepReason " << step_id << ">";
}


/* ====================================================================== */
/* EstablishReason */

/* Returns an Establish reason (or a dummy reason if reasons are not
   needed. */
const Reason& EstablishReason::make(const Parameters& params,
				    const Link& link) {
  return params.transformational ? *(new EstablishReason(link)) : DUMMY;
}

/* Constructs an Establish reason. */
EstablishReason::EstablishReason(const Link& link)
  : link(link) {}


/* Checks if this reason involves the given link. */
bool EstablishReason::involves(const Link& link) const {
  return &this->link == &link;
}


/* Prints this reason on the given stream. */
void EstablishReason::print(ostream& os) const {
  os << "#<EstablishReason " << link << ">";
}


/* ====================================================================== */
/* ProtectReason */

/* Returns a Protect reason (or a dummy reason if reasons are not
   needed. */
const Reason& ProtectReason::make(const Parameters& params,
				  const Link& link, const size_t step_id) {
  return params.transformational ? *(new ProtectReason(link, step_id)) : DUMMY;
}


/* Constructs a Protect reason. */
ProtectReason::ProtectReason(const Link& link, size_t step_id)
  : link(link), step_id(step_id) {}


/* Checks if this reason involves the given link. */
bool ProtectReason::involves(const Link& link) const {
  return &this->link == &link;
}


/* Checks if this reason involves the given step. */
bool ProtectReason::involves(const Step& step) const {
  return step_id == step.id;
}


/* Prints this reason on the given stream. */
void ProtectReason::print(ostream& os) const {
  os << "#<ProtectReason " << link << " step " << step_id << ">";
}
