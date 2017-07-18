
#include "Domain.h"

namespace parser { namespace pddl {

void Action::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, const Domain & d ) const 
{
	if ( name.find("ACTIVITY") != std::string::npos ) {
		s << "( :ACTION " << name << "\n";

		s << "  :PARAMETERS ";
		TokenStruct< std::string > astruct;
		
		/* changes for printing the parameters */
		unsigned noOfAgents = 0;
		for ( unsigned i = 0; i < params.size(); ++i ) {
			if ( d.types[params[i]]->getName().find("AGENT") != std::string::npos )
				if ( ! ( d.types[params[i]]->getName().find("AGENT-") != std::string::npos ) )
					noOfAgents++;
		}
		printParams( 0, s, astruct, d, noOfAgents);
		
		s << "  :PRECONDITION\n";
		if ( pre ) pre->PDDLPrint( s, 1, astruct, d );
		else s << "\t()";
		s << "\n";

		s << "  :EFFECT\n";
		if ( eff ) eff->PDDLPrint( s, 1, astruct, d );
		else s << "\t()";
		s << "\n";

		s << ")\n";
	}	
	else {
		s << "( :ACTION " << name << "\n";

		s << "  :PARAMETERS ";

		TokenStruct< std::string > astruct;

		printParams( 0, s, astruct, d );

		s << "  :PRECONDITION\n";
		if ( pre ) pre->PDDLPrint( s, 1, astruct, d );
		else s << "\t()";
		s << "\n";

		s << "  :EFFECT\n";
		if ( eff ) eff->PDDLPrint( s, 1, astruct, d );
		else s << "\t()";
		s << "\n";

		s << ")\n";
	}
}

void Action::parseConditions( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert_token( ":" );
	std::string s = f.getToken();
	if ( s == "PRECONDITION" ) {
		f.next();
		f.assert_token( "(" );
		if ( f.getChar() != ')' ) {
			pre = createCondition( f, d );
			pre->parse( f, ts, d );
		}
		else ++f.c;

		f.next();
		f.assert_token( ":" );
		s = f.getToken();
	}
	if ( s != "EFFECT" ) f.tokenExit( s );
		
	f.next();
	f.assert_token( "(" );
	if ( f.getChar() != ')' ) {
		eff = createCondition( f, d );
		eff->parse( f, ts, d );
	}
	else ++f.c;
	f.next();
	f.assert_token( ")" );
}

void Action::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert_token( ":PARAMETERS" );
	f.assert_token( "(" );

	TokenStruct< std::string > astruct = f.parseTypedList( true, d.types );
	params = d.convertTypes( astruct.types );

	parseConditions( f, astruct, d );
}

CondVec Action::precons() {
	And * a = dynamic_cast< And * >( pre );
	if ( a ) return a->conds;

	CondVec precons;
	if ( pre ) precons.push_back( pre );
	return precons;
}

GroundVec Action::addEffects() {
	GroundVec adds;
	And * a = dynamic_cast< And * >( eff );
	for ( unsigned i = 0; a && i < a->conds.size(); ++i ) {
		Ground * g = dynamic_cast< Ground * >( a->conds[i] );
		if ( g ) adds.push_back( g );
	}

	Ground * g = dynamic_cast< Ground * >( eff );
	if ( g ) adds.push_back( g );

	return adds;
}

GroundVec Action::deleteEffects() {
	GroundVec deletes;
	And * a = dynamic_cast< And * >( eff );
	for ( unsigned i = 0; a && i < a->conds.size(); ++i ) {
		Not * n = dynamic_cast< Not * >( a->conds[i] );
		if ( n ) deletes.push_back( n->cond );
	}

	Not * n = dynamic_cast< Not * >( eff );
	if ( n ) deletes.push_back( n->cond );

	return deletes;
}

} } // namespaces
