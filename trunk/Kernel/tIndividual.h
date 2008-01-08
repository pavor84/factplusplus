/* This file is part of the FaCT++ DL reasoner
Copyright (C) 2003-2008 by Dmitry Tsarkov

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

#ifndef _TINDIVIDUAL_H
#define _TINDIVIDUAL_H

#include "tConcept.h"

class DlCompletionTree;
class TRelated;

/// class to represent individuals
class TIndividual: public TConcept
{
public:		// types
		/// pointers to RELATED constructors
	typedef std::vector<TRelated*> RelatedSet;
		/// pointers to concept expressions
	typedef std::vector<const DLTree*> ConceptSearchSet;

public:		// members
		/// pointer to nominal node (works for singletons only)
	DlCompletionTree* node;

		/// index for axioms <this,C>:R
	RelatedSet RelatedIndex;

	// precompletion support

		/// vector that contains LINKS to the concept expressions that are labels of an individual
	ConceptSearchSet CSSet;
		/// new concept expression to be added to the label
	DLTree* PCConcept;

public:		// interface
		/// the only c'tor
	explicit TIndividual ( const std::string& name )
		: TConcept(name)
		, node(NULL)
		, PCConcept(NULL)
		{}
		/// empty d'tor
	virtual ~TIndividual ( void ) { delete PCConcept; }

		/// check whether a concept is indeed a singleton
	virtual bool isSingleton ( void ) const { return true; }
		/// init told subsumers of the individual by it's description
	virtual void initToldSubsumers ( void )
	{
		getTold().clear();
		if ( isRelated() )	// check if domain and range of RELATED axioms affects TS
			updateToldFromRelated();
		// normalise description if the only parent is TOP
		if ( isPrimitive() && Description && Description->Element() == TOP )
			removeDescription();

		// not a completely defined if there are extra rules or related individuals
		bool CD = !hasExtraRules() && isPrimitive() && !isRelated();
		if ( Description != NULL || !getTold().empty() )
			CD &= TConcept::initToldSubsumers(Description);
		setCompletelyDefined(CD);
	}

	// related things

		/// update told subsumers from the RELATED axioms in a given range
	template<class Iterator>
	void updateTold ( Iterator begin, Iterator end )
	{
		for ( Iterator p = begin; p < end; ++p )
			SearchTSbyRoleAndSupers((*p)->getRole());
	}
		/// update told subsumers from all relevant RELATED axioms
	void updateToldFromRelated ( void );
		/// check if individual connected to something with RELATED statement
	bool isRelated ( void ) const { return !RelatedIndex.empty(); }
		/// set individual related
	void addRelated ( TRelated* p ) { RelatedIndex.push_back(p); }

	// precompletion interface

		/// check whether EXPR already exists in the precompletion set
	bool containsPCExpr ( const DLTree* expr ) const
	{
		if ( expr == NULL )	// special case it
			return true;
		for ( ConceptSearchSet::const_iterator p = CSSet.begin(), p_end = CSSet.end(); p < p_end; ++p )
			if ( equalTrees ( *p, expr ) )
				return true;

		return false;
	}
		/// unconditionally adds EXPR to the precompletion information
	void addPCExprAlways ( const DLTree* expr )
	{
		CSSet.push_back(expr);
		PCConcept = createSNFAnd ( PCConcept, clone(expr) );
	}
		/// add EXPR to th ePC information if it is a new expression; @return true if was added
	bool addPCExpr ( const DLTree* expr )
	{
		if ( containsPCExpr(expr) )
			return false;
		addPCExprAlways(expr);
		return true;
	}
		/// update individual's description from precompletion information
	void usePCInfo ( void )
	{
		delete Description;
		Description = PCConcept;
		PCConcept = NULL;

		// we change description of a concept, so we need to rebuild the TS info
		// note that precompletion succeed; so there is no need to take into account
		// RELATED information
		TConcept::initToldSubsumers();
	}
		/// remove all precompletion-related information
	void clearPCInfo ( void ) { delete PCConcept; PCConcept = NULL; CSSet.clear(); }
}; // TIndividual

#endif

