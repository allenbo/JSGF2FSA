#include <stdio.h>
#include <sys/time.h>
#include "parser.h"
#include "node.h"


#define BUFF_SIZE 1024

int main(int argc, char* argv[]) {
	
	if(3 != argc) {
		printf("Usage: %s grammar_file out_file\n", argv[0]);
		return 0;
	}

	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL);
	Parser parser(argv[1]);
	parser.parse();
	parser.printNode(argv[2]);
	gettimeofday(&end, NULL);
	printf("Total time is %fs\n", (end.tv_sec-start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0);
	return 0;
}
