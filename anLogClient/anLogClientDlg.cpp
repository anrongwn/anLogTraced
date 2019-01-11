
// anLogClientDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CanLogClientDlg �Ի���



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


// CanLogClientDlg ��Ϣ�������

BOOL CanLogClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CanLogClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CanLogClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CanLogClientDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CanLogClientDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	//char * p = new char[10];
	int r = 0;
	if (ipc_) {
		std::string  logname(strLogName_.operator LPCSTR());
		r = ipc_->start(logname);

		r = ipc_->connect();
	}
}

//������
using u_len = union {
	char a[sizeof(size_t)];
	size_t x;
};
void CanLogClientDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

				//�õ���ʱ��
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
