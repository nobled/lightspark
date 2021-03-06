/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009,2010  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef _FLASH_SYSTEM_H
#define _FLASH_SYSTEM_H

#include "compat.h"
#include "asobject.h"

namespace lightspark
{

class Capabilities: public ASObject
{
public:
	Capabilities(){};
	static void sinit(Class_base* c);
	ASFUNCTION(_constructor);
	ASFUNCTION(_getLanguage);
};

class ApplicationDomain: public ASObject
{
public:
	ApplicationDomain(){}
	static void sinit(Class_base* c);
	static void buildTraits(ASObject* o);
	ASFUNCTION(_constructor);
	ASFUNCTION(_getCurrentDomain);
	ASFUNCTION(hasDefinition);
	ASFUNCTION(getDefinition);
};

class Security: public ASObject
{
public:
	static void sinit(Class_base* c);
	ASFUNCTION(_getExactSettings);
	ASFUNCTION(_setExactSettings);
	ASFUNCTION(_getSandboxType);
	ASFUNCTION(allowDomain);
	ASFUNCTION(allowInsecureDomain);
	ASFUNCTION(loadPolicyFile);
	ASFUNCTION(showSettings);
};

ASObject* fscommand(ASObject* obj,ASObject* const* args, const unsigned int argslen);

};
#endif
