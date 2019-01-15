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
// Reference counters.

#ifndef REFCOUNT_H_
#define REFCOUNT_H_

// An object with a reference counter.
class RCObject {
 public:
  // Increases the reference count for the given object.
  static void ref(const RCObject* o) {
    if (o != 0) {
      o->ref_count_++;
    }
  }

  // Decreases the reference count for the given object.
  static void deref(const RCObject* o) {
    if (o != 0) {
      o->ref_count_--;
    }
  }

  // Decreases the reference count for the given object and deletes it if the
  // reference count becomes zero.
  static void destructive_deref(const RCObject* o) {
    if (o != 0) {
      o->ref_count_--;
      if (o->ref_count_ == 0) {
        delete o;
      }
    }
  }

  // Deletes this object.
  virtual ~RCObject() {}

 protected:
  // Constructs an object with a reference counter.
  RCObject() : ref_count_(0) {}

  // Copy constructor.
  RCObject(const RCObject& o) : ref_count_(0) {}

 private:
  // Reference counter.
  mutable unsigned long ref_count_;
};

#endif  // REFCOUNT_H_
