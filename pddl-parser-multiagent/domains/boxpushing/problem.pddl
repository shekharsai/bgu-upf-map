(define (problem push-box-1_1) (:domain boxpushing)

(:objects
	;a1 a2 a3 a4 a5 a6 a7 
	a1 a2 - agent
	box1 - box
	;box2 box3 box4 - box
	loc1 loc2 - location
)

(:init
	(box-at-loc box1 loc2)
	;(box-at-loc box2 loc1)
	;(box-at-loc box3 loc1)
	;(box-at-loc box4 loc1)
	(agent-at-loc a1 loc2)
	(agent-at-loc a2 loc2)
	;(agent-at-loc a3 loc2)
	;(agent-at-loc a4 loc2)
	;(agent-at-loc a5 loc2)
	;(agent-at-loc a6 loc2)
	;(agent-at-loc a7 loc2)
	;(agent-at-loc a8 loc2)
	(connected loc1 loc2)
	(connected loc2 loc1)
)

(:goal 
	(and
		(box-at-loc box1 loc1)
		;(box-at-loc box2 loc2)
		;(box-at-loc box3 loc2)
		;(box-at-loc box4 loc2)
	)
)

)
