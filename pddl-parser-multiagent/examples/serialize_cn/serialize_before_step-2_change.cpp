/** Some Important Issues
This is a better place to write some important issues.
	1. Have all predicates with different names. Why?
		(a) As POS- NEG- specifically been put using only predicate names.
	2. Have all the agents together in the action parameter list.
		(a) Joint-Activity (a1, a2, a3, ..., ak, objs)
		(b) Irrespective of any type Truck or Plane use AGENT as I have string comparison using 
		this word.
	3. The name of an activity should have "ACTIVITY" string in it. Also, you need to specify more 
	than one agent.  
	4. Currently, it handles constraints over only one object, e.g. a boat, a bridge etc. Easy to 
	extend though. Also, it splits blindly, which means, to have correct splits of a Joint activity,
	make sure that there is no predicates specifying other agents in the eff or preconditions. 
	Little more work is needed to resolve this limitation. 
	5. //TODO During translation an action cannot be public or private. So, remove IN-JOINT from the preconditions in the Shlomi's code.
**/

// To check for memory leaks:
// valgrind --leak-check=yes examples/serialize ../multiagent/codmap/domains/tablemover/tablemover.pddl ../multiagent/codmap/domains/tablemover/table1_1.pddl

#include <parser/Instance.h>
#include <multiagent/MultiagentDomain.h>

using namespace parser::pddl;

parser::multiagent::MultiagentDomain * d;
Instance * ins;
std::set< unsigned> prob;

std::set< std::vector < unsigned > > probVector;

typedef std::map< unsigned, std::vector< int > > VecMap;

// step-2 that is finding conflicting actions e.g. (A1->a,!b, and A2->c,b)
std::map < std::string, std::vector< std::string > > ambiguousActions( const parser::multiagent::NetworkNode * n) {
	std::map < std::string, std::vector < std::string > > listOfAmbiguousActions;
	for ( unsigned i = 0; i < n->templates.size(); ++i ) {			
		Action * legal_a = d->actions[d->actions.index( n->templates[i]->name )];
		bool ambiguous = false;
		std::vector < std::string > ambActions;		
		for ( unsigned j = 0; j < n->templates.size(); j++) {		
			if (i!=j) 
			{			
				Action * probably_legal_a = d->actions[d->actions.index( n->templates[j]->name )];			
				for ( unsigned k = 0; k < legal_a->addEffects().size(); ++k ) 			
					for ( unsigned l = 0; l < probably_legal_a->deleteEffects().size(); ++l ) 
						if ( ( (dynamic_cast< Ground * > (legal_a->addEffects()[k]))->name == 
								(dynamic_cast< Ground * > (probably_legal_a->deleteEffects()[l]))->name) ) { 
							ambiguous = true; 	
							break;												
						}													
				if (ambiguous) 
				{
					ambActions.push_back( (std::string) probably_legal_a->name );
				}
			}
		}
		listOfAmbiguousActions[ (std::string) legal_a->name ] = ambActions;		
	}		
	return listOfAmbiguousActions;
}

// updated by shashank
bool deletes( const Ground * ground, const parser::multiagent::NetworkNode * n, IntVec impParams ) {
	for ( unsigned i = 0; i < n->templates.size(); ++i ) {			
		Action * a = d->actions[d->actions.index( n->templates[i]->name )];		
		CondVec pres = a->precons();		
		for ( unsigned j = 0; j < pres.size(); ++j ) {
			Ground * g = dynamic_cast< Ground * >( pres[j] );
			if ( g && g->name == ground->name &&
			     std::find( g->params.begin(), g->params.end(), impParams[0] ) != g->params.end() )  			    
				return true;
		}
	}
	return false;
}

