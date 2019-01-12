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

#include "refcount.h"

#ifdef DEBUG_MEMORY
std::map<std::string, size_t> RCObject::creation_count;
std::map<std::string, size_t> RCObject::deletion_count;

void RCObject::print_statistics(std::ostream& os) {
  for (std::map<std::string, size_t>::const_iterator ti =
           creation_count.begin();
       ti != creation_count.end(); ti++) {
    os << (*ti).first << ": " << (*ti).second << '/'
       << deletion_count[(*ti).first] << std::endl;
  }
}
#endif
