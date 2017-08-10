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
	(broken ?b - bridge)
	(:private
		(tired-agent-pr ?agent - agent )
	)
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

(:action ja-activity-clean-build 
	:agent ?a - agent
	:parameters (?a1 - agent ?x - location ?b - bridge ?y - location) 
	:precondition (and
					(at ?a ?x)
					(has-bridge ?b ?x ?y) 
			)
	:effect	(and 
					(at ?a ?y)
					(not (at ?a ?x))
					;(has-bridge ?b ?x ?y)
					(not (has-bridge ?b ?y ?x))
			 )
)

(:action build
	:agent ?a - agent
	:parameters (?b - bridge ?x - location ?y - location)  
	:precondition (and
					(at ?a ?x)
					(has-bridge ?b ?x ?y) 
					(broken ?b)
			)
	:effect	(and 
					(at ?a ?y)
					(not (at ?a ?x))
					;(has-bridge ?b ?x ?y)
					(not (broken ?b))
					(not (has-bridge ?b ?y ?x))
					;(has-bridge ?b ?y ?x)
					(broken ?b)
			 )
)

(:action clean
	:agent ?a - agent
	:parameters (?x - location ?y - location ?b - bridge) 
	:precondition (and
					(at ?a ?x)
					(has-bridge ?b ?x ?y) 
					(broken ?b)
			)
	:effect	(and 
					(at ?a ?y)
					(not (at ?a ?x))
					;(not (has-bridge ?b ?x ?y))
					(has-bridge ?b ?y ?x)
					;(broken ?b)
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

;(:concurrency-constraint v1
;	:parameters (?d - door)
;	:bounds (1 3)
;	:actions ( (move 1) )
;)

(:concurrency-constraint v3
	:parameters (?l - location ?b - bridge)
	:bounds (3 5)
	:actions ( 
				(clean 1 3) 
				;(build 2 1)	
				(ja-activity-clean-build 2 3)
			 )
)

;(:concurrency-constraint v4
;	:parameters (?s - switch)
;	:bounds (1 1)
;	:actions ( (pushswitch 1) )
;)

)

