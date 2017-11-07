//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #3
// October 11, 2017
//
// Jong-Won Park
// Embedded Software Laboratory
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include <stdlib.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "string_sw.h"
int cmd_typechk(char *tmp)
{
	char head[201];
	for(int i=0;i<200;i++)	head[i]=tmp[i];
	for(int i=0;i<200;i++)	if(head[i]==' '||head[i]=='\n')	head[i]='\0';
	if(strcmp(head,"ls")==0||strcmp(head,"man")==0||strcmp(head,"grep")==0||strcmp(head,"sort")==0||strcmp(head,"awk")==0||strcmp(head,"bc")==0)	return 1;
	if(strcmp(head,"head")==0||strcmp(head,"tail")==0||strcmp(head,"cat")==0||strcmp(head,"cp")==0) return 2;
	if(strcmp(head,"mv")==0||strcmp(head,"rm")==0||strcmp(head,"cd")==0)	return 3;
	if(strcmp(head,"pwd")==0||strcmp(head,"exit")==0)	return 4;
	return -1;
}
char *pwd (int full)
{
    static char buffer[1024];
    char *ptr;
    if (getcwd(buffer, 1024) != buffer)
        return NULL;

    if (full)
        return buffer;
    
    if (strcmp("/", buffer) == 0)
        return buffer;

    ptr = strrchr(buffer, '/');
    return ptr+1;
}

void print_prompt()
{
    int ret;
    char *ptr = pwd(0);
    char head[200];

    if (!ptr)
        exit(1);
    if (strlen(ptr) > 190)
        exit(1);
    strcpy(head, "swsh:");
    strcat(head, ptr);
    strcat(head, "> ");
    ret = write(2, head, strlen(head));

    if (ret <= 0)
        exit(1);
}
void cmd_exec(char *input,int *pc,int *cur_pid)
{
	char tmp[200]; // Initialize character array...
	int i;
	for(i=0;i<200;i++)	tmp[i]='\0';
	i=0;
	while(input[i]!='\n')
	{
		tmp[i]=input[i];
		i++;
	}
	tmp[i]='\n';
	while(*input!='|'&&*input!='\n')	input++;
	if(*input=='|')
	{
		input++;
		while(*input==' ')	input++;
	}
	for(i=0;i<200;i++)	if(tmp[i]==' ')	tmp[i]='\0';	
	char *x = tmp;
	while(*x!='\n'&&*x!='|')	x++;
	int block = 1;
	int pip =0;
	if(*x=='|')	pip = 1;	
	int j=0;
	if(*x=='|')	x--;
	for(int i = 0 ; i <x-tmp ; i++)	if(tmp[i] == '\0') block++;
	char* b[block+1];
	b[j] = tmp;	j++;
	for(int i = 0 ; i <x-tmp ; i++)
	{
		if(tmp[i] == '\0'&&tmp[i+1]!='|'&&tmp[i+1]!='>')
		{
			b[j] = tmp+i+1;
			j++;
		}
	}
	b[block]=NULL;
	if(cmd_typechk(b[0])==-1 && !(b[0][0]=='.'&&b[0][1]=='/') )
	{
		if(b[0][0]!='\n')	write(2,"swsh: Command not found\n",24);
		return;
	}
	if(*x=='\n'||*x==' ')	*x='\0';
	if(strcmp(b[0],"cd")==0&&pip==0)
	{
		chdir(b[1]);
		return;
	}
	pid_t pid;
	int dc[2];
	int status;
	if((pid=fork())==0)
	{
		if(pc!=NULL)
		{
			dup2(pc[0],0);
			close(pc[1]);
		}
		if(pip)
		{
			pipe(dc);
			dup2(dc[1],1);
			close(dc[0]);
		}
		if(execvp(b[0],b)<0)
		{
			write(2,"swsh: Command not found\n",24);
			exit(1);
		}
		else	close(pc[0]);
		exit(1);
	}
	else
	{
		*cur_pid = pid;
		if(pip)
		{
			cmd_exec(input,dc,cur_pid);
		}
		*cur_pid = pid;
		return;
	}
}
int main(void)
{
    pid_t cur_pid = -1;
    int ret;
    char input[240];
    while (1) {
        if(cur_pid!=-1)
	{
		int status;
		waitpid(cur_pid,&status,0);
		print_prompt();
		cur_pid=-1;
	}
	else	print_prompt();
	for(int i=0;i<240;i++)	input[i]='\0';
        ret = read(0, input, 200);
	cmd_exec(input,NULL,&cur_pid);
	if (ret < 0)
            exit(1);
        else if (ret == 0) {
            write(2, "exit\n", 5);
            break;
        }
    }

    return 0;
}
