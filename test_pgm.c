#include "stdio.h"



int program_main()
{
	int arr[100],i;
	printf("initializing array and printing");
	for(i = 0;i < 100;i++)
	{
		arr[i]= i;
		printf("%d ",arr[i]);
		if((i%10)==0)
		{
			#if 0
			//checkpoint_here();
			printf("\ncheckpointed %dth time",i/10);
			printf("\nwill delay for some time ,press ctrl-c to kill the current execution");
			#endif
			printf("\ndelay...");
			sleep(3);
			printf("\nout of delay");
		}
	}
	return 0;
}
	
