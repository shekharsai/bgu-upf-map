(define (problem maze5_4_1) 
(:domain maze)
(:objects
	; does not efectively parse private	
	a1 a2 - agent
	loc4x2 loc3x2 - location
	b3 - bridge				
	
	;(:private			
	;	always-dummy-pr - object 
	;	ss1-pr - agent - agent
	;		driver7-pr - location	
	;)
)
(:init
	(at a1 loc4x2)
	(at a2 loc4x2)
	(has-bridge b3 loc3x2 loc4x2)
	(has-bridge b3 loc4x2 loc3x2)	
)
(:goal (and
		(at a1 loc3x2)	
	)
)

)
