/* -*-C++-*- */
/*
 * Template chain class.
 *
 * Copyright (C) 2002-2004 Carnegie Mellon University
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
 * $Id: chain.h,v 6.1 2003-07-13 16:50:24 lorens Exp $
 */
#ifndef CHAIN_H
#define CHAIN_H

#include <config.h>
#include <refcount.h>


/* ====================================================================== */
/* Chain */

/*
 * Template chain class.
 */
template<typename T>
struct Chain : RCObject {
  /* The data at this location in the chain. */
  T head;
  /* The rest of the chain. */
  const Chain<T>* tail;

  /* Constructs a chain with the given head and tail. */
  Chain<T>(const T& head, const Chain<T>* tail)
    : head(head), tail(tail) {
    ref(tail);
  }

  /* Deletes this chain. */
  ~Chain<T>() {
    destructive_deref(tail);
  }

  /* Returns the size of this chain. */
  size_t size() const {
    size_t result = 0;
    for (const Chain<T>* ci = this; ci != NULL; ci = ci->tail) {
      result++;
    }
    return result;
  }

  /* Checks if this chain contains the given element. */
  bool contains(const T& h) const {
    for (const Chain<T>* ci = this; ci != NULL; ci = ci->tail) {
      if (h == ci->head) {
	return true;
      }
    }
    return false;
  }

  /* Returns a chain with the first occurance of the given element removed. */
  const Chain<T>* remove(const T& h) const {
    if (h == head) {
      return tail;
    } else if (tail != NULL) {
      Chain<T>* prev = new Chain<T>(head, NULL);
      const Chain<T>* top = prev;
      for (const Chain<T>* ci = tail; ci != NULL; ci = ci->tail) {
	if (h == ci->head) {
	  prev->tail = ci->tail;
	  ref(ci->tail);
	  break;
	} else {
	  Chain<T>* tmp = new Chain<T>(ci->head, NULL);
	  prev->tail = tmp;
	  ref(tmp);
	  prev = tmp;
	}
      }
      return top;
    } else {
      return this;
    }
  }
};


#endif /* CHAIN_H */
