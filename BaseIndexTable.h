#pragma once

#include "Index.h"

class CBaseIndexTable {
public:
	CBaseIndexTable();
	virtual ~CBaseIndexTable();

	bool exist(UINT index);
	void insert(PINDEX_NODE pNode, UINT index);
	void erase(UINT index);
	UINT length();
	INDEX_NODE& at(UINT index);
	INDEX_NODE& operator[](UINT index);
	void clear();
	void clean();

protected:
	PINDEX_NODE m_table;
	UINT m_tableLength;
};
