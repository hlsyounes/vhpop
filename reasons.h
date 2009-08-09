/* -*-C++-*- */
/*
 * Reasons for refinements in a plan.
 *
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
 * $Id: reasons.h,v 3.4 2003-03-01 18:50:35 lorens Exp $
 */
#ifndef REASONS_H
#define REASONS_H

#include <config.h>
#include <cstdlib>
#include <iostream>

struct Parameters;
struct Link;
struct Step;


/* ====================================================================== */
/* Reason

/*
 * Abstract reason.
 */
struct Reason {
  /* A dummy reason. */
  static const Reason& DUMMY;

  /* Register use of this object. */
  static void register_use(const Reason* r) {
    if (r != NULL) {
      r->ref_count_++;
    }
  }

  /* Unregister use of this object. */
  static void unregister_use(const Reason* r) {
    if (r != NULL) {
      r->ref_count_--;
      if (r->ref_count_ == 0) {
	delete r;
      }
    }
  }

  /* Deletes this reason. */
  virtual ~Reason();

  /* Checks if this reason is a dummy reason. */
  bool dummy() const;

  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const;

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const;

protected:
  /* Creates a reason. */
  Reason();

  /* Prints this reason on the given stream. */
  virtual void print(std::ostream& os) const = 0;

private:
  /* Reference counter. */
  mutable size_t ref_count_;

  friend std::ostream& operator<<(std::ostream& os, const Reason& r);
};

/* Output operator for reasons. */
std::ostream& operator<<(std::ostream& os, const Reason& r);


/* ====================================================================== */
/* InitReason */

/*
 * Reason attached to elements of the initial plan.
 */
struct InitReason : public Reason {
  /* Returns an Init reason (or a dummy reason if reasons are not
     needed. */
  static const Reason& make(const Parameters& params);

protected:
  /* Prints this reason on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs an Init reason. */
  InitReason();
};


/* ====================================================================== */
/* AddStepReason */

/*
 * Reason attached to elements added along with a step.
 */
struct AddStepReason : public Reason {
  /* Returns an AddStep reason (or a dummy reason if reasons are not
     needed. */
  static const Reason& make(const Parameters& params, size_t step_id);

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const;

protected:
  /* Prints this reason on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Id of added step. */
  const size_t step_id_;

  /* Constructs an AddStep reason. */
  AddStepReason(size_t step_id);
};


/* ====================================================================== */
/* EstablishReason */

/*
 * Reason attached to elements needed to establish a link.
 */
struct EstablishReason : public Reason {
  /* Returns an Establish reason (or a dummy reason if reasons are not
     needed. */
  static const Reason& make(const Parameters& params, const Link& link);

  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const;

protected:
  /* Prints this reason on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Established link */
  const Link* link_;

  /* Constructs an Establish reason. */
  EstablishReason(const Link& link);
};


/* ====================================================================== */
/* ProtectReason */

/*
 * Reason attached to elements needed to protect a link.
 */
struct ProtectReason : public Reason {
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
  virtual void print(std::ostream& os) const;

private:
  /* Protected link. */
  const Link* link_;
  /* Id of threatening step. */
  const size_t step_id_;

  /* Constructs a Protect reason. */
  ProtectReason(const Link& link, size_t step_id);
};


#endif /* REASONS_H */
