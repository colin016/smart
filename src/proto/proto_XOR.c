#include "proto.h"

char start_XOR(char *str0, char *str1,char *output) {
	size_t i = 0;
	size_t str0Len;
	size_t str1Len;

	str0Len = strlen(str0);
	str1Len = strlen(str1);

	if (str0Len > str1Len) {
		int j = 0;
		for (i = 0; i < str0Len; i++) {
			output[i] = str0[i] ^ str1[j++];
			if(j==str1Len)
			{
				j=0;
			}
		}
	} else {
		int j = 0;
		for (i = 0; i < str1Len; i++) {
			output[i] = str0[j++] ^ str1[i];
			if (j == str0Len) {
				j = 0;
			}
		}
	}

	return 0;
}
