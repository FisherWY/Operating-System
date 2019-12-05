# include <cstdlib>
# include <iostream>
# include <cstring>

// 定义进程数
# define PROCESS_NUMBER 10
// 定义最高优先级（0-2）
# define MAX_PRIORITY 3
// 定义时间片
# define QUANTUM 2

struct pcb
{
	int id;	// 进程ID
	int status;	// 进程状态：0-就绪，1-运行，2-阻塞
	int prio;	// 进程优先级
	int life;	// 剩余生命周期
	pcb *next;	// 优先级链表指针
} *priority_array[MAX_PRIORITY];	// 优先级数组+链表

static int id_list[PROCESS_NUMBER];	// 进程ID列表，记录进程创建状态
int life = 0;	// 系统剩余总生命周期

char command[8][10];	// 命令列表

/**
 * 系统初始化
 **/
void init()
{
	for (int i = 0; i < MAX_PRIORITY; i++)
	{
		priority_array[i] = NULL;
	}
	strcpy(command[0], "quit");
	strcpy(command[1], "help");
	strcpy(command[2], "ps");
	strcpy(command[3], "create");
	strcpy(command[4], "kill");
	strcpy(command[5], "runtime");
	strcpy(command[6], "sleep");
	strcpy(command[7], "awake");
}

/**
 * 输出系统当前进程状态
 * p: 遍历指针
 **/
void ps()
{
	struct pcb *p;
	printf("\n=================================================\n");
	printf("Total system life: %d\n", life);
	for (int i = 0; i < MAX_PRIORITY; i++)
	{
		printf("Priority level: %d\n", i);
		p = priority_array[i];
		while (p != NULL)
		{
			printf("id: %d, status: %d, priority: %d, life: %d\n", p->id, p->status, p->prio, p->life);
			p = p->next;
		}
	}
	printf("=================================================\n\n");
}

/**
 * 随机创建一个新进程
 * i：进程ID
 * prio：进程优先级
 * plife：进程生命周期
 * p：指向同等优先级链表表头，遍历中发生变化
 * p_previous：指向p的上一个节点，用于尾插法
 * s：指向新创建的进程
 **/
void create()
{
	int i = 0, prio = 0, plife = 0;
	struct pcb *p = NULL, *p_previous = NULL, *s = NULL;
	// 检查是否到达进程数量上限，同时分配进程ID
	while (id_list[i] != 0 && i <= PROCESS_NUMBER - 1)
		i++;
	if (PROCESS_NUMBER == i)
	{
		printf("System max process error!\n");
		return;
	}
	// 创建新进程的参数，增加总生命周期
	id_list[i] = 1;
	prio = rand()%MAX_PRIORITY;
	plife = rand()%20 + 1;
	life += plife;
	// 创建新进程
	s = new pcb;
	s->id = i;
	s->status = 0;
	s->prio = prio;
	s->life = plife;
	s->next = NULL;
	// 插入到进程优先级链表中
	p = priority_array[prio];
	// 如果表头为NULL，新进程为表头
	if (p == NULL)
		priority_array[prio] = s;
	else	// 表头不为空，使用尾插法插入到链表尾部
	{
		while (p != NULL)
		{
			p_previous = p;
			p = p->next;
		}
		p_previous->next = s;
	}
	// 插入完成，输出当前进程状态
	printf("Successfully create process(id: %d), current system status: \n", i);
	ps();
}

/**
 * 阻塞某个进程
 * i：进程x的优先级索引
 * find：进程x找到的标记
 * p：指向进程x
 **/
void sleep(int x)
{
	int i = 0, find = 0;
	struct pcb *p = NULL;
	// 搜索ID为x的进程
	while (!find && i!=MAX_PRIORITY)
	{
		p = priority_array[i];
		if (p == NULL)
		{
			i++;
			continue;
		}
		while (p != NULL)
		{
			if (p->id == x)
			{
				find = 1;
				break;
			}
			else
				p = p->next;
		}
		if (!find)
			i++;
		else
			break;
	}
	// 未找到进程x，操作终止
	if (!find)
	{
		printf("Sleep: Invaild process number!\n");
		return;
	}
	// 将进程x的状态设为阻塞
	if (p->status == 2)
		printf("The process(id: %d) has been blocked, cannot sleep again!\n", x);
	else
		p->status = 2;
	// 操作结束后系统进程状态
	ps();
}

/**
 * 唤醒某个进程
 * i：进程x的优先级索引
 * find：进程x找到的标记
 * p：指向进程x
 **/
void awake(int x)
{
	int i = 0, find = 0;
	struct pcb *p = NULL;
	// 搜索ID为x的进程
	while (!find && i!=MAX_PRIORITY)
	{
		p = priority_array[i];
		if (p == NULL)
		{
			i++;
			continue;
		}
		while (p != NULL)
		{
			if (p->id == x)
			{
				find = 1;
				break;
			}
			else
				p = p->next;
		}
		if (!find)
			i++;
		else
			break;
	}
	// 未找到进程x，操作终止
	if (!find)
	{
		printf("Awake: Invaild process number!\n");
		return;
	}
	// 将进程x的状态设为就绪
	if (p->status != 2)
		printf("The process(id: %d) has been awaked, cannot awaked again!\n", x);
	else
		p->status = 0;
	// 操作结束后系统进程状态
	ps();
}

