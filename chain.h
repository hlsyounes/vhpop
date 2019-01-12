// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
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
// Template chain class.

#ifndef CHAIN_H_
#define CHAIN_H_

#include "refcount.h"

// Template chain class.
template <typename T>
class Chain : public RCObject {
 public:
  // The data at this location in the chain.
  T head;
  // The rest of the chain.
  const Chain<T>* tail;

  // Constructs a chain with the given head and tail.
  Chain<T>(const T& head, const Chain<T>* tail) : head(head), tail(tail) {
    ref(tail);
  }

  // Deletes this chain.
  ~Chain<T>() { destructive_deref(tail); }

  // Returns the size of this chain.
  int size() const {
    int result = 0;
    for (const Chain<T>* ci = this; ci != 0; ci = ci->tail) {
      result++;
    }
    return result;
  }

  // Checks if this chain contains the given element.
  bool contains(const T& h) const {
    for (const Chain<T>* ci = this; ci != 0; ci = ci->tail) {
      if (h == ci->head) {
        return true;
      }
    }
    return false;
  }

  // Returns a chain with the first occurance of the given element removed.
  const Chain<T>* remove(const T& h) const {
    if (h == head) {
      return tail;
    } else if (tail != 0) {
      Chain<T>* prev = new Chain<T>(head, 0);
      const Chain<T>* top = prev;
      for (const Chain<T>* ci = tail; ci != 0; ci = ci->tail) {
        if (h == ci->head) {
          prev->tail = ci->tail;
          ref(ci->tail);
          break;
        } else {
          Chain<T>* tmp = new Chain<T>(ci->head, 0);
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

#endif  // CHAIN_H_
