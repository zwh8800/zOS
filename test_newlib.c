#include <stdio.h>

void test_newlib()
{
	FILE* fp = fopen("console1", "r");
	fprintf(fp, "hehe\r\n");
	
}
