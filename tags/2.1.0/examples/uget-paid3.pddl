(define (problem uget-paid3)
  (:domain uni-bw)
  (:objects home office bank p d)
  (:init (place home) (place office) (place bank)
	 (object p) (object d) (object b)
	 (at b home) (at p home) (at d home) (in p))
  (:goal (and (at p bank) (at d office) (at b home))))
