;
;;	Apartmentmovers Domain
;
(define (problem apartmentmovers1_1) (:domain apartmentmovers)
;
(:objects
	a1 a2 a3 a4 - agent
	loc1 loc2 loc3 loc4 - location
	table1 sofa1 bed1 table2 sofa2 bed2 - furniture
	TV1 fridge1 AC1 TV2 fridge2 AC2 - electronics 
	tr1 tr2 - truck 
	carton1 - carton
	carton2 - carton		
)
;
(:init
	(agent-at a1 loc1)
	(agent-at a2 loc1)
	(agent-at a3 loc1)
	(agent-at a4 loc1)
	(can-walk a1)
	(can-walk a2)
	(can-walk a3)
	(can-walk a4)
	
	(truck-at tr1 loc1)
	(truck-at tr2 loc1)		
	
	(furniture-at sofa1 loc1)
	(furniture-at table1 loc1)
	(furniture-at bed1 loc1)	
	(electronics-at TV1 loc1)
	(electronics-at fridge1 loc1)
	(electronics-at AC1 loc1)
	
	(furniture-at sofa2 loc3)
	(furniture-at table2 loc3)
	(furniture-at bed2 loc3)	
	(electronics-at TV2 loc3)
	(electronics-at fridge2 loc3)
	(electronics-at AC2 loc3)
	
	
	(connected loc1 loc3 )
	(connected loc3 loc1 )
	(connected loc3 loc2 )
	(connected loc2 loc3 )
	(connected loc2 loc4 )
	(connected loc4 loc2 )
	
	(carton-at carton1 loc1)	
	(carton-at carton2 loc3)
)
;
(:goal 
	(and
		(furniture-at sofa1 loc4)
		(furniture-at table1 loc4)
		(furniture-at bed1 loc4)	
		(electronics-at TV1 loc4)
		(electronics-at fridge1 loc4)
		(electronics-at AC1 loc4)
	
		(furniture-at sofa2 loc4)
		(furniture-at table2 loc4)
		(furniture-at bed2 loc4)	
		(electronics-at TV2 loc4)
		(electronics-at fridge2 loc4)
		(electronics-at AC2 loc4)		
 	)
)
;
)
