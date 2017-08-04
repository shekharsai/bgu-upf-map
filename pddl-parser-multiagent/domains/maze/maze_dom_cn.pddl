(define (domain maze)
(:requirements :typing :concurrency-network :multi-agent)
(:types agent location door boat switch bridge)
(:predicates
	(at ?a - agent ?x - location)
	(has-switch ?s - switch ?x - location ?y - location ?z - location)
	(blocked ?x - location ?y - location)
	(has-door ?d - door ?x - location ?y - location)
	(has-boat ?b - boat ?x - location ?y - location)
	(has-bridge ?b - bridge ?x - location ?y - location)
	(:private
		(kamina-pr)
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

(:action activity 
	:agent ?a - agent
	:parameters (?a - agent ?b - bridge ?x - location ?y - location) 
	:precondition (and
					(at ?a ?x)
					;(has-bridge ?b ?x ?y) 
			)
	:effect	(and 
					(at ?a ?y)
					;(not (at ?a ?x))
					(has-bridge ?b ?x ?y)
					;(not (has-bridge ?b ?y ?x))
			 )
)

(:action build
	:agent ?a - agent
	:parameters (?b - bridge ?x - location ?y - location) 
	:precondition (and
					(at ?a ?x)
					;(has-bridge ?b ?x ?y) 
			)
	:effect	(and 
					(at ?a ?y)
					;(not (at ?a ?x))
					(has-bridge ?b ?x ?y)
					(not (has-bridge ?b ?y ?x))
			 )
)

(:action clean
	:agent ?a - agent
	:parameters (?x - location ?y - location ?b - bridge ) 
	:precondition (and
					(at ?a ?x)
					;(has-bridge ?b ?x ?y) 
			)
	:effect	(and 
					(at ?a ?y)
					;(not (at ?a ?x))
					(has-bridge ?b ?x ?y)
					;(has-bridge ?b ?y ?x)
			 )
)


(:concurrency-constraint v3
	:parameters (?b - bridge)
	:bounds (1 3)
	:actions ( (build 1) (clean 3) (activity 2) )
)

)
