#include "framework.h"
#include "Something.h"
#include "SomethingDlg.h"
#include "afxdialogex.h"
#include "BackstageData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 静态成员定义

CListCtrl CSomethingDlg::m_mainList;
CEdit CSomethingDlg::m_edit_nameInput;
CEdit CSomethingDlg::m_edit_suffInput;
CStatic CSomethingDlg::m_static_infoShow;

BYTE CSomethingDlg::m_mode_value = MODE_CONT;
BYTE CSomethingDlg::m_attr_value = ATTR_ALL;
BYTE CSomethingDlg::m_type_value = TYPE_ALL;
bool CSomethingDlg::m_case_value = false;
BYTE CSomethingDlg::m_root_vid = 0;
UINT CSomethingDlg::m_root_frn = 0;

std::mutex CSomethingDlg::m_mutex_info_visit;
std::mutex CSomethingDlg::m_mutex_data_visit;
std::atomic_bool CSomethingDlg::m_atomic_updata_run = false;
std::atomic_bool CSomethingDlg::m_atomic_data_inmem = false;

// ！静态成员定义

CSomethingDlg::CSomethingDlg(CWnd* pParent) : CDialogEx(IDD_SOMETHING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSomethingDlg::~CSomethingDlg()
{
	DataOut();
}

void CSomethingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAINLIST, m_mainList);
	DDX_Control(pDX, IDC_EDIT_NAMEINPUT, m_edit_nameInput);
	DDX_Control(pDX, IDC_EDIT_SUFFINPUT, m_edit_suffInput);
	DDX_Control(pDX, IDC_STATIC_INFOLABEL, m_static_infoShow);
	DDX_Control(pDX, IDC_STATIC_ROOTLABEL, m_static_rootShow);
}

