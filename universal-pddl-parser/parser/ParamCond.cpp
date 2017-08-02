
#include "Domain.h"

namespace parser { namespace pddl {

void ParamCond::printParams( unsigned first, std::ostream & s, TokenStruct< std::string > & ts, const Domain & d ) const {
	s << "(";
	for ( unsigned i = first; i < params.size(); ++i ) {
		std::stringstream ss;		
		ss << "?" << d.types[params[i]]->getName() << ts.size();
		ts.insert( ss.str() );
		s << " " << ss.str();
		if ( d.typed ) s << " - " << d.types[params[i]]->name;
	}
	s << " )\n";
}

// added by shashank
void ParamCond::printParams( unsigned first, std::ostream & s, TokenStruct< std::string > & ts, const Domain & d, unsigned noOfAgents) const {	
	/** joint-sail ( agt1, agt2, agt3, ...., boat1, loc1, loc2, cntr1, cntr2 )
		noOfAgents - contains the number of agents involved in this activity */
	s << "(";
	for ( unsigned i = first; i < params.size(); ++i ) {
		std::stringstream ss;
		if ( ! ( i >= 1 && i < noOfAgents ) ) {
			ss << "?" << d.types[params[i]]->getName() << ts.size();
		}
		ts.insert( ss.str() );
		if ( ! ( i >= 1 && i < noOfAgents ) )  // just to maintain the style
			s << " " << ss.str();
				
		if ( d.typed && ! ( i >= 1 && i < noOfAgents ) ) s << " - " << d.types[params[i]]->name;
	}
	s << " )\n";	
}

} } // namespaces


