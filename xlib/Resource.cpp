/*
 * Copyright 2021, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT license.
 */

#include <unordered_map>
#include <string>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#include "xorg/Xlcint.h"
#include "xorg/Xresinternal.h"
}

#include "Debug.h"

static std::unordered_map<XrmQuark, std::string> sQuarksToStrings;
static std::unordered_map<std::string, XrmQuark> sStringsToQuarks;
static XrmQuark sLastQuark = 1;

extern "C" XrmQuark
XrmUniqueQuark()
{
	return sLastQuark++;
}

extern "C" XrmString
XrmQuarkToString(XrmQuark quark)
{
	const auto& result = sQuarksToStrings.find(quark);
	if (result == sQuarksToStrings.end())
		return NULL;

	return strdup(result->second.c_str());
}

extern "C" XrmQuark
_XrmInternalStringToQuark(const char* name, int len, Signature sig, Bool permstring)
{
	if (!name)
		return 0;

	const std::string string(name, len < 0 ? strlen(name) : len);
	const auto& result = sStringsToQuarks.find(string);
	if (result != sStringsToQuarks.end())
		return result->second;

	XrmQuark quark = XrmUniqueQuark();
	sQuarksToStrings.insert({quark, string});
	sStringsToQuarks.insert({string, quark});
	return quark;
}

extern "C" XrmQuark
XrmStringToQuark(const char* string)
{
	return _XrmInternalStringToQuark(string, -1, -1, False);
}

extern "C" XrmQuark
XrmPermStringToQuark(const char* string)
{
	return _XrmInternalStringToQuark(string, -1, -1, True);
}

extern "C" XrmMethods
_XrmInitParseInfo(XPointer* state)
{
	return NULL;
}
