#include "StringProcess.h"

static CASE_MAP s_caseMap;
static SIMI_DP s_simiDp;

bool strpro::wideCharToMultiByte(wchar_t* wstr, USHORT& length, char* str)
{
	char* _str = str;
	USHORT _length = length;
	wchar_t* _wstr = wstr, * _wend = wstr + length;

	while (_wstr != _wend)
	{
		if (*_wstr >= 0X80)
		{
			break;
		}
		*_str++ = (char)*_wstr++;
	}
	if (_wstr == _wend)
	{
		length = _str - str;
	}
	else
	{
		length = WideCharToMultiByte(CP_ACP, 0, wstr, length, str, MAX_PATH, NULL, NULL);
	}
	str[length] = 0;
	return (length == _length);
}

void strpro::splitSuffix(wchar_t* name, USHORT& nameLen, wchar_t*& suff, USHORT& suffLen)
{
	suff = NULL;
	suffLen = 0;
	if (!name)
	{
		return;
	}
	wchar_t* c = name + nameLen - 1;
	while (c >= name)
	{
		if (*c == L'.')
		{
			break;
		}
		c--;
	}
	if (c >= name)
	{
		suff = c + 1;
		suffLen = name + nameLen - c - 1;
		nameLen -= suffLen + 1;
		*c = 0;
	}
}

void strpro::tolower(char* str, USHORT length)
{
	char* end = str + length;
	while (str != end)
	{
		if (*str >= 'A' && *str <= 'Z')
		{
			*str += 32;
		}
		str++;
	}
}

bool strpro::hasZh(const char* str)
{
	while (str && *str)
	{
		if (*str++ > 0X80)
		{
			return true;
		}
	}
	return false;
}

bool strpro::strncmp(const char* str1, const char* str2, USHORT length)
{
	const char* end = str1 + length;
	while (str1 != end)
	{
		if (*str1++ != *str2++)
		{
			return false;
		}
	}
	return true;
}

bool strpro::strnicmp(const char* str1, const char* str2, USHORT length)
{
	const char* end = str1 + length;
	while (str1 != end)
	{
		if (*str1 != *str2 && s_caseMap.map[*str1] != *str2)
		{
			return false;
		}
		str1++;
		str2++;
	}
	return true;
}

/* 根据模式串长度返回认为相似的最大编辑距离 */
#define MaxDist(a) ((a <= 1) ? 0 : ((a <= 5) ? 1 : ((a > 10) ? 3 : 2)))
/* 大减小作差 */
#define Diff(a, b) ((a >= b) ? (a - b) : (b - a))
/* 编辑距离相似模式 */
bool strpro::match_simi_en(const char* format, USHORT flength, const char* str, USHORT slength)
{
	USHORT maxDist = MaxDist(flength);

	if (Diff(flength, slength) > maxDist)
	{
		return false; // 长度差大于最大编辑距离一定不相似
	}

	for (USHORT i = 1; i <= flength; ++i)
	{
		USHORT min = -1;
		for (USHORT j = 1; j <= slength; ++j)
		{
			USHORT case1 = s_simiDp.dp[i - 1][j] + 1;
			USHORT case2 = s_simiDp.dp[i][j - 1] + 1;
			USHORT case3 = s_simiDp.dp[i - 1][j - 1];
			if (format[i - 1] != str[j - 1] && s_caseMap.map[format[i - 1]] != str[i - 1]) // 忽略大小写
			{
				case3 += 1;
			}
			s_simiDp.dp[i][j] = min(min(case1, case2), case3);
			if (s_simiDp.dp[i][j] < min)
			{
				min = s_simiDp.dp[i][j];
			}
		}
		if (min > maxDist)
		{
			return false; // 某行最小值大于最大编辑距离，一定不相似
		}
	}
	return (s_simiDp.dp[flength][slength] <= maxDist);
}