BEGIN_MESSAGE_MAP(CSomethingDlg, CDialogEx)

	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_MAINLIST, &CSomethingDlg::OnLvnGetdispinfoMainlist)

	ON_COMMAND(ID_MODE_PRE, &CSomethingDlg::OnModePre)
	ON_COMMAND(ID_MODE_SIMI, &CSomethingDlg::OnModeSimi)
	ON_COMMAND(ID_MODE_CONT, &CSomethingDlg::OnModeCont)
	ON_UPDATE_COMMAND_UI(ID_MODE_PRE, &CSomethingDlg::OnUpdateModePre)
	ON_UPDATE_COMMAND_UI(ID_MODE_SIMI, &CSomethingDlg::OnUpdateModeSimi)
	ON_UPDATE_COMMAND_UI(ID_MODE_CONT, &CSomethingDlg::OnUpdateModeCont)
	ON_COMMAND(ID_ATTR_SYS, &CSomethingDlg::OnAttrSys)
	ON_COMMAND(ID_ATTR_HID, &CSomethingDlg::OnAttrHid)
	ON_COMMAND(ID_ATTR_RON, &CSomethingDlg::OnAttrRon)
	ON_COMMAND(ID_ATTR_NOR, &CSomethingDlg::OnAttrNor)
	ON_UPDATE_COMMAND_UI(ID_ATTR_SYS, &CSomethingDlg::OnUpdateAttrSys)
	ON_UPDATE_COMMAND_UI(ID_ATTR_RON, &CSomethingDlg::OnUpdateAttrRon)
	ON_UPDATE_COMMAND_UI(ID_ATTR_HID, &CSomethingDlg::OnUpdateAttrHid)
	ON_UPDATE_COMMAND_UI(ID_ATTR_NOR, &CSomethingDlg::OnUpdateAttrNor)
	ON_COMMAND(ID_TYPE_ALL, &CSomethingDlg::OnTypeAll)
	ON_COMMAND(ID_TYPE_DIR, &CSomethingDlg::OnTypeDir)
	ON_COMMAND(ID_TYPE_DAT, &CSomethingDlg::OnTypeDat)
	ON_COMMAND(ID_TYPE_VID, &CSomethingDlg::OnTypeVid)
	ON_COMMAND(ID_TYPE_AUD, &CSomethingDlg::OnTypeAud)
	ON_COMMAND(ID_TYPE_PIC, &CSomethingDlg::OnTypePic)
	ON_COMMAND(ID_TYPE_DOC, &CSomethingDlg::OnTypeDoc)
	ON_COMMAND(ID_TYPE_ZIP, &CSomethingDlg::OnTypeZip)
	ON_COMMAND(ID_TYPE_EXE, &CSomethingDlg::OnTypeExe)
	ON_COMMAND(ID_TYPE_OTH, &CSomethingDlg::OnTypeOth)
	ON_UPDATE_COMMAND_UI(ID_TYPE_ALL, &CSomethingDlg::OnUpdateTypeAll)
	ON_UPDATE_COMMAND_UI(ID_TYPE_DIR, &CSomethingDlg::OnUpdateTypeDir)
	ON_UPDATE_COMMAND_UI(ID_TYPE_DAT, &CSomethingDlg::OnUpdateTypeDat)
	ON_UPDATE_COMMAND_UI(ID_TYPE_VID, &CSomethingDlg::OnUpdateTypeVid)
	ON_UPDATE_COMMAND_UI(ID_TYPE_AUD, &CSomethingDlg::OnUpdateTypeAud)
	ON_UPDATE_COMMAND_UI(ID_TYPE_PIC, &CSomethingDlg::OnUpdateTypePic)
	ON_UPDATE_COMMAND_UI(ID_TYPE_DOC, &CSomethingDlg::OnUpdateTypeDoc)
	ON_UPDATE_COMMAND_UI(ID_TYPE_ZIP, &CSomethingDlg::OnUpdateTypeZip)
	ON_UPDATE_COMMAND_UI(ID_TYPE_EXE, &CSomethingDlg::OnUpdateTypeExe)
	ON_UPDATE_COMMAND_UI(ID_TYPE_OTH, &CSomethingDlg::OnUpdateTypeOth)
	ON_COMMAND(ID_CASE_IGNO, &CSomethingDlg::OnCaseIgno)
	ON_COMMAND(ID_CASE_DIST, &CSomethingDlg::OnCaseDist)
	ON_UPDATE_COMMAND_UI(ID_CASE_IGNO, &CSomethingDlg::OnUpdateCaseIgno)
	ON_UPDATE_COMMAND_UI(ID_CASE_DIST, &CSomethingDlg::OnUpdateCaseDist)
	ON_COMMAND(ID_CHOOSE_LOOK, &CSomethingDlg::OnChooseLook)
	ON_COMMAND(ID_CHOOSE_ALL, &CSomethingDlg::OnChooseAll)

	ON_NOTIFY(NM_DBLCLK, IDC_MAINLIST, &CSomethingDlg::OnNMDblclkMainlist)
	ON_NOTIFY(NM_RCLICK, IDC_MAINLIST, &CSomethingDlg::OnNMRClickMainlist)
	ON_EN_CHANGE(IDC_EDIT_NAMEINPUT, &CSomethingDlg::OnEnChangeEditNameinput)
	ON_EN_CHANGE(IDC_EDIT_SUFFINPUT, &CSomethingDlg::OnEnChangeEditSuffinput)

	ON_COMMAND(ID_SHOWATTR, &CSomethingDlg::OnShowattr)
	ON_COMMAND(ID_COPYPATH, &CSomethingDlg::OnCopypath)
	ON_COMMAND(ID_OPENFILE, &CSomethingDlg::OnOpenfile)
	ON_COMMAND(ID_OPENPATH, &CSomethingDlg::OnOpenpath)
	ON_COMMAND(ID_COPYFILETO, &CSomethingDlg::OnCopyfileto)
	ON_COMMAND(ID_MOVEFILETO, &CSomethingDlg::OnMovefileto)
	ON_COMMAND(ID_DELETEFILE, &CSomethingDlg::OnDeletefile)
END_MESSAGE_MAP()

BOOL CSomethingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 读取数据
	std::thread t1(DataIn);
	t1.detach();

	// 后台更新
	std::thread t2(DataUpdata);
	t2.detach();

	// 设置图标
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// TODO: 在此添加额外的初始化代码

	// 导入主菜单
	m_mainMenu.LoadMenu(IDR_MAINMENU);
	SetMenu(&m_mainMenu);

	// 创建图标列表
	CreateIconList();

	// 初始化主列表
	CreateMainList();

	// 除非将焦点设置到控件，否则返回 TRUE
	return TRUE;
}

// 如果向对话框添加最小化按钮，则需要下面的代码来绘制该图标。
// 对于使用文档/视图模型的 MFC 应用程序，这将由框架自动完成。

void CSomethingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CSomethingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CSomethingDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) // 回车
		{
			Search();
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CSomethingDlg::CreateIconList()
{
	m_iconList.Create(22, 22, ILC_COLOR32, 16, 16);
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_OTH));
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_AUD));
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_VID));
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_PIC));
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_DOC));
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_ZIP));
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_EXE));
	m_iconList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TYPE_DIR));
}

