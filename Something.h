#pragma once

#include "resource.h"

class CSomethingApp : public CWinApp {
public:
	CSomethingApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
	DECLARE_MESSAGE_MAP()
};

extern CSomethingApp theApp;
