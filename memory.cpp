# include <iostream>
# include <cstdio>
# include <vector>
// 内存容量，单位为kb
# define RAM_SIZE 65536
// 不可再分割的内存大小
# define ALLOC_MIN_SIZE 0

/**
 * 空闲分区表-数据结构(Free Partition table)
 * 起始地址(内存地址-kb) | 大小(kb) | 使用情况(0:未使用 1:使用中)
 * startAddr | size | status
 **/
struct fpTable
{
	int startAddr;
	int size;
	int status;
};

/**
 * 空闲分区链-数据结构(Free Partition chain)
 * table: 指向对应的空闲分区表地址
 * previous: 双向链表的上一项
 * next: 双向链表的下一项
 **/
struct fpChain
{
	fpTable *table;
	fpChain *previous;
	fpChain *next;
};

/**
 * 已使用的内存
 * 起始地址(内存地址-kb) | 大小(kb)
 * startAddr | size
 **/
struct used
{
	int startAddr;
	int size;
};

// 空闲分区表
std::vector<fpTable *> ramTable;
// 已使用分区表
std::vector<used *> usedTable;
// 空闲分区链
fpChain *ramChain;

/**
 * 系统初始化
 **/
void init()
{
	// 初始化空闲分区表
	fpTable *tableNode = new fpTable;
	tableNode->startAddr = 0;
	tableNode->size = RAM_SIZE;
	tableNode->status = 0;
	ramTable.push_back(tableNode);
	// 初始化空闲分区链
	fpChain *chainNode = new fpChain;
	chainNode->table = tableNode;
	chainNode->previous = chainNode;
	chainNode->next = chainNode;
	ramChain = chainNode;
}

/**
 * 查看内存使用情况
 **/
void ps()
{
	printf("\n=====空闲分区表================\n");
	printf("起始地址 | 大小(kb) | 使用情况\n");
	for (int i = 0; i < ramTable.size(); i++)
	{
		printf("%8d | %8d | %5d\n", ramTable[i]->startAddr, ramTable[i]->size, ramTable[i]->status);
	}
	printf("===============================\n\n");
	printf("=====已使用的分区表==========\n");
	printf("进程号 | 起始地址 | 大小(kb)\n");
	for (int i = 0; i < usedTable.size(); i++)
	{
		printf("%6d | %8d | %8d\n", i, usedTable[i]->startAddr, usedTable[i]->size);
	}
	printf("=============================\n\n");
}

/**
 * 申请内存分配
 * s: 申请的内存大小
 **/
void allocate(int s)
{
	// 参数错误
	if (s <= 0 || s > RAM_SIZE)
	{
		printf("ERROR: 参数错误！\n");
		return;
	}
	// 没有空闲内存了
	if (ramTable.size() == 0)
	{
		printf("ERROR: 内存已分配完毕，申请失败！\n");
		return;
	}
	// 寻找满足要求的空闲内存块
	bool find = false;
	fpChain *chainPointer = ramChain;
	fpTable *tablePointer = chainPointer->table;
	do {
		if ((tablePointer->size - s) >= ALLOC_MIN_SIZE)
		{
			find = true;
			break;
		}
		else
		{
			chainPointer = chainPointer->next;
			tablePointer = chainPointer->table;
		}
	} while(chainPointer != ramChain);
	// 内存空间不足
	if (!find)
	{
		printf("ERROR: 内存空间不足，申请失败！\n");
		return;
	}
	// 分配空间，添加到已使用分区表，修改空闲分区表、空闲分区链
	used *usedPointer = new used;
	usedPointer->startAddr = tablePointer->startAddr;
	usedPointer->size = s;
	usedTable.push_back(usedPointer);
	// 寻找要修改的表项位置
	int pos = 0;
	for (int i = 0; i < ramTable.size(); i++)
	{
		if (ramTable[i]->startAddr == tablePointer->startAddr)
		{
			pos = i;
			break;
		}
	}
	// 空间刚好分配完，删除分区表和分区链中对应表项
	if (tablePointer->size == s)
	{
		printf("INFO: 该块内存分配完毕，删除空闲分区表和分区链中对应项\n");
		// 删除表项
		ramTable.erase(ramTable.begin() + pos);
		// 摘链
		if (ramTable.size() == 0)
		{
			printf("INFO: 已无空闲分区，将空闲链表置为NULL\n");
			ramChain = NULL;
		}
		else
		{
			// 如果刚好是双向链表的头部，则使用下一节点作为链表头部
			if (chainPointer == ramChain)
			{
				ramChain = chainPointer->next;
			}
			chainPointer->next->previous = chainPointer->previous;
			chainPointer->previous->next = chainPointer->next;
		}
		delete chainPointer;
	}
	else	// 分配之后仍有剩余空间
	{
		printf("INFO: 该块内存仍有剩余空间，修改空闲分区表大小\n");
		tablePointer->size -= s;
		tablePointer->startAddr += s;
	}
}

/**
 * 释放内存
 * index: 要释放内存的进程号
 **/
