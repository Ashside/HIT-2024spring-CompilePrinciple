#include "Semantic.h"

unsigned int getHash(chat *name)
{
	unsigned int val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~0x3fff)
		{
			val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
		}
	}
	return val;
}

void reportError(ErrorTypeEnum errorType, int line, const char *msg)
{
	printf("Error type %d at Line %d: %s\n", errorType, line, msg);
}