/* -*-C++-*- */
/*
 * Hashing.
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
 * $Id: hashing.h,v 1.1 2003-03-01 18:51:26 lorens Exp $
 */
#ifndef HASHING_H
#define HASHING_H

#if !defined __GNUC__ || __GNUC__ < 3
# include <hash_map>
# include <hash_set>
namespace hashing {
  using ::hash_map;
  using ::hash_multimap;
  using ::hash_set;
  using ::hash;
}
#else
# include <ext/hash_map>
# include <ext/hash_set>
# if __GNUC_MINOR__ == 0
namespace hashing = std;
# else
namespace hashing = ::__gnu_cxx;
# endif
#endif


/*
 * Generic hash function object for pointers.
 */
#if defined __GNUC__ && __GNUC__ >= 3
# if __GNUC_MINOR__ == 0
namespace std
# else
namespace __gnu_cxx
# endif
{
#endif
  template<typename T> struct hash<const T*> {
    size_t operator()(const T* p) const {
      return size_t(p);
    }
  };
#if defined __GNUC__ && __GNUC__ >= 3
}
#endif


#endif /* HASHING_H */
