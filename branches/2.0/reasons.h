/* -*-C++-*- */
/*
 * Reasons for refinements in a plan.
 *
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
 * $Id: reasons.h,v 3.1 2002-05-26 11:04:07 lorens Exp $
 */
#ifndef REASONS_H
#define REASONS_H

#include "support.h"

struct Parameters;
struct Link;
struct Step;


/*
 * Abstract reason.
 */
struct Reason : public Collectible, public Printable {
  /* A dummy reason. */
  static const Reason& DUMMY;

  /* Checks if this reason is a dummy reason. */
  bool dummy() const;

  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const;

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const;
};


/*
 * Reason attached to elements of the initial plan.
 */
struct InitReason : public Reason {
  /* Returns an Init reason (or a dummy reason if reasons are not
     needed. */
  static const Reason& make(const Parameters& params);

protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Constructs an Init reason. */
  InitReason();
};


/*
 * Reason attached to elements added along with a step.
 */
struct AddStepReason : public Reason {
  /* Id of added step. */
  const size_t step_id;

  /* Returns an AddStep reason (or a dummy reason if reasons are not
     needed. */
  static const Reason& make(const Parameters& params, size_t step_id);

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const;

protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Constructs an AddStep reason. */
  AddStepReason(size_t step_id);
};


/*
 * Reason attached to elements needed to establish a link.
 */
struct EstablishReason : public Reason {
  /* Established link */
  const Link& link;

  /* Returns an Establish reason (or a dummy reason if reasons are not
     needed. */
  static const Reason& make(const Parameters& params, const Link& link);

  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const;

protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Constructs an Establish reason. */
  EstablishReason(const Link& link);
};


/*
 * Reason attached to elements needed to protect a link.
 */
struct ProtectReason : public Reason {
  /* Protected link. */
  const Link& link;
  /* Id of threatening step. */
  const size_t step_id;

  /* Returns a Protect reason (or a dummy reason if reasons are not
     needed. */
  static const Reason& make(const Parameters& params,
			    const Link& link, const size_t step_id);

  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const;

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const;

protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Constructs a Protect reason. */
  ProtectReason(const Link& link, size_t step_id);
};


#endif /* REASONS_H */
