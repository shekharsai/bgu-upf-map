;;
;;	BOXPUSHING DOMAIN 
;;
( DEFINE ( DOMAIN BOXPUSHING )
( :REQUIREMENTS :TYPING )
( :TYPES
	AGENT - OBJECT
	BOX - OBJECT
	LOCATION - OBJECT
	AGENT-COUNT - OBJECT
)
( :CONSTANTS
	ACOUNT-0 - AGENT-COUNT
)
( :PREDICATES
	( BOX-AT-LOC ?BOX0 - BOX ?LOCATION1 - LOCATION )
	( POS-BOX-AT-LOC ?BOX0 - BOX ?LOCATION1 - LOCATION )
	( NEG-BOX-AT-LOC ?BOX0 - BOX ?LOCATION1 - LOCATION )
	( AGENT-AT-LOC ?AGENT0 - AGENT ?LOCATION1 - LOCATION )
	( CONNECTED ?LOCATION0 - LOCATION ?LOCATION1 - LOCATION )
	( TIRED ?AGENT0 - AGENT )
	( TAKEN ?AGENT0 - AGENT )
	( CONSEC ?AGENT-COUNT0 - AGENT-COUNT ?AGENT-COUNT1 - AGENT-COUNT )
	( ACTIVE-V2 ?BOX0 - BOX ?LOCATION1 - LOCATION ?LOCATION2 - LOCATION )
	( COUNT-V2 ?AGENT-COUNT0 - AGENT-COUNT )
	( SAT-V2 ?AGENT-COUNT0 - AGENT-COUNT )
	( AFREE )
	( IN-JOINT )
	( P-ACTIVITY-PUSH-BOX ?BOX0 - BOX ?LOCATION1 - LOCATION ?LOCATION2 - LOCATION )
	( P-PUSH-BOX ?BOX0 - BOX ?LOCATION1 - LOCATION ?LOCATION2 - LOCATION )
	( NEXT-ACTIVITY-PUSH-BOX-2 ?BOX0 - BOX ?LOCATION1 - LOCATION ?LOCATION2 - LOCATION )
)
( :ACTION START-V2
  :PARAMETERS ( ?BOX0 - BOX ?LOCATION1 - LOCATION ?LOCATION2 - LOCATION )
  :PRECONDITION
	( AND
		( AFREE )
	)
  :EFFECT
	( AND
		( NOT ( AFREE ) )
		( ACTIVE-V2 ?BOX0 ?LOCATION1 ?LOCATION2 )
		( COUNT-V2 ACOUNT-0 )
	)
)
( :ACTION DO-PUSH-BOX
  :PARAMETERS ( ?AGENT0 - AGENT ?BOX1 - BOX ?LOCATION2 - LOCATION ?LOCATION3 - LOCATION ?AGENT-COUNT4 - AGENT-COUNT ?AGENT-COUNT5 - AGENT-COUNT )
  :PRECONDITION
	( AND
		( AGENT-AT-LOC ?AGENT0 ?LOCATION2 )
		( BOX-AT-LOC ?BOX1 ?LOCATION2 )
		( CONNECTED ?LOCATION2 ?LOCATION3 )
		( NOT ( TIRED ?AGENT0 ) )
		( NOT ( IN-JOINT ) )
		( ACTIVE-V2 ?BOX1 ?LOCATION2 ?LOCATION3 )
		( NOT ( TAKEN ?AGENT0 ) )
		( COUNT-V2 ?AGENT-COUNT4 )
		( CONSEC ?AGENT-COUNT4 ?AGENT-COUNT5 )
		( NOT ( P-ACTIVITY-PUSH-BOX ?BOX1 ?LOCATION2 ?LOCATION3 ) )
		( NOT ( P-PUSH-BOX ?BOX1 ?LOCATION2 ?LOCATION3 ) )
	)
  :EFFECT
	( AND
		( AGENT-AT-LOC ?AGENT0 ?LOCATION3 )
		( POS-BOX-AT-LOC ?BOX1 ?LOCATION3 )
		( NOT ( AGENT-AT-LOC ?AGENT0 ?LOCATION2 ) )
		( NEG-BOX-AT-LOC ?BOX1 ?LOCATION2 )
		( TIRED ?AGENT0 )
		( TAKEN ?AGENT0 )
		( NOT ( COUNT-V2 ?AGENT-COUNT4 ) )
		( COUNT-V2 ?AGENT-COUNT5 )
		( P-PUSH-BOX ?BOX1 ?LOCATION2 ?LOCATION3 )
	)
)
( :ACTION DO-ACTIVITY-PUSH-BOX-1
  :PARAMETERS ( ?AGENT0 - AGENT ?BOX2 - BOX ?LOCATION3 - LOCATION ?LOCATION4 - LOCATION ?AGENT-COUNT5 - AGENT-COUNT ?AGENT-COUNT6 - AGENT-COUNT )
  :PRECONDITION
	( AND
		( AGENT-AT-LOC ?AGENT0 ?LOCATION3 )
		( BOX-AT-LOC ?BOX2 ?LOCATION3 )
		( CONNECTED ?LOCATION3 ?LOCATION4 )
		( NOT ( TIRED ?AGENT0 ) )
		( NOT ( IN-JOINT ) )
		( ACTIVE-V2 ?BOX2 ?LOCATION3 ?LOCATION4 )
		( NOT ( TAKEN ?AGENT0 ) )
		( COUNT-V2 ?AGENT-COUNT5 )
		( CONSEC ?AGENT-COUNT5 ?AGENT-COUNT6 )
		( NOT ( P-PUSH-BOX ?BOX2 ?LOCATION3 ?LOCATION4 ) )
	)
  :EFFECT
	( AND
		( AGENT-AT-LOC ?AGENT0 ?LOCATION4 )
		( POS-BOX-AT-LOC ?BOX2 ?LOCATION4 )
		( NOT ( AGENT-AT-LOC ?AGENT0 ?LOCATION3 ) )
		( NEG-BOX-AT-LOC ?BOX2 ?LOCATION3 )
		( TAKEN ?AGENT0 )
		( NOT ( COUNT-V2 ?AGENT-COUNT5 ) )
		( COUNT-V2 ?AGENT-COUNT6 )
		( IN-JOINT )
		( NEXT-ACTIVITY-PUSH-BOX-2 ?BOX2 ?LOCATION3 ?LOCATION4 )
		( P-ACTIVITY-PUSH-BOX ?BOX2 ?LOCATION3 ?LOCATION4 )
	)
)
( :ACTION DO-ACTIVITY-PUSH-BOX-2
  :PARAMETERS ( ?AGENT0 - AGENT ?BOX2 - BOX ?LOCATION3 - LOCATION ?LOCATION4 - LOCATION ?AGENT-COUNT5 - AGENT-COUNT ?AGENT-COUNT6 - AGENT-COUNT )
  :PRECONDITION
	( AND
		( AGENT-AT-LOC ?AGENT0 ?LOCATION3 )
		( BOX-AT-LOC ?BOX2 ?LOCATION3 )
		( CONNECTED ?LOCATION3 ?LOCATION4 )
		( NOT ( TIRED ?AGENT0 ) )
		( NEXT-ACTIVITY-PUSH-BOX-2 ?BOX2 ?LOCATION3 ?LOCATION4 )
		( ACTIVE-V2 ?BOX2 ?LOCATION3 ?LOCATION4 )
		( NOT ( TAKEN ?AGENT0 ) )
		( COUNT-V2 ?AGENT-COUNT5 )
		( CONSEC ?AGENT-COUNT5 ?AGENT-COUNT6 )
	)
  :EFFECT
	( AND
		( AGENT-AT-LOC ?AGENT0 ?LOCATION4 )
		( POS-BOX-AT-LOC ?BOX2 ?LOCATION4 )
		( NOT ( AGENT-AT-LOC ?AGENT0 ?LOCATION3 ) )
		( NEG-BOX-AT-LOC ?BOX2 ?LOCATION3 )
		( NOT ( IN-JOINT ) )
		( NOT ( NEXT-ACTIVITY-PUSH-BOX-2 ?BOX2 ?LOCATION3 ?LOCATION4 ) )
		( TAKEN ?AGENT0 )
		( NOT ( COUNT-V2 ?AGENT-COUNT5 ) )
		( COUNT-V2 ?AGENT-COUNT6 )
	)
)
( :ACTION END-V2
  :PARAMETERS ( ?BOX0 - BOX ?LOCATION1 - LOCATION ?LOCATION2 - LOCATION ?AGENT-COUNT3 - AGENT-COUNT )
  :PRECONDITION
	( AND
		( NOT ( IN-JOINT ) )
		( COUNT-V2 ?AGENT-COUNT3 )
		( SAT-V2 ?AGENT-COUNT3 )
		( ACTIVE-V2 ?BOX0 ?LOCATION1 ?LOCATION2 )
	)
  :EFFECT
	( AND
		( NOT ( COUNT-V2 ?AGENT-COUNT3 ) )
		( AFREE )
		( NOT ( ACTIVE-V2 ?BOX0 ?LOCATION1 ?LOCATION2 ) )
		( NOT ( P-PUSH-BOX ?BOX0 ?LOCATION1 ?LOCATION2 ) )
		( NOT ( P-ACTIVITY-PUSH-BOX ?BOX0 ?LOCATION1 ?LOCATION2 ) )
		( FORALL
			( ?AGENT4 - AGENT )
			( NOT ( TAKEN ?AGENT4 ) )
		)
		( FORALL
			( ?LOCATION4 - LOCATION )
			( AND
				( WHEN
					( POS-BOX-AT-LOC ?BOX0 ?LOCATION4 )
					( AND
						( BOX-AT-LOC ?BOX0 ?LOCATION4 )
						( NOT ( POS-BOX-AT-LOC ?BOX0 ?LOCATION4 ) )
					)
				)
				( WHEN
					( NEG-BOX-AT-LOC ?BOX0 ?LOCATION4 )
					( AND
						( NOT ( BOX-AT-LOC ?BOX0 ?LOCATION4 ) )
						( NOT ( NEG-BOX-AT-LOC ?BOX0 ?LOCATION4 ) )
					)
				)
			)
		)
	)
)
)

;Total compilation time = 0.002273

