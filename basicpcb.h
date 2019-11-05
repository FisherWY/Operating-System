// 进程控制块PCB头文件
# ifndef basicpcb_h
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# define basicpcb_h

char *errormsg[256];

// 进程控制块
struct pcb
{
	// 进程ID
	int pid;
	// 进程父ID
	int ppid;
	// 进程优先级
	int prio;
	// 进程状态
	int state;
	// 上次运行时间
	int lasttime;
	// 进程运行总时间
	int tottime;
};

// 进程在广义二叉树或者进程链表的节点结构
struct pnode
{
	// 当前节点对应的进程控制块
	pcb *node;
	// 进程链树中，当前节点的子节点
	pnode *sub;
	// 进程树中，当前节点的兄弟节点
	pnode *brother;
	// 进程链表中，当前节点的下一节点
	pnode *next;
};

// 信号量机制
struct semaphore
{
	// 信号量资源名称
	char name[5];
	// 计数值
	int count;
	// 当前进程ID
	int curpid;
	// 等待进程链表
	pnode *wlist;
};

// 获取错误信息
# define geterror(eno) printf("%s\n", errormsg[eno]);

// 生成错误信息
void initerror() 
{
	errormsg[0] = (char *) malloc(20);
	strcpy(errormsg[0], "Error command!");
	// errormsg[0] = "Error command!";
	errormsg[1] = (char *) malloc(20);
	// errormsg[1] = "Error parameter!";
	strcpy(errormsg[1], "Error parameter!");
}

// 获取子字符串
char *substr(char *s, int start, int end)
{
	char *s1;
	int len = strlen(s);
	if (start<0 || end>=len || start>end)
		return NULL;
	s1 = (char *) malloc(end - start + 2);

	int pos = 0;
	for (; pos <= end-start; pos++)
	{
		s1[pos] = s[pos+start];
	}
	s1[pos] = '\0';

	return s1;
}

// 查找字符C在Str中的位置
int instr(char *s, char c)
{
	unsigned int i;
	for (i = 0; i < strlen(s); i++)
	{
		if (s[i] == c)
		{
			return i;
		}
	}
	return -1;
}

// 将Str字符串转为string数组
int *strtoarray(char *s)
{
	/**
	 * str: XXX,XXX,XXX
	 * a: 用于记录每个字符的下标
	 * count: 用于记录字符串中','出现的次数
	 * x1: 储存','在字符串s1中出现的位置
	 * s1: 记录截取字符串后的s
	 * s2: 储存每个子字符串的指针
	 * c: 储存','
	 **/
	int *a, count, x1;
	unsigned int i;
	char c, *s1, *s2;
	if (!s)
	{
		printf("String can't be NULL!\n");
		return NULL;
	}

	count = 0;
	s1 = s;
	for (i = 0; i < strlen(s1); i++)
	{
		if (s1[i] == ',')
			count++;
	}
	count++;

	a = (int *) malloc(count);
	c = ',';
	for (i = 0; i < count; i++)
	{
		x1 = instr(s1, c);
		if (x1 >= 0)
			s2 = substr(s1, 0, x1-1);
		else
			s2 = s1;
		// 将string转为int
		a[i] = atoi(s2);
		s1 = substr(s1, x1+1, strlen(s1)-1);
	}
	return a;
}

# endif