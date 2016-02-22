// Copyright(c) 2016 Andrew L. Sandoval
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// CO2WaterFlowDlg.cpp : implementation file
//
#include "stdafx.h"
#include "CO2WaterFlow.h"
#include "CO2WaterFlowDlg.h"
#include "afxdialogex.h"
#include <fileapi.h>
#include <array>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CCO2WaterFlowDlg dialog

void CCO2WaterFlowDlg::BuildCOMPortMap()
{
	m_COMPortMap.clear();
	for(UINT uiIndex = 0; uiIndex < 255; ++uiIndex)
	{
		std::array<wchar_t, 255> wzPort;
		swprintf_s(&wzPort[0], wzPort.size(), L"COM%u", uiIndex);

		std::array<wchar_t, 255> wzName{ 0 };
		DWORD dw = QueryDosDeviceW(&wzPort[0], &wzName[0], static_cast<DWORD>(wzName.size()));

		if(dw == 0)
		{
			continue;
		}
		m_COMPortMap[&wzPort[0]] = &wzName[0];
	}
}

CCO2WaterFlowDlg::CCO2WaterFlowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CO2WATERFLOW_DIALOG, pParent),
	m_hIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME))
{
	// m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	BuildCOMPortMap();
}

void CCO2WaterFlowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_COMDropDown);
}

BEGIN_MESSAGE_MAP(CCO2WaterFlowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CCO2WaterFlowDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SCANPORTS, &CCO2WaterFlowDlg::OnBnClickedButtonScanports)
	ON_BN_CLICKED(IDC_BUTTON_SETTHRESHOLD, &CCO2WaterFlowDlg::OnBnClickedButtonSetthreshold)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCO2WaterFlowDlg message handlers

BOOL CCO2WaterFlowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	while(m_COMPortMap.empty())
	{
		UINT option = MessageBox(L"No COM Ports available.  Attach Water Flow Monitor and try again!", L"ERROR", MB_TOPMOST|MB_RETRYCANCEL);
		if(IDRETRY != option)
		{
			EndDialog(0);
			return FALSE;
		}
		BuildCOMPortMap();
	}
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCANPORTS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SETTHRESHOLD)->EnableWindow(FALSE);

	//
	// Fill in port list:
	RefreshCOMList();
	SetDlgItemInt(IDC_EDIT_NEWTHRESHOLD, 45, FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCO2WaterFlowDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCO2WaterFlowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCO2WaterFlowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCO2WaterFlowDlg::OnBnClickedButtonOpen()
{
	int sel = m_COMDropDown.GetCurSel();
	std::wstring wstrFile(m_COMSelectionMap[sel]);
	if(false == m_SerialPort.Open(wstrFile))
	{
		GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SCANPORTS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SETTHRESHOLD)->EnableWindow(FALSE);
		return;
	}
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SCANPORTS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SETTHRESHOLD)->EnableWindow(TRUE);
	//
	// Set a timer to read data
	SetTimer(1, 125, nullptr);
}

void CCO2WaterFlowDlg::OnTimer(UINT_PTR uiTimer)
{
	CDialog::OnTimer(uiTimer);
	std::string strMessage;
	bool bMessage = m_SerialPort.ReadNullZString(strMessage);
	if(false == bMessage || true == strMessage.empty())
	{
		return;
	}
	auto split = strMessage.find('/');
	if(strMessage[0] == '#' || split == strMessage.npos)
	{
		std::array<wchar_t, 2048> wzMessage;
		swprintf_s(&wzMessage[0], wzMessage.size(), L"%hs", strMessage.c_str() + 1);
		SetDlgItemText(IDC_STATUS, &wzMessage[0]);
		return;
	}
	bool bSilenced = false;
	if(strMessage[0] == '*')
	{
		bSilenced = true;
		SetDlgItemTextW(IDC_SILENCED_STATUS, L"System Powered Off / Alarm Disabled");
	}
	else
	{
		SetDlgItemTextW(IDC_SILENCED_STATUS, L"System Powered On / Alarm Enabled");
	}
	std::string strThreshold = strMessage.substr(split + 1);
	strMessage.erase(split);
	UINT uiCount = static_cast<UINT>(atol(strMessage.c_str() + (bSilenced ? 1 : 0)));
	UINT uiThreshold = static_cast<UINT>(atol(strThreshold.c_str()));

	SetDlgItemInt(IDC_QFLOWRATE, uiCount, FALSE);
	SetDlgItemInt(IDC_THRESHOLD_STATIC, uiThreshold, FALSE);

	//
	// Calculate flow rate for status
	double dRate = (uiCount * 4);		// pulses per second
	dRate /= 40;						// @TODO make this configurable - this is for the light object sensor
	std::array<wchar_t, 1024> wzStatus;
	swprintf_s(&wzStatus[0], wzStatus.size(), L"%.02f Liters per Minute", dRate);
	SetDlgItemText(IDC_STATUS, &wzStatus[0]);

	if(uiCount < uiThreshold)
	{
		SetDlgItemText(IDC_STATUS_2, L"ALARM: Water flow is below the threshold!");
		SetForegroundWindow();
		FlashWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_STATUS_2, L"Water flow is at or above threshold");
	}
}


void CCO2WaterFlowDlg::RefreshCOMList()
{
	for(auto &entry : m_COMSelectionMap)
	{
		m_COMDropDown.DeleteString(entry.first);
	}

	m_COMDropDown.Clear();
	m_COMSelectionMap.clear();
	int iLastAdded = -1;
	for(auto &entry : m_COMPortMap)
	{
		int idx = m_COMDropDown.AddString(entry.first.c_str());
		m_COMSelectionMap[idx] = entry.first.c_str();
		iLastAdded = idx;
	}
	m_COMDropDown.SetCurSel(iLastAdded);

	if(1 == m_COMPortMap.size())
	{
		//
		// Try the only one there...
		OnBnClickedButtonOpen();
	}
}

void CCO2WaterFlowDlg::OnBnClickedButtonScanports()
{
	BuildCOMPortMap();
	RefreshCOMList();
}


void CCO2WaterFlowDlg::OnBnClickedButtonSetthreshold()
{
	UINT uiThreshold = GetDlgItemInt(IDC_EDIT_NEWTHRESHOLD, nullptr, FALSE);
	BYTE bThreshold = static_cast<BYTE>(uiThreshold);
	m_SerialPort.WriteByte(bThreshold);
}
