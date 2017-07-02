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
	(disha ?BO - BOAT ?L2 - LOCATION)
)

( :ACTION dude
  :agent ?AGENT0 - AGENT
  :PARAMETERS ( ?AGENT1 - AGENT ?BOAT2 - BOAT ?LOCATION3 - LOCATION ?LOCATION4 - LOCATION )
  :PRECONDITION
	( AND		
		( disha ?BOAT2 ?LOCATION3 )		
	)
  :EFFECT
	( AND
		( disha ?BOAT2 ?LOCATION4 )
		( NOT ( disha ?BOAT2 ?LOCATION3 ) )		
	)
)
( :ACTION dudex
  :agent ?AGENT0 - AGENT
  :PARAMETERS ( ?AGENT1 - AGENT ?BOAT2 - BOAT ?LOCATION3 - LOCATION ?LOCATION4 - LOCATION )
  :PRECONDITION
	( AND		
		( disha ?BOAT2 ?LOCATION3 )		
	)
  :EFFECT
	( AND
		( disha ?BOAT2 ?LOCATION4 )
		( NOT ( disha ?BOAT2 ?LOCATION3 ) )		
	)
)

(:concurrency-constraint v2
	:parameters (?b - boat ?LOCATION3 - LOCATION)
	:bounds (3 inf)
	:actions ( 
		(dude 2) 
		(dudex 2) 
	)
)

)

