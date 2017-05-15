( DEFINE ( DOMAIN MAPANALYZER )

( :REQUIREMENTS :TYPING :DURATIVE-ACTIONS )

( :TYPES
	CAR - VEHICLE
	VEHICLE - OBJECT
	JUNCTION - OBJECT
	GARAGE - OBJECT
	ROAD - OBJECT
)

( :PREDICATES
	( CONNECTED ?JUNCTION0 - JUNCTION ?JUNCTION1 - JUNCTION )
	( AT_JUN ?VEHICLE0 - VEHICLE ?JUNCTION1 - JUNCTION )
	( STARTING ?VEHICLE0 - VEHICLE ?GARAGE1 - GARAGE )
	( ARRIVED ?VEHICLE0 - VEHICLE ?JUNCTION1 - JUNCTION )
	( ROAD_CONNECT ?ROAD0 - ROAD ?JUNCTION1 - JUNCTION ?JUNCTION2 - JUNCTION )
	( CLEAR ?JUNCTION0 - JUNCTION )
	( IN_PLACE ?ROAD0 - ROAD )
	( AVAILABLE ?ROAD0 - ROAD )
	( AT_GARAGE ?GARAGE0 - GARAGE ?JUNCTION1 - JUNCTION )
)

( :FUNCTIONS
	( DISTANCE ?JUNCTION0 - JUNCTION ?JUNCTION1 - JUNCTION )
	( BUILD-TIME )
	( REMOVE-TIME )
	( SPEED ?VEHICLE0 - VEHICLE )
	( ARRIVED-TIME )
	( START-TIME )
)

( :DURATIVE-ACTION MOVE_VEHICLE_ROAD
  :PARAMETERS ( ?JUNCTION0 - JUNCTION ?JUNCTION1 - JUNCTION ?VEHICLE2 - VEHICLE ?ROAD3 - ROAD )
  :DURATION ( = ?DURATION ( / ( DISTANCE ?JUNCTION0 ?JUNCTION1 ) ( SPEED ?VEHICLE2 ) ) )
  :CONDITION
	( AND
		( AT START ( AT_JUN ?VEHICLE2 ?JUNCTION0 ) )
		( AT START ( ROAD_CONNECT ?ROAD3 ?JUNCTION0 ?JUNCTION1 ) )
		( AT START ( IN_PLACE ?ROAD3 ) )
		( AT END ( CLEAR ?JUNCTION1 ) )
	)
  :EFFECT
	( AND
		( AT START ( CLEAR ?JUNCTION0 ) )
		( AT START ( NOT ( AT_JUN ?VEHICLE2 ?JUNCTION0 ) ) )
		( AT END ( AT_JUN ?VEHICLE2 ?JUNCTION1 ) )
	)
)

( :DURATIVE-ACTION VEHICLE_ARRIVED
  :PARAMETERS ( ?JUNCTION0 - JUNCTION ?VEHICLE1 - VEHICLE )
  :DURATION ( = ?DURATION ( ARRIVED-TIME ) )
  :CONDITION
	( AND
		( AT START ( AT_JUN ?VEHICLE1 ?JUNCTION0 ) )
	)
  :EFFECT
	( AND
		( AT END ( CLEAR ?JUNCTION0 ) )
		( AT END ( ARRIVED ?VEHICLE1 ?JUNCTION0 ) )
		( AT END ( NOT ( AT_JUN ?VEHICLE1 ?JUNCTION0 ) ) )
	)
)

( :DURATIVE-ACTION VEHICLE_START
  :PARAMETERS ( ?JUNCTION0 - JUNCTION ?VEHICLE1 - VEHICLE ?GARAGE2 - GARAGE )
  :DURATION ( = ?DURATION ( START-TIME ) )
  :CONDITION
	( AND
		( AT START ( AT_GARAGE ?GARAGE2 ?JUNCTION0 ) )
		( AT START ( STARTING ?VEHICLE1 ?GARAGE2 ) )
		( AT START ( CLEAR ?JUNCTION0 ) )
	)
  :EFFECT
	( AND
		( AT START ( NOT ( STARTING ?VEHICLE1 ?GARAGE2 ) ) )
		( AT END ( NOT ( CLEAR ?JUNCTION0 ) ) )
		( AT END ( AT_JUN ?VEHICLE1 ?JUNCTION0 ) )
	)
)

( :DURATIVE-ACTION BUILD_ROAD
  :PARAMETERS ( ?JUNCTION0 - JUNCTION ?JUNCTION1 - JUNCTION ?ROAD2 - ROAD )
  :DURATION ( = ?DURATION ( * ( DISTANCE ?JUNCTION0 ?JUNCTION1 ) ( BUILD-TIME ) ) )
  :CONDITION
	( AND
		( AT START ( CLEAR ?JUNCTION1 ) )
		( AT START ( AVAILABLE ?ROAD2 ) )
		( AT START ( CONNECTED ?JUNCTION0 ?JUNCTION1 ) )
	)
  :EFFECT
	( AND
		( AT START ( IN_PLACE ?ROAD2 ) )
		( AT START ( NOT ( AVAILABLE ?ROAD2 ) ) )
		( AT END ( ROAD_CONNECT ?ROAD2 ?JUNCTION0 ?JUNCTION1 ) )
		( AT END ( ROAD_CONNECT ?ROAD2 ?JUNCTION1 ?JUNCTION0 ) )
	)
)

( :DURATIVE-ACTION REMOVE_ROAD
  :PARAMETERS ( ?JUNCTION0 - JUNCTION ?JUNCTION1 - JUNCTION ?ROAD2 - ROAD )
  :DURATION ( = ?DURATION ( * ( DISTANCE ?JUNCTION0 ?JUNCTION1 ) ( REMOVE-TIME ) ) )
  :CONDITION
	( AND
		( AT START ( ROAD_CONNECT ?ROAD2 ?JUNCTION0 ?JUNCTION1 ) )
		( AT START ( ROAD_CONNECT ?ROAD2 ?JUNCTION1 ?JUNCTION0 ) )
		( AT START ( IN_PLACE ?ROAD2 ) )
	)
  :EFFECT
	( AND
		( AT START ( NOT ( ROAD_CONNECT ?ROAD2 ?JUNCTION0 ?JUNCTION1 ) ) )
		( AT START ( NOT ( ROAD_CONNECT ?ROAD2 ?JUNCTION1 ?JUNCTION0 ) ) )
		( AT END ( NOT ( IN_PLACE ?ROAD2 ) ) )
		( AT END ( AVAILABLE ?ROAD2 ) )
	)
)
)