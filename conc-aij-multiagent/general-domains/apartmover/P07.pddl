;
;;	Apartmentmovers Domain
;
(define (problem apartmentmovers1_1) 
(:domain apartmentmovers)
(:objects
	a1 a2 a3 a4 a5 a6 a7 - agent
	loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
	table1 sofa1 bed1 table2 sofa2 bed2 table3 sofa3 bed3 - furniture
	TV1 fridge1 AC1 TV2 fridge2 AC2 TV3 fridge3 AC3 - electronics
	tr1 tr2 tr3 tr4 - truck
	carton1 - carton
	carton2 - carton		
	carton3 - carton
)
(:init
	(agent-at a1 loc1)
	(agent-at a2 loc1)
	(agent-at a3 loc1)
	(agent-at a4 loc1)
	(agent-at a5 loc6)
	(agent-at a6 loc6)
	(agent-at a7 loc7)
	(can-walk a1)
	(can-walk a2)
	(can-walk a3)
	(can-walk a4)
	(can-walk a5)
	(can-walk a6)
	(can-walk a7)
	(truck-at tr1 loc1)
	(truck-at tr2 loc5)
	(truck-at tr3 loc7)				
	(truck-at tr4 loc4)
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
	(furniture-at sofa3 loc2)
	(furniture-at table3 loc2)
	(furniture-at bed3 loc2)
	(electronics-at TV3 loc2)
	(electronics-at fridge3 loc2)
	(electronics-at AC3 loc2)	
	(connected loc1 loc3 )
	(connected loc3 loc1 )
	(connected loc3 loc2 )
	(connected loc2 loc3 )
	(connected loc2 loc4 )
	(connected loc4 loc2 )
	(connected loc4 loc5 )
	(connected loc5 loc4 )
	(connected loc7 loc3 )
	(connected loc3 loc7 )
	(connected loc7 loc6 )
	(connected loc6 loc7 )
	(carton-at carton1 loc1)	
	(carton-at carton2 loc3)
	(carton-at carton3 loc2)
	(same-agent a1 a1)
	(same-agent a2 a2)
	(same-agent a3 a3)
	(same-agent a4 a4)
	(same-agent a5 a5)
	(same-agent a6 a6)
)
(:goal 
	(and
		(furniture-at sofa1 loc4)
		(furniture-at table1 loc4)
		(furniture-at bed1 loc4)	
		(electronics-at TV1 loc4)
		(electronics-at fridge1 loc4)
		(electronics-at AC1 loc4)
		(furniture-at sofa2 loc7)
		(furniture-at table2 loc7)
		(furniture-at bed2 loc7)	
		(electronics-at TV2 loc7)
		(electronics-at fridge2 loc7)
		(electronics-at AC2 loc7)
		(furniture-at sofa3 loc5)
		(furniture-at table3 loc5)
		(furniture-at bed3 loc5)
		(electronics-at TV3 loc5)
		(electronics-at fridge3 loc5)
		(electronics-at AC3 loc5)		
 	)
)
)