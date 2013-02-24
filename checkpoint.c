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
#include <stdlib.h>

char * _ckpt_dir = ".";

jmp_buf ckpt_env;

unsigned long ckpt_stkaddr,ckpt_stksize;

int searchinfile (char *str,char *filepath,unsigned long * lowval,unsigned long *hival)
{
	int dummy2;
	FILE *fpl;
	int MAX_LEN= 4000;
	char line[MAX_LEN];
	char low[10],hi[10];
	int i,j;
	int dummy;
#ifdef DBG
	printf("stackaddress in searchfunc last  %x first %x i %x\n",&dummy,&dummy2,&i);
#endif
	memset ( line , 0 , MAX_LEN );
	
	fpl = fopen ( filepath , "r" );
	
	while(fgets(line,2000,fpl)!= NULL)
	{
		if ( ( strstr ( line , str ) != NULL))
		{
#ifdef DBG
			printf("line found: \n %s",line);
#endif
			for(i = 0,j=0; line[i]!= '-';i++,j++)
				low[j]= line[i];
			low[j]='\0';
			for(j=0,i++; line[i] != ' ';i++,j++)
				hi[j]=line[i];
			hi[j]='\0';
#ifdef DBG
			printf("low address %s\n",low);
			printf("hi address %s\n",hi);
#endif
			(*lowval) = strtoll(low,NULL,16);
			(*hival)= strtoll(hi,NULL,16);
#ifdef DBG
			printf("converted lowval %x",(*lowval));
			printf("converted hival %x",(*hival));
#endif 




			fclose(fpl);
			return 1;
		}
	}

	printf("search for string in /proc failed");
	return 0;
}



int cur_stack_write(int fd)
{
	unsigned long low,hi,size;
	int pid;
	char proc_file_string[50];
	char pidnum[10];
	char search_string[10];

	int hfd;

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
	ckpt_stksize = hi-ckpt_stkaddr;
#ifdef DBG
	printf("ckpt_stksize %x\n",ckpt_stksize);
	printf("size %x",size);
#endif
	if ( write(fd, (char *)&low, 4) != 4 )
	{

		printf("error:couldnt write to chkpt file ");
		return -1;
		
	}
	if ( write(fd, (char *)&ckpt_stksize, 4) != 4 )
	{

		printf("error:couldnt write to chkpt file ");			
		return -1;
	}
	if ( write(fd, ckpt_stkaddr, ckpt_stksize) != ckpt_stksize )
	{	
		printf("error:couldnt write to chkpt file ");		
		return -1;		

	}




}
void write_heap()
{
	unsigned long low,hi,size;
	int pid;
	char proc_file_string[50];
	char pidnum[10];
	char search_string[10];

	int hfd;


	pid = getpid();
	strcpy(proc_file_string,"/proc/");
	sprintf(pidnum,"%d/",pid);
	strcat(proc_file_string,pidnum);
	strcat(proc_file_string,"maps");
#ifdef DBG
	printf("%s\n",proc_file_string);
#endif
	strcpy(search_string,"[heap]");
	
	searchinfile(search_string,proc_file_string,&low,&hi);
#ifdef DBG
	printf("heap low address %x\n",low);
	printf("heap hi address %x\n", hi);

	printf("heap size %x",(hi-low)-7);
#endif
	size = (hi -low) -7;

	hfd = open("./ckpt.hckpt",O_WRONLY|O_CREAT,0644);
	if(hfd == -1)
	{
		printf("error:unable to create heap chpt file");
		exit(-1);
	}
	if(write(hfd,low+4,size)!= size)
	{
		printf("error:couldnt write to ckpt file");
		exit(-1);
	}
	close(hfd);
}

void checkpoint_here()
{

	int fd,jfd;
	char tempname[50];
	int i;
	void *ptr;
	FILE *fp;
	char *str;
#ifdef DBG
	printf("stack address var addr before setjmp %x\n",&dummy);
#endif

	int dummy = 0x85;

#ifdef DBG

	str = "rm -f ~/checkpoint/maps";
	system(str);

	str="cp /proc/self/maps ~/checkpoint/maps";
	system(str);

	printf("environ addr %x",&ckpt_env);

	exit(0);

#endif
	__asm__("movl %esp,ckpt_stkaddr");
	
	
	 printf("ckptstkaddr %x \n",ckpt_stkaddr);
	 if ( ! setjmp(ckpt_env) )
	 {

		 ptr = &ckpt_env;
		 jfd = open("./jmpbufinfo", O_WRONLY | O_CREAT,0644);

		 for(i = 0; i < (sizeof(jmp_buf));i++)
		 {
	
			 /*write to file from buffer*/
			 if( write(jfd, (char *)ptr, 1) != 1 )
			 {
			
				 printf("error:writejmpbuf \n");
				
				 exit(-1);
			 }

			 
			 ptr = ptr+1;
		 }
		 close(jfd);

		 ptr = &ckpt_env;
		fp = fopen("./jmpbufdetstk","w");
		 for(i = 0; i < (sizeof(jmp_buf)/4);i++)
		 {
		 fprintf(fp,"0x%x\n",*((unsigned long*)ptr));
		 ptr = ptr+4;
		 }
		 fclose(fp);
		 sprintf ( tempname, "%s%s%d", _ckpt_dir, "/ckpt.temp.", getpid() );
		 remove ( tempname );	
		 fd = open(tempname, O_WRONLY | O_CREAT,0644);
		 if(fd == -1)
		 {
			 printf("cant open created chkpt file");
			 exit(-1);
		 }
		 cur_stack_write(fd);
		 
		 close(fd);
		 write_heap();
		 rename(tempname,"ckpt.ckpt");
		 exit(0);
		
	 }
	 else
	 {
		 __asm__("movl %esp,ckpt_stkaddr");
		 
		 //printf("arrived back stkaddr %x\n",ckpt_stkaddr);
		 
		
	 }
	 //printf("dummy end marker %x",dummy);
}
