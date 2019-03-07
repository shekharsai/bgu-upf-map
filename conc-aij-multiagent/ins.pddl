( DEFINE ( PROBLEM PUSH-BOX-1_1 )
( :DOMAIN BOXPUSHING )
( :OBJECTS
	A1 A2 A3 A4 A5 A6 - AGENT
	BOX1 BOX2 BOX3 BOX4 BOX5 BOX6 BOX7 - BOX
	LOC1 LOC2 LOC3 LOC4 LOC5 LOC6 - LOCATION
)
( :INIT
	( AGENT-AT-LOC A1 LOC2 )
	( AGENT-AT-LOC A2 LOC2 )
	( AGENT-AT-LOC A3 LOC2 )
	( AGENT-AT-LOC A4 LOC2 )
	( AGENT-AT-LOC A5 LOC5 )
	( AGENT-AT-LOC A6 LOC5 )
	( BOX-AT-LOC BOX1 LOC1 )
	( BOX-AT-LOC BOX2 LOC1 )
	( BOX-AT-LOC BOX3 LOC1 )
	( BOX-AT-LOC BOX4 LOC6 )
	( BOX-AT-LOC BOX5 LOC6 )
	( BOX-AT-LOC BOX6 LOC6 )
	( BOX-AT-LOC BOX7 LOC6 )
	( CONNECTED LOC1 LOC2 )
	( CONNECTED LOC2 LOC1 )
	( CONNECTED LOC3 LOC2 )
	( CONNECTED LOC2 LOC3 )
	( CONNECTED LOC4 LOC3 )
	( CONNECTED LOC3 LOC4 )
	( CONNECTED LOC4 LOC5 )
	( CONNECTED LOC5 LOC4 )
	( CONNECTED LOC6 LOC5 )
	( CONNECTED LOC5 LOC6 )
	( SAME-AGENT A1 A1 )
	( SAME-AGENT A2 A2 )
	( SAME-AGENT A3 A3 )
	( SAME-AGENT A4 A4 )
	( SAME-AGENT A5 A5 )
	( SAME-AGENT A6 A6 )
	( NEG-IN-JOINT )
)
( :GOAL
	( AND
		( BOX-AT-LOC BOX1 LOC6 )
		( BOX-AT-LOC BOX2 LOC6 )
		( BOX-AT-LOC BOX3 LOC6 )
		( BOX-AT-LOC BOX4 LOC1 )
		( BOX-AT-LOC BOX5 LOC1 )
		( BOX-AT-LOC BOX6 LOC1 )
		( BOX-AT-LOC BOX7 LOC1 )
		( NEG-IN-JOINT )
	)
)
)
