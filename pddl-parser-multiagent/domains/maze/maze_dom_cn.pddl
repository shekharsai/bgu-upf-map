(define (domain maze)

(:requirements :typing :concurrency-network :multi-agent)

(:types agent boat location door bridge switch)

(:predicates
	(at ?a - agent ?x - location)
	(has-switch ?s - switch ?x - location ?y - location ?z - location)
	(blocked ?x - location ?y - location)
	(has-door ?d - door ?x - location ?y - location)
	(has-boat ?b - boat ?x - location ?y - location)
	(has-bridge ?b - bridge ?x - location ?y - location)	
	(next ?a - agent)	
	(at ?b - boat ?l - location)
)

( :ACTION ACTIVITY-SAIL
  :agent ?agent0 - agent
  :PARAMETERS (?agent1 - AGENT ?BOAT2 - BOAT ?LOCATION3 - LOCATION ?LOCATION4 - LOCATION )
  :PRECONDITION
	( AND
		( AT ?AGENT0 ?LOCATION3 )
		( at ?BOAT2 ?LOCATION3 )
	)
  :EFFECT
	( AND
		( AT ?AGENT0 ?LOCATION4 )
		( not ( AT ?AGENT0 ?LOCATION3 ) )		
		( not ( at ?BOAT2 ?LOCATION3 ) )
		( at ?BOAT2 ?LOCATION4 )
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

(:action cross 
	:agent ?a - agent
	:parameters (?b - bridge ?x - location ?y - location) 
	:precondition (and
					(at ?a ?x)
					(has-bridge ?b ?x ?y) 
			)
	:effect	(and 
					(at ?a ?y)
					(not (at ?a ?x))
					(not (has-bridge ?b ?x ?y))
					(not (has-bridge ?b ?y ?x))
			 )
)

(:concurrency-constraint v1
	:parameters (?d - boat)
	:bounds (1 1)
	:actions ( 
				( ACTIVITY-SAIL 2 ) 
				( ROW 1 )	
			)
)

(:concurrency-constraint v2
	:parameters (?b - bridge)
	:bounds (1 1)
	:actions ( (cross 1) )
)
	
)
