;;
;; 			Box-Pushing domain
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

	(:action activity-2-push-box
		:agent ?agent0 - agent 
	  	:parameters (?agent1 - agent ?box - box ?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 	  			
	  			(agent-at-loc ?agent0 ?from-location)
	  			(box-at-loc ?box ?from-location)
	  			(connected ?from-location ?to-location)
	  			(not (tired ?agent0))
	  		)
	  	:effect 
	  		(and	  			
	  			(agent-at-loc ?agent0 ?to-location)
	  			(box-at-loc ?box ?to-location)
	  			(not (agent-at-loc ?agent0 ?from-location))
	  			(not (box-at-loc ?box ?from-location))
	  		)
	)
	
	(:action activity-3-push-box
		:agent ?agent0 - agent 
	  	:parameters (?agent1 - agent ?agent2 - agent ?box - box ?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 	  			
	  			(agent-at-loc ?agent0 ?from-location)
	  			(box-at-loc ?box ?from-location)
	  			(connected ?from-location ?to-location)
	  			(not (tired ?agent0))
	  		)
	  	:effect 
	  		(and	  			
	  			(agent-at-loc ?agent0 ?to-location)
	  			(box-at-loc ?box ?to-location)
	  			(not (agent-at-loc ?agent0 ?from-location))
	  			(not (box-at-loc ?box ?from-location))
	  		)
	)
	
	(:action activity-4-push-box
		:agent ?agent0 - agent 
	  	:parameters (?agent1 - agent ?agent2 - agent ?agent3 - agent ?box - box ?from-location - location ?to-location - location)
	  	:precondition 
	  		(and 	  			
	  			(agent-at-loc ?agent0 ?from-location)
	  			(box-at-loc ?box ?from-location)
	  			(connected ?from-location ?to-location)
	  			(not (tired ?agent0))
	  		)
	  	:effect 
	  		(and	  			
	  			(agent-at-loc ?agent0 ?to-location)
	  			(box-at-loc ?box ?to-location)
	  			(not (agent-at-loc ?agent0 ?from-location))
	  			(not (box-at-loc ?box ?from-location))
	  		)
	)

(:concurrency-constraint v1
    :parameters (?box - box ?from-location - location ?to-location - location) 
    :bounds (2 4) 
    :actions 
    	( 
    		(push-box 1 2 3) 
    		(activity-2-push-box 2 3 4)
    		(activity-3-push-box 3 4 5)
    		(activity-4-push-box 4 5 6)    
    	) 
)

;(:concurrency-constraint v2
;    :parameters (?from-location - location ?to-location - location) 
;    :bounds (1 4) 
;    :actions ( ( move-agent 1 2 ) ) 
;)

)
