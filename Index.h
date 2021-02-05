#pragma once

#include "NodePool.h"

#define INDEX_ATTRSYS 0X80000000
#define INDEX_ATTRRON 0X40000000
#define INDEX_ATTRHID 0X20000000
#define INDEX_ATTRNOR 0X10000000
#define INDEX_TYPEDIR 0X08000000

#define ATTR_SYS 0X08
#define ATTR_RON 0X04
#define ATTR_HID 0X02
#define ATTR_NOR 0X01
#define ATTR_ALL 0X0F

typedef struct _INDEX_NODE {

	UINT info;
	UINT name;
	UINT suff;

	_INDEX_NODE() : info(0), name(0), suff(0) {}

	void changeAttributes(BYTE attr)
	{
		info = ((info & 0XFFFFFFF) | (attr << 28));
	}

}INDEX_NODE, * PINDEX_NODE;

// 根据索引结点 info 变量以及后缀名结点获取文件类型
#define GetFileType(info, pSuffNode) (((info) & INDEX_TYPEDIR) ? TYPE_DIR : ((pSuffNode) ? ((pSuffNode)->mark) : TYPE_OTH))
// 翻译结点内存池相对地址得到结点指针
#define TranslateAddress(address) ((address) ? (GlobalDataV0::g_nodePool.translate(address)) : NULL)