/**
 * 杀死某个进程
 * i：进程x的优先级索引
 * find：进程x找到的标记
 * p：指向进程x
 * p_previous：指向进程x在优先级链表上的前一个节点
 **/
void kill(int x)
{
	int i = 0, find = 0;
	struct pcb *p = NULL, *p_previous = NULL;
	// 搜索ID为x的进程
	while (!find && i!=MAX_PRIORITY)
	{
		p = priority_array[i];
		if (p == NULL)
		{
			i++;
			continue;
		}
		while (p != NULL)
		{
			if (p->id == x)
			{
				find = 1;
				break;
			}
			else
			{
				p_previous = p;
				p = p->next;
			}
		}
		if (!find)
			i++;
		else
			break;
	}
	// 未找到进程x，操作终止
	if (!find)
	{
		printf("Kill: Invaild process number!\n");
		return;
	}
	// 杀死进程，同时摘链
	if (p == priority_array[i])	// 进程节点在链表头部
	{
		priority_array[i] = priority_array[i]->next;
		id_list[x] = 0;
		life -= p->life;
		delete p;
	}
	else	// 进程节点不在链表头部
	{
		p_previous->next = p->next;
		id_list[x] = 0;
		life -= p->life;
		delete p;
	}
	// 操作结束后系统进程状态
	ps();
}

/**
 * 运行一个时间片周期
 * i：优先级链表索引
 * prio：新的优先级
 * t：新的优先级链表索引
 * pp：指向新优先级链表要插入的节点
 * qq：指向新优先级链表要插入的节点的上一个节点
 * r_previous：要运行的进程的上一个节点
 * r：要运行的进程
 **/
void runtime()
{
	int i = MAX_PRIORITY - 1, prio = 0, t = 0;
	struct pcb *pp = NULL, *qq = NULL, *r_previous = NULL, *r = NULL;
	// 寻找优先级最高的，状态为就绪的进程，指针r指向它
	do
	{
		while (i >= 0 && priority_array[i] == NULL)
			i--;
		if (i < 0)
		{
			printf("No process in the system, create/awake a process and try again!\n");
			return;
		}
		r_previous = priority_array[i];
		r = priority_array[i];
		while (r != NULL && r->status != 0)
		{
			r_previous = r;
			r = r->next;
		}
		i--;
	} while (r == NULL);
	// 运行一个时间片周期
	printf("Process(id: %d) gain the CPU time and will execute %d quantum\n", r->id, QUANTUM);
	r->status = 1;	// 将进程状态置为运行中
	printf("Process %d is running......\n", r->id);
	// 制造运行时间
	for (int j = 0; j < 600000; j++)
	{
		for (int k = 0; k < 1000; k++)
		{

		}
	}
	printf("Time out, change to ready status, reduce priority\n");
	r->status = 0;
	r->prio /= 2;
	// 更新系统生命周期和进程生命周期
	if (r->life - QUANTUM > 0)
	{
		r->life -= QUANTUM;
		life -= QUANTUM;
	}
	else
	{
		life -= r->life;
		r->life = 0;
	}
	// 检查进程生命周期是否结束
	if (r->life == 0)
	{
		printf("Process %d completed, now release it!\n", r->id);
		kill(r->id);
	}
	else
	{
		if (r_previous == r)
			priority_array[i+1] = r->next;
		else
			r_previous->next = r->next;
		// 添加到新优先级链表的尾部
		t = r->prio;
		pp = priority_array[t];
		while (pp != NULL)
		{
			qq = pp;
			pp = pp->next;
		}
		if (qq == NULL)
			priority_array[t] = r;
		else
			qq->next = r;
		r->next = NULL;
	}
	// 一个生命周期结束，显示系统进程状态
	ps();
}

/**
 * 命令行控制台
 **/
void terminal()
{
	char cmdstr[32];
	int cmd = 0, id = 0;
	while (1)
	{
		printf("cmd: ");
		scanf("%s", cmdstr);

		for (int i = 0; i < 8; i++)
		{
			if (!strcmp(command[i], cmdstr))
			{
				cmd = i;
				break;
			}
		}

		switch (cmd)
		{
			case 0:
				return;
			case 1:
				printf("\n=================================================\n");
				printf("id: 当前进程ID  status: 进程状态:0-就绪,1-运行,2-阻塞  prio: 进程优先级  life: 进程生命\n");
				printf("随机创建一个新进程: create\n");
				printf("杀死一个进程: kill\n");
				printf("使一个进程进入阻塞状态: sleep\n");
				printf("唤醒一个进程: awake\n");
				printf("查看当前进程信息: ps\n");
				printf("运行一个时间片周期: runtime\n");
				printf("获取帮助: help\n");
				printf("退出: quit\n");
				printf("=================================================\n\n");
				break;
			case 2:
				ps();
				break;
			case 3:
				create();
				break;
			case 4:
				printf("Which process you want to kill: ");
				scanf("%d", &id);
				kill(id);
				break;
			case 5:
				runtime();
				break;
			case 6:
				printf("Which process you want to sleep: ");
				scanf("%d", &id);
				sleep(id);
				break;
			case 7:
				printf("Which process you want to awake: ");
				scanf("%d", &id);
				awake(id);
				break;
			default:
				printf("Error command!\n");
		}
	}
}

int main(int argc, char const *argv[])
{
	init();
	terminal();
	return 0;
}