void release(int index)
{
	if (index < 0 || index >= usedTable.size())
	{
		printf("ERROR: 参数错误！\n");
		return;
	}
	// 获取要释放的内存块
	used *usedPointer = usedTable[index];
	// 空链情况，直接新建空闲分区并插入
	if (ramChain == NULL)
	{
		fpTable *tableNode = new fpTable;
		tableNode->startAddr = usedPointer->startAddr;
		tableNode->size = usedPointer->size;
		tableNode->status = 0;
		ramTable.push_back(tableNode);
		fpChain *chainNode = new fpChain;
		chainNode->table = tableNode;
		chainNode->previous = chainNode;
		chainNode->next = chainNode;
		ramChain = chainNode;
		usedTable.erase(usedTable.begin() + index);
		return;
	}
	// 非空链情况，寻求与前后空闲分区合并
	fpChain *chainPointer = ramChain;
	fpTable *tablePointer = chainPointer->table;
	do {
		// 与前后空闲分区都相邻，合并前后空闲分区
		if ((tablePointer->startAddr + tablePointer->size == usedPointer->startAddr) &&
			(usedPointer->startAddr + usedPointer->size == chainPointer->next->table->startAddr))
		{
			printf("INFO: 与前后空闲分区都相邻\n");
			tablePointer->size = tablePointer->size + usedPointer->size + chainPointer->next->table->size;
			for (int i = 0; i < ramTable.size(); i++)
			{
				if (chainPointer->next->table->startAddr == ramTable[i]->startAddr)
				{
					ramTable.erase(ramTable.begin() + i);
				}
			}
			chainPointer->next->next->previous = chainPointer->next->previous;
			chainPointer->next = chainPointer->next->next;
			usedTable.erase(usedTable.begin() + index);
			return;
		}
		// 与前面空闲分区相邻，前面空闲分区空间增大
		if (tablePointer->startAddr + tablePointer->size == usedPointer->startAddr)
		{
			printf("INFO: 与前面空闲分区相邻\n");
			tablePointer->size += usedPointer->size;
			usedTable.erase(usedTable.begin() + index);
			return;
		}
		// 与后面空闲分区相邻，后面空闲分区内存起址前移，扩大分区空间
		if (usedPointer->startAddr + usedPointer->size == tablePointer->startAddr)
		{
			printf("INFO: 与后面空闲分区相邻\n");
			tablePointer->startAddr = usedPointer->startAddr;
			tablePointer->size += usedPointer->size;
			usedTable.erase(usedTable.begin() + index);
			return;
		}
		chainPointer = chainPointer->next;
		tablePointer = chainPointer->table;
	} while(chainPointer != ramChain);
	// 非空链情况，新建独立空闲分区
	printf("INFO: 非空链情况，新建独立空闲分区\n");
	// 寻找插入点
	chainPointer = ramChain;
	tablePointer = chainPointer->table;
	do {
		if (tablePointer->startAddr > usedPointer->startAddr)
			break;
		else
		{
			chainPointer = chainPointer->next;
			tablePointer = chainPointer->table;
		}
	} while(chainPointer != ramChain);
	printf("INFO: 插入点位于 %d\n", tablePointer->startAddr);
	// 新建分区并插入
	fpTable *tableNode = new fpTable;
	tableNode->startAddr = usedPointer->startAddr;
	tableNode->size = usedPointer->size;
	tableNode->status = 0;
	for (int i = 0; i < ramTable.size(); i++)
	{
		if (ramTable[i]->startAddr == tablePointer->startAddr)
		{
			printf("INFO: 找到插入点 %d\n", i);
			ramTable.insert(ramTable.begin() + i, tableNode);
			break;
		}
	}
	// 插入到双向链表中
	fpChain *chainNode = new fpChain;
	chainNode->table = tableNode;
	chainNode->next = chainPointer;
	chainNode->previous = chainPointer->previous;
	chainPointer->previous->next = chainNode;
	chainPointer->previous = chainNode;
	// 修改链表头部
	if (chainPointer == ramChain)
	{
		ramChain = chainNode;
	}
	usedTable.erase(usedTable.begin() + index);
}

/**
 * 控制台
 **/
void terminal()
{
	char cmdstr[32];
	int input;
	while (1)
	{
		printf("cmd: ");
		scanf("%s", cmdstr);

		if (!strcmp(cmdstr, "exit"))
		{
			return;
		}

		if (!strcmp(cmdstr, "help"))
		{
			printf("\n=================================================\n");
			printf("申请内存: allocate\n");
			printf("释放内存: release\n");
			printf("查看当前空余内存: ps\n");
			printf("获取帮助: help\n");
			printf("退出: exit\n");
			printf("=================================================\n\n");
			continue;
		}

		if (!strcmp(cmdstr, "ps"))
		{
			ps();
			continue;
		}

		if (!strcmp(cmdstr, "allocate"))
		{
			printf("要申请的内存大小(kb): ");
			scanf("%d", &input);
			allocate(input);
			continue;
		}

		if (!strcmp(cmdstr, "release"))
		{
			printf("要释放内存的进程号: ");
			scanf("%d", &input);
			release(input);
			continue;
		}

		printf("cmd: 未知的命令！\n");
	}
}

int main(int argc, char const *argv[])
{
	init();
	ps();
	terminal();
	return 0;
}