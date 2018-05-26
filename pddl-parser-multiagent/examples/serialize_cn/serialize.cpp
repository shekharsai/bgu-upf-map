/**
* @author: Shashank Shekhar, BGU Israel. 
* email: shekhar@cs.bgu.ac.il 
**

*************************************************************************************************** 
Before you dive into the code, you should keep the following steps in mind.	
Note that our code is restricted to very specific cases as per our ICAPS paper, so it does not 
handle all the scenarios, however, it is very easy to make changes in the code according to any 
specific requirements.  	

Relevant for the ICAPS paper. 

	1. The name of each predicate must be different, why?
		(a) POS- NEG- have been added using only predicate names.
			
	2. Keep all the agents together in the activity parameter list. Since it is an extension over 
	CJR's formalism and syntax, the prime acting agent would never appear in the parameter list, 
	see their action schema for more details on this. 
		(a) For a collaborative action, the agent list will be (a1, a2, ..., ak, objs) -- k agents
		(b) Irrespective of a object-type Truck or Plane use AGENT as the code has string 
		comparisons, especially for a collaborative action.
	
	3. The name of a collaborative action should have "ACTIVITY" string in it. Also, you need to 
	specify more than one agent. See our domains for example.  
	
	4. It splits blindly, which means, to have correct splits of a Joint activity, make sure that 
	there are no predicates specifying other agents in the effect or precondition. A little more 
	work is needed to resolve this limitation. 
	
	5. TODO during translation an action cannot be public or private. So, remove IN-JOINT from the 
	preconditions in the planner's code -- whichever planner you are using.
	
	6. Simplification - the single-agent action name will appear in the name of a collaborative
	action, for example, push-box and activity-push-box.
	
	7. As there is some problem in the MA-Parser (in upf-map, unresolved), during parsing a 
	problem file, it always skips the first private object. Therefore, always have a dummy object 
	in each problem file, for an example, "dummy-pr - object", we also use this.
	
	8. It should capture POS- or NEG- for a predicate only if its param set is superset of node 
	param set, e.g., V1->params   
	
	9. Since, the number of agents is not known beforehand, so, unlike CJR, 'inf' does not work. 
	We need to give an appropriate number in the node for upper bounds.
   	
   	10. Don't have a check on a node having inappropriate bounds, and it has a collaborative action 
   	in its action list. 
   	
Note that, in the description, our paper considers all actions as grounded actions, so that the 
explanations become easy. The code is based on non-grounded operators. However, relevant modifications 
are easy to do in case one defines a collaborative action with many more SA elements in it. 
   	
The changes below are part of our submitted journal article.
   	
   	12. We have changed the interpretation now. Now, we have a stronger notion to define a 
   	well-formed multi-action instead of well-defined that has some potential weakness as pointed 
   	out in our ICAPS paper.  
   	
   	13. However, the current interpretation does not affect our domains' compilation.  
   	
   	14. Note that, the code for point 2(d), you need to manually change AND to OR. For example, 
   	(NOT (AND () () ()) ) to (OR (NOT ()) (NOT ()) ), the parser does not support it, so cannont 
   	be automated, and did not dive into the parser's code.   

****************************************************************************************************
*	To check for memory leaks: 
* 		valgrind --leak-check=yes examples/serialize 
*				../multiagent/../tablemover/tablemover.pddl 
*						../multiagent/../tablemover/table1_1.pddl 
****************************************************************************************************
*/

#include <parser/Instance.h>
#include <multiagent/MultiagentDomain.h>
#include <vector>
#include <time.h>

using namespace parser::pddl;
using namespace std;

parser::multiagent::MultiagentDomain * d; Instance * ins;
std::set< unsigned> prob; std::set< std::vector < unsigned > > probVector;
std::map< std::string, std::set< std::vector < unsigned >>> node_wise_probVector;
typedef std::map< unsigned, std::vector< int > > VecMap;

std::map< std::string, std::vector<std::string> > actionPairWithDiffEffOnObjSet( 
									const parser::multiagent::NetworkNode * n, const Domain & cd);
	
// This function is added for the journal version, it was not part of the icaps work. 
// Note: this does not check for an agent performing sa-push is tired etc. 
// Here, we are interested in different effects on shared object, e.g., pushed(B) or not-pushed(B). 
// Partially done!! Since we added a stronger interpretation (journal), this check is not mandatory. 
// for a domain description. Follow up the potential weaknesses mentioned in the ICAPS paper.   
bool isTheDomainDescriptionAmbiguous( parser::multiagent::MultiagentDomain *d ) {
	return false;
	for( unsigned i=0; i<d->nodes.size(); i++) {
		std::cout << d->nodes[i] << std::endl;
		for( unsigned j=0; j<d->nodes[i]->templates.size(); j++) {
			std::cout << d->nodes[i]->templates[j]->params << std::endl;
		}
	}
	// Add code snippet to make sure (2push,push) and (push,2push) are not possible as per a given 
	// domain, and in case if it is possible, notify the modeler to resolve it.	
	// For each node V1,...,Vk, the algorithm is run 
	for( unsigned i = 0; i < d->nodes.size(); ++i ) {
		std::map< std::string, std::vector <std::string> > pairs;	
		pairs = actionPairWithDiffEffOnObjSet( d->nodes[i], *d );	
	}		
}

// These are very specific checks that are bases on parameters 
// It is solely based on the idea of sharing object subset like CJR's
bool doTheyTargetTheSameObjectSubset( 
	IntVec network, 
	IntVec legal, 
	IntVec legalActionParams, 
	IntVec illegal, 
	IntVec illegalActionParams
	) {
	bool decision = false;
	unsigned count = 0;
	for( int &i : network ) {
		bool flag = false;
		for( int &j : legal ) {
			if( i == legalActionParams[j] )
				for( int &k : illegal )
					if( i == illegalActionParams[k] ) {
						count++; 
						flag = true;
						break;
					}
			if( flag )	break;
		}
	}
	if( count == network.size() ) {
		decision = true;
	}	
	return decision;
}

// how many times a word has occured in a long string
int key_search(const std::string& s, const std::string& key) {
    int count = 0; size_t pos=0;
    while ((pos = s.find(key, pos)) != std::string::npos) {
        ++count; ++pos;
    }
    return count;
}

// Returns pair of actions (sa or collaborative) with different effects on a set of objects.
// Following the domain file, the below snippet checks for each concurrency-constraint node v1.
// Two actions with negative interactions, are different too, as of now. 
// Activity-drop-table contains 2 agents by default, otherwise parse the number of agents.
std::map< std::string, std::vector<std::string> > 
	actionPairWithDiffEffOnObjSet( const parser::multiagent::NetworkNode * n, const Domain & cd ) {		
	std::map< std::string, std::vector<std::string> > listOfAmbiguousActions;	
	for( unsigned i = 0; i < n->templates.size(); ++i ) 
	{
		Action* legal_a = d->actions[ d->actions.index( n->templates[i]->name )];	
		std::vector< std::string > ambActions;			
		for( unsigned j = 0; i != j && j < n->templates.size(); j++ ) {				
			bool ambiguous1 = false, ambiguous2 = false;			
			Action * probably_legal_a = d->actions[ d->actions.index( n->templates[j]->name )];				
			for( unsigned k = 0; k < legal_a->addEffects().size(); ++k ) { 			
				for( unsigned l = 0; l < probably_legal_a->deleteEffects().size(); ++l ) {
					if( (dynamic_cast< Ground * > (legal_a->addEffects()[k]))->name == 
						(dynamic_cast< Ground * > (probably_legal_a->deleteEffects()[l]))->name ) {
							ambiguous1 = doTheyTargetTheSameObjectSubset( 
													n->params,
													legal_a->addEffects()[k]->params,
													legal_a->params,
													probably_legal_a->deleteEffects()[l]->params,
													probably_legal_a->params );
							if( ambiguous1 )
								break;												
						}
				}
			}
			if( !ambiguous1 ) {
				for( unsigned k = 0; k < legal_a->deleteEffects().size(); ++k ) { 			
					for( unsigned l = 0; l < probably_legal_a->addEffects().size(); ++l ) {
						if(((dynamic_cast<Ground *> (legal_a->deleteEffects()[k]))->name == 
							(dynamic_cast<Ground *> (probably_legal_a->addEffects()[l]))->name)) 
							{	
								ambiguous2 = doTheyTargetTheSameObjectSubset(															
													n->params,
													legal_a->deleteEffects()[k]->params,
													legal_a->params,
													probably_legal_a->addEffects()[l]->params,
													probably_legal_a->params );
								if( ambiguous2 ) break;												
							}
					}
				}
			}
			if ( ambiguous1 || ambiguous2 ) 
				ambActions.push_back( (std::string) probably_legal_a->name );
		}	
		listOfAmbiguousActions[ (std::string) legal_a->name ] = ambActions;		
	}
	return listOfAmbiguousActions;
}

