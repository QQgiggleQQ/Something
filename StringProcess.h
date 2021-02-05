#pragma once

#include <string.h>
#include <windows.h>

/* 大小写字母间的映射 */
typedef struct _CASE_MAP {

	char map[256];

	_CASE_MAP()
	{
		memset(map, 0, sizeof(map));

		for (BYTE i = 'A'; i <= 'Z'; ++i)
		{
			map[i] = i + 32;
			map[i + 32] = i;
		}
	}
}CASE_MAP;


#ifndef FORMAT_MAXSIZE
#define FORMAT_MAXSIZE 64 // 最大模式串长度
#endif

/* 求编辑距离所用的dp矩阵 */
typedef struct _SIMI_DP {

	USHORT dp[FORMAT_MAXSIZE + 10][FORMAT_MAXSIZE + 10];

	_SIMI_DP()
	{
		memset(dp, 0, sizeof(dp));

		for (USHORT i = 0; i < FORMAT_MAXSIZE + 10; ++i)
		{
			dp[0][i] = i;
			dp[i][0] = i;
		}
	}
}SIMI_DP;

namespace strpro {

	// 宽字符转多字节
	bool wideCharToMultiByte(wchar_t* wstr, USHORT& length, char* str);

	// 分割后缀名
	void splitSuffix(wchar_t* name, USHORT& nameLen, wchar_t*& suff, USHORT& suffLen);

	// 转小写
	void tolower(char* str, USHORT length);

	// 双字节编码统一认为中文
	bool hasZh(const char* str);

	// 字符串比较区分大小写
	bool strncmp(const char* str1, const char* str2, USHORT length);

	// 字符串比较忽略大小写
	bool strnicmp(const char* str1, const char* str2, USHORT length);

	bool match_simi_en(const char* format, USHORT flength, const char* str, USHORT slength);
	bool match_simi_zh(const char* format, USHORT flength, const char* str, USHORT slength);

	bool match_simi_case(const char* format, USHORT flength, const char* str, USHORT slength);
	bool match_simi_nocase(const char* format, USHORT flength, const char* str, USHORT slength);

	bool match_prec_case(const char* format, USHORT flength, const char* str, USHORT slength);
	bool match_prec_nocase(const char* format, USHORT flength, const char* str, USHORT slength);

	bool match_cont_case(const char* format, USHORT flength, const char* str, USHORT slength);
	bool match_cont_nocase(const char* format, USHORT flength, const char* str, USHORT slength);
}