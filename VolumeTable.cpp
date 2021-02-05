#include "VolumeTable.h"

static char s_buffer[1024] = { 0 };

CVolumeTable::CVolumeTable() : std::vector<CIndexTable>() {}

void CVolumeTable::initialize()
{
	clear();
	GlobalDataV0::g_nodePool.clear();

	std::vector<BYTE> idVector = getVolumeId();
	resize(idVector.size());

	for (UINT i = 0; i < size(); ++i)
	{
		at(i).initialize(idVector[i]);
	}
}

void CVolumeTable::create()
{
	initialize();

	for (auto& iTable : *this)
	{
		iTable.create();
	}
}

void CVolumeTable::updata()
{
	for (auto& iTable : *this)
	{
		iTable.updata();
	}
}

void CVolumeTable::save(FILE* output)
{
	UINT volumeCount = size();
	fwrite(&volumeCount, sizeof(UINT), 1, output);

	for (auto& iTable : *this)
	{
		iTable.save(output);
	}
}

bool CVolumeTable::read(FILE* input)
{
	initialize();

	UINT volumeCount = 0;
	if (1 != fread(&volumeCount, sizeof(UINT), 1, input) || volumeCount != this->size())
	{
		return false;
	}

	for (auto& iTable : *this)
	{
		if (!iTable.read(input))
		{
			return false;
		}
	}

	return true;
}

char* CVolumeTable::getPath(UINT info)
{
	UINT vid = info >> 27;
	UINT frn = info & 0X07FFFFFF;
	if (vid < size())
	{
		at(vid).getPath(frn, s_buffer);
	}
	else
	{
		*s_buffer = 0;
	}
	return s_buffer;
}

char* CVolumeTable::getName(UINT info)
{
	UINT vid = info >> 27;
	UINT frn = info & 0X07FFFFFF;
	if (vid < size())
	{
		at(vid).getName(frn, s_buffer);
	}
	else
	{
		*s_buffer = 0;
	}
	return s_buffer;
}

char* CVolumeTable::getSuff(UINT info)
{
	UINT vid = info >> 27;
	UINT frn = info & 0X07FFFFFF;
	if (vid < size())
	{
		at(vid).getSuff(frn, s_buffer);
	}
	else
	{
		*s_buffer = 0;
	}
	return s_buffer;
}

void CVolumeTable::search(QUERY* query, CResultTable* result)
{
	result->clear();

	static CTypeIconIndexMap s_typeIconIndexMap;

	USHORT flengthName = (USHORT)query->name.length();
	const char* formatName = query->name.c_str();
	CVolumeTable::MATCH_FUNCTION matchName = getNameMatchFunction(query->mode);

	USHORT flengthSuff = (USHORT)query->suff.length();
	const char* formatSuff = query->suff.c_str();
	CVolumeTable::MATCH_FUNCTION matchSuff = getSuffMatchFunction(query->mode);

	bool nameNeedMatch = query->nameNeedMatch();
	bool suffNeedMatch = query->suffNeedMatch();
	bool rootNeedMatch = query->rootNeedMatch();
	bool typeNeedMatch = query->typeNeedMatch();
	bool zhNeedMatch = strpro::hasZh(query->name.c_str());

	for (UINT i = 0; i < size(); ++i)
	{
		CIndexTable& iTable = at(i);

		if (rootNeedMatch && iTable.id() != query->root_vid) continue; // 过滤卷

		UINT tableLength = iTable.length();
		for (UINT frn = 0; frn < tableLength; ++frn)
		{
			INDEX_NODE& iNode = iTable[frn];

			if (!((iNode.info >> 28) & query->attr)) continue; // 过滤属性同时过滤空索引
			
			PNODE pSuffNode = TranslateAddress(iNode.suff); // 翻译后缀名结点

			if (suffNeedMatch && (!pSuffNode)) continue; // 需要匹配后缀但后缀名为空

			BYTE type = GetFileType(iNode.info, pSuffNode);
			if (typeNeedMatch && !(type & query->type)) continue;  // 过滤类型

			if (rootNeedMatch && !iTable.hasParent(frn, query->root_frn)) continue; // 过滤根路径

			if (suffNeedMatch && (!matchSuff(formatSuff, flengthSuff, pSuffNode->str, pSuffNode->length))) continue; // 过滤后缀

			PNODE pNameNode = TranslateAddress(iNode.name); // 翻译文件名结点
			if (!pNameNode) continue; // 文件名结点不可能为空

			if (zhNeedMatch && !(pNameNode->mark & NAME_NOCHINA)) continue; // 需匹配中文但不含中文

			if (nameNeedMatch && (!matchName(formatName, flengthName, pNameNode->str, pNameNode->length))) continue; // 过滤文件名

			USHORT length = pNameNode->length + (pSuffNode ? pSuffNode->length : 0);
			result->push_back(_ANS_NODE(s_typeIconIndexMap[type], min(length, 255), (i << 27) | frn));
		}
	}
	result->sort();
}

CVolumeTable::MATCH_FUNCTION CVolumeTable::getNameMatchFunction(BYTE mode)
{
	switch (mode)
	{
	case (MODE_PREC):
		return strpro::match_prec_nocase;
	case (MODE_PREC | MODE_CASE):
		return strpro::match_prec_case;
	case MODE_CONT:
		return strpro::match_cont_nocase;
	case (MODE_CONT | MODE_CASE):
		return strpro::match_cont_case;
	case MODE_SIMI:
		return strpro::match_simi_nocase;
	case (MODE_SIMI | MODE_CASE):
		return strpro::match_simi_case;
	default:
		return strpro::match_prec_case;
	}
}

CVolumeTable::MATCH_FUNCTION CVolumeTable::getSuffMatchFunction(BYTE matchMode)
{
	return strpro::match_cont_case;
}
