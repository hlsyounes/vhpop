#!/bin/sh

# Number of blocks.
n=$1
if (test -z "$1") then
    echo Number of blocks not specified.
    exit 1
fi


#
# Output domain description.
#

echo "(define (domain blocks)"
echo "  (:predicates (on ?x ?y) (ontable ?x) (clear ?x) (handempty) (holding ?x))"
echo "  (:action pick-up"
echo "           :parameters (?x)"
echo "           :precondition (and (clear ?x) (ontable ?x) (handempty))"
echo "           :effect (and (not (ontable ?x)) (not (clear ?x)) (not (handempty))"
echo "                        (holding ?x)))"
echo "  (:action put-down"
echo "           :parameters (?x)"
echo "           :precondition (holding ?x)"
echo "           :effect (and (not (holding ?x)) (clear ?x) (handempty) (ontable ?x)))"
echo "  (:action stack"
echo "           :parameters (?x ?y)"
echo "           :precondition (and (holding ?x) (clear ?y))"
echo "           :effect (and (not (holding ?x)) (not (clear ?y)) (clear ?x)"
echo "                        (handempty) (on ?x ?y)))"
echo "  (:action unstack"
echo "           :parameters (?x ?y)"
echo "           :precondition (and (on ?x ?y) (clear ?x) (handempty))"
echo "           :effect (and (holding ?x) (clear ?y) (not (clear ?x))"
echo "                        (not (handempty)) (not (on ?x ?y)))))"


#
# Output problem description.
#

echo
echo "(define (problem blocks-$n)"
echo "  (:domain blocks)"
echo -n "  (:objects"
i=1
while (test $i -le $n) do
    echo -n " b$i"
    i=`expr $i + 1`
done
echo ")"
echo -n "  (:init"
i=1
while (test $i -le $n) do
    if (test $i -gt 1) then
	echo -n "        "
    fi
    echo " (clear b$i) (ontable b$i)"
    i=`expr $i + 1`
done
echo "         (handempty))"
if (test $n -gt 2) then
    echo -n "  (:goal (and"
    i=1
    j=2
    while (test $i -lt $n) do
	if (test $i -gt 1) then
	    echo
	    echo -n "             "
	fi
	echo -n " (on b$j b$i)"
	i=`expr $i + 1`
	j=`expr $j + 1`
    done
    echo "))"
else
    echo -n "  (:goal (on b2 b1))"
fi
echo ")"
