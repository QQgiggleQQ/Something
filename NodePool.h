#pragma once

#include "Result.h"
#include "FileType.h"
#include "BaseNodePool.h"
#include "StringProcess.h"

class CNodePool : public CBaseNodePool {
public:
	CNodePool();

	UINT nameInsert(wchar_t* name, USHORT length);
	UINT suffInsert(wchar_t* suff, USHORT length);

	void save(FILE* output);
	bool read(FILE* input);

private:
	typedef struct _SAVE_NODE {
		UINT virtCount;
		UINT tailChunk;
		UINT tailOffset;
	}SAVE_NODE, * PSAVE_NODE;

	char m_buffer[1024];
	CSuffixTypeMap m_suffixTypeMap;
};

class GlobalDataV0 {
public:
	static CNodePool g_nodePool;
	static CResultTable g_resultTable;
};