/* 中文相似模式 */
bool strpro::match_simi_zh(const char* format, USHORT flength, const char* str, USHORT slength)
{
	return false;
}

bool strpro::match_simi_case(const char* format, USHORT flength, const char* str, USHORT slength)
{
	USHORT maxDist = MaxDist(flength);

	if (Diff(flength, slength) > maxDist)
	{
		return false; // 长度差大于最大编辑距离一定不相似
	}

	for (USHORT i = 1; i <= flength; ++i)
	{
		USHORT min = -1;
		for (USHORT j = 1; j <= slength; ++j)
		{
			USHORT case1 = s_simiDp.dp[i - 1][j] + 1;
			USHORT case2 = s_simiDp.dp[i][j - 1] + 1;
			USHORT case3 = s_simiDp.dp[i - 1][j - 1];
			if (format[i - 1] != str[j - 1])
			{
				case3 += 1;
			}
			s_simiDp.dp[i][j] = min(min(case1, case2), case3);
			if (s_simiDp.dp[i][j] < min)
			{
				min = s_simiDp.dp[i][j];
			}
		}
		if (min > maxDist)
		{
			return false; // 某行最小值大于最大编辑距离，一定不相似
		}
	}
	return (s_simiDp.dp[flength][slength] <= maxDist);
}

bool strpro::match_simi_nocase(const char* format, USHORT flength, const char* str, USHORT slength)
{
	USHORT maxDist = MaxDist(flength);

	if (Diff(flength, slength) > maxDist)
	{
		return false; // 长度差大于最大编辑距离一定不相似
	}

	for (USHORT i = 1; i <= flength; ++i)
	{
		USHORT min = -1;
		for (USHORT j = 1; j <= slength; ++j)
		{
			USHORT case1 = s_simiDp.dp[i - 1][j] + 1;
			USHORT case2 = s_simiDp.dp[i][j - 1] + 1;
			USHORT case3 = s_simiDp.dp[i - 1][j - 1];
			if (format[i - 1] != str[j - 1] && s_caseMap.map[format[i - 1]] != str[i - 1]) // 忽略大小写
			{
				case3 += 1;
			}
			s_simiDp.dp[i][j] = min(min(case1, case2), case3);
			if (s_simiDp.dp[i][j] < min)
			{
				min = s_simiDp.dp[i][j];
			}
		}
		if (min > maxDist)
		{
			return false; // 某行最小值大于最大编辑距离，一定不相似
		}
	}
	return (s_simiDp.dp[flength][slength] <= maxDist);
}

/* 准确模式不忽略大小写 */
bool strpro::match_prec_case(const char* format, USHORT flength, const char* str, USHORT slength)
{
	if (flength != slength)
	{
		return false;
	}
	return strncmp(format, str, flength);
}

/* 准确模式忽略大小写 */
bool strpro::match_prec_nocase(const char* format, USHORT flength, const char* str, USHORT slength)
{
	if (flength != slength)
	{
		return false;
	}
	return strnicmp(format, str, flength);
}

/* 包含模式不忽略大小写 */
bool strpro::match_cont_case(const char* format, USHORT flength, const char* str, USHORT slength)
{
	if (flength > slength)
	{
		return false;
	}

	const char* end = str + slength - flength;
	const char* start = str;
	while (start <= end)
	{
		if (*start == *format)
		{
			if (strncmp(start, format, flength))
			{
				return true;
			}
		}
		start++;
	}
	return false;
}

/* 包含模式忽略大小写 */
bool strpro::match_cont_nocase(const char* format, USHORT flength, const char* str, USHORT slength)
{
	if (flength > slength)
	{
		return false;
	}

	const char* end = str + slength - flength;
	const char* start = str;
	while (start <= end)
	{
		if (*start == *format || s_caseMap.map[*format] == *start)
		{
			if (strnicmp(start, format, flength))
			{
				return true;
			}
		}
		start++;
	}
	return false;
}
