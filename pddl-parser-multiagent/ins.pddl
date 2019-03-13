( DEFINE ( PROBLEM PUSH-BOX-1_1 )
( :DOMAIN BOXPUSHING )
( :OBJECTS
	A1 A2 A3 A4 A5 - AGENT
	BOX1 BOX2 BOX3 BOX4 BOX5 BOX6 BOX7 - BOX
	LOC1 LOC2 LOC3 LOC4 LOC5 LOC6 - LOCATION
	ACOUNT-1 ACOUNT-2 ACOUNT-3 ACOUNT-4 - AGENT-COUNT
)
( :INIT
	( AGENT-AT-LOC A1 LOC2 )
	( AGENT-AT-LOC A2 LOC2 )
	( AGENT-AT-LOC A3 LOC2 )
	( AGENT-AT-LOC A4 LOC2 )
	( AGENT-AT-LOC A5 LOC5 )
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
	( AFREE )
	( CONSEC ACOUNT-0 ACOUNT-1 )
	( CONSEC ACOUNT-1 ACOUNT-2 )
	( CONSEC ACOUNT-2 ACOUNT-3 )
	( CONSEC ACOUNT-3 ACOUNT-4 )
	( SAT-V1 ACOUNT-2 )
	( SAT-V1 ACOUNT-3 )
	( SAT-V1 ACOUNT-4 )
	( SAT-V2 ACOUNT-1 )
	( SAT-V2 ACOUNT-2 )
	( SAT-V2 ACOUNT-3 )
	( SAT-V2 ACOUNT-4 )
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
		( AFREE )
	)
)
)
