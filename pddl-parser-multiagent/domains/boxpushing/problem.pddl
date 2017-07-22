(define (problem push-box-1_1) (:domain boxpushing)

(:objects
	a1 a2 - agent
	box - box
	loc1 loc2 - location
)

(:init
	(box-at-loc box loc1)
	(agent-at-loc a1 loc1)
	(agent-at-loc a2 loc1)
	(connected loc1 loc2)
)

(:goal 
	(and
		(box-at-loc box loc2)
	)
)

)
