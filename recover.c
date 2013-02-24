#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

extern jmp_buf ckpt_env;


void recurse_and_recover(long *recover_hi,unsigned long  ckpt_size,int fd)

{
	int dummy;
	unsigned long cur_top_stack;
	int jfd;
	int i;
	void *ptr;
	cur_top_stack = &dummy;
#ifdef DBG
	printf("ckpt_size = %x /4 = %x\n",ckpt_size,ckpt_size/4);
	printf("recover_hi %x",recover_hi);
	printf("recover_hi-(csize/4)-100 %x\n",recover_hi-((ckpt_size/4)+0x100));
	printf("curtopstk %x\n",cur_top_stack);
#endif

	if(cur_top_stack>(recover_hi-((ckpt_size/4)+0x100)))
		recurse_and_recover(recover_hi,ckpt_size,fd);
#ifdef DBG
	printf("recover info written to addr %x",recover_hi-((ckpt_size/4)+1));
	printf("recover info written to addr %x",recover_hi-((ckpt_size/4)+1));
	printf("recover info written to addr %x",recover_hi-((ckpt_size/4)+1));
	printf("recover info written to addr %x",recover_hi-((ckpt_size/4)+1));
	printf("recover info written to addr %x",recover_hi-((ckpt_size/4)+1));
#endif
	read(fd,recover_hi-((ckpt_size/4)),ckpt_size-4);
	close(fd);

#ifdef DBG
printf("came till longjmp\n");
		 ptr = &ckpt_env;
		 jfd = open("./jmpbufinforec",O_WRONLY | O_CREAT,0644);
		for(i = 0; i < (sizeof(jmp_buf));i++)
		 {
	
			 /*write to file*/
			 if( write(jfd, (char *)ptr, 1) != 1 )
			 {
			
				 printf("error:writejmpbuf \n");
				
				 exit(-1);
			 }

			 
			 ptr = ptr+1;
		 }
		 close(jfd);

			//exit(0);
#endif
	
longjmp(ckpt_env,1);
	exit(0);
}

void recover_stack(int fd,long recover_hi)
{
	unsigned long ckpt_top_stack,ckpt_size;
	read(fd, (char *)&ckpt_top_stack, 4);
	printf("checkpointed: top of stack %x\n",ckpt_top_stack);
	printf("recover:bottom(hi) of stack %x\n",recover_hi);
	read(fd,(char*)&ckpt_size,4);
	printf("checkpinted: stack size %x\n",ckpt_size);
	recurse_and_recover(recover_hi,ckpt_size,fd);
}



void recover_main()
{

	char tempname[50];
	int fd;
	unsigned long low,hi,size,h_low,h_hi,h_size;
	int pid;
	char proc_file_string[50];
	char pidnum[10];
	char search_string[10];
	int hfd;

	void *ptr;
	char *ptr2;
	FILE *fp;
	int i;
	char *str;
	int jfd;
#ifdef DBG
str="cp /proc/self/maps ~/checkpoint/mapsr";
		 system(str);
printf("environ addr %x\n",&ckpt_env);
exit(0);
#endif
	pid = getpid();
	strcpy(proc_file_string,"/proc/");
	sprintf(pidnum,"%d/",pid);
	strcat(proc_file_string,pidnum);
	strcat(proc_file_string,"maps");
	printf("%s\n",proc_file_string);
	strcpy(search_string,"[stack]");
	searchinfile(search_string,proc_file_string,&low,&hi);
#ifdef DBG
	printf("stack low address %x\n",low);
	printf("stack hi address %x\n", hi);
#endif
	size = hi-low;
	printf("size %x",size);
	tempname[0] = 0;
#if 1
	hfd = open("./ckpt.hckpt",O_RDONLY);
	strcpy(search_string,"[heap]");
	searchinfile(search_string,proc_file_string,&h_low,&h_hi);
#ifdef DBG
	printf("recover:heap top address %x \n",h_low);
	printf("recover: heap bottom address %x \n",h_hi);
#endif
	h_size = h_hi-h_low-7;
#ifdef DBG
	printf("recover:hsize %x\n",h_size);

	printf("modifying heap from %x \n ",h_low);
#endif
	read(hfd,h_low+4,h_size);

#ifdef DBG
	printf("modifying heap content successs\n");
#endif
	close(hfd);
#endif
/* reconstruct jmpbuf from stored file*/
	jfd = open("./jmpbufinfo", O_RDONLY);

	ptr = &ckpt_env;ptr2 = ptr;
	for(i = 0;i < (sizeof(jmp_buf));i++)
	{
		/* read from file to buffer*/
		read(jfd,(char*)ptr2,1);
		ptr2 = ptr2 +1;
	}
	close(jfd);
	strcat(tempname, "./");
	strcat(tempname,"ckpt.ckpt");

	fd = open(tempname, O_RDONLY);
	if(fd == -1)
	{
		printf("checkpoint file open error");
		exit(-1);
	}

recover_stack(fd,hi);
	close(fd);
}
