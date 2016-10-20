#include "stdafx.h"
#include "Utilities.h"

/*static*/int CUtilities::strcmp_caseinsensitive(const char *string1, const char *string2)
{
#if defined(_MSC_VER)
	return _stricmp(string1, string2);
#else
	return strcasecmp(string1, string2);
#endif
}