// Returns set of conflicting actions in network nodes
// Again, conflicting actions here,means, if they have something different to say about the 
// propositions dealing with common objects set (keeping privacy in mind). 
std::map< std::string, std::vector< std::string > > ambiguousActions( 
	const parser::multiagent::NetworkNode * n, 
	const Domain & cd 
	) {
		std::map< std::string, std::vector < std::string > > listOfAmbiguousActions;
		for( unsigned i = 0; i < n->templates.size(); ++i ) {
			Action * legal_a = d->actions[ d->actions.index( n->templates[i]->name ) ];			
			std::vector< std::string > ambActions;			
			for( unsigned j = 0; j < n->templates.size(); j++ )	
				if( i != j ) {
					bool ambiguous1 = false; 
					bool ambiguous2 = false;			
					Action * probably_legal_a = d->actions[ d->actions.index( n->templates[j]->name )];	
					for( unsigned k = 0; k < legal_a->addEffects().size(); ++k ) 			
						for( unsigned l = 0; l < probably_legal_a->deleteEffects().size(); ++l ) 
						{
							if((( dynamic_cast< Ground * > (legal_a->addEffects()[k]))->name == 
									(dynamic_cast< Ground * > (probably_legal_a->deleteEffects()[l]))->name )) 
								{
									ambiguous1 = doTheyTargetTheSameObjectSubset( 
															n->params,
															legal_a->addEffects()[k]->params,
															legal_a->params,
															probably_legal_a->deleteEffects()[l]->params,
															probably_legal_a->params );
									if( ambiguous1 )
										break;												
								}
						}
					if( !ambiguous1 ) {
						for( unsigned k = 0; k < legal_a->deleteEffects().size(); ++k ) 			
							for( unsigned l = 0; l < probably_legal_a->addEffects().size(); ++l ) 
							{
								if(((dynamic_cast<Ground *> (legal_a->deleteEffects()[k]))->name == 
									(dynamic_cast<Ground *> (probably_legal_a->addEffects()[l]))->name)) 
									{	
										ambiguous2 = doTheyTargetTheSameObjectSubset(															
															n->params,
															legal_a->deleteEffects()[k]->params,
															legal_a->params,
															probably_legal_a->addEffects()[l]->params,
															probably_legal_a->params );
										if( ambiguous2 ) break;												
									}
							}
					}
					if ( ambiguous1 || ambiguous2 ) 
						ambActions.push_back( (std::string) probably_legal_a->name );
				}		
			listOfAmbiguousActions[ (std::string) legal_a->name ] = ambActions;		
		}
		return listOfAmbiguousActions;
}

// Returns true is action possibleComp appears in the Joint Activity, jointActivity.
bool isActionAComponentOfJA( Action * jointActivity,  Action * possibleComp ) {
	bool decision = false;
	int counter = 0;	
	std::vector< int > activityParams;
	for( int &j : jointActivity->params )
		activityParams.push_back( j );
	if( ( jointActivity->name ).find( possibleComp->name ) != std::string::npos ) {
		for( unsigned i = 0; i < possibleComp->params.size(); i++ )
			for( unsigned j = 0; j < activityParams.size(); j++ ) 
				if( possibleComp->params[i] == activityParams[j] ) { 					
					counter++;
					activityParams.erase( activityParams.begin() + j );
					break;
				}
		if( counter == (int) possibleComp->params.size() )
			decision = true;		
	}
	return decision; 
}

// Just handles |< push, 2-push (JA) >| or |< push, clean, <push-clean (JA) >>|
// Action-component-name should always appear in JA, along with its whole paramList() 
std::map< std::string, std::vector< std::string > > findComponentsOfJointActivities( 
	const parser::multiagent::NetworkNode * n ) {
	std::map < std::string, std::vector < std::string > > listOfJointActivityComponents;
	for( unsigned i = 0; i < n->templates.size(); ++i ) {
		std::vector< std::string > listOfComp;
		Action * jointActivity = d->actions[ d->actions.index( n->templates[i]->name ) ];		
		if( ( jointActivity->name ).find( "ACTIVITY" ) != std::string::npos ) 
		{
			for ( unsigned j = 0; j < n->templates.size(); ++j ) 
				if ( i != j ) {					
					Action * possibleComp = d->actions[ d->actions.index( n->templates[j]->name ) ];					
					bool decision = isActionAComponentOfJA( jointActivity, possibleComp );	
					if (decision) {
						listOfComp.push_back( possibleComp->name );
					}				
				}
			listOfJointActivityComponents[ jointActivity->name ] = listOfComp;
		}
	}
	return listOfJointActivityComponents;
}

// Currently, only the deleted effects are being checked against the preconditions
bool deletes( const Ground * ground, const parser::multiagent::NetworkNode * n, IntVec impParams) {
	for( unsigned i = 0; i < n->templates.size(); ++i ) {
		std::vector< int > list = n->templates[i]->params;			
		Action * a = d->actions[ d->actions.index( n->templates[i]->name )];		
		CondVec pres = a->precons();				
		for( unsigned j = 0; j < pres.size(); ++j ) 
		{	
			Not * n = dynamic_cast< Not * >( pres[j] );
			if( n ) continue;	
			Ground * g = dynamic_cast< Ground * >( pres[j] );												
			int counter = 0;
			if( g && g->name == ground->name ) {
				bool decision = false;
				for( unsigned h = 0; h < g->params.size(); h++ ) {
					for( unsigned f = 0; f < list.size(); f++ )											
						if( list[ f ] == g->params[ h ] && g->params.size() <= list.size() ) 
					 		decision = true;				
					if( decision )
						counter++;					
				}
				if( counter == (int) g->params.size() )		
					return true;
			}
		}		
	}
	return false;
}

// Currently, only the deleted effects are being checked against the preconditions
// not elegantly done, just for the sake of the aaai deadline!
bool deletes_old( const Ground * ground, const parser::multiagent::NetworkNode * n, IntVec impParams ) {
	IntVec networkParams = n->params;
	for( unsigned i = 0; i < n->templates.size(); ++i ) {			
		Action * a = d->actions[ d->actions.index( n->templates[i]->name )];		
		IntVec currActParams = a->params;
		CondVec pres = a->precons();				
		for( unsigned j = 0; j < pres.size(); ++j ) {		
			Ground * g = dynamic_cast< Ground * >( pres[j] );			
			int counter = 0;						
			if( g && g->name == ground->name ) {
				for( int d1 = 0; d1 < (int) networkParams.size(); d1++ ) {   			    
					std::vector< int > list;
					for( int d2 = 0; d2 < (int) currActParams.size(); d2++)
						if( networkParams[d1] == currActParams[d2] ) 
							list.push_back(d2);				
					bool decision = false;
					for( int &h : list ) {						
						std::stringstream number;
						number << h << std::endl;
						std::stringstream precond1;
						precond1 << pres[j] << std::endl;												
						std::string s2 = number.str();
						s2.erase(0, s2.find_first_not_of(" \n\r\t"));
						s2.erase(s2.find_last_not_of(" \n\r\t")+1);						
						if( precond1.str().find( s2 ) != std::string::npos )
						 	decision = true;
					}
					if( decision )
						counter++;					
				}
				if( counter == (int) networkParams.size() )		
					return true;
			}
		}		
	}
	return false;
}

