#include <stdio.h>
int main(int argc, char** argv)
{
	int x;
	int *px;

	x = 10;
	px = &x;
	printf("%p\n", px);

	*px = 11;
	printf("%d\n", x);

	return 0;
}

