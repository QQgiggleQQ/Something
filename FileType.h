#pragma once

#include "StringHashMap.h"

#define TYPE_AUD 0B00000001 // 音频文件
#define TYPE_VID 0B00000010 // 视频文件
#define TYPE_PIC 0B00000100 // 图片文件
#define TYPE_DOC 0B00001000 // 文档文件
#define TYPE_ZIP 0B00010000 // 压缩文件
#define TYPE_EXE 0B00100000 // 执行文件
#define TYPE_OTH 0B01000000 // 其他文件
#define TYPE_DIR 0B10000000 // 目录文件
#define TYPE_DAT 0B01111111 // 数据文件
#define TYPE_ALL 0B11111111 // 全部文件

// 扩展名 - 类型映射，暂时不可动态修改，查找成功返回对应类型，查找失败返回其他类型。
class CSuffixTypeMap : public TStringHashMap<unsigned char> {
public:
	CSuffixTypeMap() : TStringHashMap<unsigned char>(512, TYPE_OTH)
	{
		const char* aud[33] = { "aac", "ac3", "aif", "aifc", "aiff", "au", "cda", "dts", "fla", "flac", "it", "m1a", "m2a", "m3u", "m4a",
		"mid", "midi", "mka", "mod", "mp2", "mp3", "mpa", "ogg", "ra", "rmi", "spc", "rmi", "snd", "umx", "voc", "wav", "wma", "xm" };

		const char* vid[70] = { "3g2", "3gp", "3gp2", "3gpp", "amr", "amv", "asf", "avi", "bdmv", "bik", "d2v", "divx", "drc", "dsa",
		"dsm", "dss", "dsv", "evo", "f4v", "flc", "fli", "flic", "flv", "hdmov", "ifo", "ivf", "m1v", "m2p", "m2t", "m2ts", "m2v", "m4b",
		"m4p", "m4v", "mkv", "mp2v", "mp4", "mp4v", "mpe", "mpeg", "mpg", "mpls", "mpv2", "mpv4", "mov", "mts", "ogm", "ogv", "pss", "pva",
		"qt", "ram", "ratdvd", "rm", "rmm", "rmvb", "roq", "rpm", "smil", "smk", "swf", "tp", "tpr", "ts", "vob", "vp6", "webm", "wm",
		"wmp", "wmv" };

		const char* pho[15] = { "ani", "bmp", "gif", "ico", "jpe", "jpeg", "jpg", "pcx", "png", "psd", "tga", "tif", "tiff", "webp","wmf" };

		const char* doc[54] = { "c", "chm", "cpp", "csv", "cxx", "dat", "db", "doc", "docm", "docx", "dot", "dotm", "dotx", "h", "hpp", "htm", "html",
		"hxx", "ini", "java", "js", "json", "log", "lua", "mht", "mhtml", "odt", "pdf", "potx", "potm", "ppam", "ppsm", "ppsx", "pps", "ppt", "pptm", "pptx",
		"rtf", "sldm", "sldx", "thmx", "txt", "vsd", "wpd", "wps", "wri", "xlam", "xls", "xlsb", "xlsm", "xlsx", "xltm", "xltx", "xml" };

		const char* zip[43] = { "7z", "ace", "arj", "bz2", "cab", "gz", "gzip", "jar", "r00", "r01", "r02", "r03", "r04", "r05",
		"r06", "r07", "r08", "r09", "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
		"r24", "r25", "r26", "r27", "r28", "r29", "rar", "tar", "tgz", "z", "zip" };

		const char* exe[6] = { "bat", "cmd", "exe", "msi", "msp", "scr" };

		for (int i = 0; i < 33; ++i) insert(aud[i], TYPE_AUD);
		for (int i = 0; i < 70; ++i) insert(vid[i], TYPE_VID);
		for (int i = 0; i < 15; ++i) insert(pho[i], TYPE_PIC);
		for (int i = 0; i < 49; ++i) insert(doc[i], TYPE_DOC);
		for (int i = 0; i < 43; ++i) insert(zip[i], TYPE_ZIP);
		for (int i = 0; i < 06; ++i) insert(exe[i], TYPE_EXE);
	}
};

// 类型 - 图标编号映射，根据类型返回对应图标在 ImageList 中的编号。
class CTypeIconIndexMap {
public:
	CTypeIconIndexMap()
	{
		memset(m_map, 0, sizeof(m_map));
		m_map[TYPE_OTH] = 0;
		m_map[TYPE_AUD] = 1;
		m_map[TYPE_VID] = 2;
		m_map[TYPE_PIC] = 3;
		m_map[TYPE_DOC] = 4;
		m_map[TYPE_ZIP] = 5;
		m_map[TYPE_EXE] = 6;
		m_map[TYPE_DIR] = 7;
	}

	// 对于其他类型和无效类型返回 0 ，即其他类型图标编号。
	unsigned char operator[](unsigned char type)
	{
		return m_map[type];
	}

private:
	unsigned char m_map[256];
};
