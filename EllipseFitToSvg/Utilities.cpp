#include "stdafx.h"
#include "Utilities.h"

#if !defined(_MSC_VER)
#include <strings.h>
#endif

/*static*/int CUtilities::strcmp_caseinsensitive(const char *string1, const char *string2)
{
#if defined(_MSC_VER)
	return _stricmp(string1, string2);
#else
	return strcasecmp(string1, string2);
#endif
}