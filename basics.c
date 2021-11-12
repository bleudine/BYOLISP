#include <stdio.h>

void printHello(int n) {
	for (int i = 0; i < n; i++) {
		printf("%d. When the hurlyburly's done!\n", i);
	}
	return;
}

int main(int argc, char** argv) {
	for (int i = 0; i < 5; i++) {
		printf("%d. Hello there\n", i);
	}
	
	int count = 0;
	while (count < 5) {
		printf("%d. Hello WORLD !!\n", count);
		count++;
	}
	printHello(10);
	return 0;
}