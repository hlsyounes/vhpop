/* -*-C++-*- */
/*
 * Template chain class.
 *
 * $Id: chain.h,v 1.3 2001-12-27 19:59:49 lorens Exp $
 */
#ifndef CHAIN_H
#define CHAIN_H


/*
 * Template chain class.
 */
template<typename T>
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
      Chain<T>* prev = new Chain<T>(head, NULL);
      const Chain<T>* top = prev;
      for (const Chain<T>* ci = tail; ci != NULL; ci = ci->tail) {
	if (h == ci->head) {
	  prev->tail = ci->tail;
	  break;
	} else {
	  Chain<T>* tmp = new Chain<T>(ci->head, NULL);
	  prev->tail = tmp;
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
