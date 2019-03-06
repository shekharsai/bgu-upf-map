( DEFINE ( PROBLEM PUSH-BOX-1_1 )
( :DOMAIN BOXPUSHING )
( :OBJECTS
	A1 A2 A3 A4 - AGENT
	BOX1 BOX2 BOX3 BOX4 BOX5 BOX01 BOX02 BOX03 BOX04 BOX05 - BOX
	LOC1 LOC2 LOC3 LOC4 LOC5 LOC01 LOC02 LOC03 LOC04 LOC05 - LOCATION
	ACOUNT-1 ACOUNT-2 ACOUNT-3 ACOUNT-4 - AGENT-COUNT
)
( :INIT
	( AGENT-AT-LOC A1 LOC2 )
	( AGENT-AT-LOC A2 LOC2 )
	( AGENT-AT-LOC A3 LOC2 )
	( AGENT-AT-LOC A4 LOC2 )
	( BOX-AT-LOC BOX1 LOC1 )
	( BOX-AT-LOC BOX2 LOC1 )
	( BOX-AT-LOC BOX3 LOC3 )
	( BOX-AT-LOC BOX4 LOC4 )
	( BOX-AT-LOC BOX5 LOC3 )
	( BOX-AT-LOC BOX01 LOC01 )
	( BOX-AT-LOC BOX02 LOC01 )
	( BOX-AT-LOC BOX03 LOC03 )
	( BOX-AT-LOC BOX04 LOC04 )
	( BOX-AT-LOC BOX05 LOC03 )
	( CONNECTED LOC1 LOC2 )
	( CONNECTED LOC1 LOC01 )
	( CONNECTED LOC2 LOC1 )
	( CONNECTED LOC01 LOC1 )
	( CONNECTED LOC3 LOC2 )
	( CONNECTED LOC2 LOC3 )
	( CONNECTED LOC4 LOC2 )
	( CONNECTED LOC2 LOC4 )
	( CONNECTED LOC4 LOC5 )
	( CONNECTED LOC5 LOC4 )
	( CONNECTED LOC01 LOC02 )
	( CONNECTED LOC02 LOC01 )
	( CONNECTED LOC03 LOC02 )
	( CONNECTED LOC02 LOC03 )
	( CONNECTED LOC04 LOC02 )
	( CONNECTED LOC02 LOC04 )
	( CONNECTED LOC04 LOC05 )
	( CONNECTED LOC05 LOC04 )
	( AFREE )
	( CONSEC ACOUNT-0 ACOUNT-1 )
	( CONSEC ACOUNT-1 ACOUNT-2 )
	( CONSEC ACOUNT-2 ACOUNT-3 )
	( CONSEC ACOUNT-3 ACOUNT-4 )
	( SAT-V2 ACOUNT-2 )
	( SAT-V2 ACOUNT-3 )
	( SAT-V2 ACOUNT-4 )
	( SAT-V1 ACOUNT-1 )
	( SAT-V1 ACOUNT-2 )
	( SAT-V1 ACOUNT-3 )
	( SAT-V1 ACOUNT-4 )
)
( :GOAL
	( AND
		( BOX-AT-LOC BOX1 LOC05 )
		( BOX-AT-LOC BOX2 LOC05 )
		( BOX-AT-LOC BOX3 LOC05 )
		( BOX-AT-LOC BOX4 LOC05 )
		( BOX-AT-LOC BOX5 LOC05 )
		( BOX-AT-LOC BOX01 LOC5 )
		( BOX-AT-LOC BOX02 LOC5 )
		( BOX-AT-LOC BOX03 LOC5 )
		( BOX-AT-LOC BOX04 LOC5 )
		( BOX-AT-LOC BOX05 LOC5 )
		( AFREE )
	)
)
)
