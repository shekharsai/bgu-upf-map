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
	(same-agent ?a1 - agent ?a2 - agent)
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
(:action 2-row 
	:agent ?a0 - agent
	:parameters (?a1 - agent ?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a0 ?x)
				(at ?a1 ?x)
				(not (same-agent ?a0 ?a1))
				(has-boat ?b ?x ?y)				
			 )
	:effect	(and 
			(at ?a0 ?y)
			(at ?a1 ?y)
			(not (at ?a0 ?x)) 			
			(not (at ?a1 ?x))
		)
)
(:action 3-row 
	:agent ?a0 - agent
	:parameters (?a1 - agent ?a2 - agent ?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a0 ?x)
				(at ?a1 ?x)
				(at ?a2 ?x)
				(not (same-agent ?a0 ?a1))
				(not (same-agent ?a0 ?a2))
				(not (same-agent ?a2 ?a1))
				(has-boat ?b ?x ?y)				
			 )
	:effect	(and 
			(at ?a0 ?y)
			(at ?a1 ?y)
			(at ?a2 ?y)
			(not (at ?a0 ?x)) 			
			(not (at ?a1 ?x))
			(not (at ?a2 ?x))
		)
)

(:action 4-row 
	:agent ?a0 - agent
	:parameters (?a1 - agent ?a2 - agent ?a3 - agent ?b - boat ?x - location ?y - location)
	:precondition (and
				(at ?a0 ?x)
				(at ?a1 ?x)
				(at ?a2 ?x)
				(not (same-agent ?a0 ?a1))
				(not (same-agent ?a0 ?a2))
				(not (same-agent ?a2 ?a1))
				(has-boat ?b ?x ?y)				
			 )
	:effect	(and 
			(at ?a0 ?y)
			(at ?a1 ?y)
			(at ?a2 ?y)
			(not (at ?a0 ?x)) 			
			(not (at ?a1 ?x))
			(not (at ?a2 ?x))
		)
)


)

