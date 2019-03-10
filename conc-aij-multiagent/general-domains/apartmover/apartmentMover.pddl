;
;;	Apartmentmover domain
;
(define (domain apartmentmovers)
;
(:requirements :typing :conditional-effects :multi-agent :concurrency-network)
;
(:types	
	agent 
	furniture
	electronics		
	carton
	truck	
	location 
)
;
(:predicates
	(agent-at ?agent - agent ?loc - location)
	(furniture-at ?furniture - furniture ?loc - location)	
	(electronics-at ?electronics - electronics ?loc - location)
	(electronics-in ?electronics - electronics ?cb - carton)		
	(carton-at ?carton - carton ?loc - location)
	(truck-at ?truck - truck ?loc - location)	
	(connected ?loc1 - location ?loc2 - location)
	(loaded-furniture ?furniture - furniture ?tr - truck)
	(loaded-carton ?carton - carton ?tr - truck)
	(inside ?e - electronics ?bigcarton - carton)	
	(packed ?cb - carton)
	;- private -;
	(tired ?a - agent)
	(can-walk ?a - agent)	
)
;
(:action consume
	:agent ?a - agent 
	:parameters ()
	:precondition (and
					(tired ?a)
				  )
	:effect	(and
				(not (tired ?a))
			)
)
;
(:action move-agent
	:agent ?a - agent 
	:parameters (?r1 - location ?r2 - location)
	:precondition (and
					(agent-at ?a ?r1)
					(connected ?r1 ?r2)
					(can-walk ?a)
				) 
	:effect	(and
					(agent-at ?a ?r2)
					(not (agent-at ?a ?r1))
				 )
)
;
(:action load-furniture 
	:agent ?a - agent
	:parameters (?fur - furniture ?tr - truck ?loc - location)
	:precondition (and
					(agent-at ?a ?loc)
					(furniture-at ?fur ?loc)
					(truck-at ?tr ?loc)
					(not (tired ?a))
				  )
	:effect	(and
				(tired ?a)
			)
)
;
(:action 2-load-furniture 
	:agent ?a - agent
	:parameters (?a1 - agent ?fur - furniture ?tr - truck ?loc - location)
	:precondition (and
					(agent-at ?a ?loc)
					(furniture-at ?fur ?loc)
					(truck-at ?tr ?loc)
					(not (tired ?a))	
				  )
	:effect	(and
				(loaded-furniture ?fur ?tr)	
				(not (furniture-at ?fur ?loc)) 			
			 )
)
;
(:action unload-furniture 
	:agent ?a - agent
	:parameters (?fur - furniture ?tr - truck ?loc1 - location)
	:precondition (and
					(loaded-furniture ?fur ?tr)
					(agent-at ?a ?loc1)
					(truck-at ?tr ?loc1)
					(not (tired ?a))	
				  )
	:effect	(and
				(tired ?a)
			)
)
;
(:action 2-unload-furniture 
	:agent ?a - agent
	:parameters (?a1 - agent ?fur - furniture ?tr - truck ?loc1 - location)
	:precondition (and
					(loaded-furniture ?fur ?tr)
					(agent-at ?a ?loc1)
					(truck-at ?tr ?loc1)
					(not (tired ?a))	
				  )
	:effect	(and
				(furniture-at ?fur ?loc1)
				(not (loaded-furniture ?fur ?tr))
			 )
)
;
(:action 3-unload-furniture 
	:agent ?a - agent
	:parameters (?a1 - agent ?a2 - agent ?fur - furniture ?tr - truck ?loc1 - location)
	:precondition (and
					(loaded-furniture ?fur ?tr)
					(agent-at ?a ?loc1)
					(truck-at ?tr ?loc1)
					(not (tired ?a))	
				  )
	:effect	(and
				(furniture-at ?fur ?loc1)
				(not (loaded-furniture ?fur ?tr))
			 )
)
;
(:action drive-truck
	:agent ?a - agent
	:parameters (?t - truck ?r1 - location ?r2 - location)
	:precondition (and
					(not (tired ?a))	
					(agent-at ?a ?r1)
					(truck-at ?t ?r1)
					(connected ?r1 ?r2)					
				  )
	:effect	(and
				(not (truck-at ?t ?r1))
				(truck-at ?t ?r2)					
				(not (agent-at ?a ?r1))										
				(agent-at ?a ?r2)	
				(tired ?a)				
			)
)
;
(:action load-carton
	:agent ?a - agent 
	:parameters (?cb - carton ?t - truck ?r1 - location)
	:precondition (and
					(carton-at ?cb ?r1)
					(packed ?cb) 
					(agent-at ?a ?r1)
					(truck-at ?t ?r1)
					(not (tired ?a))							
				  )
	:effect	(and
					(loaded-carton ?cb ?t)
					(not (carton-at ?cb ?r1))
					(tired ?a)
				 )
)
;
(:action 2-load-carton
	:agent ?a - agent 
	:parameters (?a0 - agent ?cb - carton ?t - truck ?r1 - location)
	:precondition (and
					(carton-at ?cb ?r1)
					(packed ?cb) 
					(agent-at ?a ?r1)
					(truck-at ?t ?r1)
					(not (tired ?a))							
				  )
	:effect	(and
					(loaded-carton ?cb ?t)
					(not (carton-at ?cb ?r1))
				 )
)
;
(:action unload-carton
	:agent ?a - agent 
	:parameters (?cb - carton ?t - truck ?r1 - location)
	:precondition (and
					(loaded-carton ?cb ?t)
					(truck-at ?t ?r1)
					(agent-at ?a ?r1)
					(not (tired ?a))							
				  )
	:effect	(and
					(not (loaded-carton ?cb ?t))					
					(carton-at ?cb ?r1)
					(tired ?a)
				 )
)
;
(:action 2-unload-carton
	:agent ?a - agent 
	:parameters (?a0 - agent ?cb - carton ?t - truck ?r1 - location)
	:precondition (and
					(loaded-carton ?cb ?t)
					(truck-at ?t ?r1)
					(agent-at ?a ?r1)
					(not (tired ?a))							
				  )
	:effect	(and
					(not (loaded-carton ?cb ?t))					
					(carton-at ?cb ?r1)
				 )
)
;
(:action unpack-appliance
	:agent ?a - agent 
	:parameters (?elec - electronics ?cb - carton ?loc1 - location)
	:precondition (and
					(packed ?cb)
					(carton-at ?cb ?loc1)
					(electronics-in ?elec ?cb)
					(agent-at ?a ?loc1)												
				  )
	:effect	(and
				(electronics-at ?elec ?loc1)
				(not (electronics-in ?elec ?cb))
				(not (packed ?cb))
			)
)
;
(:action pack-appliance
	:agent ?a - agent 
	:parameters (?elec - electronics ?cb - carton ?loc1 - location)
	:precondition (and
					(electronics-at ?elec ?loc1)
					(agent-at ?a ?loc1)
					(carton-at ?cb ?loc1)										
				  )
	:effect	(and
					(electronics-in ?elec ?cb)
					(packed ?cb)
					(not (electronics-at ?elec ?loc1))
				 )
)
;
)

