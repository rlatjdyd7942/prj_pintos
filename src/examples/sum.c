#include <stdio.h>
#include <syscall.h>

int atoi(const char *str) {
	int i = 0;
	int num = 0;
	while (str[i] != '\0') {
		if ('0' <= str[i] && str[i] <= '9') {
			num *= 10;
			num += str[i] - '0';
		} else {
			return -1;
		}
	}
	return num;
}

int
main (int argc, char **argv) {
	int i = 0;
	int val[4];
	for (i = 0; i < argc; ++i) {
		val[i] = atoi(argv[i]);
	}
	printf("%d\n", sum_of_four_integers(val[0], val[1], val[2], val[3]));
	return EXIT_SUCCESS;
}
