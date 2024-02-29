#include <stdio.h>
#include <assert.h>

#include "hanoi-nr.c"

int main() {
	hanoi(3, 'A', 'B', 'C');
	return 0;
}
