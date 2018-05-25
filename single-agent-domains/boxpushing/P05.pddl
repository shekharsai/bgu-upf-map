;
(define (problem push-box-1_1) (:domain boxpushing)
;
(:objects	
	a1 a2 a3 a4 a5 - agent
	box1 box2 box3 box4 box5 box01 box02 box03 box04 box05 box11 box12 - box
	loc1 loc2 loc3 loc4 loc5 loc01 loc02 loc03 loc04 loc05 - location
)
;
(:init
	(agent-at-loc a1 loc2)
	(agent-at-loc a2 loc2) 
	(agent-at-loc a3 loc2)
	(agent-at-loc a4 loc2)
	(agent-at-loc a5 loc2)
	
	(box-at-loc box1 loc1)
	(box-at-loc box2 loc1)
	(box-at-loc box3 loc3)
	(box-at-loc box4 loc4)
	(box-at-loc box5 loc3)
	
	(box-at-loc box01 loc01)
	(box-at-loc box02 loc01)
	(box-at-loc box03 loc03)
	(box-at-loc box04 loc04)
	(box-at-loc box05 loc03)
	
	(box-at-loc box11 loc01)
	(box-at-loc box12 loc01)	
	
	(connected loc1 loc2) (connected loc1 loc01)
	(connected loc2 loc1) (connected loc01 loc1)
	(connected loc3 loc2) (connected loc2 loc02) 						  
	(connected loc2 loc3) (connected loc03 loc3)
	(connected loc4 loc2) 
	(connected loc2 loc4)
	(connected loc4 loc5)
	(connected loc5 loc4)
	
	(connected loc01 loc02) 
	(connected loc02 loc01) 
	(connected loc03 loc02)
	(connected loc02 loc03)
	(connected loc04 loc02)
	(connected loc02 loc04)
	(connected loc04 loc05)
	(connected loc05 loc04)	
)
;
(:goal 
	(and
		(box-at-loc box1 loc05)
		(box-at-loc box2 loc05)
		(box-at-loc box3 loc05)
		(box-at-loc box4 loc05)
		(box-at-loc box5 loc05)	
		(box-at-loc box01 loc5)
		(box-at-loc box02 loc5)
		(box-at-loc box03 loc5)
		(box-at-loc box04 loc5)
		(box-at-loc box05 loc5)	
		(box-at-loc box11 loc5)
		(box-at-loc box12 loc5)			
	)
)
;
)
