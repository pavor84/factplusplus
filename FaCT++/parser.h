/* This file is part of the FaCT++ DL reasoner
Copyright (C) 2003-2009 by Dmitry Tsarkov

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

#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include "comparser.h"
#include "dltree.h"
#include "Kernel.h"

/// class for parsing LISP-like ontologies
class DLLispParser: public CommonParser
{
protected:	// members
		/// Kernel to be filled
	ReasoningKernel* Kernel;

protected:	// methods
		/// error by given exception
	void errorByException ( const EFPPCantRegName& ex ) const { parseError(ex.what()); }

		/// @return concept-like Id of just scanned name
	DLTree* getConcept ( void )
	{
		DLTree* ret = Kernel->Concept(scan.GetName());
		NextLex();
		return ret;
	}
		/// @return singleton Id of just scanned name
	DLTree* getSingleton ( void )
	{
		DLTree* ret = Kernel->Individual(scan.GetName());
		NextLex();
		return ret;
	}
		/// @return role-like Id of just scanned name
	DLTree* getRole ( void )
	{
		DLTree* ret = Kernel->Role(scan.GetName());
		NextLex();
		return ret;
	}
		/// @return role-like Id of just scanned name
	DLTree* getDataRole ( void )
	{
		DLTree* ret = Kernel->DataRole(scan.GetName());
		NextLex();
		return ret;
	}
		/// @return datavalue of a data type TYPE with an Id of a just scanned name
	DLTree* getDTValue ( DLTree* type )
	{
		DLTree* ret = Kernel->getDataTypeCenter().getDataValue(scan.GetName(),type);
		NextLex();
		deleteTree(type);
		return ret;
	}

		/// get role expression, ie role, role's inverse or (if allowComplex) complex role expression
	DLTree* getRoleExpression ( bool allowComplex );
		/// parse simple DL command
	void parseCommand ( void );
		/// parse role arguments if defprimrole command
	void parseRoleArguments ( DLTree* role );
		/// parse list of concept expressions (in disjoint-like commands)
	void parseConceptList ( bool singletonsOnly );
		/// get concept-like expression for simple variants
	DLTree* processConceptTree ( void );
		/// get concept-like expression for complex constructors
	DLTree* processComplexConceptTree ( void );

public:		// interface
		/// the only c'tor
	DLLispParser ( std::istream* in, ReasoningKernel* kernel )
		: CommonParser (in)
		, Kernel (kernel)
	{
	}
		/// empty d'tor
	~DLLispParser ( void ) {}

		/// main parsing method
	void Parse ( void );
};	// DLLispParser

#endif