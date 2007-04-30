/* This file is part of the FaCT++ DL reasoner
Copyright (C) 2003-2007 by Dmitry Tsarkov

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "tAxiom.h"
#include "tRole.h"

bool TAxiom :: simplify ( void )
{
	bool ret = false;
	register unsigned int i = 0;
	while ( i < Disjuncts.size() )
	{
		DLTree* p = Disjuncts[i];

		if ( isPosNP(p) )
			simplifyPosNP(i);
		else if ( isNegNP(p) )
			simplifyNegNP(i);
		else if ( isOr(p) )
			simplifyOr(i);
		else
		{
			++i;
			continue;
		}

		// something was simplified
		ret = true;
	}

	return ret;
}

DLTree* TAxiom :: createAnAxiom ( void )
{
	assert ( !Disjuncts.empty() );	// could not create an axiom from empty absorption set

	const_iterator p = begin();

	// create new OR vertex for the axiom:
	DLTree* Or = *p;

	for ( ++p; p != end(); ++p )
		Or = createSNFAnd ( *p, Or );

	assert ( isSNF (Or) );	// safety check for G

	inUse = true;
	return createSNFNot(Or);
}

#ifdef RKG_DEBUG_ABSORPTION
void TAxiom :: dump ( std::ostream& o ) const
{
	o << "(neg-and ";
	for ( const_iterator p = begin(), p_end = end(); p != p_end; ++p )
		o << *p;
	o << ")" << std::endl;
}
#endif

unsigned int TAxiom :: absorbIntoConcept ( void )
{
	std::vector<iterator> Cons;

	// finds all primitive concept names
	for ( iterator p = begin(); p != end(); ++p )
		if ( isName(*p) &&		// FIXME!! review this during implementation of Nominal Absorption
			 getConcept(*p)->isPrimitive() )
			Cons.push_back(p);

	// if no concept names -- return;
	if ( Cons.empty() )
		return 0;

	// FIXME!! as for now: just take the 1st concept name
	iterator bestConcept = Cons[0];

	// locate concept
	TConcept* Concept = getConcept(*bestConcept);

	replace ( new DLTree(TOP), bestConcept );

#ifdef RKG_DEBUG_ABSORPTION
	std::cout << " C-Absorb GCI to concept " << Concept->getName() << ": ";
	dump(std::cout);
#endif

	// adds a new definition
	Concept->addDesc(createAnAxiom());
	Concept->removeSelfFromDescription();

	return Cons.size();
}

unsigned int TAxiom :: absorbIntoDomain ( void )
{
	std::vector<iterator> Cons;
	iterator bestSome = end();

	// find all forall concepts
	for ( iterator p = begin(); p != end(); ++p )
		if ( (*p)->Element() == NOT &&
			 ( (*p)->Left()->Element() == FORALL	// \neg ER.C
			   || (*p)->Left()->Element() == LE ))	// \neg >= n R.C
		{
			Cons.push_back(p);
			// check for the direct domain case
			if ( (*p)->Left()->Right()->Element() == BOTTOM )
			{	// found proper absorption candidate
				bestSome = p;
				break;
			}
		}

	// if there are no EXISTS concepts -- return;
	if ( Cons.empty() )
		return 0;

	TRole* Role;

	if ( bestSome != end() )
	{
		Role = resolveRole ( (*bestSome)->Left()->Left() );
		// replace the SOME concept expression with TOP
		replace ( new DLTree(TOP), bestSome );
	}
	else
		// FIXME!! as for now: just take the 1st concept name
		Role = resolveRole ( (*Cons[0])->Left()->Left() );

#ifdef RKG_DEBUG_ABSORPTION
	std::cout << " R-Absorb GCI to the domain of role " << Role->getName() << ": ";
	dump(std::cout);
#endif

	assert ( Role != NULL );
	Role->setDomain(createAnAxiom());

	return Cons.size();
}
