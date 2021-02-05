#pragma once

#include "VolumeTable.h"

#define DATABASE_PATH "C:\\Program Files (x86)\\Something_Database.db"

class GlobalData : public GlobalDataV0 {
public:
	static CVolumeTable m_volumeTable;

public:
	static bool read();
	static void create();
	static void save();
	static void updata();
	static void search(QUERY* pQuery);
};