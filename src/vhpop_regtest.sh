#!/bin/bash
#
# Copyright (C) 2019 Google Inc
#
# This file is part of VHPOP.
#
# VHPOP is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# VHPOP is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with VHPOP; if not, write to the Free Software Foundation,
# Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

set -o pipefail

pass=1

readonly VHPOP=${VHPOP:-./vhpop}

function timestamp() {
  if [[ "$(date +%N)" = N ]]; then
    expr 1000 '*' $(date +%s)
  else
    expr 1000 '*' $(date +%s) + $(date +%N) / 1000000
  fi
}

function expect_ok() {
  if [[ $? = 0 ]]; then
    echo pass "[$(expr $(timestamp) - ${1})ms]"
  else
    echo fail
    pass=0
  fi
}

echo -n sussman_anomaly_lifted...
start=$(timestamp)
HEAPCHECK=normal VHPOP_FREE_ALL_MEMORY= ${VHPOP} examples/blocks-world-domain.pddl examples/sussman-anomaly.pddl 2>/dev/null | grep -v '^Time: ' | diff src/testdata/sussman_anomaly_lifted.golden -
expect_ok ${start}

echo -n sussman_anomaly_ground...
start=$(timestamp)
HEAPCHECK=normal VHPOP_FREE_ALL_MEMORY= ${VHPOP} examples/blocks-world-domain.pddl examples/sussman-anomaly.pddl 2>/dev/null | grep -v '^Time: ' | diff src/testdata/sussman_anomaly_ground.golden -
expect_ok ${start}
