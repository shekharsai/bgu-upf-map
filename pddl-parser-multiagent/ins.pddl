( DEFINE ( PROBLEM PUSH-BOX-1_1 )
( :DOMAIN BOXPUSHING )
( :OBJECTS
	A1 A2 A3 - AGENT
	BOX1 BOX2 - BOX
	LOC1 LOC2 LOC3 - LOCATION
)
( :INIT
	( AGENT-AT-LOC A1 LOC2 )
	( AGENT-AT-LOC A2 LOC2 )
	( AGENT-AT-LOC A3 LOC2 )
	( BOX-AT-LOC BOX1 LOC1 )
	( BOX-AT-LOC BOX2 LOC1 )
	( CONNECTED LOC1 LOC2 )
	( CONNECTED LOC2 LOC1 )
	( CONNECTED LOC3 LOC2 )
	( CONNECTED LOC2 LOC3 )
	( AFREE )
)
( :GOAL
	( AND
		( BOX-AT-LOC BOX1 LOC3 )
		( BOX-AT-LOC BOX2 LOC3 )
		( AFREE )
	)
)
)
