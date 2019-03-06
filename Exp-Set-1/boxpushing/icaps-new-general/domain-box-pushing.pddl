;;
;; The Box-Pushing domain
;; Note that, for each action we use (:agent ?a - agent) that has no explicit or specific meaning. 
;; We want to use the parser from upf-map hence we need to place that agent parameter there - a regular parameter.
;;
(define (domain boxpushing)	
	(:requirements :typing :concurrency-network :multi-agent)   	
  	(:types		
  		agent 
		box 	
		location 		
  	)  	
  	(:predicates 	
  		(box-at-loc ?box - box ?location - location)
		(agent-at-loc ?agent - agent ?loc - location)
		(connected ?from - location ?to - location)
		(tired ?agent - agent)
		(same-agent ?a1 - agent ?a2 - agent)
		(intact ?b - box)
  	)
	(:action move-agent
		:agent ?agent - agent
	  	:parameters (?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 
	  			(agent-at-loc ?agent ?from-location)
	  			(connected ?from-location ?to-location)
	  		)
	  	:effect 
	  		(and
	  			(not (agent-at-loc ?agent ?from-location))
	  			(agent-at-loc ?agent ?to-location)
	  		)
	)	  
	(:action push-box
		:agent ?agent - agent
	  	:parameters (?box - box ?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 	  			
	  			(agent-at-loc ?agent ?from-location) 
	  			(box-at-loc ?box ?from-location)
	  			(connected ?from-location ?to-location)
	  			(not (tired ?agent))
	  		)
	  	:effect 
	  		(and	  			
	  			(agent-at-loc ?agent ?to-location)
	  			(box-at-loc ?box ?to-location)
	  			(not (agent-at-loc ?agent ?from-location))
	  			(not (box-at-loc ?box ?from-location))
	  			(tired ?agent)
	  		)
	)		
	(:action 2-push-box
		:agent ?agent - agent
		:parameters (?a1 - agent ?box - box ?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 	  			
	  			(agent-at-loc ?agent ?from-location)
	  			(agent-at-loc ?a1 ?from-location)
	  			(box-at-loc ?box ?from-location)
	  			(connected ?from-location ?to-location)
	  			(not (tired ?agent))
	  			(not (tired ?a1))
	  			(not (same-agent ?agent ?a1))
	  		)
	  	:effect 
	  		(and	  			
	  			(agent-at-loc ?agent ?to-location)
	  			(agent-at-loc ?a1 ?to-location)
	  			(box-at-loc ?box ?to-location)
	  			(not (agent-at-loc ?agent ?from-location))
	  			(not (agent-at-loc ?a1 ?from-location))
	  			(not (box-at-loc ?box ?from-location))
	  		)
	)
	(:action 3-push-box
		:agent ?agent - agent
		:parameters (?a1 - agent ?a2 - agent ?box - box ?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 	  			
	  			(agent-at-loc ?agent ?from-location)
	  			(agent-at-loc ?a1 ?from-location)
	  			(agent-at-loc ?a2 ?from-location)
	  			(box-at-loc ?box ?from-location)
	  			(connected ?from-location ?to-location)
	  			(not (tired ?agent))
	  			(not (tired ?a1))
	  			(not (tired ?a2))
	  			(not (same-agent ?agent ?a1))
	  			(not (same-agent ?agent ?a2))
	  			(not (same-agent ?a2 ?a1))
	  		)
	  	:effect 
	  		(and	  			
	  			(agent-at-loc ?agent ?to-location)
	  			(agent-at-loc ?a1 ?to-location)
	  			(agent-at-loc ?a2 ?to-location)
	  			(box-at-loc ?box ?to-location)
	  			(not (agent-at-loc ?agent ?from-location))
	  			(not (agent-at-loc ?a1 ?from-location))
	  			(not (agent-at-loc ?a2 ?from-location))
	  			(not (box-at-loc ?box ?from-location))
	  		)
	)
	(:action 4-push-box
		:agent ?agent - agent
		:parameters (?a1 - agent ?a2 - agent ?a3 - agent ?box - box ?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 	  			
	  			(agent-at-loc ?agent ?from-location)
	  			(agent-at-loc ?a1 ?from-location)
	  			(agent-at-loc ?a2 ?from-location)
	  			(agent-at-loc ?a3 ?from-location)
	  			(box-at-loc ?box ?from-location)
	  			(connected ?from-location ?to-location)
	  			(not (tired ?agent))
	  			(not (tired ?a1))
	  			(not (tired ?a2))
	  			(not (tired ?a3))
	  			(not (same-agent ?agent ?a1))
	  			(not (same-agent ?agent ?a2))
	  			(not (same-agent ?agent ?a3))
	  			(not (same-agent ?a2 ?a1))
	  			(not (same-agent ?a3 ?a1))
	  			(not (same-agent ?a2 ?a3))
	  		)
	  	:effect 
	  		(and	  			
	  			(agent-at-loc ?agent ?to-location)
	  			(agent-at-loc ?a1 ?to-location)
	  			(agent-at-loc ?a2 ?to-location)
	  			(agent-at-loc ?a3 ?to-location)
	  			(box-at-loc ?box ?to-location)
	  			(not (agent-at-loc ?agent ?from-location))
	  			(not (agent-at-loc ?a1 ?from-location))
	  			(not (agent-at-loc ?a2 ?from-location))
	  			(not (agent-at-loc ?a3 ?from-location))
	  			(not (box-at-loc ?box ?from-location))
	  		)
	)
)
