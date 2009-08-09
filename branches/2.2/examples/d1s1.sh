#!/bin/sh
#
# The D^1S^1 domain by Barrett & Weld.
#

# Number of operators.
n=$1
if (test -z "$n") then
    echo Number of operators not specified.
    exit 1
fi
if (test $n -lt 1) then
    echo There needs to be at least one operator.
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
if (test $k -lt 1) then
    echo There needs to be at least one goal.
    exit 1
fi


#
# Output domain description.
#

echo "(define (domain d1s1-$n)"
echo -n "  (:predicates"
i=1
while (test $i -le $n) do
    echo -n " (i$i) (g$i)"
    i=`expr $i + 1`
done
echo -n ")"
i=1
while (test $i -le $n) do
    echo
    echo "  (:action a$i"
    echo "           :parameters ()"
    echo "           :precondition (i$i)"
    echo -n "           :effect "
    if (test $i -gt 1) then
	echo -n "(and (g$i) (not (i`expr $i - 1`)))"
    else
	echo -n "(g$i)"
    fi
    echo -n ")"
    i=`expr $i + 1`
done
echo ")"
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
while (test $i -le $n) do
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


#
# Output problem description.
#

echo "(define (problem d1s1-$n-$k)"
echo "  (:domain d1s1-$n)"
echo -n "  (:init"
i=0
while (test $i -lt $n) do
    echo -n " (i${iorder[$i]})"
    i=`expr $i + 1`
done
echo ")"
echo -n "  (:goal "
if (test $k -gt 1) then
    echo -n "(and"
    i=0
    while (test $i -lt $k) do
	echo -n " (g${gorder[$i]})"
	i=`expr $i + 1`
    done
    echo -n ")"
else
    echo -n "(g${gorder[0]})"
fi
echo "))"
