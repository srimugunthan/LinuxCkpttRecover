#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

main()
{
  int choice,status,died,pid,i;
  char s[100];
  char ip[100];
  char f[20];
  static char file_name[20], test_name[40] ;  
  FILE *fp,*ft;
  void displaymenu();	
  printf("\n\n\n");
  printf("*************************\n");
  printf("Please make sure to run the following command as root to put off \naddress space randomization of 2.6 kernel\n");
  printf("\"sysctl -w kernel.randomize_va_space=0\"\n\n");
  printf("\n\n");
   printf("*************************\n");
  printf("\nSelect one in menu\n"); 
  displaymenu();
  printf("\n:");
  scanf("%d",&choice);
  
   	
    while(1)
    {	    
       switch(choice)
       {
          case 1:   
                    printf("\nENTER THE FILENAME TO CHECKPOINT:"); 	   
           	    scanf("%s",file_name);
	            sprintf(test_name, "%s %s %s %s", "cat", file_name, ">>", "skele.c");
	            system(test_name);
	            sprintf(test_name, "%s %s %s", "cp", "example.c", file_name);
	            system(test_name);
	            sprintf(s,"gcc -w checkpoint.c common.c recover.c example.c");
	
		    system(s);
		    printf("\nCOMPILATION COMPLETED\n");
		    printf("\nSTARTED EXECUTION.....\n");
		    printf("PRESS CTRL+C TO CHECKPOINT\n");
	            system("./a.out");
                    break;	
          
	 case 3:  system("./a.out -r");	 
	          break;
	
	 case 2:  printf("ENTER THE IP ADDRESS TO TRANSFER THE CHECKPOINTED PROGRAM ?\n ");
	          scanf("%s",ip);
		  strcpy(f,ip);
		  strcat(ip,":.");
		  strcpy(s,"scp -r ./a.out ./checkpoint ./ckpt.ckpt ./ckpt.hckpt ./jmpbufinfo ");
		  strcat(s,ip);
		  system(s);
		  strcpy(s,"ssh ");
		  strcat(s,f); 
	          system(s);
		  exit(1);
	          break;     	  
	 case 4: exit(1);		  
      default:break;	
     }  
	displaymenu();
       printf("ENTER THE CHOICE: \n");
       scanf("%d",&choice);
	       
    }     
}


void displaymenu()
{
     printf("\n\n");
   printf("Copy the program file to checkpoint folder\n");
   printf("1. Compile and Checkpoint A program\n");
   printf("2. Transfer checkpointed info and exec to remote machine\n");
   printf("3. recover execution from the checkpoint   \n");
   printf("4. QUIT\n");
 printf("\n\n");  
}   
