#include "BackstageData.h"

CVolumeTable GlobalData::m_volumeTable;

bool GlobalData::read()
{
	FILE* input = NULL;
	fopen_s(&input, DATABASE_PATH, "rb");
	if (!input)
	{
		return false;
	}
	if (!m_volumeTable.read(input))
	{
		fclose(input);
		return false;
	}
	if (!g_nodePool.read(input))
	{
		fclose(input);
		return false;
	}
	fclose(input);
	return true;
}

void GlobalData::create()
{
	m_volumeTable.create();
}

void GlobalData::save()
{
	FILE* output = NULL;
	fopen_s(&output, DATABASE_PATH, "wb");
	if (output)
	{
		m_volumeTable.save(output);
		g_nodePool.save(output);
		fclose(output);
	}
}

void GlobalData::updata()
{
	m_volumeTable.updata();
}

void GlobalData::search(QUERY* pQuery)
{
	m_volumeTable.search(pQuery, &g_resultTable);
}
