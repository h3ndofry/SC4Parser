// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>

void printfunction(int another)
{
	printf("%d", another);
}

int main()
{
    std::cout << "Hello World!\n"; 
}

void ReadBufferFromFile(char* path, char *buffer)
{

}

extern "C"
{
	__declspec(dllexport)
	void
	__cdecl
	test(void *Array, int size)// char array[], int size)
	{
		//printf("%d", number);
		char *char_array;
		char_array = (char*)Array;
		char_array[0] = 'b';
		printf("%s", char_array[0]);
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// Decode file (LUA & other compressed files)
// Input : Data.file.input : input buffer
//         Data.filesize   : input buffer size
// Output: Data.file.output: output buffer (CMemFile)

// New generation FSH/QFS decompressor/compressor
// Version 1.22 - copyright (c) Denis Auroux 1998-2002
// auroux@math.polytechnique.fr

void mmemcpy(unsigned char *dest, unsigned char *src, int len) /* LZ-compatible memcopy */
{
	while (len--) *(dest++) = *(src++);
}

unsigned char *uncompress_data(unsigned char *inbuf, int *buflen)
{
	unsigned char *outbuf;
	unsigned char packcode;
	int a, b, c, len, offset;
	int inlen, outlen, inpos, outpos;

	/* length of data */
	inlen = *buflen;
	outlen = (inbuf[2] << 16) + (inbuf[3] << 8) + inbuf[4];
	outbuf = (unsigned char*)malloc(outlen);
	if (outbuf == NULL)
	{
		printf("Insufficient memory.\n");
		free(outbuf);
		return NULL;
	}

	/* position in file */
	if (inbuf[0] & 0x01) inpos = 8; else inpos = 5;
	outpos = 0;

	/* main decoding loop */
	while ((inpos < inlen) && (inbuf[inpos] < 0xFC))
	{



		packcode = inbuf[inpos];
		a = inbuf[inpos + 1];
		b = inbuf[inpos + 2];

		if (!(packcode & 0x80)) {
			len = packcode & 3;
			mmemcpy(outbuf + outpos, inbuf + inpos + 2, len);
			inpos += len + 2;
			outpos += len;
			len = ((packcode & 0x1c) >> 2) + 3;
			offset = ((packcode >> 5) << 8) + a + 1;
			mmemcpy(outbuf + outpos, outbuf + outpos - offset, len);
			outpos += len;
		}
		else if (!(packcode & 0x40)) {
			len = (a >> 6) & 3;
			mmemcpy(outbuf + outpos, inbuf + inpos + 3, len);
			inpos += len + 3;
			outpos += len;
			len = (packcode & 0x3f) + 4;
			offset = (a & 0x3f) * 256 + b + 1;
			mmemcpy(outbuf + outpos, outbuf + outpos - offset, len);
			outpos += len;
		}
		else if (!(packcode & 0x20)) {
			c = inbuf[inpos + 3];
			len = packcode & 3;
			mmemcpy(outbuf + outpos, inbuf + inpos + 4, len);
			inpos += len + 4;
			outpos += len;
			len = ((packcode >> 2) & 3) * 256 + c + 5;
			offset = ((packcode & 0x10) << 12) + 256 * a + b + 1;
			mmemcpy(outbuf + outpos, outbuf + outpos - offset, len);
			outpos += len;
		}
		else {
			len = (packcode & 0x1f) * 4 + 4;
			mmemcpy(outbuf + outpos, inbuf + inpos + 1, len);
			inpos += len + 1;
			outpos += len;
		}
	}

	/* trailing bytes */
	if ((inpos < inlen) && (outpos < outlen)) {
		mmemcpy(outbuf + outpos, inbuf + inpos + 1, inbuf[inpos] & 3);
		outpos += inbuf[inpos] & 3;
	}

	if (outpos != outlen)
		printf("Warning: bad length ? %d instead of %d\n", outpos, outlen);
	*buflen = outlen;
	return outbuf;
}


