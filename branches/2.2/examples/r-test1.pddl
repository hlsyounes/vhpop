(define (problem r-test1)
  (:domain robot-domain)
  (:objects rm1 rm2 box1 box2)
  (:init (location rm1)
	  (location rm2)
	  (object box1) (object box2) (object robot)
	  (connected rm1 rm2)
	  (connected rm2 rm1)
	  (at box1 rm2) (at box2 rm2)
	  (empty-handed)
	  (at robot rm1))
  (:goal (at box1 rm1)))
