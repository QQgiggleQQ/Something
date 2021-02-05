#pragma once

#include "Query.h"
#include "IndexTable.h"

class CVolumeTable : public std::vector<CIndexTable> {
public:
	CVolumeTable();

	void create();
	void updata();

	void save(FILE* output);
	bool read(FILE* input);

	char* getPath(UINT info);
	char* getName(UINT info);
	char* getSuff(UINT info);

	void search(QUERY* query, CResultTable* result);

private:
	void initialize();

	typedef bool (*MATCH_FUNCTION) (const char* format, USHORT flength, const char* str, USHORT slength);
	MATCH_FUNCTION getNameMatchFunction(BYTE matchMode);
	MATCH_FUNCTION getSuffMatchFunction(BYTE matchMode);
};