// returns true if at least one instance of "POS-" or "NEG-" gets added
bool addEff( Domain * cd, Action * a, Condition * c ) {
	Not * n = dynamic_cast< Not * >( c );
	Ground * g = dynamic_cast< Ground * >( c );
	std::set< std::vector < unsigned > >::iterator it;
	for ( it = probVector.begin(); it != probVector.end(); ++it ) { 
		std::vector< unsigned > deleteChoices  = ( std::vector< unsigned > ) *it;
		if ( n && deleteChoices[0] == (unsigned) d->preds.index( n->cond->name ) && (int) deleteChoices[1] == 1) {
			cd->addEff( 0, a->name, "NEG-" + n->cond->name, n->cond->params );
			return 1;
		}
		if ( g && deleteChoices[0] == (unsigned) d->preds.index( g->name ) && (int) deleteChoices[1] == 1 ) {
			cd->addEff( 0, a->name, "POS-" + g->name, g->params );
			return 1;
		}
		if ( n && deleteChoices[0] == (unsigned) d->preds.index( n->cond->name ) && (int) deleteChoices[1] == 2) {
			cd->addEff( 0, a->name, "NEG-" + n->cond->name, n->cond->params );
			return 1;
		}
	}
	
	if ( n )
		cd->addEff( 1, a->name, n->cond->name, n->cond->params );
	else if ( g )
		cd->addEff( 0, a->name, g->name, g->params );
	else if ( c ) {
		if ( !a->eff ) a->eff = new And;
		And * aa = dynamic_cast< And * >( a->eff );
		aa->add( c->copy( *cd ) );
	}

	return 0;
}

