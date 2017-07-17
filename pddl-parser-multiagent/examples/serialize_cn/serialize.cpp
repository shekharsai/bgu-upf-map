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

// updated by shashank
bool deletes( const Ground * ground, const parser::multiagent::NetworkNode * n, IntVec impParams ) {
	for ( unsigned i = 0; i < n->templates.size(); ++i ) {		
		Action * a = d->actions[d->actions.index( n->templates[i]->name )];
		CondVec pres = a->precons();		
		for ( unsigned j = 0; j < pres.size(); ++j ) {
			Ground * g = dynamic_cast< Ground * >( pres[j] );
			if ( g && 
				 g->name == ground->name &&
			     std::find( g->params.begin(), g->params.end(), impParams[0] ) != g->params.end() )  			    
				return true;
		}
	}
	return false;
}

// returns true if at least one instance of "POS-" or "NEG-" added
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
	
	// Identify problematic fluents (preconditions deleted by agents) (edited by shashank) 	
	for ( unsigned i = 0; i < d->nodes.size(); ++i ) {
		for ( unsigned j = 0; d->nodes[i]->upper > 1 && j < d->nodes[i]->templates.size(); ++j ) 
		{	
			// in future impParams may contain multiple values TODO
			IntVec impParams =  d->nodes[i]->templates[i]->params; 
			Action * a = d->actions[ d->actions.index( d->nodes[i]->templates[j]->name ) ];
			GroundVec dels = a->deleteEffects();	
			GroundVec added = a->addEffects();	
			unsigned choice = 1;
			for ( unsigned k = 0; k < dels.size(); ++k ) 
			{
				std::cout << impParams[0] << "\t" <<  dels[k]->params << "\n";
				bool choiceA = 
						std::find( dels[k]->params.begin(), dels[k]->params.end(), impParams[0] ) != dels[k]->params.end();
				bool choiceB = deletes( dels[k], d->nodes[i], impParams );
				bool choiceC = false;
								
				if ( choiceB )
					for ( unsigned t = 0; t < added.size(); t++ )
						if ( added[t]->name == dels[k]->name )
							choiceC = true; // if only deleted, and not added proposition.
				std::cout << "choices " << choiceA << choiceB << choiceC << "\n";
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
	cd->createPredicate( "AFREE" );
	cd->createPredicate( "ACTIVITY-FREE" ); // in case if a joint activity can also participate
	cd->createPredicate( "ATEMP" );
	cd->createPredicate( "TAKEN", StringVec( 1, "AGENT" ) );
	cd->createPredicate( "CONSEC", StringVec( 2, "AGENT-COUNT" ) );
	cd->createPredicate( "CONSEC1", StringVec( 2, "AGENT-COUNT" ) );
	cd->createPredicate( "CONSEC1", StringVec( 1, "AGENT" ) );
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

	// Add actions
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
			
			bool concurEffs = 0;
			for ( unsigned k = 0; k < d->nodes[x]->templates.size(); ++k ) 
			{
				int action = d->actions.index( d->nodes[x]->templates[k]->name );
				std::string name = d->actions[action]->name;
				
				// TODO // the below is for splitting a joint activity
				if ( name.find("ACTIVITY") != std::string::npos ) 
				{
					for ( unsigned i = 0; i < d->nodes.size(); ++i ) {
						VecMap::iterator j = ccs.find( d->mf[i] );
						if ( ( name.find("ACTIVITY") != std::string::npos ) && 
											( j->second.size() > 1 || d->nodes[i]->upper > 1 ) ) 
						{							
							cd->createPredicate( "ACTIVE-" + name, d->typeList( d->nodes[i] ) );							
						}
					}
					
					// creating the first half of the joint action									
					const std::string firstHalf = "BEGIN-" + name;
					Action * doFirstPart = cd->createAction( firstHalf, d->typeList( d->actions[action] ) );				
					unsigned size = doFirstPart->params.size();	
						
					// copy old preconditions
					And * oldpre = dynamic_cast< And * >( d->actions[action]->pre );					
					if ( oldpre ) 
						doFirstPart->pre = new And( oldpre, *cd );
					else {
						And * a = new And;
						a->add( d->actions[action]->pre->copy( *cd ) );
						doFirstPart->pre = a;
					}
					
					// copy old effects					
					And * oldeff = dynamic_cast< And * >( d->actions[action]->eff );					
					for ( unsigned l = 0; oldeff && l < oldeff->conds.size(); ++l )
						concurEffs |= addEff( cd, doFirstPart, oldeff->conds[l] );
					if ( !oldeff ) 
						concurEffs |= addEff( cd, doFirstPart, d->actions[action]->eff );
					
					// add new parameters
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 )						
						cd->addParams( firstHalf, StringVec( 2, "AGENT-COUNT" ) );
					
					// add new preconditions
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addPre( 0, firstHalf, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
						cd->addPre( 1, firstHalf, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, firstHalf, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, firstHalf, "CONSEC", incvec( size, size + 2 ) );
						cd->addPre( 0, firstHalf, "ACTIVITY-FREE" );						
					}
					else 
						cd->addPre( 0, firstHalf, "AFREE" );
					
					// add new effects
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						// cd->addEff( 0, firstHalf, "NEXT", incvec( 1, 2 ) );
						cd->addEff( 0, firstHalf, "ACTIVE-" + name, d->nodes[x]->templates[k]->params );
						cd->addEff( 0, firstHalf, "TAKEN", IntVec( 1, 0 ) );
						cd->addEff( 1, firstHalf, "ACTIVITY-FREE" );
						cd->addEff( 1, firstHalf, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, firstHalf, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
					}	
					
					// creating the second half	of the joint action				
					const std::string secondHalf = "TERMINATE-" + name;
					Action * doSecondPart = cd->createAction( secondHalf, d->typeList( d->actions[action] ) );				
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
						cd->addParams( secondHalf, StringVec( 2, "AGENT-COUNT" ) );
					
					// add new preconditions
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addPre( 0, secondHalf, "ACTIVE-" + name, d->nodes[x]->templates[k]->params );
						cd->addPre( 0, secondHalf, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
						cd->addPre( 1, secondHalf, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, secondHalf, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, secondHalf, "CONSEC", incvec( size, size + 2 ) );
					}
					else 
						cd->addPre( 0, secondHalf, "AFREE" );
					
					// add new effects
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addEff( 0, secondHalf, "ACTIVITY-FREE" );	
						cd->addEff( 1, secondHalf, "ACTIVE-" + name, d->nodes[x]->templates[k]->params );					
						cd->addEff( 0, secondHalf, "TAKEN", IntVec( 1, 0 ) );
						cd->addEff( 1, secondHalf, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, secondHalf, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
					}
				} 
				else { 	// if the action is not a joint activity				
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
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addPre( 0, name, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
						cd->addPre( 1, name, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, name, "CONSEC", incvec( size, size + 2 ) );
					}
					else cd->addPre( 0, name, "AFREE" );

					// add new effects
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addEff( 0, name, "TAKEN", IntVec( 1, 0 ) );
						cd->addEff( 1, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
					}
				}
			}
		
			// A lot of changes needed fro the upf version for our END- version
			if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
				std::string name = "END-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				Action * end = cd->createAction( name, d->typeList( d->nodes[x] ) );
				cd->addParams( name, StringVec( 1, "AGENT-COUNT" ) );

				// preconditions
				cd->addPre( 0, name, "ACTIVITY-FREE" );
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
					// cd->addEff( 0, name, concurEffs ? "ATEMP" : "AFREE" );
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
							a1->add( new Ground ( cd->preds.get( d->preds[i]->name ), {0,2,3} ) );
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

				cd->addEff( 0, name, "ATEMP" );
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
	
	std::set< std::vector < unsigned > >::iterator it;
	for ( it = probVector.begin(); it != probVector.end(); ++it ) { 
		std::vector< unsigned > deleteChoices  = ( std::vector< unsigned > ) *it;				
		unsigned i = (unsigned) deleteChoices[0];
		unsigned choice = (unsigned) deleteChoices[1];
		if ( choice == 1 ) {
			std::string name = "ADD-" + d->preds[i]->name;
			unsigned size = d->preds[i]->params.size();
			cd->createAction( name, d->typeList( d->preds[i] ) );
			cd->addPre( 0, name, "ATEMP" );
			cd->addPre( 0, name, "POS-" + d->preds[i]->name, incvec( 0, size ) );
			// cd->addPre( 1, name, "NEG-" + d->preds[i]->name, incvec( 0, size ) );
			cd->addEff( 0, name, d->preds[i]->name, incvec( 0, size ) );
			cd->addEff( 1, name, "POS-" + d->preds[i]->name, incvec( 0, size ) );

			name = "DELETE-" + d->preds[i]->name;
			cd->createAction( name, d->typeList( d->preds[i] ) );
			cd->addPre( 0, name, "ATEMP" );
			// cd->addPre( 1, name, "POS-" + d->preds[i]->name, incvec( 0, size ) );
			cd->addPre( 0, name, "NEG-" + d->preds[i]->name, incvec( 0, size ) );
			cd->addEff( 1, name, d->preds[i]->name, incvec( 0, size ) );
			cd->addEff( 1, name, "NEG-" + d->preds[i]->name, incvec( 0, size ) );
		}	
		else if ( choice == 2 ) {
			std::string name = "DELETE-" + d->preds[i]->name;
			unsigned size = d->preds[i]->params.size();
			cd->createAction( name, d->typeList( d->preds[i] ) );
			cd->addPre( 0, name, "ATEMP" );
			cd->addPre( 0, name, "NEG-" + d->preds[i]->name, incvec( 0, size ) );
			cd->addEff( 1, name, d->preds[i]->name, incvec( 0, size ) );
			cd->addEff( 1, name, "NEG-" + d->preds[i]->name, incvec( 0, size ) );
		}			
	}
	
	//TODO
	Action * freeit = cd->createAction( "POS-NEG-FREE" );
	cd->addPre( 0, "POS-NEG-FREE", "ATEMP" );
	for ( it = probVector.begin(); it != probVector.end(); ++it ) { 
		std::vector< unsigned > deleteChoices  = ( std::vector< unsigned > ) *it;				
		unsigned i = (unsigned) deleteChoices[0];
		unsigned choice = (unsigned) deleteChoices[1];
		if ( choice == 1 ) {		
			std::cout << "in choice 1 \n";
			Forall * f = new Forall;
			f->params = cd->convertTypes( d->typeList( d->preds[i] ) );
			And * a = new And;
			a->add( new Not( new Ground( cd->preds.get( "POS-" + d->preds[i]->name ), incvec( 0, f->params.size() ) ) ) );
			a->add( new Not( new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), incvec( 0, f->params.size() ) ) ) );
			f->cond = a;
			dynamic_cast< And * >( freeit->pre )->add( f );
		}
		else if ( choice == 2 ) {
			std::cout << "in choice 2 \n";
			Forall * f = new Forall;
			f->params = cd->convertTypes( d->typeList( d->preds[i] ) );
			And * a = new And;
			// a->add( new Not( new Ground( cd->preds.get( "POS-" + d->preds[i]->name ), incvec( 0, f->params.size() ) ) ) );
			a->add( new Not( new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), incvec( 0, f->params.size() ) ) ) );
			f->cond = a;
			dynamic_cast< And * >( freeit->pre )->add( f );
		}
	}
	cd->addEff( 0, "POS-NEG-FREE", "AFREE" );
	cd->addEff( 1, "POS-NEG-FREE", "ATEMP" );
	
	std::cout << *cd;

	// Generate single-agent instance
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

	std::cerr << *cins;

	delete cins;
	delete cd;
	delete ins;
	delete d;
}