// TODO returns true if (>=1) instances of "POS-" or "NEG-" or both get added, related to the constrained object
bool addEff( Domain * cd, Action * a, Condition * c, std::string nodeName ) {		
	probVector.clear();
	probVector = node_wise_probVector[ nodeName ];
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

bool addEff( Domain * cd, Action * a, Condition * c ) {
	Not * n = dynamic_cast< Not * >( c );
	Ground * g = dynamic_cast< Ground * >( c );		
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

bool canAddNewCondition( Domain * cd, std::string actaualAction, std::string cond ) {
	bool decision = true;
	for( unsigned i7 = 0; i7 < cd->actions.size(); i7++) 
		if( actaualAction == cd->actions[i7]->name ) 
		{			
			std::vector< Condition * > condSet = cd->actions[i7]->precons();
			for( unsigned int i8 = 0; i8 < condSet.size(); i8++ ) {
				std::stringstream buffer;
				buffer << condSet[i8] << std::endl;
				if( ( buffer.str() ).find(cond) != std::string::npos )
					decision = false;
			}
		}
	return decision;
}

bool canAddNewEffect( Domain * cd, std::string actaualAction, std::string cond ) {
	bool decision = true;
	for( unsigned i7 = 0; i7 < cd->actions.size(); i7++) 
		if( actaualAction == cd->actions[i7]->name ) 
		{			
			std::vector< Ground * > posEffectSet = cd->actions[i7]->addEffects();
			for( unsigned int i8 = 0; i8 < posEffectSet.size(); i8++ ) {
				std::stringstream buffer;
				buffer << posEffectSet[i8] << std::endl;
				if( ( buffer.str() ).find(cond) != std::string::npos )
					decision = false;
			}
			std::vector< Ground * > negEffectSet = cd->actions[i7]->deleteEffects();
			for( unsigned int i8 = 0; i8 < negEffectSet.size(); i8++ ) {
				std::stringstream buffer;
				buffer << negEffectSet[i8] << std::endl;
				if( ( buffer.str() ).find(cond) != std::string::npos )
					decision = false;
			}
		}
	return decision;
}

// need to be done properly, as of now kind of hard coded
// does not consider a proposition, if an agent is involved in it. 
bool agentInvolved( StringVec paramList, IntVec params ) {
	for( unsigned j = 0; j < params.size(); j++ )
		if( paramList[ params[ j ] ] == "AGENT" && params[j]!=1 ) 
			return true;
	return false;
}

// captures only cases when node[i]->params is subset of del[j]->params or vice versa.
bool subsetVector( IntVec parent, IntVec child ) {
	unsigned counter;
	IntVec pr, ch;
	counter = 0; pr.clear(); ch.clear();
	for( int &i: parent )
		pr.push_back(i);
	for( int &i: child )
		ch.push_back(i);
	for( unsigned i = 0; i < pr.size(); i++ ) {
		for( unsigned j = 0; j < ch.size(); j++ )
			if( pr[i] == ch[j] ) {
				counter++; 
				ch.erase( ch.begin() + j ); 
				break;
			}
	}	
	if( counter == parent.size() )
		return true;	
	return false;			
}

int main( int argc, char *argv[] ) {    

	if ( argc < 3 ) {
		std::cout << "Usage: ./transform <domain.pddl> <task.pddl>\n";
		exit( 1 );
	}	
	
	d = new parser::multiagent::MultiagentDomain( argv[1] );
	ins = new Instance( *d, argv[2] );	

	// For measuring the compilation time 
	clock_t t1, t2; t1 = clock();
    
	// Actions with conflicting effects, e.g., <stack, [unstack,pickup]>
	std::vector< std::map< std::string, std::map< std::string, std::vector< std::string >>> > pairOfProbActions;
	for( unsigned i = 0; i < d->nodes.size(); ++i ) {
	 	std::map< std::string, std::map < std::string, std::vector < std::string >>> pair;	
		pair[d->nodes[i]->name] = ambiguousActions( d->nodes[i], *d);	
		pairOfProbActions.push_back( pair );	
	}
		
	// Currently contains (or can hadle) a maximum 2 single agent actions, e.g., push:push and clean:push.
	std::vector< std::map< std::string, std::map< std::string, std::vector< std::string >>> > listOfJointActivityComponents;
	for( unsigned i = 0; i < d->nodes.size(); ++i ) {
	 	std::map< std::string, std::map < std::string, std::vector < std::string >>> pair;	
		pair[ d->nodes[i]->name ] = findComponentsOfJointActivities( d->nodes[i] );	
		listOfJointActivityComponents.push_back( pair );	
	}
	
	// This loop is for identifying problematic fluents (preconditions that get deleted), that halts concurrent execution.  	
	for( unsigned i = 0; i < d->nodes.size(); ++i ) 
	{	
		// Interactions are captured only through set of objects, following the CJR's specifications.
		if( ( d->nodes[ i ]->params ).size() == 0	)
			continue;
		for( unsigned j = 0; d->nodes[i]->upper > 1 && j < d->nodes[i]->templates.size(); ++j ) {
			IntVec impParams;
			impParams = d->nodes[i]->templates[j]->params; 			
			
			Action * a = d->actions[ d->actions.index( d->nodes[i]->templates[j]->name ) ];
			GroundVec dels = a->deleteEffects(); 
			GroundVec added = a->addEffects();	
						
			unsigned choice = 1;			
			for( unsigned k = 0; k < dels.size(); ++k ) {								
				// a non-sense but necessary check :D					
				bool superChoice = agentInvolved( d->typeList( a ), dels[k]->params );
				if( superChoice )
					continue;
				bool choiceA = subsetVector( dels[k]->params, impParams );					
				bool choiceB = deletes( dels[k], d->nodes[i], impParams );
				bool choiceC = false;
								
				if( choiceB )
					for( unsigned t = 0; t < added.size(); t++ )
						if( added[t]->name == dels[k]->name )
							choiceC = true; // flase - if only !P, and does not get added P.
				
				if( choiceA && choiceB && choiceC ) { 	
					choice = 1;	
					probVector.insert( { (unsigned) d->preds.index( dels[k]->name ), choice } );				
				}
				else if( choiceA && choiceB && !choiceC ) {					
					choice = 2;					
					probVector.insert( { (unsigned) d->preds.index( dels[k]->name ), choice } );
				}
			}
		}
		node_wise_probVector[ (std::string) d->nodes[i]->name ] = probVector;
		probVector.clear();		
	}
	
	VecMap ccs;
	for( unsigned i = 0; i < d->mf.size(); ++i )
		ccs[d->mf[i]].push_back( i );

	if( isTheDomainDescriptionAmbiguous( d ) ) {
		// if we still use the notion of well-defined multi-actions
		std::cout << "\n\nAmbiguous domain description!" << std::endl;
		std::cout << "\nThe modeler is advised to modify this domain." << std::endl;
		std::cout << "\nTo resolve the ambiguities from the domain description:" << std::endl;
		std::cout << "\t1. Define enough collaborative actions" << std::endl;
		std::cout << "\t2. You can also restrict the multi-actions by changing the given bounds - (min-max)" << std::endl;
		exit( 1 );
	}
	
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

	// Add predicates -- a set of bugs has been removed from the upf version of the code, probably 
	// there might be some more.
	for( unsigned i = 0; i < d->preds.size(); ++i ) {
		TokenStruct< Lifted * > predcts;
		cd->createPredicate( d->preds[i]->name, d->typeList( d->preds[i] ) ); // adds the real ones			
		for( unsigned j = 0; j < d->nodes.size(); j++ ) {			
			probVector = node_wise_probVector[ (std::string) d->nodes[j]->name ];			
			std::set< std::vector < unsigned > >::iterator it;
			for( it = probVector.begin(); it != probVector.end(); ++it ) 
			{ 
				std::vector< unsigned > deleteChoices  = ( std::vector< unsigned > ) *it;
				bool exists;				
				if( deleteChoices[0] == i && deleteChoices[1] == (unsigned) 1 ) {
					exists = false;	
					predcts = cd->listOfPredicates();
					for (unsigned z = 0; z < predcts.size(); z++) 
	 					if ( predcts[z]->name == "POS-" + d->preds[i]->name ) 
	 						exists = true;
	 				if (! exists)				
						cd->createPredicate( "POS-" + d->preds[i]->name, d->typeList(d->preds[i]) );
						
					exists = false;	
					predcts = cd->listOfPredicates();
					for (unsigned z = 0; z < predcts.size(); z++) 
	 				if ( predcts[z]->name == "NEG-" + d->preds[i]->name ) 
	 					exists = true;
	 				if (! exists)
						cd->createPredicate( "NEG-" + d->preds[i]->name, d->typeList(d->preds[i]) );
				}						
				if( deleteChoices[0] == i && deleteChoices[1] == (unsigned) 2 ) {
					exists = false;	
					predcts = cd->listOfPredicates();
					for (unsigned z = 0; z < predcts.size(); z++) 
	 				if ( predcts[z]->name == "NEG-" + d->preds[i]->name ) 
	 					exists = true;
	 				if (! exists)
						cd->createPredicate( "NEG-" + d->preds[i]->name, d->typeList(d->preds[i]) );
				}
			}
			probVector.clear();
		}
	}		
	
	cd->createPredicate( "TAKEN", StringVec( 1, "AGENT" ) );
	cd->createPredicate( "CONSEC", StringVec( 2, "AGENT-COUNT" ) );
	
	for( unsigned i = 0; i < d->nodes.size(); ++i ) {
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
	
	// in case if a joint activity can also participate
	cd->createPredicate( "IN-JOINT" ); 
	
	// generate all the updated actions
	for( VecMap::iterator i = ccs.begin(); i != ccs.end(); ++i ) {
		std::set< unsigned > visited;
		for( unsigned j = 0; j < i->second.size(); ++j ) {			
			int x = i->second[j];
			visited.insert( x );

			if( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
				std::string name = "START-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				cd->createAction( name, d->typeList( d->nodes[x] ) );

				if( j > 0 ) {
					for ( unsigned k = 0; k < d->edges.size(); ++k )
						if ( d->edges[k].second == x ) {
							std::set< unsigned >::iterator it = visited.find( d->edges[k].first );
							if ( it != visited.end() )
								cd->addPre( 0, name, "DONE-" + d->nodes[d->edges[k].first]->name );
						}
					cd->addOrPre( name, "DONE-" + d->nodes[i->second[j - 1]]->name, "SKIPPED-" + 
																d->nodes[i->second[j - 1]]->name );
					cd->addPre( 0, name, "ACTIVE-" + d->nodes[i->second[j - 1]]->name, incvec( 0, size ) );
					cd->addPre( 1, name, "USED-" + d->nodes[x]->name );
					
				}
				else cd->addPre( 0, name, "AFREE" );

				if( j < 1 ) 
					cd->addEff( 1, name, "AFREE" );
				cd->addEff( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );
				cd->addEff( 0, name, "COUNT-" + d->nodes[x]->name, IntVec( 1, -1 ) );
				if( i->second.size() > 1 )
					cd->addEff( 0, name, "USED-" + d->nodes[x]->name );
			}

			if( i->second.size() > 1 ) {
				std::string name = "SKIP-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				cd->createAction( name, d->typeList( d->nodes[x] ) );

				if( j > 0 ) {
					for ( unsigned k = 0; k < d->edges.size(); ++k )
						if ( d->edges[k].first == i->second[j] ) {
							std::set< unsigned >::iterator it = visited.find( d->edges[k].second );
							if ( it != visited.end() )
								cd->addPre( 0, name, "SKIPPED-" + d->nodes[d->edges[k].second]->name );
						}
					cd->addOrPre( name, "DONE-" + d->nodes[i->second[j - 1]]->name, "SKIPPED-" + 
																d->nodes[i->second[j - 1]]->name );
					cd->addPre( 0, name, "ACTIVE-" + d->nodes[i->second[j - 1]]->name, incvec( 0, size ) );
					cd->addPre( 1, name, "USED-" + d->nodes[x]->name );
				}
				else cd->addPre( 0, name, "AFREE" );

				if( !j ) 
					cd->addEff( 1, name, "AFREE" );
				cd->addEff( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );
				cd->addEff( 0, name, "SKIPPED-" + d->nodes[x]->name );
				cd->addEff( 0, name, "USED-" + d->nodes[x]->name );  
			}
						
			std::set< std::string > setOfSAActions, setOfCollabActions, setOfAllActions;
			for( unsigned k = 0; k < d->nodes[x]->templates.size(); ++k ) { 
				if( (d->nodes[x]->templates[k]->name).find("ACTIVITY") != std::string::npos )
					 setOfCollabActions.insert(d->nodes[x]->templates[k]->name);
				else 
					setOfSAActions.insert(d->nodes[x]->templates[k]->name);	 
				setOfAllActions.insert(d->nodes[x]->templates[k]->name);	
			}
						
			// The actual list of the action starts from here
			bool concurEffs = 0;			
			for( unsigned k = 0; k < d->nodes[x]->templates.size(); ++k ) 
			{			
				int action = d->actions.index( d->nodes[x]->templates[k]->name );
				std::string name = d->actions[action]->name;
											
				// TODO Code below is for splitting a joint activity
				if( name.find("ACTIVITY") != std::string::npos ) {										
					// creating the first part of the Joint Activity									
					const std::string start_JA = "DO-"+ name + "-1";
					StringVec ja_type_list = d->typeList( d->actions[action] );
					int noOfAgents = 0;				
					 
					// updated parameter list : one of the ways to do it
					for( unsigned p = 0; p < ja_type_list.size(); p++ )
						if ( ja_type_list[p] == "AGENT" )
							noOfAgents++;					
							
					// create extra predicates for Joint Activity
					for( int p = 2; p <= noOfAgents; p++) {
						std::stringstream convert; convert << p;
						std::string s2 = convert.str();
						s2.erase(0, s2.find_first_not_of(" \n\r\t"));
						s2.erase(s2.find_last_not_of(" \n\r\t")+1);
						cd->createPredicate( "NEXT-"+ name + "-" + s2, d->typeList( d->nodes[x] ) );						
					}
					
					Action * do_start_part = cd->createAction( start_JA, d->typeList( d->actions[action] ) );
					unsigned size = do_start_part->params.size();	
															
					// copy old preconditions
					And * oldpre = dynamic_cast< And * >( d->actions[action]->pre );												
					if( oldpre ) 
						do_start_part->pre = new And( oldpre, *cd );
					else {
						And * a = new And;
						a->add( d->actions[action]->pre->copy( *cd ) );
						do_start_part->pre = a;
					}
					
					// copy the old effects					
					And * oldeff = dynamic_cast< And * >( d->actions[action]->eff );					
					for( unsigned l = 0; oldeff && l < oldeff->conds.size(); ++l ) 
						concurEffs |= addEff( cd, do_start_part, oldeff->conds[l], d->nodes[x]->name );					
					if( !oldeff ) 
						concurEffs |= addEff( cd, do_start_part, d->actions[action]->eff );
					
					// add the new parameters
					if( i->second.size() > 1 || d->nodes[x]->upper > 1 )						
						cd->addParams( start_JA, StringVec( 2, "AGENT-COUNT" ) );
					
					// add the new preconditions
					TokenStruct< Lifted * > predcts;
					if( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addPre( 1, start_JA, "IN-JOINT" );	
						cd->addPre( 0, start_JA, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );						
						cd->addPre( 1, start_JA, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, start_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, start_JA, "CONSEC", incvec( size, size + 2 ) );
					 			
						// additional, as per step-2 "pairOfProbActions"
						for(unsigned t = 0; t < pairOfProbActions.size(); t++) {
							std::map < std::string, std::vector < std::string > > mappedProbActions;
							mappedProbActions = pairOfProbActions[t][d->nodes[x]->name];							
						 	std::vector< std::string > listAction = mappedProbActions[(std::string) name];
						 	if( listAction.size() > 0 ) {
						 		for (unsigned y = 0; y < listAction.size(); y++) 
						 		{
						 			bool exists = false;
						 			predcts = cd->listOfPredicates();
						 			for (unsigned z = 0; z < predcts.size(); z++) {
						 				if ( predcts[z]->name == "P-" + listAction[y] ) 
						 					exists = true;	
						 			}
						 			const StringVec & params = d->typeList( d->nodes[x] );
						 			StringVec mainParamList; mainParamList.push_back("AGENT");
						 			for ( unsigned ii = 0; ii < params.size(); ++ii) 
						 				mainParamList.push_back(params[ii]);							 			
						 			if (! exists) 	
						 				cd->createPredicate( "P-" + listAction[y], mainParamList );												 			
						 				
						 			bool decision = canAddNewCondition( cd, start_JA, "P-" + listAction[y] );
						 			for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) {
						 				const IntVec & params = d->nodes[x]->templates[s]->params;
						 				IntVec intVec; intVec.push_back(0);
						 				for ( unsigned ii = 0; ii < params.size(); ++ii) 
							 				intVec.push_back(params[ii]); 
			 							if ( d->nodes[x]->templates[s]->name == name && decision ) 
			 								cd->addPre( 1, start_JA, "P-" + listAction[y], intVec );						 							 		
			 						}
			 					}
			 				}	
						 	break;
						}
						
						// TODO
						// Implements the step 2(d), for each Collab action: if it shares elements with
						// other collab actions -- needed for a well-formed multi-action.
						set< std::string >::iterator iter;
						for( iter = setOfCollabActions.begin(); iter != setOfCollabActions.end(); ++iter ) {							
							// e(this-collaborative-action) 
							std::map< std::string, int > countTotalOccSuper; set< std::string >::iterator iterLoc;
							for( iterLoc = setOfSAActions.begin(); iterLoc != setOfSAActions.end(); ++iterLoc ) {
								int count = key_search( *iter, *iterLoc );
								if( count >= 1 )	
									countTotalOccSuper[*iterLoc] = count;	
							}
							
							// e(current-collaborative-action) 
							std::map< std::string, int > countTotalOccSub; set< std::string >::iterator iterLocCur;
							for( iterLocCur = setOfSAActions.begin(); iterLocCur != setOfSAActions.end(); ++iterLocCur ) {
								int count = key_search( name, *iterLocCur );
								if( count >= 1 )	
									countTotalOccSub[*iterLocCur] = count;	
							}		
							
							// If they share some elements
							bool ifTheyShareElements = false;
							for( std::map<std::string, int>::iterator it = countTotalOccSuper.begin();
								it != countTotalOccSuper.end(); ++it) {
								for( std::map<std::string, int>::iterator it1 = countTotalOccSub.begin();
									it1 != countTotalOccSub.end(); ++it1) {
									if(it->first == it1->first)
										ifTheyShareElements = true;
								}
							}
							
							// code for: if a_i belongs to e(a_c) \ e(a), add in the precondition
							// might contain some bug
							std::map< std::string, int > countTotalOcc;
							if( ifTheyShareElements ) {														
								std::map< std::string, int > countTotalOcc;
								for( std::map<std::string, int>::iterator it = countTotalOcc.begin();
									it != countTotalOcc.end(); ++it) 
								{
									int ifExtra = it->second;
									for( std::map<std::string, int>::iterator it1 = countTotalOccSub.begin();
										it1 != countTotalOccSub.end(); ++it1) {
										if( it->first == it1->first )
											ifExtra = it->second - it1->second;											
									}
									if( ifExtra >= 1 )
										countTotalOcc[it->first] = ifExtra;  												
								}
							}
											
							// create predicates if already not present, in the names of the components
							std::vector<int> paramAppearInFor={};
							for( std::map<std::string, int>::iterator it = countTotalOcc.begin();
								it != countTotalOcc.end(); ++it) {
								bool exists = false; predcts = cd->listOfPredicates();					 				
					 			for( unsigned z = 0; z < predcts.size(); z++ ) {
					 				if( predcts[z]->name == "P-" + it->first ) {
					 					exists = true;						 		
					 				}	
					 			}
					 			for( int z1 = 0; z1 < it->second; z1++ ) {
					 				std::vector<int> agentParam = cd->convertTypes(StringVec(1, "AGENT"));
					 				paramAppearInFor.insert( paramAppearInFor.end(), 
					 										agentParam.begin(), agentParam.end());
				 				}
					 			const StringVec & params = d->typeList( d->nodes[x] );
				 				StringVec mainParamList; mainParamList.push_back("AGENT");
				 				for ( unsigned ii = 0; ii < params.size(); ++ii) 
				 					mainParamList.push_back(params[ii]);
					 			if( !exists ) 
					 				cd->createPredicate( "P-" + it->first, mainParamList );						 			
					 		}
					 		
					 		// add as preconditions (neg (AND (prop1) (prop2) )): step 2(d)
							Forall *f = new Forall; 
							f->params = paramAppearInFor; 
							int incParamPos = 1;									
							And * andFormula = new And;							
							When * ss = new When; And * a1 = new And; 
							for( std::map<std::string, int>::iterator it = countTotalOcc.begin();
								it != countTotalOcc.end(); ++it) {
								for( int z1 = 0; z1 < it->second; z1++ ) {
									incParamPos += 1;
									std::vector<int> paramsNew = 
										incvec( size + incParamPos, size + incParamPos + 1 );
									std::vector<int> paramsNode = d->nodes[x]->templates[k]->params;
									paramsNew.insert( paramsNew.end(), (paramsNode).begin(), paramsNode.end());			
									a1->add( new Not ( new Ground( cd->preds.get( "P-" + it->first ), paramsNew ) ) );
									// f->cond = new Ground( cd->preds.get( "P-" + it->first ), paramsNew );								
									// dynamic_cast< And* >( doit->pre )->add( f );	
								}		 		
							}
							// ss->cond = a1;
							// andFormula->add(a1); 
							// Not * neg = new Not;
							f->cond = a1;
							dynamic_cast< And* >( do_start_part->pre )->add( f );	
						}							
					}
					else 
						cd->addPre( 0, start_JA, "AFREE" ); // part of CJR's extended work
					
					// add new effects
					if( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {								
						cd->addEff( 0, start_JA, "TAKEN", IntVec( 1, 0 ) );						
						cd->addEff( 1, start_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, start_JA, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
						cd->addEff( 0, start_JA, "IN-JOINT" );						
						
						std::stringstream convert; convert << 2;
						std::string s2 = convert.str();
						s2.erase(0, s2.find_first_not_of( " \n\r\t" ) );
						s2.erase(s2.find_last_not_of( " \n\r\t") + 1 );
						bool decision = canAddNewEffect( cd, start_JA, "NEXT-" + name + "-" + s2 );
						
						for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) { 
			 				if ( d->nodes[x]->templates[s]->name == name && decision ) 
								cd->addEff( 0, start_JA, "NEXT-"+ name + "-" + s2, d->nodes[x]->templates[s]->params );							
						}
						
						// Will add effects as per point 2(b), the action took place
						bool exists = false; predcts = cd->listOfPredicates();
			 			for( unsigned z = 0; z < predcts.size(); z++ ) 
			 				if ( predcts[z]->name == "P-" + d->actions[action]->name ) 
			 					exists = true;						 						 			
			 			
			 			const StringVec & params = d->typeList( d->nodes[x] );
			 			StringVec mainParamList; mainParamList.push_back("AGENT");
			 			for ( unsigned ii = 0; ii < params.size(); ++ii) 
			 				mainParamList.push_back(params[ii]);		
			 			if (! exists) 
			 				cd->createPredicate( "P-" + d->actions[action]->name, mainParamList );
			 			
			 			decision = canAddNewEffect( cd, start_JA, "P-" + d->actions[action]->name );
			 			for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) {
			 				const IntVec & params = d->nodes[x]->templates[s]->params;
			 				IntVec intVec; intVec.push_back(0);
			 				for ( unsigned ii = 0; ii < params.size(); ++ii) 
				 				intVec.push_back(params[ii]);
				 				
			 				if ( decision && d->nodes[x]->templates[s]->name == d->actions[action]->name )
								cd->addEff( 0, start_JA, "P-" + d->actions[action]->name, intVec );
						}
						
						// will add effects as per point 2(b), if collab. action contains that particular SA action
						set< std::string >::iterator iter;
						for( iter = setOfSAActions.begin(); iter != setOfSAActions.end(); ++iter )
						{
							std::string singleAgentAction = *iter;
							if( name.find(singleAgentAction) != std::string::npos )	
							{ 	
								// Add all the effects
								bool exists = false; predcts = cd->listOfPredicates();
					 			for (unsigned z = 0; z < predcts.size(); z++) 
					 				if ( predcts[z]->name == "P-" + singleAgentAction ) 
					 					exists = true;						 			
					 			
					 			// append AGENT for creating P-Action-Name( agt, constraint object set )
					 			const StringVec & params = d->typeList( d->nodes[x] );
					 			StringVec mainParamList; mainParamList.push_back("AGENT");
					 			for ( unsigned ii = 0; ii < params.size(); ++ii) 
					 				mainParamList.push_back(params[ii]);					 			
					 			if (! exists) 
					 				cd->createPredicate( "P-"+singleAgentAction, mainParamList );
					 			
					 			decision = canAddNewEffect( cd, start_JA, "P-" + singleAgentAction );
					 			for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) 
					 			{
					 				const IntVec & params = d->nodes[x]->templates[s]->params;
					 				IntVec intVec; intVec.push_back(0);
					 				for ( unsigned ii = 0; ii < params.size(); ++ii) 
						 				intVec.push_back(params[ii]);
					 				if ( decision && d->nodes[x]->templates[s]->name == d->actions[action]->name )
										cd->addEff( 0, start_JA, "P-" + singleAgentAction, intVec );
								}					
							}										
						}																		
					}	
					
					// TODO 
					// Not covered yet is the intermediate split actions, in case when #agents > 2, since we don't formulate 
					// such actions, jump to the line number 882
					// the below snippet is easy to modify as per the required intermediate components
					for ( unsigned agent = 2; agent < (unsigned) noOfAgents; agent++ ) {
						std::stringstream convert;
						convert << agent;
						const std::string intermediate_JA = "DO-" + name + "-" + convert.str();					
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
							concurEffs |= addEff( cd, doSecondPart, oldeff->conds[l], d->nodes[x]->name );
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
									
					// Create the k^th part of the joint action	
					std::stringstream convert;
					convert << noOfAgents;
					const std::string end_JA = "DO-" + name + "-" + convert.str();					
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
						concurEffs |= addEff( cd, doSecondPart, oldeff->conds[l], d->nodes[x]->name );
					if ( !oldeff ) 
						concurEffs |= addEff( cd, doSecondPart, d->actions[action]->eff );
					
					// add new parameters
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 )						
						cd->addParams( end_JA, StringVec( 2, "AGENT-COUNT" ) );
					
					// add new preconditions
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						std::stringstream convert; convert << noOfAgents;
						std::string s2 = convert.str();
						s2.erase(0, s2.find_first_not_of(" \n\r\t"));
						s2.erase(s2.find_last_not_of(" \n\r\t")+1);
						bool decision = canAddNewCondition( cd, start_JA, "NEXT-" + name + "-" + s2 );
						for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) 
			 				if ( d->nodes[x]->templates[s]->name == name && decision ) 
								cd->addPre( 0, end_JA, "NEXT-"+ name + "-" + s2, d->nodes[x]->templates[s]->params );
						
						cd->addPre( 0, end_JA, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
						cd->addPre( 1, end_JA, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, end_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, end_JA, "CONSEC", incvec( size, size + 2 ) );							
					}
					else 
						cd->addPre( 0, end_JA, "AFREE" );
					
					// newly added effects
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addEff( 1, end_JA, "IN-JOINT" );	
						std::stringstream convert; convert << noOfAgents;
						std::string s2 = convert.str();
						s2.erase(0, s2.find_first_not_of(" \n\r\t"));
						s2.erase(s2.find_last_not_of(" \n\r\t")+1);
						bool decision = canAddNewEffect( cd, end_JA, "NEXT-" + name + "-" + s2 );
						for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) 
			 				if ( d->nodes[x]->templates[s]->name == name && decision ) 
								cd->addEff( 1, end_JA, "NEXT-"+ name + "-" + s2, d->nodes[x]->templates[s]->params );

						cd->addEff( 0, end_JA, "TAKEN", IntVec( 1, 0 ) );
						cd->addEff( 1, end_JA, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, end_JA, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );
					}
				} 
				/*
				 ** When the action is single agent action irrespective of it being a component of a multi-action. */			
				else 
				{ 
					name = "DO-" + d->actions[action]->name;
					unsigned size = d->actions[action]->params.size();
					Action * doit = cd->createAction( name, d->typeList( d->actions[action] ) );
					
					// copy old preconditions
					And * oldpre = dynamic_cast< And * >( d->actions[action]->pre );
					if( oldpre ) doit->pre = new And( oldpre, *cd );
					else {
						And * a = new And;
						a->add( d->actions[action]->pre->copy( *cd ) );
						doit->pre = a;
					}

					// copy old effects
					And * oldeff = dynamic_cast< And * >( d->actions[action]->eff );
					for( unsigned l = 0; oldeff && l < oldeff->conds.size(); ++l ) 
						concurEffs |= addEff( cd, doit, oldeff->conds[l], d->nodes[x]->name );					
					if( !oldeff ) 
						concurEffs |= addEff( cd, doit, d->actions[action]->eff );

					// add new parameters
					if( i->second.size() > 1 || d->nodes[x]->upper > 1 )
						cd->addParams( name, StringVec( 2, "AGENT-COUNT" ) );

					// add new preconditions
					TokenStruct< Lifted * > predcts = cd->listOfPredicates();
					if( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addPre( 1, name, "IN-JOINT" ); 
						cd->addPre( 0, name, "ACTIVE-" + d->nodes[x]->name, d->nodes[x]->templates[k]->params );
						cd->addPre( 1, name, "TAKEN", IntVec( 1, 0 ) );
						cd->addPre( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addPre( 0, name, "CONSEC", incvec( size, size + 2 ) );

						// extra preconditions to avoid conflicting effects between two actions
						// captures the interfering actions 
						for( unsigned t = 0; t < pairOfProbActions.size(); t++ ) {
							std::map < std::string, std::vector < std::string > > mappedProbActions;
							mappedProbActions = pairOfProbActions[t][d->nodes[x]->name];
						 	std::vector< std::string > listAction = mappedProbActions[ (std::string) d->actions[action]->name ];
						 	for (unsigned y = 0; y < listAction.size(); y++) {
					 			bool exists = false; predcts = cd->listOfPredicates();
					 			for (unsigned z = 0; z < predcts.size(); z++) {
					 				if ( predcts[z]->name == "P-" + listAction[y] ) {
					 					exists = true;						 		
					 				}	
					 			}
					 			const StringVec & params = d->typeList( d->nodes[x] );
					 			StringVec mainParamList; mainParamList.push_back("AGENT");
					 			for ( unsigned ii = 0; ii < params.size(); ++ii) 
					 				mainParamList.push_back(params[ii]);
					 			if (! exists) {
					 				cd->createPredicate( "P-" + listAction[y], mainParamList );
					 			}
					 			
					 			bool decision = canAddNewCondition( cd, name, "P-" + listAction[y] );					 					
					 			for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) {
					 				const IntVec & params = d->nodes[x]->templates[s]->params;
					 				IntVec intVec; intVec.push_back(0);
					 				for ( unsigned ii = 0; ii < params.size(); ++ii) 
						 				intVec.push_back(params[ii]);
			 						if ( d->nodes[x]->templates[s]->name == d->actions[action]->name && decision ) {
				 						cd->addPre( 1, name, "P-" + listAction[y], intVec);
					 				}
					 			}
					 		}
						}			
						
						// TODO
						// implements the step 2(d), for each single agent action part of a Collab Action.						
						set< std::string >::iterator iter;
						for( iter = setOfCollabActions.begin(); iter != setOfCollabActions.end(); ++iter ) {							
							// if e(a) and e(current-collab-act) are not disjoint
							if( key_search( *iter, d->actions[action]->name )) {
								std::map< std::string, int > countTotalOcc; set< std::string >::iterator iterLoc;
								for( iterLoc = setOfSAActions.begin(); iterLoc != setOfSAActions.end(); ++iterLoc ) {
									int count = key_search( *iter, *iterLoc );
									if( 1 == key_search( *iterLoc, d->actions[action]->name ))
										count -= 1;
									if( count >= 1 )	
										countTotalOcc[*iterLoc] = count;	
								}		
														
								// create predicates if already not present, in the names of the components
								std::vector<int> paramAppearInFor={};
								for( std::map<std::string, int>::iterator it = countTotalOcc.begin();
									it != countTotalOcc.end(); ++it) {
									bool exists = false; predcts = cd->listOfPredicates();					 				
						 			for( unsigned z = 0; z < predcts.size(); z++ ) 
						 			{
						 				if( predcts[z]->name == "P-" + it->first ) 
						 					exists = true;						 	
						 			}		
					 				for( int z1 = 0; z1 < it->second; z1++ ) {
					 					std::vector<int> agentParam = cd->convertTypes(StringVec(1, "AGENT"));
					 					paramAppearInFor.insert( paramAppearInFor.end(), 
					 												agentParam.begin(), agentParam.end());
				 					}						 			
						 			const StringVec & params = d->typeList( d->nodes[x] );
					 				StringVec mainParamList; mainParamList.push_back("AGENT");
					 				for ( unsigned ii = 0; ii < params.size(); ++ii) 
					 					mainParamList.push_back(params[ii]);
						 			if( !exists ) 
						 				cd->createPredicate( "P-" + it->first, mainParamList );						 			
						 		}
						 		
						 		// add as preconditions (neg (AND (prop1) (prop2) )): step 2(d)
						 		// f->params = {1, 1}; //cd->convertTypes( StringVec( 1, "AGENT" ) );
								Forall *f = new Forall; 
								f->params = paramAppearInFor; int incParamPos = 1;	
									
								And * andFormula = new And;							
								When * ss = new When; And * a1 = new And; 
								for( std::map<std::string, int>::iterator it = countTotalOcc.begin();
									it != countTotalOcc.end(); ++it) {
									for( int z1 = 0; z1 < it->second; z1++ ) {
										incParamPos += 1;
										std::vector<int> paramsNew = 
											incvec( size + incParamPos, size + incParamPos + 1 );
										std::vector<int> paramsNode = d->nodes[x]->templates[k]->params;
										paramsNew.insert( paramsNew.end(), (paramsNode).begin(), paramsNode.end());			
										a1->add( new Not ( new Ground( cd->preds.get( "P-" + it->first ), paramsNew ) ) );
										// f->cond = new Ground( cd->preds.get( "P-" + it->first ), paramsNew );								
										// dynamic_cast< And* >( doit->pre )->add( f );	
									}		 		
								}
								// ss->cond = a1;
								// andFormula->add(a1); 
								// Not * neg = new Not;
								f->cond = a1;
								dynamic_cast< And* >( doit->pre )->add( f );	
							}
						}
						
						/**
						// extra preconditions to avoid two SA actions, if there is a JA available for them.
						for (unsigned t = 0; t < listOfJointActivityComponents.size(); t++) {							
							std::map < std::string, std::vector < std::string > > mappedCompActions;
							mappedCompActions = listOfJointActivityComponents[t][d->nodes[x]->name];
							std::vector< std::string > arrKeys;
							for( const auto& element : mappedCompActions ) {
								for( const auto& value : element.second )	
							   		if( value == d->actions[action]->name ) {
										arrKeys.push_back(element.first); 					
										break;
									}
							}	
							for( const auto& key : arrKeys ) {
								std::vector< std::string > listAction = mappedCompActions[ key ];
								if( listAction.size() == 1 ) {
									bool exists = false;
					 				predcts = cd->listOfPredicates();					 				
						 			for( unsigned z = 0; z < predcts.size(); z++ ) {
						 				if( predcts[z]->name == "P-" + listAction[0] ) {
						 					exists = true;						 		
						 				}	
						 			}
						 			if( !exists ) {
						 				cd->createPredicate( "P-" + listAction[0], d->typeList( d->nodes[x] ) );
						 			}
						 			
						 			// The name is only to avoid same action name in a JA. For example: Activity-A1-A2
						 			bool decision = canAddNewCondition( cd, name, "P-" + listAction[0] );						 			
						 			for( unsigned s = 0; s < d->nodes[x]->templates.size(); s++ ) {
				 						if( d->nodes[x]->templates[s]->name == d->actions[action]->name && decision )
						 					cd->addPre( 1, name, "P-" + listAction[0], d->nodes[x]->templates[s]->params);
						 			}						 			
						 		} 
						 		else 	
						 			for( unsigned h = 0; h < listAction.size(); h++ ) {							 				
						 				bool exists = false;
						 				bool sameActions = ( listAction[h] == d->actions[action]->name );	
						 				predcts = cd->listOfPredicates();						 				
								 		for( unsigned z = 0; z < predcts.size(); z++ ) {									 			
								 			if( (predcts[z]->name == "P-" + listAction[h]) ) {									 				
								 				exists = true;						 		
								 			}	
								 		}
							 			if( !exists && !sameActions ) {
							 				cd->createPredicate( "P-" + listAction[h], d->typeList( d->nodes[x] ) );								 			 	
							 			}
							 			bool decision = canAddNewCondition( cd, name, "P-" + listAction[h] );
							 			if( !sameActions )
								 			for( unsigned s = 0; s < d->nodes[x]->templates.size(); s++ ) 
						 						if( d->nodes[x]->templates[s]->name == d->actions[action]->name && decision )
								 					cd->addPre( 1, name, "P-" + listAction[h], d->nodes[x]->templates[s]->params);
						 			}						 									 		
							}
						} */
					}					
					else 
						cd->addPre( 0, name, "AFREE" );					
								
					if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
						cd->addEff( 0, name, "TAKEN", IntVec( 1, 0 ) );
						cd->addEff( 1, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
						cd->addEff( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size + 1, size + 2 ) );										
						bool exists = false;
						predcts = cd->listOfPredicates();
			 			for (unsigned z = 0; z < predcts.size(); z++) 
			 				if ( predcts[z]->name == "P-" + d->actions[action]->name ) 
			 					exists = true;						 			
			 			
			 			const StringVec & params = d->typeList( d->nodes[x] );
			 			StringVec mainParamList; mainParamList.push_back("AGENT");
			 			for ( unsigned ii = 0; ii < params.size(); ++ii) 
			 				mainParamList.push_back(params[ii]);
			 				
			 			if (! exists) 
			 				cd->createPredicate( "P-" + d->actions[action]->name, mainParamList );
			 			
			 			bool decision = canAddNewEffect( cd, name, "P-" + d->actions[action]->name );
			 			for ( unsigned s = 0; s < d->nodes[x]->templates.size(); s++) {
			 				const IntVec & params = d->nodes[x]->templates[s]->params;
			 				IntVec intVec; intVec.push_back(0);
			 				for ( unsigned ii = 0; ii < params.size(); ++ii) 
				 				intVec.push_back(params[ii]);
			 				if ( decision && d->nodes[x]->templates[s]->name == d->actions[action]->name )
								cd->addEff( 0, name, "P-" + d->actions[action]->name, intVec );
						}
					}
				}
			}
		
			// Our version of END- action that does all the bookkeepings, as per our algorithm
			if ( i->second.size() > 1 || d->nodes[x]->upper > 1 ) {
				std::string name = "END-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				Action * end = cd->createAction( name, d->typeList( d->nodes[x] ) );
				cd->addParams( name, StringVec( 1, "AGENT-COUNT" ) );

				// all preconditions
				cd->addPre( 1, name, "IN-JOINT" );				
				cd->addPre( 0, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
				cd->addPre( 0, name, "SAT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
				cd->addPre( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );				
				
				// all effects including the new ones needed for aaai
				cd->addEff( 1, name, "COUNT-" + d->nodes[x]->name, incvec( size, size + 1 ) );
				if ( i->second.size() > 1 ) {
					cd->addEff( 0, name, "DONE-" + d->nodes[x]->name );				
				}
				else {					
					cd->addEff( 0, name, "AFREE" );
					cd->addEff( 1, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );
					
					// the below snippet is for adding all the P-Action( n->nodes[x]->params )
					for( unsigned d1 = 0; d1 < d->nodes[x]->templates.size(); d1++) {
						int action = d->actions.index( d->nodes[x]->templates[d1]->name );
						std::string actionName = d->actions[action]->name;
						// if( actionName.find( "ACTIVITY" ) == std::string::npos ) : changed later
						cd->addEff( 1, name, "P-" + actionName, incvec( 0, size ) );
					}
					
					Forall * f = new Forall;		
					// f->params = {1, 1};//cd->convertTypes( StringVec( 1, "AGENT" ) );
					f->params = cd->convertTypes( StringVec( 1, "AGENT" ) );
					f->cond = new Not( new Ground( cd->preds.get( "TAKEN" ),  incvec(size+1, size+2)));
					dynamic_cast< And* >( end->eff )->add( f );
					
					/** addition of quantifiers ***/	
					probVector = node_wise_probVector[ (std::string) d->nodes[x]->name ];										
					std::set< std::vector < unsigned >>::iterator it;
					std::vector< std::vector< unsigned >> choicesToAdd;
					std::vector< unsigned > choicesToAdd_flag;
					for( it = probVector.begin(); it != probVector.end(); ++it ) { 
						std::vector< unsigned > deleteChoices  = ( std::vector< unsigned > ) *it;				
						choicesToAdd.push_back( deleteChoices );
						choicesToAdd_flag.push_back( 0 );
					}
					
					// the generation method of choicesToAdd_copy might have some error.
					std::vector< std::vector< unsigned >> choicesToAdd_copy;					
					for( unsigned indx = 0; indx < choicesToAdd.size(); indx++ ) {
						if( choicesToAdd_flag[ indx ] == 1 )
							continue;
						std::vector< unsigned > choicesA = choicesToAdd[ indx ];
						unsigned realIndex = indx;
						unsigned ch1 = choicesA[ 0 ]; unsigned ch2 = choicesA[ 1 ];
						choicesToAdd_flag[ indx ] = 1;
						for( unsigned indy = indx + 1; indy < choicesToAdd.size(); indy++ ) {
							std::vector< unsigned > choicesB = choicesToAdd[ indy ];
							unsigned ch3 = choicesB[ 0 ]; unsigned ch4 = choicesB[ 1 ];	
							if( ch1 == ch3 && choicesToAdd_flag[ indy ] != 1 ) {
								choicesToAdd_flag[ indy ] = 1;
								if( ch2 >= ch4 ) 
									realIndex = indy;
							}
						}
						choicesToAdd_copy.push_back( choicesToAdd[ realIndex ] );						
					}
									
					// this loop brings all the POS- and NEG- predicates in the effects 
					for( unsigned dx = 0; dx < choicesToAdd_copy.size(); dx++ ) {
						unsigned i = (unsigned) choicesToAdd_copy[ dx ][ 0 ];
						unsigned choice = (unsigned) choicesToAdd_copy[ dx ][ 1 ];
												
						Forall * f1 = new Forall;
						And * andFormula = new And;							
						When * ss = new When; And * a1 = new And; 
						IntVec nodeParam = cd->convertTypes( d->typeList( d->nodes[x] ) ); 						
						IntVec predParam = cd->convertTypes( d->typeList( d->preds[i] ) ); 
						IntVec predParam_flag;
						for( unsigned d1 = 0; d1 < predParam.size(); d1++ )
							predParam_flag.push_back( -1 );
						
						unsigned indxI = nodeParam.size() + 1;
						for( unsigned d2 = 0; d2 < predParam.size(); d2++ ) {
							int counter = 0;
							for( unsigned d1 = 0; d1 < nodeParam.size(); d1++ ) 
								if( nodeParam[ d1 ] == predParam[ d2 ] )
								 	counter++;
								 								
							for( unsigned d1 = 0; d1 < nodeParam.size() && counter >= 2; d1++ )  
								if( nodeParam[ d1 ] == predParam[ d2 ] ) {
									nodeParam.erase( nodeParam.begin() + d1 );
									d1--;									
								}
						}
						
						for( unsigned d2 = 0; d2 < predParam.size(); d2++ ) {
							if( predParam_flag[ d2 ] != -1 ) 
								continue;													
							for( unsigned d1 = 0; d1 < nodeParam.size(); d1++ )		
								if( nodeParam[ d1 ] == predParam[ d2 ] ) {
									predParam_flag[ d2 ] = 0;
									predParam[ d2 ] = d1;
									break;
								}
						}										
						
						IntVec predParams, forAllParams;
						for( unsigned d2 = 0; d2 < predParam.size(); d2++ ) 							  
							if( predParam_flag[ d2 ] == -1 ) {
								forAllParams.push_back( predParam[ d2 ] );
								predParams.push_back( indxI++ );								
							}
							else 
								predParams.push_back( predParam[ d2 ] );
						
						if ( choice == 1 ) {	
							f1->params = forAllParams;		
							ss->pars = new Ground ( cd->preds.get( "POS-" + d->preds[i]->name ), predParams );							
							a1->add( new Ground ( cd->preds.get( d->preds[i]->name ), predParams ) );
							a1->add( new Not ( new Ground( cd->preds.get( "POS-" + d->preds[i]->name ), predParams ) ) );
							ss->cond = a1;							
							andFormula->add(ss);	
														
							ss = new When;							
							ss->pars = new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), predParams );
							a1 = new And;
							a1->add( new Not( new Ground( cd->preds.get( d->preds[i]->name ), predParams ) ) );
							a1->add( new Not( new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), predParams ) ) );
							ss->cond = a1;
							andFormula->add(ss);
							f1->cond = andFormula;
							dynamic_cast< And * >( end->eff )->add( f1 );													
						}
						else if ( choice == 2 ) {
							f1 = new Forall;
							f1->params = forAllParams; 
							ss = new When;							
							ss->pars = new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), predParams );
							a1 = new And;
							a1->add( new Not( new Ground( cd->preds.get( d->preds[i]->name ), predParams ) ) );
							a1->add( new Not( new Ground( cd->preds.get( "NEG-" + d->preds[i]->name ), predParams ) ) );
							ss->cond = a1;
							f1->cond = ss;
							dynamic_cast< And * >( end->eff )->add( f1 );	
						}
						predParams.clear();
						forAllParams.clear();
					}
					probVector.clear();
				}
			}

			if ( i->second.size() > 1 && j + 1 == i->second.size() ) {
				std::string name = "FINISH-" + d->nodes[x]->name;
				unsigned size = d->nodes[x]->params.size();
				Action * finish = cd->createAction( name, d->typeList( d->nodes[x] ) );

				cd->addOrPre( name, "DONE-" + d->nodes[x]->name, "SKIPPED-" + d->nodes[x]->name );
				cd->addPre( 0, name, "ACTIVE-" + d->nodes[x]->name, incvec( 0, size ) );

				// cd->addEff( 0, name, "ATEMP" );
				for( unsigned k = 0; k < i->second.size(); ++k ) {
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
	unsigned nagents = 0;
	for( unsigned i = 0; i < d->nodes.size(); ++i ) {
		VecMap::iterator j = ccs.find( d->mf[i] );
		if( j->second.size() > 1 || d->nodes[i]->upper > 1 ) 
			if( (d->nodes[i]->upper > nagents) && (d->nodes[i]->upper != 1000000) )
				nagents = d->nodes[i]->upper;		
	}	

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
	
	// print it via standard error output '2>'
	// std::cerr << *cins;
	
	// end time
	t2 = clock();
		
	float diff ((float)t2-(float)t1);
    std::cout<< "\n\n;Total compilation time = " << diff/CLOCKS_PER_SEC << std::endl <<"\n";
    
	delete cins;
	delete cd;
	delete ins;
	delete d;    
}
