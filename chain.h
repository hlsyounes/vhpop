/* -*-C++-*- */
/*
 * Template chain class.
 *
 * $Id: chain.h,v 1.1 2001-09-04 18:18:16 lorens Exp $
 */
#ifndef CHAIN_H
#define CHAIN_H


/*
 * Template chain class.
 */
template<class T>
struct Chain : public gc {
  T head;
  const Chain<T>* tail;

  /* Constructs a chain with the given head and tail. */
  Chain<T>(const T& h, const Chain<T>* t)
    : head(h), tail(t) {
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
      return new Chain<T>(head, tail->remove(h));
    } else {
      return this;
    }
  }
};
