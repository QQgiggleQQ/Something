#pragma once

#pragma warning(disable : 26454)

#include <thread>
#include <mutex>
#include <atomic>

class CSomethingDlg : public CDialogEx {
public:
	CSomethingDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~CSomethingDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SOMETHING_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 支持

protected:
	HICON m_hIcon;
	CMenu m_mainMenu;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

private:
	
	CImageList m_iconList; // 图标列表
	CStatic m_static_rootShow; // 搜索根目录展示框

	// 创建图标列表
	void CreateIconList();

	// 创建初始化主列表
	void CreateMainList();

	// 数据访问控制
	static BYTE m_mode_value;
	static BYTE m_attr_value;
	static BYTE m_type_value;
	static bool m_case_value;
	static BYTE m_root_vid;
	static UINT m_root_frn;

	static std::mutex m_mutex_info_visit;
	static std::mutex m_mutex_data_visit;
	static std::atomic_bool m_atomic_data_inmem;
	static std::atomic_bool m_atomic_updata_run;

	// 数据访问函数
	static void DataIn();
	static void DataUpdata();
	static void DataOut();
	static void Search();
	static void FirstSearch();

	// 需在静态线程函数中使用到
	static CListCtrl m_mainList; // 主列表
	static CEdit m_edit_nameInput; // 文件名输入框
	static CEdit m_edit_suffInput; // 扩展名输入框
	static CStatic m_static_infoShow; // 各类信息展示框

private:
	// 显示属性窗口
	BOOL ShowFileProperties(CString& strPath);

public:
	// 主菜单项响应函数
	afx_msg void OnModePre();
	afx_msg void OnModeSimi();
	afx_msg void OnModeCont();
	afx_msg void OnUpdateModePre(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModeSimi(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModeCont(CCmdUI* pCmdUI);
	afx_msg void OnAttrSys();
	afx_msg void OnAttrHid();
	afx_msg void OnAttrRon();
	afx_msg void OnAttrNor();
	afx_msg void OnUpdateAttrSys(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAttrRon(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAttrHid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAttrNor(CCmdUI* pCmdUI);
	afx_msg void OnTypeAll();
	afx_msg void OnTypeDir();
	afx_msg void OnTypeDat();
	afx_msg void OnTypeVid();
	afx_msg void OnTypeAud();
	afx_msg void OnTypePic();
	afx_msg void OnTypeDoc();
	afx_msg void OnTypeZip();
	afx_msg void OnTypeExe();
	afx_msg void OnTypeOth();
	afx_msg void OnUpdateTypeAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeDir(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeDat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeVid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeAud(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypePic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeDoc(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeZip(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeExe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTypeOth(CCmdUI* pCmdUI);
	afx_msg void OnCaseIgno();
	afx_msg void OnCaseDist();
	afx_msg void OnUpdateCaseIgno(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCaseDist(CCmdUI* pCmdUI);
	afx_msg void OnChooseLook();
	afx_msg void OnChooseAll();
	// ！主菜单项相应函数

	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLvnGetdispinfoMainlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkMainlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickMainlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEditNameinput();
	afx_msg void OnEnChangeEditSuffinput();
	afx_msg void OnShowattr();
	afx_msg void OnCopypath();
	afx_msg void OnOpenfile();
	afx_msg void OnOpenpath();
	afx_msg void OnCopyfileto();
	afx_msg void OnMovefileto();
	afx_msg void OnDeletefile();
};
