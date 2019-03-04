; 
(define (problem push-box-1_1) (:domain boxpushing)
;
(:objects	
	a1 a2 a3 - agent
	box1 box2 - box
	loc1 loc2 loc3 - location
) 
;
(:init
	(agent-at-loc a1 loc2)
	(agent-at-loc a2 loc2)
	(agent-at-loc a3 loc2)
	
	(box-at-loc box1 loc1)
	(box-at-loc box2 loc1)
	
	(connected loc1 loc2)
	(connected loc2 loc1)
	(connected loc3 loc2)
	(connected loc2 loc3)	
	(same-agent a1 a1)
	(same-agent a2 a2)
	(same-agent a3 a3)
)
;
(:goal 
	(and
		(box-at-loc box1 loc3)
		(box-at-loc box2 loc3)				
	)
)
;
)
