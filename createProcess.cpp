# include "basicpcb.h"

// 进程树根节点
pnode *proot;
// 进程链表头节点
pnode *plink;

/**
 * 创建进程
 * para[0]: 要创建的新进程的pid
 * para[1]: 新进程的父进程pid
 * para[2]: 新进程的优先级
 **/
int createpc(int *para)
{
	/**
	 * p: 进程链表plink的操作指针
	 * p1: 新进程的指针
	 * pp: 新进程p1的父节点
	 **/
	pnode *p, *p1, *pp;
	int pflag = 0;
	for (p=plink; p; p=p->next)
	{
		// 检查当前进程是否已创建
		if (p->node->pid == para[0])
		{
			printf("pid %d is already exist!\n", para[0]);
			return -1;
		}
		// 找到进程父节点的进程控制块
		if (p->node->pid == para[1])
		{
			pflag = 1;
			pp = p;
		}
	}
	if (!pflag)
	{
		printf("Parent id %d is not exist!\n", para[1]);
		return -2;
	}

	// 创建新的进程控制块
	p1 = new pnode;
	p1->node = new pcb;
	p1->node->pid = para[0];
	p1->node->ppid = para[1];
	p1->node->prio = para[2];
	p1->sub = NULL;
	p1->next = NULL;
	p1->brother = NULL;

	// 将新进程添加到进程树中
	if(!pp->sub)
		pp->sub = p1;
	else
	{
		// 循环遍历至兄弟子进程的最后一个
		for (p=pp->sub; p->brother; p=p->brother);
		p->brother = p1;
	}

	// 将新进程添加到进程链表中
	for (p=plink; p->next; p=p->next);
	p->next = p1;

	return 0;
}

// 显示进程信息
void showdetail()
{
	pnode *p, *p1;
	p = plink;
	// 将所有进程信息打印
	while (p != NULL)
	{
		printf("(pid: %d - prio: %d): ", p->node->pid, p->node->prio);
		p1 = p->sub;
		// 打印子进程信息
		while (p1 != NULL)
		{
			printf("   (pid: %d - prio: %d)", p1->node->pid, p1->node->prio);
			p1 = p1->brother;
		}
		printf("\n");
		p = p->next;
	}
	printf("\n");
}

int main(int argc, char const *argv[])
{
	initerror();
	short cflag, pflag;
	char cmdstr[32];

	proot = new pnode;
	proot->node = new pcb;
	proot->node->pid = 0;
	proot->node->ppid = -1;
	proot->node->prio = 0;
	proot->next = NULL;
	proot->sub = NULL;
	proot->brother = NULL;

	plink = proot;

	while(1)
	{
		cflag = 0;
		pflag = 0;
		printf("cmd: ");
		scanf("%s", cmdstr);

		// 程序退出
		if (!strcmp(cmdstr, "exit"))
			break;

		// 查看进程信息
		if (!strcmp(cmdstr, "showdetail"))
		{
			cflag = 1;
			pflag = 1;
			showdetail();
		}

		// 帮助信息
		if (!strcmp(cmdstr, "help"))
		{
			cflag = 1;
			pflag = 1;
			printf("创建新进程: createpc(pid,ppid,prio)\n");
		}

		// 创建新进程，判断createpc是否为cmdstr字串
		if (strstr(cmdstr, "createpc"))
		{
			int *para;
			char *s;
			
			cflag = 1;

			// 获取创建新进程的参数 -> pid,ppid,prio
			int start = instr(cmdstr, '(');
			int end = instr(cmdstr, ')');
			s = substr(cmdstr, start+1, end-1);

			para = (int *) malloc(3);
			para = strtoarray(s);
			createpc(para);

			pflag = 1;
		}

		// 输入错误或参数错误
		if (!cflag)
			geterror(0);
		if (!pflag)
			geterror(1);
	}

	return 0;

}
