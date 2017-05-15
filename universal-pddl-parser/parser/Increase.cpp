
#include "Domain.h"

namespace parser { namespace pddl {

Increase::Increase( const Increase * i, Domain & d )
	: value( i->value ), ground( 0 ) {
	if ( i->ground ) ground = new Ground( d.funcs.get( i->ground->name ) );
}

void Increase::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, const Domain & d ) const {
	tabindent( s, indent );
	s << "( INCREASE ( TOTAL-COST ) ";
	if ( ground ) ground->PDDLPrint( s, 0, ts, d );
	else s << ( int )value;
	s << " )";
}

void Increase::parse( Filereader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert_token( "(" );
	f.assert_token( "TOTAL-COST" );
	f.assert_token( ")" );
	if ( f.getChar() == '(' ) {
		f.assert_token( "(" );
		ground = new Ground( d.funcs.get( f.getToken( d.funcs ) ) );
		ground->parse( f, ts, d );
	}
	else {
		std::stringstream ss( f.getToken() );
		ss >> value;
	}
	f.next();
	f.assert_token( ")" );
}

} } // namespaces
