#!/bin/sh
#
# The one operator tower of Hanoi domain from the UCPOP distribution.
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

echo "(define (domain hanoi-domain)"
echo "  (:requirements :equality)"
echo "  (:predicates (disk ?x) (smaller ?x ?y) (on ?x ?y) (clear ?x))"
echo "  (:action move-disk"
echo "           :parameters (?disk ?below-disk ?new-below-disk)"
echo "           :precondition (and (disk ?disk)"
echo "                              (smaller ?disk ?new-below-disk)"
echo "                              (not (= ?new-below-disk ?below-disk))"
echo "                              (not (= ?new-below-disk ?disk))"
echo "                              (not (= ?below-disk ?disk))"
echo "                              (on ?disk ?below-disk)"
echo "                              (clear ?disk)"
echo "                              (clear ?new-below-disk))"
echo "           :effect (and (clear ?below-disk)"
echo "                        (on ?disk ?new-below-disk)"
echo "                        (not (on ?disk ?below-disk))"
echo "                        (not (clear ?new-below-disk)))))"
echo


#
# Output problem description.
#

echo "(define (problem hanoi-$n)"
echo "  (:domain hanoi-domain)"
echo -n "  (:objects p1 p2 p3"
i=1
while (test $i -le $n) do
    echo -n " d$i"
    i=`expr $i + 1`
done
echo ")"
echo -n "  (:init"
i=1
while (test $i -le 3) do
    j=1
    while (test $j -le $n) do
	echo -n " (smaller d$j p$i)"
	j=`expr $j + 1`
    done
    i=`expr $i + 1`
done
i=1
while (test $i -lt $n) do
    j=`expr $i + 1`
    while (test $j -le $n) do
	echo -n " (smaller d$i d$j)"
	j=`expr $j + 1`
    done
    i=`expr $i + 1`
done
echo -n " (clear p1) (clear p2) (clear d1)"
i=1
while (test $i -le $n) do
    echo -n " (disk d$i)"
    i=`expr $i + 1`
done
i=1;
while (test $i -lt $n) do
    echo -n " (on d$i d`expr $i + 1`)"
    i=`expr $i + 1`
done
echo " (on d$n p3))"
echo -n "  (:goal (and"
i=1;
while (test $i -lt $n) do
    echo -n " (on d$i d`expr $i + 1`)"
    i=`expr $i + 1`
done
echo " (on d$n p1))))"
