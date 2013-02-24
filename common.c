#include "stdio.h"
#include<signal.h>
extern void checkpoint_here();

int main(int argc,char *argv[])
{
	int stack_top;
	int recover = 0;
	signal (SIGINT, checkpoint_here);

	printf("stack top variable address %x",&stack_top);
	if(argc == 2)
	{

		if(argv[1][0] != '-')
		{
			printf("bad command line\n");
			exit(-1);
		
	
		}
		else
		{
			if(argv[1][1] == 'r')
			{
				recover = 1;
			}
			else
			{
				printf("bad command line\n");
				exit(-1);
			}
		}

	}
	else if(argc > 2)
	{
		printf("bad command line");
		exit(-1);
	
	}

	if(recover)
	{
		printf("recovering process");
		recover_main();
	}
	else
	{
		program_main();
	}
}
