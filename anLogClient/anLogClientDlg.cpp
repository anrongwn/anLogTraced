
// anLogClientDlg.cpp : 实现文件
//
#include <vld.h>
#include "stdafx.h"
#include "anLogClient.h"
#include "anLogClientDlg.h"
#include "afxdialogex.h"
#include <random>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CanLogClientDlg 对话框



CanLogClientDlg::CanLogClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ANLOGCLIENT_DIALOG, pParent)
	, strLogName_(_T("anSPTrace"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	ipc_ = std::make_unique<anIPCClient>();
}

void CanLogClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, strLogName_);
}

BEGIN_MESSAGE_MAP(CanLogClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CanLogClientDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CanLogClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CanLogClientDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CanLogClientDlg 消息处理程序

BOOL CanLogClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
CanLogClientDlg::~CanLogClientDlg() {
	if (ipc_) {
		ipc_->stop();
	}

	//anlog::close("anLogClient");
	//g_log->flush();
}
void CanLogClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CanLogClientDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CanLogClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CanLogClientDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CanLogClientDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	//char * p = new char[10];
	int r = 0;
	if (ipc_) {
		std::string  logname(strLogName_.operator LPCSTR());
		r = ipc_->start(logname);

		r = ipc_->connect();
	}
}

//包长度
using u_len = union {
	char a[sizeof(size_t)];
	size_t x;
};
void CanLogClientDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!ipc_) return;

	static std::thread writer;
	static std::atomic_bool stop = { ATOMIC_FLAG_INIT };

	if (writer.joinable()) {
		std::atomic_exchange(&stop, true);

		writer.join();

		g_log->debug("-----writer notify exit{}", 1);
		this->GetDlgItem(IDC_BUTTON2)->SetWindowTextA("Write");
	}
	else {
		std::atomic_exchange(&stop, false);

		writer = std::thread([&](anIPCClient * ipc) {
			int r = 0;
			
			std::default_random_engine re;
			std::default_random_engine re2;
			std::uniform_int_distribution<unsigned> u(0, 5);
			char level = 0x31;
			while (!stop) {
				SYSTEMTIME st = { 0x00 };
				char date_tmp[24] = { 0x00 };

				//得到当时间
				::GetLocalTime(&st);
				sprintf_s(date_tmp, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, \
					st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
				
				level = 0x30 + u(re);
				std::vector<char> v;
				//v.push_back(level);
				v.push_back('*');
				v.insert(v.end(), date_tmp, date_tmp + 23);
				v.push_back('*');

				std::string id = std::to_string(re2());
				v.insert(v.end(), id.begin(), id.end());

				r = ipc->write(level, v.data(), v.size());

				if (r) {
					break;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			g_log->debug("-----writer thread exit{}", 1);

		}, this->ipc_.get());

		this->GetDlgItem(IDC_BUTTON2)->SetWindowTextA("Stop");
	}
}