void CSomethingDlg::CreateMainList()
{
	m_mainList.InsertColumn(0, L"名称", LVCFMT_LEFT, 240);
	m_mainList.InsertColumn(1, L"路径", LVCFMT_LEFT, 760);
	m_mainList.SetImageList(&m_iconList, LVSIL_SMALL);
	m_mainList.SetExtendedStyle(m_mainList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
}

void CSomethingDlg::DataIn()
{
	// 锁
	std::lock_guard<std::mutex> guard1(m_mutex_info_visit);
	std::lock_guard<std::mutex> guard2(m_mutex_data_visit);

	// 读取
	if (!GlobalData::read())
	{
		m_static_infoShow.SetWindowTextW(L"数据非法或过期，正在重建...");

		// 创建
		GlobalData::create();
	}

	// 更新
	GlobalData::updata();

	// 启动更新线程
	m_atomic_updata_run = true;
	m_atomic_data_inmem = true;

	// 首次搜索
	FirstSearch();
}

void CSomethingDlg::DataUpdata()
{
	// 等待数据读入
	while (!m_atomic_data_inmem);

	// 允许更新时更新
	while (m_atomic_updata_run)
	{
		m_mutex_data_visit.lock();

		GlobalData::updata();

		m_mutex_data_visit.unlock();

		// 每隔1秒更新一次
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void CSomethingDlg::DataOut()
{
	std::lock_guard<std::mutex> guard(m_mutex_data_visit);

	// 写出
	GlobalData::save();

	// 终止更新线程
	m_atomic_updata_run = false;
}

void CSomethingDlg::Search()
{
	// 数据未读入
	if (!m_atomic_data_inmem) return;

	// 锁
	std::lock_guard<std::mutex> guard1(m_mutex_info_visit);
	std::lock_guard<std::mutex> guard2(m_mutex_data_visit);

	FirstSearch();
}

void CSomethingDlg::FirstSearch()
{
	// 新建查询
	CString buffer;
	m_edit_nameInput.GetWindowTextW(buffer);
	std::string _name = CW2A(buffer.GetBuffer());
	m_edit_suffInput.GetWindowTextW(buffer);
	std::string _suff = CW2A(buffer.GetBuffer());
	QUERY* pQuery = new QUERY(m_type_value, m_attr_value, m_case_value ? m_mode_value | MODE_CASE : m_mode_value,
		m_root_vid, m_root_frn, _name, _suff);

	// 搜索
	GlobalData::search(pQuery);

	// 释放查询
	delete pQuery;

	// 设置VirtualList的ItemCount
	m_mainList.SetItemCount(GlobalData::g_resultTable.size());

	// 显示结果
	m_static_infoShow.SetWindowTextW((std::to_wstring(GlobalData::g_resultTable.size()) + L"个结果...").c_str());
}

void CSomethingDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	ASSERT(pPopupMenu != NULL);

	// Check the enabled state of various menu items.
	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu; // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child Windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue; // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE); // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CSomethingDlg::OnLvnGetdispinfoMainlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM* pItem = &(pDispInfo)->item;
	int index = pItem->iItem;

	if (pItem->mask & LVIF_TEXT)
	{
		ANS_NODE& ansNode = GlobalData::g_resultTable.at(index);
		CString wname(GlobalData::m_volumeTable.getName(ansNode.info));
		CString wpath(GlobalData::m_volumeTable.getPath(ansNode.info));

		if (pItem->iSubItem == 0)
		{
			pItem->iImage = ansNode.iconId;
			lstrcpy(pItem->pszText, wname.GetBuffer());
		}
		else
		{
			lstrcpy(pItem->pszText, wpath.GetBuffer());
		}
	}
	*pResult = 0;
}

BOOL CSomethingDlg::ShowFileProperties(CString& strPath)
{
	SHELLEXECUTEINFO info = { 0 };
	info.lpFile = strPath;
	info.hwnd = GetSafeHwnd();
	info.nShow = SW_SHOWNORMAL;
	info.lpVerb = L"properties";
	info.fMask = SEE_MASK_INVOKEIDLIST;
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	return ShellExecuteExW(&info);
}

// 菜单项响应函数

void CSomethingDlg::OnModePre()
{
	m_mode_value = MODE_PREC;
	Search();
}

void CSomethingDlg::OnModeSimi()
{
	m_mode_value = MODE_SIMI;
	Search();
}

void CSomethingDlg::OnModeCont()
{
	m_mode_value = MODE_CONT;
	Search();
}

void CSomethingDlg::OnUpdateModePre(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_mode_value == MODE_PREC);
}

void CSomethingDlg::OnUpdateModeSimi(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_mode_value == MODE_SIMI);
}

