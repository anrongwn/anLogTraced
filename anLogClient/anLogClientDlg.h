
// anLogClientDlg.h : 头文件
//

#pragma once
#include <memory>
#include <thread>
#include <string>
#include <sstream>
#include "anIPCClient.h"

// CanLogClientDlg 对话框
class CanLogClientDlg : public CDialogEx
{
// 构造
public:
	CanLogClientDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CanLogClientDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANLOGCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CString strLogName_;

private:
	std::unique_ptr<anIPCClient> ipc_;
};
