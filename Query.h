#pragma once

#include <string>

#include "Index.h"

#define MODE_PREC 0B0001 // 准确
#define MODE_CONT 0B0010 // 包含
#define MODE_SIMI 0B0100 // 相似
#define MODE_CASE 0B1000 // 区分大小写

typedef struct _QUERY {

	BYTE type; // 目标文件类型
	BYTE attr; // 目标文件属性
	BYTE mode; // 名称匹配模式

	BYTE root_vid; // 目标目录卷id
	UINT root_frn; // 目标目录frn

	std::string name; // 目标名称
	std::string suff; // 目标后缀

	_QUERY(BYTE _type, BYTE _attr, BYTE _mode, BYTE _root_vid, UINT _root_frn, std::string _name, std::string _suff)
	{
		type = _type;
		attr = _attr;
		mode = _mode;
		root_vid = _root_vid;
		root_frn = _root_frn;
		name = _name;
		suff = _suff;
	}

	bool nameNeedMatch()
	{
		if (name == "")
		{
			return (mode & MODE_PREC); // 空串情况下，只有准确模式需要匹配
		}
		return true;
	}

	bool suffNeedMatch()
	{
		return suff != "";
	}

	bool typeNeedMatch()
	{
		return type != TYPE_ALL;
	}

	bool attrNeedMatch()
	{
		return attr != ATTR_ALL;
	}

	bool rootNeedMatch()
	{
		return root_vid != 0 && root_frn != 0;
	}

}QUERY;