void CSomethingDlg::OnUpdateModeCont(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_mode_value == MODE_CONT);
}

void CSomethingDlg::OnAttrSys()
{
	if (m_attr_value == ATTR_SYS)
	{
		MessageBox(L"至少选择一项！", L"提示");
	}
	else if (m_attr_value & ATTR_SYS)
	{
		m_attr_value &= (~ATTR_SYS);
	}
	else
	{
		m_attr_value |= ATTR_SYS;
	}
	Search();
}

void CSomethingDlg::OnAttrHid()
{
	if (m_attr_value == ATTR_HID)
	{
		MessageBox(L"至少选择一项！", L"提示");
	}
	else if (m_attr_value & ATTR_HID)
	{
		m_attr_value &= (~ATTR_HID);
	}
	else
	{
		m_attr_value |= ATTR_HID;
	}
	Search();
}

void CSomethingDlg::OnAttrRon()
{
	if (m_attr_value == ATTR_RON)
	{
		MessageBox(L"至少选择一项！", L"提示");
	}
	else if (m_attr_value & ATTR_RON)
	{
		m_attr_value &= (~ATTR_RON);
	}
	else
	{
		m_attr_value |= ATTR_RON;
	}
	Search();
}

void CSomethingDlg::OnAttrNor()
{
	if (m_attr_value == ATTR_NOR)
	{
		MessageBox(L"至少选择一项！", L"提示");
	}
	else if (m_attr_value & ATTR_NOR)
	{
		m_attr_value &= (~ATTR_NOR);
	}
	else
	{
		m_attr_value |= ATTR_NOR;
	}
	Search();
}

void CSomethingDlg::OnUpdateAttrSys(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_attr_value & ATTR_SYS);
}

void CSomethingDlg::OnUpdateAttrRon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_attr_value & ATTR_RON);
}

void CSomethingDlg::OnUpdateAttrHid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_attr_value & ATTR_HID);
}

void CSomethingDlg::OnUpdateAttrNor(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_attr_value & ATTR_NOR);
}

void CSomethingDlg::OnTypeAll()
{
	m_type_value = TYPE_ALL;
	Search();
}

void CSomethingDlg::OnTypeDir()
{
	m_type_value = TYPE_DIR;
	Search();
}

void CSomethingDlg::OnTypeDat()
{
	m_type_value = TYPE_DAT;
	Search();
}

void CSomethingDlg::OnTypeVid()
{
	m_type_value = TYPE_VID;
	Search();
}

void CSomethingDlg::OnTypeAud()
{
	m_type_value = TYPE_AUD;
	Search();
}

void CSomethingDlg::OnTypePic()
{
	m_type_value = TYPE_PIC;
	Search();
}

void CSomethingDlg::OnTypeDoc()
{
	m_type_value = TYPE_DOC;
	Search();
}

void CSomethingDlg::OnTypeZip()
{
	m_type_value = TYPE_ZIP;
	Search();
}

void CSomethingDlg::OnTypeExe()
{
	m_type_value = TYPE_EXE;
	Search();
}

void CSomethingDlg::OnTypeOth()
{
	m_type_value = TYPE_OTH;
	Search();
}

void CSomethingDlg::OnUpdateTypeAll(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_ALL);
}

void CSomethingDlg::OnUpdateTypeDir(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_DIR);
}

void CSomethingDlg::OnUpdateTypeDat(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_DAT);
}

void CSomethingDlg::OnUpdateTypeVid(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_VID);
}

void CSomethingDlg::OnUpdateTypeAud(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_AUD);
}

void CSomethingDlg::OnUpdateTypePic(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_PIC);
}

void CSomethingDlg::OnUpdateTypeDoc(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_DOC);
}

void CSomethingDlg::OnUpdateTypeZip(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_ZIP);
}

void CSomethingDlg::OnUpdateTypeExe(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_EXE);
}

void CSomethingDlg::OnUpdateTypeOth(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_type_value == TYPE_OTH);
}

void CSomethingDlg::OnCaseIgno()
{
	m_case_value = false;
	Search();
}

void CSomethingDlg::OnCaseDist()
{
	m_case_value = true;
	Search();
}

void CSomethingDlg::OnUpdateCaseIgno(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(!m_case_value);
}

