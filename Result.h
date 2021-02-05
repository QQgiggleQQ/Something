#pragma once

#include <vector>
#include <algorithm>
#include <windows.h>

#pragma pack(1)
typedef struct _ANS_NODE {

	BYTE iconId; // 图标在 ImageList 中的编号，此值根据文件类型得到

	BYTE length; // 文件名长度，当结果不是太多时，按照此值由小到大简单排序

	UINT info; // 包含索引表 ID （高五位）和文件 FRN （低27位），用于获取文件名与文件路径

	bool operator < (const _ANS_NODE& other) const
	{
		return length < other.length;
	}

	_ANS_NODE(BYTE _iconId, BYTE _length, UINT _info) : iconId(_iconId), length(_length), info(_info) {}

}ANS_NODE, * PANS_NODE;
#pragma pack()

class CResultTable : public std::vector<ANS_NODE> {
public:
	CResultTable() : std::vector<ANS_NODE>() {}

	// 当结果数量较多时，排序会消耗较多时间，但此时搜索结果很可能是无意义的，故不进行排序。
	void sort()
	{
		if (this->size() <= 4096)
		{
			std::sort(begin(), end());
		}
	}
};