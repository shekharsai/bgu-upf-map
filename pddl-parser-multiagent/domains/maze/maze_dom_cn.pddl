(define (domain maze)

(:requirements :typing :concurrency-network :multi-agent)

(:types agent location door bridge boat switch)

(:predicates
	(at ?a - agent ?x - location)
	(has-switch ?s - switch ?x - location ?y - location ?z - location)
	(blocked ?x - location ?y - location)
	(has-door ?d - door ?x - location ?y - location)
	(has-boat ?b - boat ?x - location ?y - location)
	(has-bridge ?b - bridge ?x - location ?y - location)	
	(NEXT ?A2 - AGENT)	
	(atboat ?BO - BOAT ?L2 - LOCATION)
)

( :ACTION ACTIVITY-SAIL
  :agent ?AGENT0 - AGENT
  :PARAMETERS (?AGENT1 - AGENT ?AGENT4 - AGENT ?AGENT3 - AGENT ?BOAT2 - BOAT ?LOCATION3 - LOCATION ?LOCATION4 - LOCATION )
  :PRECONDITION
	( AND
		( AT ?AGENT0 ?LOCATION3 )
		( atboat ?BOAT2 ?LOCATION3 )
	)
  :EFFECT
	( AND
		( AT ?AGENT0 ?LOCATION4 )
		( Not ( AT ?AGENT0 ?LOCATION3 ) )
		( atboat ?BOAT2 ?LOCATION4 )
		( not ( atboat ?BOAT2 ?LOCATION3 ) )
	)
)

(:action row 
	:agent ?a - agent
	:parameters (?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a ?x)
				(has-boat ?b ?x ?y)
			 )
	:effect	(and 
			(at ?a ?y)
			(not (at ?a ?x)) 
			( not (has-boat ?b ?x ?y) )
			(has-boat ?b ?y ?x)
		)
)

(:concurrency-constraint v1
	:parameters (?d - boat)
	:bounds (1 2)
	:actions ( 
				( ACTIVITY-SAIL 4 ) 
				;( ROW 1 )	
			)
)

)
