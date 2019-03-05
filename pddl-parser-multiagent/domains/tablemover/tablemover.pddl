;
;;	TableMover Domain
;
(define (domain tablemover)
;
(:requirements :typing :conditional-effects :multi-agent :concurrency-network)
;
(:types agent table room)
;
(:predicates
	(lifting ?a - agent ?t - table)
	(inroom ?t - table ?r - room)
	(in ?a - agent ?r - room)
	(connected ?r1 - room ?r2 - room)
	(intact ?t - table) 
	(has-charging-point ?r - room)
	;private
	(available ?a - agent)	
	(charged ?a - agent)
)
;
(:action move-agent
	:agent ?a - agent 
	:parameters (?r1 - room ?r2 - room)
	:precondition (and
					(in ?a ?r1)
					(connected ?r1 ?r2)
					(available ?a)					
				  )
	:effect	(and
					(not (in ?a ?r1))
					(in ?a ?r2)
				 )
)
;
(:action charge-agent
	:agent ?a - agent
	:parameters (?r - room)
	:precondition (and
					(in ?a ?r)
					(has-charging-point ?r)
					(not (charged ?a))
				  )
	:effect	(and
				(charged ?a)
			)
)
;
(:action move-table 
	:agent ?a - agent
	:parameters (?t - table ?r1 - room ?r2 - room)
	:precondition (and
					(lifting ?a ?t)
					(in ?a ?r1)
					(connected ?r1 ?r2)
					(charged ?a)
				  )
	:effect	(and				
				(not (in ?a ?r1))
				(in ?a ?r2)		
				(not (charged ?a))
			)
)
;
(:action activity-move-table 
	:agent ?a - agent
	:parameters (?a1 - agent ?t - table ?r1 - room ?r2 - room)
	:precondition (and
					(lifting ?a ?t)
					(in ?a ?r1)
					(connected ?r1 ?r2)
					(charged ?a)
				  )
	:effect	(and
					(not (in ?a ?r1))
					(in ?a ?r2)	
				 )
)
;
(:action lift-table
	:agent ?a - agent
	:parameters (?t - table ?r1 - room)
	:precondition (and
					(available ?a)
					(in ?a ?r1)
					(inroom ?t ?r1)
					(intact ?t)					
					(charged ?a)					
				  )
	:effect	(and
					(lifting ?a ?t)
					(not (available ?a))
					(not (inroom ?t ?r1))
					(not ( intact ?t))					
				 )
)
;
(:action activity-lift-table
	:agent ?a - agent
	:parameters (?a1 - agent ?t - table ?r1 - room)
	:precondition (and
					(available ?a)
					(in ?a ?r1)
					(inroom ?t ?r1)	
					(intact ?t)		
					(charged ?a)		
				  )
	:effect	(and
					(lifting ?a ?t)
					(not (available ?a))
					(not (inroom ?t ?r1))	
					(intact ?t)					
				 )
)
;
(:action drop-table
	:agent ?a - agent
	:parameters (?t - table ?r1 - room)
	:precondition (and
					(in ?a ?r1)
					(lifting ?a ?t)					
				  )
	:effect	(and
					(not (lifting ?a ?t))
					(inroom ?t ?r1)										
					(available ?a)
					(not ( intact ?t)) 
					(not (charged ?a))
				 )
)
;
(:action activity-drop-table
	:agent ?a - agent
	:parameters (?a1 - agent ?t - table ?r1 - room)
	:precondition (and
					(in ?a ?r1)
					(lifting ?a ?t)														
				  )
	:effect	(and
					(not (lifting ?a ?t))
					(inroom ?t ?r1)										
					(available ?a)
					(intact ?t)
					(not (charged ?a))
			)	
)
;
(:concurrency-constraint v1
	:parameters (?r - room)
	:bounds (1 1)
	:actions ( (move-agent 1) (charge-agent 1) )
)
;
(:concurrency-constraint v2
	:parameters (?t - table ?r1 - room ?r2 - room )
	:bounds (1 4)
	:actions ( (activity-move-table  2 3 4) (move-table 1 2 3) )
)
;
(:concurrency-constraint v3
	:parameters (?t - table ?r1 - room)
	:bounds (1 2)
	:actions ( (lift-table 1 2) (activity-lift-table 2 3) )
)
;
(:concurrency-constraint v4
	:parameters (?t - table)
	:bounds (1 2)
	:actions ( (drop-table 1) (activity-drop-table 2) )
)
;
)
;
