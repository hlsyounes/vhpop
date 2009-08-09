#!/bin/sh
#
# The link-repeat domain by Veloso & Blythe.
# Used in (Veloso & Blythe 1994).
#

# Number of operators.
n=$1
if (test -z "$n") then
    echo Number of operators not specified.
    exit 1
fi

# Number of goals.
k=$2
if (test -z "$k") then
    echo Number of goals not specified.
    exit 1
fi
if (test $k -gt $n) then
    echo More goals than operators.
    exit 1
fi


#
# Output domain description.
#

echo "(define (domain link-repeat-$n)"
echo -n "  (:predicates (g)"
i=1
while (test $i -le $n) do
    echo -n " (g$i)"
    i=`expr $i + 1`
done
echo ")"
i=1
while (test $i -le $n) do
    echo "  (:action a$i"
    echo "           :parameters ()"
    echo -n "           :precondition "
    if (test $i -gt 1) then
	echo "(and (g) (g`expr $i - 1`))"
    else
	echo "(g)"
    fi
    echo "           :effect (and (g$i) (not (g))))"
    i=`expr $i + 1`
done
echo "  (:action a"
echo "           :parameters ()"
echo "           :effect (g)))"
echo


#
# Output problem description.
#

echo "(define (problem link-repeat-$n-$k)"
echo "  (:domain link-repeat-$n)"
echo "  (:init (g))"
if (test $k -gt 0) then
    echo "  (:goal (and (g) (g$k))))"
else
    echo "  (:goal (g))"
fi
