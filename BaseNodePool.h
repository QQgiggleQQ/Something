#pragma once

#include "Node.h"
#include <stdexcept>

class CBaseNodePool {
public:
	CBaseNodePool();
	virtual ~CBaseNodePool();

	void clear();
	void erase(UINT address);
	PNODE translate(UINT address);

	UINT _nodeCount();
	UINT _nameCount();
	UINT _suffCount();
	UINT _recyCount();

protected:
	const UINT ChunkSize = 0X010000;
	const UINT TableLength = 291143;

	BYTE** m_chunk;     // 块索引表
	UINT m_virtCount;   // 虚拟块数
	UINT m_tailChunk;   // 尾部实块
	UINT m_tailOffset;  // 尾部偏移
	UINT m_recyBin[64]; // 回收链表
	UINT* m_table;      // 哈希链表

	void initialize();
	void destroy();
	void* allocate(USHORT size);
	void recycle(UINT address, USHORT size);
	UINT hash(char* str);
	UINT hash(char* str, USHORT length);
	PNODE newNode(char* str, USHORT length, BYTE mark);
	UINT insert(char* str, USHORT length, BYTE mark);
};
