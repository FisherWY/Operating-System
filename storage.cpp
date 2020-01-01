# include <cstdio>
# include <iostream>
# include <cmath>

# define MEMORY_NUMBER 4	// 内存单元数
# define MEMORY_STATUS 8	// 暂存当前页面距离上次使用的时间，用8位移寄存器实现
# define PROCESS_TIME 15	// 调用内存页面的次数

struct Page
{
	int num;	// 页面号
	int time;	// 距离上次使用时间
};

Page p[MEMORY_NUMBER];	// 内存页数
int status[MEMORY_NUMBER][MEMORY_STATUS];	// 暂存内存当前的状态：缓冲区
int queue[PROCESS_TIME];	// 记录调入队列
int K;	// 调入队列计数变量

/**
 * 初始化内存单元、缓冲区
 **/
void init(Page *p, int status[MEMORY_NUMBER][MEMORY_STATUS])
{
	// 初始化页面表
	for (int i = 0; i < MEMORY_NUMBER; i++)
	{
		p[i].num = -1;
		p[i].time = 0;
	}
	// 初始化状态记录表
	for (int i = 0; i < MEMORY_NUMBER; i++)
	{
		for (int j = 0; j < MEMORY_STATUS; j++)
		{
			status[i][j] = 0;
		}
	}
}

/**
 * 取得在内存中停留最久的页面，即在status表中最小的表项
 **/
int getMax(int status[MEMORY_NUMBER][MEMORY_STATUS])
{
	int pageNo = 0, max = pow(2, MEMORY_STATUS);
	for (int i = 0; i < MEMORY_NUMBER; i++)
	{
		int sum = 0;
		for (int j = 0; j < MEMORY_STATUS; j++)
		{
			sum += status[i][j] * pow(2, MEMORY_STATUS - j - 1);
		}
		if (sum < max)
		{
			max = sum;
			pageNo = i;
		}
	}
	return pageNo;
}

/**
 * 判断页面是否已在内存中
 **/
int equation(int fold, Page *p)
{
	for (int i = 0; i < MEMORY_NUMBER; i++)
	{
		if (fold == p[i].num)
		{
			return i;
		}
	}
	return -1;
}

void update(int num, Page *p, int status[MEMORY_NUMBER][MEMORY_STATUS])
{
	for (int i = 0; i < MEMORY_NUMBER; i++)
	{
		for (int j = MEMORY_STATUS - 1; j > 0; j--)
		{
			status[i][j] = status[i][j-1];
		}
		// 更新内存调用表
		if (i == num)
		{
			status[i][0] = 1;
			p[i].time = 0;
		}
		else
		{
			status[i][0] = 0;
			p[i].time++;
		}
	}
}

/**
 * LRU算法
 **/
void lru(int fold, Page *p)
{
	int val = equation(fold, p);
	// 页面在内存中，更新最近使用时间
	if (val >= 0)
	{
		update(val, p, status);
	}
	// 页面不在内存中，从主存中调入
	else
	{
		queue[++K] = fold;	// 记录调入页面
		val = getMax(status);
		// 替换页面
		p[val].num = fold;
		p[val].time = 0;
		update(val, p, status);
	}
}

/**
 * FIFO算法
 **/
void fifo(int fold, Page *p)
{
	int val = equation(fold, p);
	if (val >= 0)
	{
		update(val, p, status);
	}
	else
	{
		// 寻找停留最久的页面
		int max = 0;
		for (int i = 0; i < MEMORY_NUMBER; i++)
		{
			if (p[i].time > max)
			{
				max = i;
			}
		}
		// 替换页面
		p[max].num = fold;
		p[max].time = 0;
		update(max, p, status);
	}
}

int main(int argc, char const *argv[])
{
	// 需要使用的页面号
	int a[15] = {1,0,1,0,2,4,1,0,0,8,7,5,4,3,2};
	char cmd[1];
	// 初始化系统
	init(p, status);
	while (1)
	{
		K = -1;
		// 模拟内存页面调用
		for (int i = 0; i < PROCESS_TIME; i++)
		{
			lru(a[i], p);
		}
		// 结果输出
		printf("\n内存状态: \n");
		for (int i = 0; i < MEMORY_NUMBER; i++)
		{
			printf("页面号: %d , 距离上次使用时间: %d\n", p[i].num, p[i].time);
		}
		printf("\n=====\n");
		printf("页面调用记录表(R7 ~ R0 寄存器):\n");
		printf("页面号 | P7 | P6 | P5 | P4 | P3 | P2 | P1 | P0\n");
		for (int i = 0; i < MEMORY_NUMBER; i++)
		{
			printf("%4d", p[i].num);
			for (int j = 0; j < MEMORY_STATUS; j++)
			{
				printf("%4d", status[i][j]);
			}
			printf("\n");
		}
		printf("\n=====\n");
		printf("调入队列为: \n");
		for (int i = 0; i < K+1; i++)
		{
			printf("%d -> ", queue[i]);
		}
		printf("\n缺页次数为: %d , 缺页率: %.6f\n", K+1, (float)(K+1)/PROCESS_TIME);
		printf("\nContinue?\n");
		scanf("%s", cmd);
		if (!strcmp(cmd, "n"))
		{
			break;
		}
	}
	return 0;
}