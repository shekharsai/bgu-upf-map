;
;;	Apartmentmovers Domain
;
(define (problem apartmentmovers1_1) (:domain apartmentmovers)
;
(:objects
	a1 a2 a3 - agent
	loc1 loc2 - location
	table sofa bed - furniture
	TV fridge AC - electronics
	tr1 tr2 - truck
	carton1 - carton	
)
;
(:init
	(agent-at a1 loc1)
	(agent-at a2 loc1)
	(agent-at a3 loc1)
	(can-walk a1)
	(can-walk a2)
	(can-walk a3)	
	(truck-at tr1 loc1)
	(truck-at tr2 loc1)			
	(furniture-at sofa loc1)
	(furniture-at table loc1)
	(furniture-at bed loc1)	
	(electronics-at TV loc1)
	(electronics-at fridge loc1)
	(electronics-at AC loc1)	
	(connected loc1 loc2 )
	(connected loc2 loc1 )	
	(carton-at carton1 loc1)	
)
;
(:goal 
	(and
		(electronics-at TV loc2)
		(electronics-at fridge loc2)
		(electronics-at AC loc2)
		(furniture-at sofa loc2)
		(furniture-at bed loc2)
		(furniture-at table loc2)		
 	)
)
;
)
