(define (domain maze)
(:requirements :typing :concurrency-network :multi-agent)
(:types agent location door bridge boat switch)
(:predicates
	(at ?a - agent ?x - location)
	(has-switch ?s - switch ?x - location ?y - location ?z - location)
	(blocked ?x - location ?y - location)
	(has-door ?d - door ?x - location ?y - location)
	(has-boat ?b - boat ?x - location ?y - location)
	(at-boat  ?b - boat ?loc1 - location)
	(has-bridge ?b - bridge ?x - location ?y - location)
)
(:action move 
	:agent ?a - agent
	:parameters (?d - door ?x - location ?y - location)
	:precondition (and
					(at ?a ?x)
					(not (blocked ?x ?y))
					(has-door ?d ?x ?y) 
				  )
	:effect	(and 
					(at ?a ?y)
					(not (at ?a ?x))
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
			) 
)
(:action activity-2-row 
	:agent ?a0 - agent
	:parameters (?a1 - agent ?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a0 ?x)
				(has-boat ?b ?x ?y)				
			 )
	:effect	(and 
			(at ?a0 ?y)
			(not (at ?a0 ?x)) 			
		)
)
(:action activity-3-row 
	:agent ?a0 - agent
	:parameters (?a1 - agent ?a2 - agent ?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a0 ?x)
				(has-boat ?b ?x ?y)				
			 )
	:effect	(and 
			(at ?a0 ?y)
			(not (at ?a0 ?x)) 			
		)
)
(:action activity-4-row 
	:agent ?a0 - agent
	:parameters (?a1 - agent ?a2 - agent ?a3 - agent ?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a0 ?x)
				(has-boat ?b ?x ?y)				
			 )
	:effect	(and 
			(at ?a0 ?y)
			(not (at ?a0 ?x)) 			
		)
)

(:action activity-5-row 
	:agent ?a0 - agent
	:parameters (?a1 - agent ?a2 - agent ?a3 - agent ?a4 - agent ?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a0 ?x)
				(has-boat ?b ?x ?y)				
			 )
	:effect	(and 
			(at ?a0 ?y)
			(not (at ?a0 ?x)) 			
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
(:action pushswitch
	:agent ?a - agent
	:parameters (?s - switch ?x - location ?y - location ?z - location)
	:precondition (and
					(at ?a ?x)
					(has-switch ?s ?x ?y ?z)
			 	  )
	:effect	(and
				(not (blocked ?y ?z))
				(not (blocked ?z ?y))
	)
)
(:concurrency-constraint v1
	:parameters (?d - door)
	:bounds (1 1)
	:actions ( (move 1) )
)
(:concurrency-constraint v2
	:parameters (?b - boat ?x - location)
	:bounds (2 5)
	:actions( 
				(row 1 2) 
				(activity-2-row 2 3)
				;(activity-3-row 3 4) 
				;(activity-4-row 4 5) 
				;(activity-5-row 5 6)  
			)
)
(:concurrency-constraint v3
	:parameters (?b - bridge ?x - location ?y - location)
	:bounds (1 5)
	:actions ( (cross 1 2 3) )
)
(:concurrency-constraint v4
	:parameters (?s - switch)
	:bounds (1 1)
	:actions ( (pushswitch 1) )
)
)

