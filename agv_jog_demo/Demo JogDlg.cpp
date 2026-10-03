
// Demo JogDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Demo Jog.h"
#include "Demo JogDlg.h"
#include "afxdialogex.h"

#include"gts.h"
#pragma comment(lib,"gts.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
short CDemoJogDlg::getAxis() {
	CString strVal;

	GetDlgItemText(IDC_EDIT_axis, strVal);
	short axis = _ttoi(strVal);

	return axis;
}

void CDemoJogDlg::JogMotion(double direction) {
	short sRtn;
	CString strVal;
	//返回值变量
	//定义一个CString类型的变量，用来存放从界面编
	TJogPrm jog;
	//辑框中获取的数据
	//定义一个结构体变量，用来存放Jog运动模式的运
	//动参数
	short axis = getAxis();
	sRtn = GT_ZeroPos(axis);
	sRtn = GT_PrfJog(axis);
	sRtn = GT_GetJogPrm(axis, &jog);

	GetDlgItemText(IDC_EDIT_acc, strVal);//获取界面输入的加速度
	jog.acc = _ttof(strVal);
	GetDlgItemText(IDC_EDIT_dec, strVal);//获取界面输入的减速度
	jog.dec = _ttof(strVal);
	GetDlgItemText(IDC_EDIT_smooth, strVal);//获取界面输人的平滑系数
	jog.smooth = _ttof(strVal);
	sRtn = GT_SetJogPrm(axis, &jog);
	GetDlgItemText(IDC_EDIT_speed, strVal);
	double Vel = _ttof(strVal) * direction;
	sRtn = GT_SetVel(axis, Vel);
	sRtn = GT_Update(1 << (axis - 1));
}

void CDemoJogDlg::axisHomeMotion(short gAxis) {
	short sRtn;
	short dir = 1;
	THomeStatus tHomests;
	sRtn = GT_AxisOn(gAxis);
	sRtn = GT_ZeroPos(gAxis);

	THomePrm tHomePrm;
	sRtn = GT_GetHomePrm(gAxis,&tHomePrm);
	if (gAxis > 0 && gAxis <= 3) {
		if (gAxis == 3) {
			dir = -1;
		}
	
		tHomePrm.mode = 10;
		tHomePrm.moveDir = -1 * dir;
		tHomePrm.edge = 0;
		tHomePrm.pad1[0] = 0;
		tHomePrm.velHigh = 30;
		tHomePrm.velLow = 20;
		tHomePrm.acc = 0.25;
		tHomePrm.dec = 0.25;
		tHomePrm.smoothTime = 25;
		tHomePrm.pad2[0] = 1;
		tHomePrm.pad2[1] = 1;
		tHomePrm.pad2[2] = 1;
		tHomePrm.homeOffset = -10000 * dir;
		tHomePrm.escapeStep = 2000;
		sRtn = GT_GoHome(gAxis, &tHomePrm);
	}
	if (gAxis == 4) {
		tHomePrm.mode = 10;
		tHomePrm.moveDir = 1;
		tHomePrm.edge = 0;
		tHomePrm.pad1[0] = 0;
		tHomePrm.velHigh = 2;
		tHomePrm.velLow = 1;
		tHomePrm.acc = 0.1;
		tHomePrm.dec = 0.1;
		tHomePrm.smoothTime =10;
		tHomePrm.pad2[0] = 1;
		tHomePrm.pad2[1] = 1;
		tHomePrm.pad2[2] = 1;
		tHomePrm.homeOffset = -2000 * dir;
		tHomePrm.escapeStep = 500;
		sRtn = GT_GoHome(gAxis, &tHomePrm);
	}
	do {
		sRtn = GT_GetHomeStatus(gAxis, &tHomests);
	} while (tHomests.run);
	sRtn = GT_ZeroPos(gAxis);
	sRtn = GT_ClrSts(1, 4);
}
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


// CDemoJogDlg 对话框



CDemoJogDlg::CDemoJogDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEMO_JOG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDemoJogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDemoJogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ini, &CDemoJogDlg::init)
	ON_BN_CLICKED(servo_enble, &CDemoJogDlg::servoenble)
	ON_BN_CLICKED(tsclr, &CDemoJogDlg::staticclr)
	ON_BN_CLICKED(zero_pos, &CDemoJogDlg::zeropos)

	ON_BN_CLICKED(servo_disenlble, &CDemoJogDlg::illeg)
	ON_BN_CLICKED(gohome, &CDemoJogDlg::gohome)
END_MESSAGE_MAP()


// CDemoJogDlg 消息处理程序

BOOL CDemoJogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

void CDemoJogDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDemoJogDlg::OnPaint()
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
HCURSOR CDemoJogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDemoJogDlg::init()
{
	// TODO: 在此添加控件通知处理程序代码
	short sRun;
	sRun = GT_Open();
	sRun = GT_Reset();
	sRun = GT_LoadConfig("gts800.cfg");
	sRun = GT_ClrSts(1, 4);
}




void CDemoJogDlg::servoenble()
{
	// TODO: 在此添加控件通知处理程序代码
	short sRtn;
	short axis = getAxis();
	sRtn = GT_AxisOn(axis);

}




void CDemoJogDlg::staticclr()
{
	// TODO: 在此添加控件通知处理程序代码
	short sRun;
	sRun = GT_ClrSts(1, 4);
}


void CDemoJogDlg::zeropos()
{
	// TODO: 在此添加控件通知处理程序代码
	short sRun;
	short axis = getAxis();
	sRun = GT_ZeroPos(axis);
}

void CDemoJogDlg::illeg()
{
	// TODO: 在此添加控件通知处理程序代码
	short sRTn;
	short axis = getAxis();
	sRTn = GT_Stop(1 << (axis - 1), 1 << (axis - 1));
	sRTn = GT_AxisOff(axis);

}



BOOL CDemoJogDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_BUTTON_Neg)->m_hWnd)
		{
			JogMotion(-1);

		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BUTTON_Pos)->m_hWnd)
		{
			JogMotion(1);
		}
	}
	else if (pMsg->message == WM_LBUTTONUP)
	{
		if ((pMsg->hwnd == GetDlgItem(IDC_BUTTON_Neg)->m_hWnd) ||
			(pMsg->hwnd == GetDlgItem(IDC_BUTTON_Pos)->m_hWnd))
		{
			short sRTn;
			short axis = getAxis();
			sRTn = GT_Stop(1 << (axis - 1), 1 << (axis - 1));
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}



void CDemoJogDlg::gohome()
{
	// TODO: 在此添加控件通知处理程序代码
	short axis = getAxis();
	axisHomeMotion(axis);
}
