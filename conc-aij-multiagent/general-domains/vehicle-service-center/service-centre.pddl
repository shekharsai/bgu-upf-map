
;vehicle-service-centre

(define (domain vehicle-service-centre)
  (:requirements :typing :conditional-effects :multi-agent :concurrency-network)  
  (:types
  	agent
  	car truck - vehicle
  	billing-centre workshop place - location 
  )  
  (:predicates 
 	(at ?agent - agent ?loc1 - location)    	
 	(vehicle-at ?vehicle - vehicle ?loc1 - location)
 	(is-open ?bc - billing-centre)
 	(billing-done ?vehicle - vehicle)
 	(under-operation ?ws - workshop)
 	(servicing-done ?vehicle - vehicle)
  )      

  (:action visit
    :agent ?agent - agent 
    :parameters (?vehicle - vehicle ?loc1 - location ?loc2 - location)
    :precondition (and
    	(at ?agent ?loc1)    	
    	(vehicle-at ?vehicle ?loc1))
    :effect (and
    	(at ?agent ?loc2)    	
    	(vehicle-at ?vehicle ?loc2)
    	(not (at ?agent ?loc1))    	
    	(not (vehicle-at ?vehicle ?loc1)))
  )           

  (:action get-billing-done ;takes vehicles available in a chunck, bills and closes! 
    :agent ?agent - agent
    :parameters (?vehicle - vehicle ?bc - billing-centre)
    :precondition (and
	(is-open ?bc)
        (at ?agent ?bc)    
        (not-billing-done ?vehicle) ;is this okay?
        (vehicle-at ?vehicle ?bc))
    :effect (and 
	(billing-done ?vehicle)
	(not (not-billing-done ?vehicle))
	(not (is-open ?bc))) ;is this required?
  )	

  (:action get-car-servicing
    :agent ?agent - agent 
    :parameters (?ws - workshop ?vehicle - car)
    :precondition (and
    	(at ?agent ?ws)
    	(vehicle-at ?vehicle ?ws)
    	(billing-done ?vehicle)
    	(under-operation ?ws)) 	    	
    :effect (and 
    	(servicing-done ?vehicle)
    	(not (billing-done ?vehicle)))
    	;(not (under-operation ?ws)) ;do we want pre-eff interaction?
  )  	

  (:action get-truck-servicing
    :agent ?agent1 - agent 
    :parameters (?agent2 - agent ?ws - workshop ?vehicle - truck)
    :precondition (and
    	(at ?agent1 ?ws)
    	(vehicle-at ?vehicle ?ws)
    	(billing-done ?vehicle)
    	(under-operation ?ws)) 	    	
    :effect (and 
    	(servicing-done ?vehicle)
    	(not (billing-done ?vehicle)))
    	;(not (under-operation ?ws)) ;do we want pre/eff interaction?
  )
      
(:concurrency-constraint v1
	:parameters (?vehicle - vehicle)
	:bounds (1 1)
	:actions ((visit 1))
)
;
(:concurrency-constraint v2
	:parameters (?bc - billing-centre)
	:bounds (1 n)
	:actions ((get-billing-done 2))
)
;
(:concurrency-constraint v3
	:parameters (?ws - workshop)
	:bounds (1 n)
	:actions ((get-car-servicing 1) (get-truck-servicing 2)) ;default 2-2 on get-truck-servicing
)
;
)
  
