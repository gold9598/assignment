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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h> // Because of the function named rename
#include <signal.h>
#include <errno.h>
#include "string_sw.h"
void handler(int sig)
{
	if(sig==SIGINT||sig == SIGTSTP)	write(2,"\n",1);
}
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
    ret = write(0, head, strlen(head));

    if (ret <= 0)
        exit(1);
}
void print_error()
{
	if(errno == EACCES)	write(2,"Permission denied\n",18);
	else if(errno == EISDIR)	write(2,"Is a directory\n",15);
	else if(errno == ENOENT)	write(2,"No such file or directory\n",26);
	else if(errno == ENOTDIR)	write(2,"Not a directory\n",16);
	else if(errno == EPERM)	write(2,"Permission denied\n",18);
	else printf("<%d>\n",errno);
}
void cmd_exec(char *input)
{
	char X[200];
	int pipenum=0;
	int input_exist=0;
	int output_exist=0;
	int i=0;
	while(input[i]!='\n')
	{
		X[i]=input[i];
		if(input[i]=='<')	input_exist++;
		if(input[i]=='>')	output_exist++;
		if(input[i]=='|')	pipenum++;
		i++;
	}
	
	char *str = X;
	/* breaking out input & output */
	char* out = NULL;
	if(input_exist)
	{
		char *p = str;
		while(*p!='<')	p++;
		*p = ' ';
		/*
		char *s = p;
		s++;
		while(*s==' ')	s++;
		char *z = s;
		while(*z!=' ')	z++;
		int len = z-s;
		int in_x[len+1];
		for(i=0;i<len;i++)	in_x[i]=*(s+i);
		in_x[i+1]='\0';
		in = in_x;
		while(p!=s)
		{
			*p='\0';
			p++;
		}
		*/
	}
	if(output_exist)
	{
		char *p = str;
		while(*p!='>')	p++;
		char *s = p;
		while(*s=='>')
		{
			*s = '\n';
			s++;
		}
		while(*s==' ')	s++;
		out = s;
		while(*s!='\n'&&*s!=' ')	s++;
		while(*s==' ')
		{
			*s='\0';
			s++;
		}
		if(*s == '\n')	*s='\0';
		*p='\n';
	}
	/* data structure declaration */
	X[i]='\n';
	char** b[pipenum+1]; // Instruction block.
	int p[pipenum][2]; // pipe.
	/* tokenization */
	for(i=0;i<pipenum+1;i++)
	{
		char *x = str;
		int inst=0;
		while(*x!='\n'&&*x!='|')
		{
			if(*x!=' ')
			{
				inst++;
				while(*x!=' '&&*x!='\n'&&*x!='|')	x++;
			}
			else	while(*x==' ')	x++;
		}
		b[i] = (char**)malloc(sizeof(char*)*(inst+1));
		b[i][inst]=NULL;
		for(int j=0;j<inst;j++)
		{
			if(j==0)	b[i][j]=str;
			else
			{
				while(*str!=' ')	str++;
				while(*str==' ')
				{
					*str='\0';
					str++;
				}
				b[i][j]=str;
			}
		}
		while(*str!='|'&&*str!='\n')
		{
			if(*str==' ')	*str='\0';
			str++;
		}
		if(*str=='|')
		{
			*str = '\0';
			str++;
			while(*str==' ')	str++;
		}
		if(*str=='\n')	*str='\0';
	}
	/*output file handling */
	if(output_exist)
	{	
		char *o = out;
		while(*o != '\n' && *o != ' ' && *o != '\0')	o++;
		*o = '\0';
	}
	/* exception handling stage */
	if(input_exist && strcmp(b[0][0],"cat")!=0)
	{
		int last=0;
		while(b[0][last]!=NULL)	last++;
		b[0][last-1]=NULL;	// nullify input option
	}	
	if(strcmp(b[0][0],"exit")==0) // in case of exit mode.
	{
		if(pipenum == 0)
		{
			if(b[0][1]==NULL)	exit(0);
			else if('0'<=*b[0][1]&&*b[0][1]<='9')	exit(*b[0][0]-'0');
		}
		else	write(2,"swsh: Command not found\n",24);
		return;
	}
	if(strcmp(b[0][0],"cd") ==0) // Change directory
	{
		if( pipenum == 0)
		{
			if(b[0][1]!=NULL)
			{
				if(-1==chdir(b[0][1]))
				{
					write(2,"Error occurred: ",16);
					print_error();
				}
			}
			else
			{
				if(-1==chdir(getenv("HOME")))
				{
					write(2,"Error occurred: ",16);
					print_error();
				}
			}
		}
		return;
	}
	if(strcmp(b[0][0],"rm")==0) // If rm with no option, proceed this.
	{
		if(b[0][2]==NULL)
		{
			if(-1==unlink(b[0][1]))
			{
				write(2,"Error occurred: ",16);
				print_error();
			}
			return;
		}
	}
	if(strcmp(b[0][0],"mv")==0)
	{
		if(-1==rename(b[0][1],b[0][2]))
		{
			write(2,"Error occurred: ",16);
			print_error();
		}
		return;	
	}
	if(strcmp(b[0][0],"pwd")==0)
	{
		char cur[255];
		getcwd(cur,255);
		write(0,cur,strlen(cur));
		write(0,"\n",1);
		return;
	}
	/* execution stage */
	pid_t pid;
	int fd;
	if(output_exist==1)	fd = open(out,O_RDWR|O_CREAT|O_TRUNC,0666);
	if(output_exist==2)	fd = open(out,O_RDWR|O_APPEND);
	for(i=0;i<pipenum;i++)	pipe(p[i]);
	for(i=0;i<pipenum+1;i++)
	{
		if((pid=fork())==0)
		{
			if(strcmp(b[i][0],"man")&&strcmp(b[i][0],"vim"))
			{
				if(setpgid(pid,pid)<0)	exit(0);
			}
			if(i>0)
			{
				if(dup2(p[i-1][0],0)<0)	exit(0);
			}
			if(i<pipenum)
			{
				if(dup2(p[i][1],1)<0)	exit(0);
			}
			if(i==pipenum&&output_exist)
			{
				dup2(fd,1);
			}
			for(int j=0;j<pipenum;j++)
			{
				close(p[j][0]);	close(p[j][1]);
			}
			if(execvp(b[i][0],b[i])<0)
			{
				write(2,"swsh: Command not found\n",24);
				exit(0);
			}
			exit(0);
		}
	}
	for(i=0;i<pipenum;i++)
	{
		close(p[i][0]);	close(p[i][1]);
	}
	if(output_exist)	close(fd);
}
int main(void)
{
    int status;
    signal(SIGINT,handler);
    signal(SIGTSTP,handler);
    int ret;
    char input[240];
    while (1) {
	pid_t pid_x = wait(&status);
	print_prompt();
	for(int i=0;i<240;i++)	input[i]='\0';
	ret = read(0, input, 200);
	if(input[0]!='\n')	cmd_exec(input);
	if (ret < 0)
            exit(1);
        else if (ret == 0) {
            write(2, "exit\n", 5);
            break;
        }
        //write(2, input, ret);
    }

    return 0;
}
