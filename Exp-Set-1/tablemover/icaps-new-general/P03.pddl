;
;;	TableMover Domain
;
(define (problem table1_1) (:domain tablemover)
;
(:objects
	a1 a2 a3 a4 - agent
	r1x1 r1x2 r2x1 r2x2 r1x3 r2x3 - room
	t1 t2 t3 - table
)
;
(:init
	(in a1 r1x1)
	(in a2 r1x1)
	(in a3 r1x1)
	(in a4 r1x1)
	(inroom t1 r1x1)
	(inroom t2 r2x1)
	(inroom t3 r1x1)
	(available a1)
	(available a2)
	(available a3)
	(available a4)
	(connected r1x1 r1x2)
	(connected r1x2 r1x1)
	(connected r1x1 r2x1)
	(connected r2x1 r1x1)
	(connected r2x1 r2x2)
	(connected r2x2 r2x1)
	(connected r1x2 r2x2)
	(connected r2x2 r1x2)	
	(connected r1x3 r1x2)
	(connected r1x2 r1x3)
	(connected r2x3 r2x2)
	(connected r2x2 r2x3)
	(connected r2x3 r1x3)
	(connected r1x3 r2x3)		
	(intact t1)
	(intact t2)
	(intact t3)	
	(has-charging-point r1x1)
	(has-charging-point r2x2)
	(same-agent a1 a1) 	
	(same-agent a2 a2)
	(same-agent a3 a3)
	(same-agent a4 a4)
)
;
(:goal 
	(and
		(inroom t1 r2x2)
		(inroom t2 r2x3)
		(inroom t3 r2x3)	
		(intact t1)		
		(intact t2)
		(intact t3)		
 	)
)
;
)
