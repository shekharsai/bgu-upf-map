( DEFINE ( PROBLEM TABLE1_1 )
( :DOMAIN TABLEMOVER )
( :OBJECTS
	A1 A2 A3 - AGENT
	T1 T2 - TABLE
	R1X1 R1X2 R2X1 R2X2 - ROOM
	ACOUNT-1 ACOUNT-2 - AGENT-COUNT
)
( :INIT
	( IN A1 R1X1 )
	( IN A2 R1X1 )
	( IN A3 R1X1 )
	( INROOM T1 R1X1 )
	( INROOM T2 R2X1 )
	( AVAILABLE A1 )
	( AVAILABLE A2 )
	( AVAILABLE A3 )
	( CONNECTED R1X1 R1X2 )
	( CONNECTED R1X2 R1X1 )
	( CONNECTED R1X1 R2X1 )
	( CONNECTED R2X1 R1X1 )
	( CONNECTED R2X1 R2X2 )
	( CONNECTED R2X2 R2X1 )
	( CONNECTED R1X2 R2X2 )
	( CONNECTED R2X2 R1X2 )
	( INTACT T1 )
	( INTACT T2 )
	( HAS-CHARGING-POINT R1X1 )
	( AFREE )
	( CONSEC ACOUNT-0 ACOUNT-1 )
	( CONSEC ACOUNT-1 ACOUNT-2 )
	( SAT-V2 ACOUNT-1 )
	( SAT-V2 ACOUNT-2 )
	( SAT-V3 ACOUNT-1 )
	( SAT-V3 ACOUNT-2 )
	( SAT-V4 ACOUNT-1 )
	( SAT-V4 ACOUNT-2 )
)
( :GOAL
	( AND
		( INROOM T1 R2X2 )
		( INROOM T2 R1X2 )
		( INTACT T1 )
		( INTACT T2 )
		( AFREE )
	)
)
)
