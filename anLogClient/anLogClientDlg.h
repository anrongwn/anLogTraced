
// anLogClientDlg.h : ͷ�ļ�
//

#pragma once
#include <memory>
#include <thread>
#include <string>
#include <sstream>
#include "anIPCClient.h"

// CanLogClientDlg �Ի���
class CanLogClientDlg : public CDialogEx
{
// ����
public:
	CanLogClientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CanLogClientDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANLOGCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
