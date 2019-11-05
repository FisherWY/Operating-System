# include "basicpcb.h"

# define SEMAPHORE_NUMBER 5
# define PROCESS_NUMBER 20

// 定义5个信号量
semaphore sem[SEMAPHORE_NUMBER];
// 定义0-19一共20个进程
pnode *pr[PROCESS_NUMBER];

// 申请资源 -> P操作
void down(char *sname, int pid)
{
	/**
	 * sflag: 信号量找到标志
	 * pflag: 进程找到标志
	 * p: 找到的进程节点
	 * s: 找到的信号量节点
	 **/
	int sflag = 0, pflag = 0;
	pnode *p;
	semaphore *s;

	// 根据名字查找信号量
	for (int i = 0; i < SEMAPHORE_NUMBER; i++)
	{
		if (!strcmp(sem[i].name, sname))
		{
			s = &sem[i];
			sflag = 1;
			break;
		}
	}

	// 根据进程ID查找进程
	for (int i = 0; i < PROCESS_NUMBER; i++)
	{
		if (pr[i]->node->pid == pid)
		{
			p = pr[i];
			pflag = 1;
			break;
		}
	}

	// 目标信号量或进程未找到
	if (!sflag || !pflag)
	{
		printf("Semaphore %s not exist or Process %d not exist.\n", sname, pid);
		printf("Semaphore find result: %d. Process find result: %d\n", sflag, pflag);
		return;
	}

	// 信号量-1
	s->count--;
	// 临界资源仍足够
	if (s->count >= 0)
	{
		s->curpid = p->node->pid;
	}
	// 临界资源不足
	else
	{
		// 添加到等待列表
		if (s->wlist != NULL)
		{
			pnode *tmp = s->wlist;
			while (tmp->next != NULL)
				tmp = tmp->next;
			tmp->next = p;
		}
		else
		{
			s->wlist = p;
		}
	}
}

// 释放资源 -> V操作
void up(char *sname)
{
	/**
	 * sflag: 信号量找到标志
	 * spos: 信号量位置
	 **/
	int sflag = 0, spos;

	// 查找信号量
	for (int i = 0; i < SEMAPHORE_NUMBER; i++)
	{
		if (!strcmp(sem[i].name, sname))
		{
			sflag = 1;
			spos = i;
			break;
		}
	}

	// 信号量未找到
	if (!sflag)
	{
		printf("Semaphore %s not found\n", sname);
		return;
	}

	/**
	 * 释放资源
	 * 如果等待列表中有进程，count数量不变，等待列表入队
	 * 如果等待列表中无进程，count数量++
	 **/
	if (sem[spos].wlist != NULL)
	{
		sem[spos].curpid = sem[spos].wlist->node->pid;
		sem[spos].wlist = sem[spos].wlist->next;
	}
	else
	{
		sem[spos].count++;
	}
}

// 查看临界资源使用状态
void showdetail()
{
	printf("\n===================================================\n");
	for (int i = 0; i < SEMAPHORE_NUMBER; i++)
	{
		if (sem[i].count <= 0)
		{
			printf("%s (Current process id: %d) | Wait list: ", sem[i].name, sem[i].curpid);
			pnode *p = sem[i].wlist;
			while (p != NULL)
			{
				printf("%5d ->", p->node->pid);
				p = p->next;
			}
			printf("   List end\n");
		}
		else
		{
			printf("%s now avaliable\n", sem[i].name);
		}
	}
	printf("===================================================\n\n");
}

// 帮助命令
void help()
{
	printf("\n===================================================\n");
	printf("sname: 临界资源名称  pid: 进程ID\n");
	printf("申请资源: down(sname,pid)\n");
	printf("释放资源: up(sname)\n");
	printf("查看当前资源使用情况: showdetail\n");
	printf("获取帮助: help\n");
	printf("退出: exit\n");
	printf("===================================================\n\n");
}

void init()
{
	// 初始化信号量semaphore
	for (int i = 0; i < SEMAPHORE_NUMBER; i++)
	{
		char sname[] = {'s', i+48, '\0'};
		strcat(sem[i].name, sname);
		sem[i].wlist = NULL;
		sem[i].count = 1;
	}

	// 初始化进程
	for (int i = 0; i < PROCESS_NUMBER; i++)
	{
		pr[i] = new pnode;
		pr[i]->node = new pcb;
		pr[i]->node->pid = i;
		pr[i]->brother = NULL;
		pr[i]->next = NULL;
		pr[i]->sub = NULL;
	}
}

void terminal()
{
	short cflag, pflag;
	char cmdstr[32];

	initerror();
	init();

	while (1)
	{
		cflag = 0;
		pflag = 0;
		printf("cmd: ");
		scanf("%s", cmdstr);

		if (!strcmp(cmdstr, "exit"))
		{
			break;
		}

		if (!strcmp(cmdstr, "showdetail"))
		{
			cflag = 1;
			pflag = 1;
			showdetail();
		}

		if (!strcmp(cmdstr, "help"))
		{
			cflag = 1;
			pflag = 1;
			help();
		}

		if (strstr(cmdstr, "down"))
		{
			cflag = 1;

			char *sname = substr(cmdstr, instr(cmdstr, '(')+1, instr(cmdstr, ',')-1);
			char *pid = substr(cmdstr, instr(cmdstr, ',')+1, instr(cmdstr, ')')-1);

			if (sname && pid)
			{
				down(sname, atoi(pid));
				pflag = 1;
			}
		}

		if (strstr(cmdstr, "up"))
		{
			cflag = 1;

			char *sname = substr(cmdstr, instr(cmdstr, '(')+1, instr(cmdstr, ')')-1);
			
			if (sname)
			{
				up(sname);
				pflag = 1;
			}
		}

		// 输入错误或参数错误
		if (!cflag)
			geterror(0);
		if (!pflag)
			geterror(1);
	}
}

int main(int argc, char const *argv[])
{
	terminal();
	return 0;
}