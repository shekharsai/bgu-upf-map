;
(define (problem push-box-1_1) (:domain boxpushing)
;
(:objects	
	a1 a2 a3 a4 - agent
	box1 box2 box3 - box
	loc1 loc2 loc3 - location
)
;
(:init
	(agent-at-loc a1 loc2)
	(agent-at-loc a2 loc2)
	(agent-at-loc a3 loc2)
	(agent-at-loc a4 loc2)
	
	(box-at-loc box1 loc1)
	(box-at-loc box2 loc1)
	(box-at-loc box3 loc3)
	
	(connected loc1 loc2)
	(connected loc2 loc1)
	(connected loc3 loc2)
	(connected loc2 loc3)	
	
)
;
(:goal 
	(and
		(box-at-loc box1 loc3)
		(box-at-loc box2 loc3)
		(box-at-loc box3 loc1)				
	)
)
;
)
