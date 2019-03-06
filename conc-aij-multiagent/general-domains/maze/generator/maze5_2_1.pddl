(define (problem maze5_2_1) (:domain maze)
(:objects
	a1 a2 a3 a4 a5 - agent
	loc1x1 loc1x2 loc2x1 loc2x2 - location
	d1 - door
	b1 b2 - bridge
	bt1 - boat
	s1 - switch
)
(:init
	(at a1 loc1x2)
	(at a2 loc2x2)
	(at a3 loc2x1)
	(at a4 loc1x1)
	(at a5 loc1x2)
	(has-door d1 loc1x1 loc1x2)
	(has-door d1 loc1x2 loc1x1)
	(blocked loc1x1 loc1x2)
	(blocked loc1x2 loc1x1)
	(has-switch s1 loc1x2 loc1x1 loc1x2)
	(has-bridge b1 loc1x1 loc2x1)
	(has-bridge b1 loc2x1 loc1x1)
	(has-boat bt1 loc1x2 loc2x2)
	(has-boat bt1 loc2x2 loc1x2)
	(has-bridge b2 loc2x1 loc2x2)
	(has-bridge b2 loc2x2 loc2x1)
)
(:goal (and
	(at a1 loc2x1)
	(at a2 loc2x2)
	(at a3 loc2x1)
	(at a4 loc2x2)
	(at a5 loc2x2)
))
)
