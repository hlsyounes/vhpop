/* -*-C++-*- */
/*
 * Reasons for refinements in a plan.
 *
 * $Id: reasons.h,v 1.1 2001-12-28 19:58:35 lorens Exp $
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
struct Reason : public Printable, public gc {
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
