#!/bin/sh
#
# The link-simple domain by Veloso & Blythe.
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

echo "(define (domain link-simple-$n)"
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
    echo "           :effect (g$i))"
    i=`expr $i + 1`
done
echo "  (:action a"
echo "           :parameters ()"
echo -n "           :effect "
if (test $n -gt 0) then
    echo -n "(and (g)"
    i=1
    while (test $i -le $n) do
	echo -n " (not (g$i))"
	i=`expr $i + 1`
    done
    echo -n ")"
else
    echo -n "(g)"
fi
echo "))"
echo


#
# Generate permutation for goal literals and initial state.
#

# Seed.
if (test ! -z "$3") then
    echo ";Random seed is $3."
    RANDOM=$3
fi

i=1
while (test $i -le $k) do
    r=$RANDOM
    ipos=`expr $r % $i`
    j=`expr $i - 1`
    while (test $j -ge $ipos) do
	iorder[`expr $j + 1`]=${iorder[$j]}
	j=`expr $j - 1`
    done
    iorder[$ipos]=$i
    r=$RANDOM
    gpos=`expr $r % $i`
    j=`expr $i - 1`
    while (test $j -ge $gpos) do
	gorder[`expr $j + 1`]=${gorder[$j]}
	j=`expr $j - 1`
    done
    gorder[$gpos]=$i
    i=`expr $i + 1`
done
r=$RANDOM
gpos=`expr $r % $i`
j=`expr $i - 1`
while (test $j -ge $gpos) do
    gorder[`expr $j + 1`]=${gorder[$j]}
    j=`expr $j - 1`
done
gorder[$gpos]=""


#
# Output problem description.
#

echo "(define (problem link-simple-$n-$k)"
echo "  (:domain link-simple-$n)"
if (test $k -gt 0) then
    echo -n "  (:init"
    i=0
    while (test $i -lt $k) do
	echo -n " (g${iorder[$i]})"
	i=`expr $i + 1`
    done
    echo ")"
fi
echo -n "  (:goal "
if (test $k -gt 0) then
    echo -n "(and"
    i=0
    while (test $i -le $k) do
	echo -n " (g${gorder[$i]})"
	i=`expr $i + 1`
    done
    echo -n ")"
else
    echo -n "(g)"
fi
echo "))"
