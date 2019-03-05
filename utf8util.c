#include <stdio.h>
#include "utf8.h"

utf8* ReadUTF8(FILE* fp, int offset)
{
	unsigned char b[3] = { 0 };
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	utf8 * str = calloc(1,sz + 1 - offset);
	if (str == NULL) {
		printf("out of memory at main");
		printf("TODO clean up the memory");
		abort();
	}
	rewind(fp);
	// bypass utf8 BOM
	if (offset > 0) {
		fread(b, 1, offset, fp);
	}
	fread(str, 1, sz - offset, fp);
	fclose(fp);
	return str;
}

utf8* ReadUTF16BE(FILE* fp)
{
	return NULL;
}

utf8* ReadUTF16LE(FILE* fp)
{
	return NULL;
}

utf8* read_utf8_file(utf8* filename) {
	FILE* fp = fopen(filename, "rb");
	utf8* str = NULL;
	if (fp != NULL)
	{
		// see http://en.wikipedia.org/wiki/Byte-order_mark for explaination of the BOM
		// encoding
		unsigned char b[3] = { 0 };
		fread(b, 1, 2, fp);
		if (b[0] == 0xEF && b[1] == 0xBB)
		{
			str = ReadUTF8(fp, 3);
		}
		else if (b[0] == 0xFE && b[1] == 0xFF)
		{
			str = ReadUTF16BE(fp);
		}
		else if (b[0] == 0 && b[1] == 0)
		{
			str = ReadUTF16LE(fp);
		}
		else
		{
			// we don't know what kind of file it is, so assume its standard
			// ascii with no BOM encoding
			rewind(fp);
			str = ReadUTF8(fp, 0);
		}
		fclose(fp);
	}
	else {
		printf("Invalid file name : %s", (char*)filename);
	}
	return str;
}

