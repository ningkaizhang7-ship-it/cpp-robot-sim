
// Demo JogDlg.h: 头文件
//

#pragma once


// CDemoJogDlg 对话框
class CDemoJogDlg : public CDialogEx
{
// 构造
public:
	short getAxis();
	void JogMotion(double direction);
	void axisHomeMotion(short gAxis);
	CDemoJogDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEMO_JOG_DIALOG };
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
	
	afx_msg void init();
	afx_msg void servoenble();
	afx_msg void staticclr();
	afx_msg void zeropos();
	afx_msg void illeg();
	BOOL CDemoJogDlg::PreTranslateMessage(MSG* pMsg);
	afx_msg void gohome();
};
