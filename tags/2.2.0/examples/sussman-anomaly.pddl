(define (problem sussman-anomaly)
  (:domain blocks-world-domain)
  (:objects a b c)
  (:init (block a) (block b) (block c) (block table)
	 (on c a) (on a table) (on b table)
	 (clear c) (clear b) (clear table))
  (:goal (and (on b c) (on a b))))