int main( int argc, char *argv[] ) {
	if ( argc < 3 ) {
		std::cout << "Usage: ./transform <domain.pddl> <task.pddl>\n";
		exit( 1 );
	}
	
	// Read multiagent domain and problem with associated concurrency network
	d = new parser::multiagent::MultiagentDomain( argv[1] );
	ins = new Instance( *d, argv[2] );
	
	// added by shashank, for identifying problematic action pairs
	std::vector< std::map< std::string, std::map< std::string, std::vector< std::string >>> > pairOfProbActions;
	for ( unsigned i = 0; i < d->nodes.size(); ++i ) {
	 	std::map< std::string, std::map < std::string, std::vector < std::string > > > pair;	
		pair[d->nodes[i]->name] = ambiguousActions( d->nodes[i] );	
		pairOfProbActions.push_back( pair );	
	}
	
	// Identify problematic fluents (preconditions deleted by agents) (edited by shashank) 	
	for ( unsigned i = 0; i < d->nodes.size(); ++i ) {
		for ( unsigned j = 0; d->nodes[i]->upper > 1 && j < d->nodes[i]->templates.size(); ++j ) 
		{	
			// In future impParams may contain multiple entries // TODO
			IntVec impParams;
			impParams = d->nodes[i]->templates[j]->params; 			
			Action * a = d->actions[ d->actions.index( d->nodes[i]->templates[j]->name ) ];
			GroundVec dels = a->deleteEffects();	
			GroundVec added = a->addEffects();	
			unsigned choice = 1;
			for ( unsigned k = 0; k < dels.size(); ++k ) 
			{				
				bool choiceA = 
						std::find( dels[k]->params.begin(), dels[k]->params.end(), impParams[0] ) != dels[k]->params.end();
				bool choiceB = deletes( dels[k], d->nodes[i], impParams );
				bool choiceC = false;
								
				if ( choiceB )
					for ( unsigned t = 0; t < added.size(); t++ )
						if ( added[t]->name == dels[k]->name )
							choiceC = true; // if only gets deleted, but does not get added any propositions.
				if ( choiceA && choiceB && choiceC ) { 	
					choice = 1;	
					probVector.insert( { (unsigned) d->preds.index( dels[k]->name ), choice } );				
				}
				else if( choiceA && choiceB && !choiceC ) {					
					choice = 2;					
					probVector.insert( { (unsigned) d->preds.index( dels[k]->name ), choice } );
				}
			}
		}		
	}
	
	VecMap ccs;
	for ( unsigned i = 0; i < d->mf.size(); ++i )
		ccs[d->mf[i]].push_back( i );

	// Create a classical domain
	Domain * cd = new Domain;
	cd->name = d->name;
	cd->condeffects = d->condeffects; 
	cd->cons = cd->typed = true;
	cd->factored = d->factored;
	cd->unfactored = d->unfactored;
	cd->neg = d->neg;
	
	// Add types
	cd->setTypes( d->copyTypes() );
	cd->createType( "AGENT-COUNT" );

	// Add constants
	cd->createConstant( "ACOUNT-0", "AGENT-COUNT" );

	// Add predicates (a huge set of bugs removed by shashank) 
	for ( unsigned i = 0; i < d->preds.size(); ++i ) {
		cd->createPredicate( d->preds[i]->name, d->typeList( d->preds[i] ) );
		std::set< std::vector < unsigned > >::iterator it;
		for ( it = probVector.begin(); it != probVector.end(); ++it ) 
		{ 
			std::vector< unsigned > deleteChoices  = ( std::vector< unsigned > ) *it;
			if ( deleteChoices[0] == i && deleteChoices[1] == (unsigned) 1 ) {				
				cd->createPredicate( "POS-" + d->preds[i]->name, d->typeList(d->preds[i]) );
				cd->createPredicate( "NEG-" + d->preds[i]->name, d->typeList(d->preds[i]) );
			}
						
			if ( deleteChoices[0] == i && deleteChoices[1] == (unsigned) 2 ) {
				cd->createPredicate( "NEG-" + d->preds[i]->name, d->typeList(d->preds[i]) );
			}
		}
	}		
	
	cd->createPredicate( "TAKEN", StringVec( 1, "AGENT" ) );
	cd->createPredicate( "CONSEC", StringVec( 2, "AGENT-COUNT" ) );
	
	for ( unsigned i = 0; i < d->nodes.size(); ++i ) {
		VecMap::iterator j = ccs.find( d->mf[i] );
		if ( j->second.size() > 1 || d->nodes[i]->upper > 1 ) {
			cd->createPredicate( "ACTIVE-" + d->nodes[i]->name, d->typeList( d->nodes[i] ) );
			cd->createPredicate( "COUNT-" + d->nodes[i]->name, StringVec( 1, "AGENT-COUNT" ) );
			cd->createPredicate( "SAT-" + d->nodes[i]->name, StringVec( 1, "AGENT-COUNT" ) );
		}
		if ( j->second.size() > 1 ) {
			cd->createPredicate( "USED-" + d->nodes[i]->name );
			cd->createPredicate( "DONE-" + d->nodes[i]->name );
			cd->createPredicate( "SKIPPED-" + d->nodes[i]->name );
		}
	}
	cd->createPredicate( "AFREE" );
	cd->createPredicate( "IN-JOINT" ); // in case if a joint activity can also participate
	
	// generate the updated actions
	for ( VecMap::iterator i = ccs.begin(); i != ccs.end(); ++i ) {
		std::set< unsigned > visited;
		for ( unsigned j = 0; j < i->second.size(); ++j ) {			
			int x = i->second[j];
			visited.insert( x );

			if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
				std::string name = "START-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				cd->createAction( name, d->typeList( d->nodes[x] ) );

				if ( j > 0 ) {
					for ( unsigned k = 0; k < d->edges.size(); ++k )
						if ( d->edges[k].second == x ) {
							std::set< unsigned >::iterator it = visited.find( d->edges[k].first );
							if ( it != visited.end() )
								cd->addPre( 0, name, "DONE-" + d->nodes[d->edges[k].first]->name );
						}
					cd->addOrPre( name, "DONE-" + d->nodes[i->second[j - 1]]->name, "SKIPPED-" + d->nodes[i->second[j - 1]]->name );
					cd->addPre( 0, name, "ACTIVE-" + d->nodes[i->second[j - 1]]->name, incvec( 0, size ) );
					cd->addPre( 1, name, "USED-" + d->nodes[x]->name );
					
				}
				else cd->addPre( 0, name, "AFREE" );

				if ( j < 1 ) cd->addEff( 1, name, "AFREE" );
				cd->addEff( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );
				cd->addEff( 0, name, "COUNT-" + d->nodes[x]->name, IntVec( 1, -1 ) );
				if ( i->second.size() > 1 )
					cd->addEff( 0, name, "USED-" + d->nodes[x]->name );
			}

			if ( i->second.size() > 1 ) {
				std::string name = "SKIP-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				cd->createAction( name, d->typeList( d->nodes[x] ) );

				if ( j > 0 ) {
					for ( unsigned k = 0; k < d->edges.size(); ++k )
						if ( d->edges[k].first == i->second[j] ) {
							std::set< unsigned >::iterator it = visited.find( d->edges[k].second );
							if ( it != visited.end() )
								cd->addPre( 0, name, "SKIPPED-" + d->nodes[d->edges[k].second]->name );
						}
					cd->addOrPre( name, "DONE-" + d->nodes[i->second[j - 1]]->name, "SKIPPED-" + d->nodes[i->second[j - 1]]->name );
					cd->addPre( 0, name, "ACTIVE-" + d->nodes[i->second[j - 1]]->name, incvec( 0, size ) );
					cd->addPre( 1, name, "USED-" + d->nodes[x]->name );
				}
				else cd->addPre( 0, name, "AFREE" );

				if ( !j ) cd->addEff( 1, name, "AFREE" );
				cd->addEff( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );
				cd->addEff( 0, name, "SKIPPED-" + d->nodes[x]->name );
				cd->addEff( 0, name, "USED-" + d->nodes[x]->name );  
			}
			
			// the actual action list starts from here
			bool concurEffs = 0;
			for ( unsigned k = 0; k < d->nodes[x]->templates.size(); ++k ) {			
				int action = d->actions.index( d->nodes[x]->templates[k]->name );
				std::string name = d->actions[action]->name;
				
				// TODO // Code below is for splitting a joint activity
				if ( name.find("ACTIVITY") != std::string::npos ) 
				{										
					// creating the start part of the Joint Activity									
					const std::string start_JA = name + "-1";
					StringVec ja_type_list = d->typeList( d->actions[action] );
					int noOfAgents = 0;
					 
					// updated parameter list : one of the ways to do it
					for ( unsigned p = 0; p < ja_type_list.size(); p++ )
						if ( ja_type_list[p] == "AGENT" )
							noOfAgents++;					
							
					// create extra predicates for Joint Activity
					for (int p = 2; p <= noOfAgents; p++) {
						std::stringstream convert; convert << p;
						cd->createPredicate( "NEXT-"+ name + "-" + convert.str() );						
					}
					Action * do_start_part = cd->createAction( start_JA, d->typeList( d->actions[action] ) );
					unsigned size = do_start_part->params.size();	
															
					// copy old preconditions
					And * oldpre = dynamic_cast< And * >( d->actions[action]->pre );												
					if ( oldpre ) 
						do_start_part->pre = new And( oldpre, *cd );
					else {
						And * a = new And;
						a->add( d->actions[action]->pre->copy( *cd ) );
						do_start_part->pre = a;
					}
					
					// copy old effects					
					And * oldeff = dynamic_cast< And * >( d->actions[action]->eff );					
					for ( unsigned l = 0; oldeff && l < oldeff->conds.size(); ++l )
						concurEffs |= addEff( cd, do_start_part, oldeff->conds[l] );
					if ( !oldeff ) 
						concurEffs |= addEff( cd, do_start_part, d->actions[action]->eff );
					
					// add new parameters
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 )						
						cd->addParams( start_JA, StringVec( 2, "AGENT-COUNT" ) );
					
					// add new preconditions
					TokenStruct< Lifted * > predcts = cd->listOfPredicates();
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						// ecai 2014
						cd->addPre( 1, start_JA, "IN-JOINT" );	
						cd->addPre( 0, start_JA, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );						
						cd->addPre( 1, start_JA, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, start_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, start_JA, "CONSEC", incvec( size, size + 2 ) );
						
						// additional, as per step 2 pairOfProbActions
						for (unsigned t = 0; t < pairOfProbActions.size(); t++) {
							std::map < std::string, std::vector < std::string > > mappedProbActions;
							mappedProbActions = pairOfProbActions[t][d->nodes[x]->name];							
						 	std::vector< std::string > listAction = mappedProbActions[(std::string) name];
						 	if ( listAction.size() > 0 ) 
						 		for (unsigned y = 0; y < listAction.size(); y++) {
						 			bool exists = false;
						 			for (unsigned z = 0; z < predcts.size(); z++) 
						 				if (predcts[z]->name == "P-" + listAction[y]) 
						 					exists = true;						 			
						 			if (! exists)
						 				cd->createPredicate( "P-" + listAction[y] );						
						 			cd->addPre( 1, start_JA, "P-" + listAction[y] );
						 		}
						 	break;
						}							
					}
					else 
						cd->addPre( 0, start_JA, "AFREE" );
					
					// add new effects
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {								
						cd->addEff( 0, start_JA, "TAKEN", IntVec( 1, 0 ) );						
						cd->addEff( 1, start_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, start_JA, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
						cd->addEff( 0, start_JA, "IN-JOINT" );
						std::stringstream convert; convert << 2;
						cd->addEff( 0, start_JA, "NEXT-"+ name + "-" + convert.str());	
							
						// step-2: aaai2018 
						bool exists = false;
			 			for (unsigned z = 0; z < predcts.size(); z++) 
			 				if (predcts[z]->name == "P-" + name) 
			 					exists = true;						 			
			 			if (! exists)
			 				cd->createPredicate( "P-" + name );
						cd->addEff( 0, start_JA, "P-" + name );
					}	
					
					// intermediate split action
					for ( unsigned agent = 2; agent < (unsigned) noOfAgents; agent++ ) {
						std::stringstream convert;
						convert << agent;
						const std::string intermediate_JA = name +"-"+ convert.str();					
						Action * doSecondPart = cd->createAction( intermediate_JA, d->typeList( d->actions[action] ) );				
						size = doSecondPart->params.size();	
						
						// copy old preconditions
						oldpre = dynamic_cast< And * >( d->actions[action]->pre );					
						if ( oldpre ) 
							doSecondPart->pre = new And( oldpre, *cd );
						else {
							And * a = new And;
							a->add( d->actions[action]->pre->copy( *cd ) );
							doSecondPart->pre = a;
						}
					
						// copy old effects					
						oldeff = dynamic_cast< And * >( d->actions[action]->eff );					
						for ( unsigned l = 0; oldeff && l < oldeff->conds.size(); ++l )
							concurEffs |= addEff( cd, doSecondPart, oldeff->conds[l] );
						if ( !oldeff ) 
							concurEffs |= addEff( cd, doSecondPart, d->actions[action]->eff );
					
						// add new parameters
						if ( i->second.size() > 1 || d->nodes[x]->upper > 1 )						
							cd->addParams( intermediate_JA, StringVec( 2, "AGENT-COUNT" ) );
					
						// add new preconditions
						if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
							cd->addPre( 0, intermediate_JA, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
							cd->addPre( 1, intermediate_JA, "TAKEN", IntVec( 1, 0 ) );
							cd->addPre( 0, intermediate_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
							cd->addPre( 0, intermediate_JA, "CONSEC", incvec( size, size + 2 ) );
							std::stringstream convert; convert << agent;
							cd->addPre( 0, intermediate_JA, "NEXT-"+ name + "-" + convert.str() );	
						}
						else 
							cd->addPre( 0, intermediate_JA, "AFREE" );
					
						// add new effects
						if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {														
							cd->addEff( 0, intermediate_JA, "TAKEN", IntVec( 1, 0 ) );
							cd->addEff( 1, intermediate_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
							cd->addEff( 0, intermediate_JA, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
							std::stringstream convert; convert << agent + 1;
							cd->addEff( 0, intermediate_JA, "NEXT-"+ name + "-" + convert.str() );
							cd->addEff( 1, intermediate_JA, "NEXT-"+ name + "-" + convert.str() );
						}
					}	
									
					// creating the last part of the joint action	
					std::stringstream convert;
					convert << noOfAgents;
					const std::string end_JA = name + "-" + convert.str();					
					Action * doSecondPart = cd->createAction( end_JA, d->typeList( d->actions[action] ) );				
					size = doSecondPart->params.size();	
						
					// copy old preconditions
					oldpre = dynamic_cast< And * >( d->actions[action]->pre );					
					if ( oldpre ) 
						doSecondPart->pre = new And( oldpre, *cd );
					else {
						And * a = new And;
						a->add( d->actions[action]->pre->copy( *cd ) );
						doSecondPart->pre = a;
					}
					
					// copy old effects					
					oldeff = dynamic_cast< And * >( d->actions[action]->eff );					
					for ( unsigned l = 0; oldeff && l < oldeff->conds.size(); ++l )
						concurEffs |= addEff( cd, doSecondPart, oldeff->conds[l] );
					if ( !oldeff ) 
						concurEffs |= addEff( cd, doSecondPart, d->actions[action]->eff );
					
					// add new parameters
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 )						
						cd->addParams( end_JA, StringVec( 2, "AGENT-COUNT" ) );
					
					// add new preconditions
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						std::stringstream convert; convert << noOfAgents;
						cd->addPre( 0, end_JA, "NEXT-"+ name + "-" + convert.str() );
						cd->addPre( 0, end_JA, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
						cd->addPre( 1, end_JA, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, end_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, end_JA, "CONSEC", incvec( size, size + 2 ) );							
					}
					else 
						cd->addPre( 0, end_JA, "AFREE" );
					
					// add new effects
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addEff( 1, end_JA, "IN-JOINT" );	
						std::stringstream convert; convert << noOfAgents;
						cd->addEff( 1, end_JA, "NEXT-"+ name + "-" + convert.str() );					
						cd->addEff( 0, end_JA, "TAKEN", IntVec( 1, 0 ) );
						cd->addEff( 1, end_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, end_JA, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
					}
				} 
				else { 				
					name = "DO-" + d->actions[action]->name;
					unsigned size = d->actions[action]->params.size();
					Action * doit = cd->createAction( name, d->typeList( d->actions[action] ) );
					
					// copy old preconditions
					And * oldpre = dynamic_cast< And * >( d->actions[action]->pre );
					if ( oldpre ) doit->pre = new And( oldpre, *cd );
					else {
						And * a = new And;
						a->add( d->actions[action]->pre->copy( *cd ) );
						doit->pre = a;
					}

					// copy old effects
					And * oldeff = dynamic_cast< And * >( d->actions[action]->eff );
					for ( unsigned l = 0; oldeff && l < oldeff->conds.size(); ++l )
						concurEffs |= addEff( cd, doit, oldeff->conds[l] );
					if ( !oldeff ) concurEffs |= addEff( cd, doit, d->actions[action]->eff );

					// add new parameters
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 )
						cd->addParams( name, StringVec( 2, "AGENT-COUNT" ) );

					// add new preconditions
					TokenStruct< Lifted * > predcts = cd->listOfPredicates();
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addPre( 1, name, "IN-JOINT" ); 
						cd->addPre( 0, name, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
						cd->addPre( 1, name, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, name, "CONSEC", incvec( size, size + 2 ) );

						// additional, as per step-2, pairOfProbActions
						for (unsigned t = 0; t < pairOfProbActions.size(); t++) {
							std::map < std::string, std::vector < std::string > > mappedProbActions;
							mappedProbActions = pairOfProbActions[t][d->nodes[x]->name];							
						 	std::vector< std::string > listAction = mappedProbActions [(std::string) d->actions[action]->name];
						 	if ( listAction.size() > 0 ) {
						 		for (unsigned y = 0; y < listAction.size(); y++) {
						 			bool exists = false;
						 			for (unsigned z = 0; z < predcts.size(); z++) 
						 				if (predcts[z]->name == "P-" + listAction[y]) 
						 					exists = true;						 			
						 			if (! exists)
						 				cd->createPredicate( "P-" + listAction[y] );
						 			cd->addPre( 1, name, "P-" + listAction[y] );
						 		}
						 	}
						}							
					}
					else cd->addPre( 0, name, "AFREE" );

					// add new effects ( updated effects )
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addEff( 0, name, "TAKEN", IntVec( 1, 0 ) );
						cd->addEff( 1, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );										
						// step-2: aaai2018 
						bool exists = false;
			 			for (unsigned z = 0; z < predcts.size(); z++) 
			 				if (predcts[z]->name == "P-" + name) 
			 					exists = true;						 			
			 			if (! exists)
			 				cd->createPredicate( "P-" + d->actions[action]->name );
						cd->addEff( 0, name, "P-" + d->actions[action]->name );
					}
				}
			}
		
			// A lot of changes needed from the upf version for our END- version
			if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
				std::string name = "END-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				Action * end = cd->createAction( name, d->typeList( d->nodes[x] ) );
				cd->addParams( name, StringVec( 1, "AGENT-COUNT" ) );

				// preconditions				
				cd->addPre( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
				cd->addPre( 0, name, "SAT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
				cd->addPre( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );				
				
				// effects
				cd->addEff( 1, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
				if ( i->second.size() > 1 ) {
					cd->addEff( 0, name, "DONE-" + d->nodes[x]->name );
				}
				else 
				{					
					cd->addEff( 0, name, "AFREE" );
					cd->addEff( 1, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );
					Forall * f = new Forall;					
					f->params = cd->convertTypes( StringVec( 1, "AGENT" ) );
					f->cond = new Not( new Ground( cd->preds.get( "TAKEN" ), incvec( size + 1, size + 2 ) ) );
					dynamic_cast< And * >( end->eff )->add( f );
					
					/* addition of quantifiers */					
					std::set< std::vector < unsigned > >::iterator it;
					for ( it = probVector.begin(); it != probVector.end(); ++it ) 
					{ 
						std::vector< unsigned > deleteChoices  = ( std::vector< unsigned > ) *it;				
						unsigned i = (unsigned) deleteChoices[0];
						unsigned choice = (unsigned) deleteChoices[1];
						
						Forall * f1 = new Forall;
						And * andFormula = new And;							
						When * ss = new When;
						And * a1 = new And;
						StringVec vec;
						
						for (int p = 0; p < (int) d->typeList( d->preds[i] ).size(); p++) {
							for (int q = 0; q < (int) d->typeList( d->nodes[x] ).size(); q++) {
								if ( ! ( d->typeList( d->preds[i] )[p] == d->typeList( d->nodes[x] )[q] ) ) {
									vec.push_back(d->typeList( d->preds[i] )[p]);							
								}
							}
						}
						
						IntVec integerVec;
						f1->params = cd->convertTypes( vec );
						unsigned f1Size = f1->params.size();
						for (int p = 0; p < (int) (size + f1Size + 1); p++ ) {
							if ( p!= (int) size)
								integerVec.push_back(p);							 
						}
					
						StringVec vector = d->typeList( d->preds[i] );
						if ( choice == 1 ) {			
							ss->pars = new Ground ( cd->preds.get( "POS-" + d->preds[i]->name ), integerVec );							
							a1->add( new Ground ( cd->preds.get( d->preds[i]->name ), integerVec ) );
							a1->add( new Not ( new Ground( cd->preds.get( "POS-" + d->preds[i]->name ), integerVec ) ) );
							ss->cond = a1;							
							andFormula->add(ss);	
														
							ss = new When;							
							ss->pars = new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), integerVec );
							a1 = new And;
							a1->add( new Not( new Ground( cd->preds.get( d->preds[i]->name ), integerVec ) ) );
							a1->add( new Not( new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), integerVec ) ) );
							ss->cond = a1;
							andFormula->add(ss);
							f1->cond = andFormula;
							dynamic_cast< And * >( end->eff )->add( f1 );													
						}
						else if ( choice == 2 ) {		
							f1 = new Forall;
							f1->params = cd->convertTypes( vec );
							ss = new When;							
							ss->pars = new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), integerVec );
							a1 = new And;
							a1->add( new Not( new Ground( cd->preds.get( d->preds[i]->name ), integerVec ) ) );
							a1->add( new Not( new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), integerVec ) ) );
							ss->cond = a1;
							f1->cond = ss;
							dynamic_cast< And * >( end->eff )->add( f1 );	
						}						
					}
				}
			}

			if ( i->second.size() > 1 && j + 1 == i->second.size() ) {
				std::string name = "FINISH-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				Action * finish = cd->createAction( name, d->typeList( d->nodes[x] ) );

				cd->addOrPre( name, "DONE-" + d->nodes[x]->name, "SKIPPED-" + d->nodes[x]->name );
				cd->addPre( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );

				// cd->addEff( 0, name, "ATEMP" );
				for ( unsigned k = 0; k < i->second.size(); ++k ) {
					cd->addEff( 1, name, "DONE-" + d->nodes[i->second[k]]->name );
					cd->addEff( 1, name, "SKIPPED-" + d->nodes[i->second[k]]->name );
					cd->addEff( 1, name, "USED-" + d->nodes[i->second[k]]->name );
					cd->addEff( 1, name, "ACTIVE-" + d->nodes[i->second[k]]->name, incvec( 0, size ) );
				}
				Forall * f = new Forall;
				f->params = cd->convertTypes( StringVec( 1, "AGENT" ) );
				f->cond = new Not( new Ground( cd->preds.get( "TAKEN" ), incvec( size, size + 1 ) ) );
				dynamic_cast< And * >( finish->eff )->add( f );
			}
		}
	}
			
	std::cout << *cd;

	// generate single-agent instance file
	unsigned nagents = d->types.get( "AGENT" )->noObjects();

	Instance * cins = new Instance( *cd );
	cins->name = ins->name;

	// add objects
	StringVec counts( 1, "ACOUNT-0" );
	for ( unsigned i = 1; i <= nagents; ++i ) {
		std::stringstream ss;
		ss << "ACOUNT-" << i;
		counts.push_back( ss.str() );
		cins->addObject( counts[i], "AGENT-COUNT" );
	}

	// create initial state
	for ( unsigned i = 0; i < ins->init.size(); ++i )
		if ( d->preds.index( ins->init[i]->name ) >= 0 )
			cins->addInit( ins->init[i]->name, d->objectList( ins->init[i] ) );
	cins->addInit( "AFREE" );
	for ( unsigned i = 1; i <= nagents; ++i ) {
		StringVec pars( 1, counts[i - 1] );
		pars.push_back( counts[i] );
		cins->addInit( "CONSEC", pars );
	}
	for ( unsigned i = 0; i < d->nodes.size(); ++i ) {
		VecMap::iterator j = ccs.find( d->mf[i] );
		if ( j->second.size() > 1 || d->nodes[i]->upper > 1 ) {
			for ( unsigned j = d->nodes[i]->lower; j <= d->nodes[i]->upper && j <= nagents; ++j )
				cins->addInit( "SAT-" + d->nodes[i]->name, StringVec( 1, counts[j] ) );
		}
	}

	// create goal state
	for ( unsigned i = 0; i < ins->goal.size(); ++i )
		cins->addGoal( ins->goal[i]->name, d->objectList( ins->goal[i] ) );
	cins->addGoal( "AFREE" );

	// std::cerr << *cins;

	delete cins;
	delete cd;
	delete ins;
	delete d;
}