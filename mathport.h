/* -*-C++-*- */
/*
 * Math library portability support.
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
 * $$
 */
#ifndef MATHCOMP_H
#define MATHPORT_H

#include <config.h>
#define __USE_ISOC99 1
#define __USE_ISOC9X 1
#include <cmath>


#ifndef INFINITY
#define INFINITY (float(HUGE_VAL))
#endif

#if !HAVE_ISINF
#if HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#define isinf(x) (!(isnan(x) || finite(x)))
#endif

#endif /* MATHCOMP_H */
