#!/bin/sh
#
# The n operator tower of Hanoi domain by Gerevini & Schubert.
#

# Number of disks.
n=$1
if (test -z "$n") then
    echo Number of disks not specified.
    exit 1
fi
if (test $n -lt 1) then
    echo There needs to be at least one disk.
    exit 1
fi


#
# Output domain description.
#

echo "(define (domain hanoi-$n-domain)"
echo "  (:requirements :equality :disjunctive-preconditions)"
echo -n "  (:constants"
i=1
while (test $i -le $n) do
    echo -n " d$i"
    i=`expr $i + 1`
done
echo ")"
echo "  (:predicates (thing ?x) (on ?x ?y) (clear ?x))"
i=1
while (test $i -le $n) do
    echo
    echo "  (:action move-d$i"
    echo "           :parameters (?from ?to)"
    echo -n "           :precondition (and (thing ?from) (thing ?to) (on d$i ?from) (clear ?to) (not (on d$i ?to))"
    j=1
    while (test $j -lt $i) do
	echo -n " (not (on d$j d$i))"
	j=`expr $j + 1`
    done
    j=1
    while (test $j -le $i) do
	echo -n " (not (= ?to d$j))"
	j=`expr $j + 1`
    done
    echo ")"
    echo "           :effect (and (on d$i ?to) (not (clear ?to)) (clear ?from) (not (on d$i ?from))))"
    i=`expr $i + 1`
done
echo ")"
echo


#
# Output problem description.
#

echo "(define (problem hanoi-$n)"
echo "  (:domain hanoi-$n-domain)"
echo "  (:objects p1 p2 p3)"
echo -n "  (:init"
i=1
while (test $i -le $n) do
    echo -n " (thing d$i)"
    i=`expr $i + 1`
done
i=1
echo -n " (thing p1) (thing p2) (thing p3)"
i=1;
while (test $i -lt $n) do
    echo -n " (on d$i d`expr $i + 1`)"
    i=`expr $i + 1`
done
echo " (on d$n p3) (clear d1) (clear p1) (clear p2))"
echo -n "  (:goal (and"
i=1;
while (test $i -lt $n) do
    echo -n " (on d$i d`expr $i + 1`)"
    i=`expr $i + 1`
done
echo " (on d$n p1))))"