void CSomethingDlg::OnUpdateCaseDist(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_case_value);
}

void CSomethingDlg::OnChooseLook()
{
	TCHAR szFolderPath[MAX_PATH] = { 0 };
	BROWSEINFO sInfo = { 0 };
	LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		if (SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			m_root_vid = (BYTE)*szFolderPath;
			m_root_frn = getFileReferenceNumber(szFolderPath);
			CString strRootShow(L"Root : ");
			strRootShow.Append(szFolderPath);
			m_static_rootShow.SetWindowTextW(strRootShow.GetBuffer());
			Search();
			return;
		}
		CoTaskMemFree(lpidlBrowse);
	}
	m_root_vid = 0;
	m_root_frn = 0;
	m_static_rootShow.SetWindowTextW(L"Root : 全盘");
	Search();
}

void CSomethingDlg::OnChooseAll()
{
	m_root_vid = 0;
	m_root_frn = 0;
	m_static_rootShow.SetWindowTextW(L"Root : 全盘");
	Search();
}

// ！菜单项响应函数

void CSomethingDlg::OnNMDblclkMainlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int raw = pNMItemActivate->iItem;
	if (raw < 0 || raw >= m_mainList.GetItemCount()) return;

	CString strPath = m_mainList.GetItemText(raw, 1) + m_mainList.GetItemText(raw, 0);
	ShellExecute(NULL, L"open", strPath, NULL, NULL, SW_SHOWNORMAL);

	*pResult = 0;
}

void CSomethingDlg::OnNMRClickMainlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int raw = pNMItemActivate->iItem;
	if (raw < 0 || raw >= m_mainList.GetItemCount()) return;

	CPoint pt = { 0 };
	GetCursorPos(&pt);

	CMenu menu, * pop = NULL;
	menu.LoadMenu(IDR_RMENU);
	pop = menu.GetSubMenu(0);
	pop->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}

void CSomethingDlg::OnEnChangeEditNameinput()
{
	Search();
}

void CSomethingDlg::OnEnChangeEditSuffinput()
{
	Search();
}

void CSomethingDlg::OnShowattr()
{
	int raw = m_mainList.GetSelectionMark();
	if (raw < 0 || raw >= m_mainList.GetItemCount()) return;

	CString strPath = m_mainList.GetItemText(raw, 1) + m_mainList.GetItemText(raw, 0);
	ShowFileProperties(strPath);
}

void CSomethingDlg::OnCopypath()
{
	int raw = m_mainList.GetSelectionMark();
	if (raw < 0 || raw >= m_mainList.GetItemCount()) return;

	char szPath[MAX_PATH] = { 0 };
	CString strPath = m_mainList.GetItemText(raw, 1);
	strPath.Append(m_mainList.GetItemText(raw, 0));
	UINT pathLength = SHUnicodeToAnsi(strPath.GetBuffer(), szPath, MAX_PATH);

	HGLOBAL hClipBuffer = GlobalAlloc(GHND, pathLength);
	if (hClipBuffer)
	{
		void* buffer = GlobalLock(hClipBuffer);
		if (buffer)
		{
			memcpy(buffer, szPath, pathLength);
		}
		GlobalUnlock(hClipBuffer);
		OpenClipboard();
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hClipBuffer);
		CloseClipboard();
	}
}

void CSomethingDlg::OnOpenfile()
{
	int raw = m_mainList.GetSelectionMark();
	if (raw < 0 || raw >= m_mainList.GetItemCount()) return;

	CString strPath = m_mainList.GetItemText(raw, 1) + m_mainList.GetItemText(raw, 0);
	ShellExecute(NULL, L"open", strPath, NULL, NULL, SW_SHOWNORMAL);
}

void CSomethingDlg::OnOpenpath()
{
	// TODO: 在此添加命令处理程序代码
	int raw = m_mainList.GetSelectionMark();
	if (raw < 0 || raw >= m_mainList.GetItemCount()) return;

	CString strPath = L"/select," + m_mainList.GetItemText(raw, 1) + m_mainList.GetItemText(raw, 0);
	ShellExecute(NULL, L"open", L"Explorer.exe", strPath, NULL, SW_SHOWNORMAL);
}


void CSomethingDlg::OnCopyfileto()
{
	// TODO: 在此添加命令处理程序代码
}


void CSomethingDlg::OnMovefileto()
{
	// TODO: 在此添加命令处理程序代码
}


void CSomethingDlg::OnDeletefile()
{
	// TODO: 在此添加命令处理程序代码
}
