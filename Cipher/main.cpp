#include <stdio.h>
#include "cipher.h"

int main(int argc, char const *argv[]) {
	std::string text = "BBBB";
	text = cipher::encrypt(text, 2);
	printf("%s\n", text.c_str());
	getchar();
	return 0;
}
