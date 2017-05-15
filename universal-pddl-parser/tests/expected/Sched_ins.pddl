( DEFINE ( PROBLEM SCHEDULE-2-0 )
( :DOMAIN SCHEDULE )
( :OBJECTS
	TWO THREE BLUE YELLOW BACK RED B0 FRONT ONE BLACK OBLONG A0 
)
( :INIT
	( IDLE PUNCH )
	( IDLE DRILL-PRESS )
	( IDLE LATHE )
	( IDLE ROLLER )
	( IDLE POLISHER )
	( IDLE IMMERSION-PAINTER )
	( IDLE SPRAY-PAINTER )
	( IDLE GRINDER )
	( RU UNWANTEDARGS )
	( SHAPE A0 OBLONG )
	( SURFACE-CONDITION A0 SMOOTH )
	( PAINTED A0 BLACK )
	( HAS-HOLE A0 ONE FRONT )
	( LASTHOLE A0 ONE FRONT )
	( LINKED A0 NOWIDTH NOORIENT ONE FRONT )
	( TEMPERATURE A0 COLD )
	( SHAPE B0 OBLONG )
	( SURFACE-CONDITION B0 SMOOTH )
	( PAINTED B0 RED )
	( HAS-HOLE B0 ONE FRONT )
	( LASTHOLE B0 ONE FRONT )
	( LINKED B0 NOWIDTH NOORIENT ONE FRONT )
	( TEMPERATURE B0 COLD )
	( CAN-ORIENT DRILL-PRESS BACK )
	( CAN-ORIENT PUNCH BACK )
	( CAN-ORIENT DRILL-PRESS FRONT )
	( CAN-ORIENT PUNCH FRONT )
	( HAS-PAINT IMMERSION-PAINTER YELLOW )
	( HAS-PAINT SPRAY-PAINTER YELLOW )
	( HAS-PAINT IMMERSION-PAINTER BLUE )
	( HAS-PAINT SPRAY-PAINTER BLUE )
	( HAS-PAINT IMMERSION-PAINTER BLACK )
	( HAS-PAINT SPRAY-PAINTER BLACK )
	( HAS-PAINT IMMERSION-PAINTER RED )
	( HAS-PAINT SPRAY-PAINTER RED )
	( HAS-BIT DRILL-PRESS THREE )
	( HAS-BIT PUNCH THREE )
	( HAS-BIT DRILL-PRESS TWO )
	( HAS-BIT PUNCH TWO )
	( HAS-BIT DRILL-PRESS ONE )
	( HAS-BIT PUNCH ONE )
	( PART B0 )
	( UNSCHEDULED B0 )
	( PART A0 )
	( UNSCHEDULED A0 )
)
( :GOAL
	( AND
		( SHAPE B0 CYLINDRICAL )
		( SHAPE A0 CYLINDRICAL )
	)
)
)