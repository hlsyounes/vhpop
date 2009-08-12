(define (problem log-a) 
  (:domain logistics-strips)
  (:objects package1 package2 package3 package4 package5 package6 package7
	    package8 pgh-truck bos-truck la-truck airplane1 airplane2
	    bos-po la-po pgh-po bos-airport la-airport pgh-airport
	    pgh bos la)
  (:init (obj package1)
	 (obj package2)
	 (obj package3)
	 (obj package4)
	 (obj package5)
	 (obj package6)
	 (obj package7)
	 (obj package8)
	 (truck pgh-truck)
	 (truck bos-truck)
	 (truck la-truck)
	 (airplane airplane1)
	 (airplane airplane2)
	 (location bos-po)
	 (location la-po)
	 (location pgh-po)
	 (location bos-airport)
	 (location la-airport)
	 (location pgh-airport)
	 (airport bos-airport)
	 (airport pgh-airport)
	 (airport la-airport)
	 (city pgh)
	 (city bos)
	 (city la)
	 (in-city pgh-po pgh)
	 (in-city pgh-airport pgh)
	 (in-city bos-po bos)
	 (in-city bos-airport bos)
	 (in-city la-po la)
	 (in-city la-airport la)
	 (at package1 pgh-po)
	 (at package2 pgh-po)
	 (at package3 pgh-po)
	 (at package4 pgh-po)
	 (at package5 bos-po)
	 (at package6 bos-po)
	 (at package7 bos-po)
	 (at package8 la-po)
	 (at airplane1 pgh-airport)
	 (at airplane2 pgh-airport)
	 (at bos-truck bos-po)
	 (at pgh-truck pgh-po)
	 (at la-truck la-po))
  (:goal (and (at package1 bos-po)
	      (at package2 bos-airport)
	      (at package3 la-po)
	      (at package4 la-airport)
	      (at package5 pgh-po)
	      (at package6 pgh-airport)
	      (at package7 pgh-po)
	      (at package8 pgh-po))))
