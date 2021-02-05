#pragma once

#include "NTFS.h"
#include "BaseIndexTable.h"

#include <winioctl.h>

class CIndexTable : public CBaseIndexTable {
public:
	CIndexTable();
	~CIndexTable();

	void initialize(BYTE id);
	void create();
	void getName(UINT frn, char* name);
	void getPath(UINT frn, char* path);
	void getSuff(UINT frn, char* suff);
	void getFullPath(UINT frn, char* path);
	BYTE id();
	bool hasParent(UINT frn, UINT pfrn);
	void updata();

private:
	void addFile(PUSN_RECORD pUsnRecord);
	void delFile(PUSN_RECORD pUsnRecord);
	void chaAttr(PUSN_RECORD pUsnRecord);
	void appendPath(UINT frn, char*& path);
	BYTE selectAttributes(DWORD dwSysAttr);
	INDEX_NODE& buildIndexNode(PUSN_RECORD pUsnRecord);

private:
	BYTE       m_id;
	HANDLE     m_handle;
	UINT       m_fileCount;
	USN        m_firstUsn;
	USN        m_nextUsn;
	DWORDLONG  m_usnJournalId;

private:
	typedef struct _SAVE_NODE {
		UINT      tableLength;
		BYTE      id;
		UINT      fileCount;
		USN       nextUsn;
		DWORDLONG usnJournalId;
	}SAVE_NODE;

public:
	void save(FILE* output);
	bool read(FILE* input);
};