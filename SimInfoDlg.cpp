
// SimInfoDlg.cpp : implementation file
//

//#include <direct.h>  // for chdir and _getcwd
#include <sys/stat.h> // for stat command in window init

#include <ctime>

#include "stdafx.h"
#include "SimInfo.h"
#include "SimInfoDlg.h"
#include "afxdialogex.h"
#include "DisplayTree.h"

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

void
CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CSimInfoDlg dialog

CSimInfoDlg::CSimInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SIMINFO_DIALOG, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void 
CSimInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_READ_DATA, m_read_data);
	DDX_Control(pDX, IDC_INS_TYPE, m_ins_type);
	DDX_Control(pDX, IDC_INS_UNIT, m_ins_unit);
	DDX_Control(pDX, IDC_INS_BUTTON, m_ins_button);
	DDX_Control(pDX, IDC_INS_SBB, m_ins_sbb);
	DDX_Control(pDX, IDC_MON1, m_mon1);
	DDX_Control(pDX, IDC_MON2, m_mon2);
	DDX_Control(pDX, IDC_MON3, m_mon3);
}

BEGIN_MESSAGE_MAP(CSimInfoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_READ_TYPES, &CSimInfoDlg::OnBnClickedReadTypes)
	ON_BN_CLICKED(IDC_READ_SIM, &CSimInfoDlg::OnBnClickedReadSim)
	ON_BN_CLICKED(IDC_VIEW_TYPES, &CSimInfoDlg::OnBnClickedViewTypes)
	ON_BN_CLICKED(IDC_VIEW_UNITS, &CSimInfoDlg::OnBnClickedViewUnits)
	ON_BN_CLICKED(IDC_VIEW_STATE, &CSimInfoDlg::OnBnClickedViewState)
	ON_BN_CLICKED(IDC_PRINT_TYPES, &CSimInfoDlg::OnBnClickedPrintTypes)
	ON_BN_CLICKED(IDC_SAVE_SIM_INFO, &CSimInfoDlg::OnBnClickedSaveSimInfo)
	ON_BN_CLICKED(IDC_SAVE_TYPE_CROSSWALK, &CSimInfoDlg::OnBnClickedSaveTypeCrosswalk)
	ON_BN_CLICKED(IDC_READ_DATA, &CSimInfoDlg::OnBnClickedReadData)
	ON_BN_CLICKED(IDC_SAVE_DATA_CROSSWALK, &CSimInfoDlg::OnBnClickedSaveDataCrosswalk)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_INS_BUTTON, &CSimInfoDlg::OnBnClickedInsButton)
	ON_BN_CLICKED(IDC_UNIT_LIST, &CSimInfoDlg::OnBnClickedUnitList)
	ON_BN_CLICKED(IDCANCEL, &CSimInfoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_MON1, &CSimInfoDlg::OnBnClickedMon1)
	ON_BN_CLICKED(IDC_MON2, &CSimInfoDlg::OnBnClickedMon2)
	ON_BN_CLICKED(IDC_MON3, &CSimInfoDlg::OnBnClickedMon3)
	ON_BN_CLICKED(IDC_SAVE_UAL, &CSimInfoDlg::OnBnClickedSaveUal)
	ON_BN_CLICKED(IDC_MINIMIZE_STATE, &CSimInfoDlg::OnBnClickedMinimizeState)
END_MESSAGE_MAP()


// CSimInfoDlg message handlers

BOOL 
CSimInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LPCSTR           lpDevice = nullptr;
	DWORD            iDevNum = 0;
	DISPLAY_DEVICE	 displayDeviceDev;
	DISPLAY_DEVICE	 displayDevice;
	DWORD            dwFlags = 0;
	int				devCount = 0;
	int				dispCount = 0;
	struct stat		sStat = {};
	char			fDir[MAX_PATH] = {};
	size_t			numConv = 0;
	char*		userprofile;
	errno_t			myerr = 0;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL){
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
	dtdlg = new DisplayTree(this);
	if (!dtdlg) {
		m_status.AddString(L"Type Tree Dialog not available !");
	}
	//dtdlg->type = 0;  // type
	dtdlg->m_tree.SetType(0);

	unitdlg = new DisplayTree(this);
	if (!unitdlg) {
		m_status.AddString(L"Unit Tree Dialog not available !");
	}
	//unitdlg->type = 1;  // unit
	unitdlg->m_tree.SetType(1);

	statedlg = new DisplayTree(this);
	if (!statedlg) {
		m_status.AddString(L"State Tree Dialog not available !");
	}
	//statedlg->type = 2;  // state
	statedlg->m_tree.SetType(2);

	if (stat("c:\\vcbt", &sStat) == 0) { // check that directory exists
		strcpy_s(fDir, "c:\\vcbt");
	} else if (stat("c:\\hvacsim", &sStat) == 0) {
		strcpy_s(fDir, "c:\\hvacsim");
	} else {
		if (_getcwd(fDir, MAX_PATH) == nullptr) {
			//userprofile = getenv("USERPROFILE");
			myerr = _dupenv_s(&userprofile, &numConv, "USERPROFILE");
			if (!myerr) {
				strcpy_s(fDir, userprofile); // userprofile should never be 0 here in spite of warning
			} else {
				strcpy_s(fDir, "c:");  // the directory of last resort...
			}

		}
	}
	memset(&ofn, 0, sizeof(ofn));
	mbstowcs_s(&numConv,ofnDir, fDir,MAX_PATH);
	ofn.lpstrInitialDir = ofnDir;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = TEXT("DAT Files *.dat\0*.dat\0All Files *.*\0*.*\0\0");

	ofn.lpstrFile = LPWSTR(ofnFile);
	ofn.lpstrTitle = ofnBox;
	ofn.lpstrDefExt = L"DAT";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = 1500;
	ofn.nMaxFileTitle = 99;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;// OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	ofn.lCustData = NULL;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
	ofnInit = 1;

	m_status.AddString(L"NIST Engineering Lab: HVACSIM+ SIM File Analyzer v1.1");
	m_status.AddString(L"Developed by: Mike Galler mikeg@nist.gov");
	m_status.AddString(L"View the Users Guide at https://nvlpubs.nist.gov/nistpubs/TechnicalNotes/NIST.TN.2110.pdf");
	m_status.AddString(L"Drop files here or use buttons below to enter a .SIM file, a .DAT file, or .XML file.");

	m_ins_sbb.SetWindowText(L"1:1");

	memset(&displayDeviceDev, 0, sizeof(DISPLAY_DEVICE));
	displayDeviceDev.cb = sizeof(DISPLAY_DEVICE);
	memset(&displayDevice, 0, sizeof(DISPLAY_DEVICE));
	displayDevice.cb = sizeof(DISPLAY_DEVICE);
	monitorCount = GetSystemMetrics(80);// monitor count: 3 on sandbar/dev machine
	virtWidth = GetSystemMetrics(16);// client area width: 1920
	virtLeft = GetSystemMetrics(76);// x coord for left side of virt screen in pixels

	currentMonitor = 0;

	/*m_status.AddString(L"Find adapters:");
	while (EnumDisplayDevices(NULL, devCount, &displayDeviceDev, 0)) {
		//m_status.AddString(displayDeviceDev.DeviceName);
		//m_status.AddString(L"  Adapters:");
		for (size_t i = 0; i < 9; i++)// 9 is arb
		{
			if (EnumDisplayDevices(displayDeviceDev.DeviceName, i, &displayDevice, 0) == 0) break;
			if (wcsstr(displayDevice.DeviceName, L"Mon") != 0) {
				m_status.AddString(displayDevice.DeviceName);
				monitorList.push_back(displayDevice);
				adapterList.push_back(displayDeviceDev);
			}
		}
		devCount++;
	}*/

	if (monitorCount > 1) {
		m_mon1.ShowWindow(1);
		m_mon2.ShowWindow(1);
	}
	if (monitorCount > 2) {
		m_mon3.ShowWindow(1);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}  // end OnInitDialog


void 
CSimInfoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX){
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void 
CSimInfoDlg::OnPaint()
{
	if (IsIconic()){
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
	} else {
		CDialogEx::OnPaint();
	}
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR 
CSimInfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void 
CSimInfoDlg::OnBnClickedReadTypes()
{
	// TODO: Add your control notification handler code here
	CString		cs1;
	FILE		*ip = NULL;
	CButton		*cbv = NULL;
	char		tfn[MAX_PATH] = {};
	static int	inSub = 0;	// MAG 200918 add check for reentry

	ofn.lpstrFilter = TEXT("DAT Files *.dat\0*.dat\0All Files *.*\0*.*\0\0");
	ofn.lpstrDefExt = L"DAT";
	ofn.Flags = OFN_FILEMUSTEXIST;

	_tcscpy_s(ofnBox,50, L"Find Type Data File");

	inSub = 1;
	ofn.lpstrFile[0] = 0;
	GetOpenFileName(&ofn);
	if (ofn.lpstrFile[0] == 0) {
		m_status.AddString(L"DAT file name not entered.");
		inSub = 0;
		return;
	} 

	cs1.Format(L"Entered file '%ls'.", ofn.lpstrFile);
	m_status.AddString(cs1);

	sprintf_s(tfn,MAX_PATH, "%ls", ofn.lpstrFile);

	_wfopen_s(&ip,ofn.lpstrFile, L"r");

	if (ip == NULL) {
		m_status.AddString(L"ERROR opening DAT file.");
		m_status.SetCurSel(m_status.GetCount() - 1);
		inSub = 0;
		return;
	}

	if (theApp.ReadTypeFile(ip) == 1) {
		theApp.SetTypesFileName(tfn);

		cbv = (CButton *)GetDlgItem(IDC_VIEW_TYPES);
		if (cbv != NULL) cbv->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_PRINT_TYPES))->EnableWindow(1);

		m_ins_type.EnableWindow(1);
		m_ins_unit.EnableWindow(1);
		m_ins_button.EnableWindow(1);
		m_ins_sbb.EnableWindow(1);
	} else {
		m_status.AddString(L"ERROR entering DAT file. Please review and try again.");
	}

	fclose(ip);
	inSub = 0;

	return;
} // end CSimInfoDlg::OnBnClickedReadTypes()


void 
CSimInfoDlg::OnBnClickedReadSim()
{
	// TODO: Add your control notification handler code here
	FILE		*ip = NULL;
	CString		cs1;
	static int	inSub = 0;	// MAG 200918 add check for reentry

	if (inSub > 0) return;
	ofn.lpstrFilter = TEXT("SIM Files *.sim\0*.sim\0All Files *.*\0*.*\0\0");
	ofn.lpstrDefExt = L"SIM";
	ofn.Flags = OFN_FILEMUSTEXIST;

	_tcscpy_s(ofnBox, 50, L"Find Simulation File");

	inSub = 1;
	ofn.lpstrFile[0] = 0;
	GetOpenFileName(&ofn);
	if (ofn.lpstrFile[0] == 0) {
		m_status.AddString(L"SIM file name not entered.");
		inSub = 0;
		return;
	}

	cs1.Format(L"Entered file '%ls'", ofn.lpstrFile);
	m_status.AddString(cs1);

	StrCpy(simFileName, ofn.lpstrFile);

	_wfopen_s(&ip, ofn.lpstrFile, L"r");

	if (ip == NULL) {
		m_status.AddString(L"Error opening SIM file");
		m_status.SetCurSel(m_status.GetCount() - 1);
		inSub = 0;
		return;
	}

	if (theApp.ReadSimFile(ip) == 1) {
		CButton *cbv = (CButton *)GetDlgItem(IDC_VIEW_UNITS);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_VIEW_STATE);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_SAVE_SIM_INFO);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_UNIT_LIST);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_READ_DATA);
		if (cbv != NULL) cbv->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_SAVE_TYPE_CROSSWALK))->EnableWindow(1);
	}

	fclose(ip);
	inSub = 0;

	return;
} // end OnBnClickedReadSim


void 
CSimInfoDlg::OnBnClickedViewTypes()
{
	// TODO: Add your control notification handler code here
	if (dtdlg->GetSafeHwnd() == 0) dtdlg->Create(IDD_DISPLAYTREE,this);
	dtdlg->ShowWindow(SW_SHOW);
	dtdlg->SetWindowPos(this, (currentMonitor * 1920) + virtLeft, 0, 640, 1080, 0);
}


void 
CSimInfoDlg::OnBnClickedViewUnits()
{
	// TODO: Add your control notification handler code here
	if (unitdlg->GetSafeHwnd() == 0) unitdlg->Create(IDD_DISPLAYTREE,this);
	unitdlg->ShowWindow(SW_SHOW);
	unitdlg->SetWindowPos(this, (currentMonitor * 1920)+640 + virtLeft, 0, 640, 1080, 0);
}


void 
CSimInfoDlg::OnBnClickedViewState()
{
	// TODO: Add your control notification handler code here
	if (statedlg->GetSafeHwnd() == 0) statedlg->Create(IDD_DISPLAYTREE, this);
	statedlg->ShowWindow(SW_SHOW);
	statedlg->SetWindowPos(this, (currentMonitor * 1920)+1280 + virtLeft, 0, 640, 1080, 0);
}


void 
CSimInfoDlg::EnableTypeButton()
{
	CButton *cbv = (CButton *)GetDlgItem(IDC_VIEW_TYPES);
	if (cbv != NULL) cbv->EnableWindow(1);
	m_ins_type.EnableWindow(1);
	m_ins_unit.EnableWindow(1);
	m_ins_button.EnableWindow(1);
	m_ins_sbb.EnableWindow(1);
}


void 
CSimInfoDlg::OnBnClickedPrintTypes()
{
	// TODO: Add your control notification handler code here
	theApp.SaveTypeFile();
}


void 
CSimInfoDlg::OnBnClickedSaveSimInfo()
{
	// TODO: Add your control notification handler code here
	FILE		*ip = NULL;
	CString		cs1;
	CString		fileName;
	int			i = 1;
	int			previ = 0;
	wchar_t		lfile[MAX_PATH] = {};

	ofn.lpstrFilter = TEXT("XML Files *.xml\0*.xml\0All Files *.*\0*.*\0\0");
	ofn.lpstrDefExt = L"XML";
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

	_tcscpy_s(ofnBox, 50, L"Save Simulation Information File");

	// create suggested name
	fileName = simFileName;
	while (i > 0) {
		i = fileName.Find('.', previ + 1);
		if (i > 0) {
			previ = i;
		}
	}
	i = previ;

	if (i <= 0) {  // not found
		fileName += ".xml";
	} else {
		fileName = fileName.Left(i);
		fileName += +".xml";
	}
		
	wcscpy_s(lfile, fileName);
	ofn.lpstrFile = lfile;// fileName.GetBuffer(fileName.GetLength());

	bool fnStat = (GetSaveFileName(&ofn)) ? 1 : 0;
	if (ofn.lpstrFile[0] == 0) {
		m_status.AddString(L"Simulation Information file name not entered.");
		return;
	}
	if (fnStat == 0) {
		cs1.Format(L"Error getting save file name!");
		return;
	}

	cs1.Format(L"Entered file '%ls'", ofn.lpstrFile);
	m_status.AddString(cs1);

	_wfopen_s(&ip, ofn.lpstrFile, L"w");

	if (ip == NULL) {
		m_status.AddString(L"Error saving Simulation Information file");
		m_status.SetCurSel(m_status.GetCount() - 1);
		return;
	}
	
	try {
		theApp.SaveSimInfoFile(ip);
		fclose(ip);
	}
	catch (...) {
		cs1.Format(L"Caught an error (breakpoint)");
	}

	return;
} // end CSimInfoDlg::OnBnClickedSaveSimInfo()


void 
CSimInfoDlg::OnBnClickedSaveTypeCrosswalk()
{
	// TODO: Add your control notification handler code here
	FILE		*ip = NULL;
	CString		cs1;
	size_t		i = 0;
	int			itemp = 1;
	int			previ = 0;
	bool		fnStat = 0;

	ofn.lpstrFilter = TEXT("XML Files *.xml\0*.xml\0All Files *.*\0*.*\0\0");
	ofn.lpstrDefExt = L"XML";
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

	_tcscpy_s(ofnBox, 50, L"Save Simulation Crosswalk File");

	cs1 = simFileName;
	if (cs1.GetLength() == 0) cs1 = "HVS UXW";
	itemp = 1; // repeated for clarity
	while (itemp > 0) {
		itemp = cs1.Find('.', previ + 1);
		if (itemp > 0) {
			previ = itemp;
		}
	}
	itemp = previ;

	if (itemp <= 0) {  // not found
		cs1 += " unit crosswalk.xml";
	} else {
		cs1 = cs1.Left(itemp);
		cs1 += +" unit crosswalk.xml";
	}

	ofn.lpstrFile = ofnFile;
	wsprintf(ofn.lpstrFile, L"%s", cs1.GetString());

	fnStat = (GetSaveFileName(&ofn)) ? 1 : 0;

	if (ofn.lpstrFile[0] == 0) {
		m_status.AddString(L"Simulation Crosswalk file name not entered.");
		return;
	}
	if (fnStat == 0) {
		m_status.AddString(L"Simulation Crosswalk file name not entered.");
		return;
	}

	cs1.Format(L"Entered file '%ls'", ofn.lpstrFile);
	m_status.AddString(cs1);

	_wfopen_s(&ip, ofn.lpstrFile, L"w");

	if (ip == NULL) {
		m_status.AddString(L"Error saving Simulation Crosswalk file");
		m_status.SetCurSel(m_status.GetCount() - 1);
		return;
	}

	theApp.SaveUnitCrosswalkFile(ip);

	wsprintf(ofnDir, L"%s", ofn.lpstrFile);
	i = wcslen(ofnDir);
	for (size_t j = i; j > 0; j--){
		if (ofnDir[j - 1] == '\\') {
			ofnDir[j] = 0;
			break;
		}
	}

	fclose(ip);

	return;
} // end CSimInfoDlg::OnBnClickedSaveTypeCrosswalk()


void 
CSimInfoDlg::OnBnClickedReadData()
{
	// TODO: Add your control notification handler code here
	FILE		*ip = NULL;
	CString		cs1;
	int			i = 0;
	static int	inSub = 0;	// MAG 200918 add check for reentry

	ofn.lpstrFilter = TEXT("Data Files *.csv\0*.csv\0All Files *.*\0*.*\0\0");
	ofn.lpstrDefExt = L"CSV";
	ofn.Flags = OFN_FILEMUSTEXIST;

	_tcscpy_s(ofnBox, 50, L"Find Data File (CSV)");

	ofn.lpstrFile[0] = 0;
	GetOpenFileName(&ofn);
	if (ofn.lpstrFile[0] == 0) {
		m_status.AddString(L"CSV Data file name not entered.");
		inSub = 0;
		return;
	}

	cs1.Format(L"Reading data file '%ls'...", ofn.lpstrFile);
	m_status.AddString(cs1);

	_wfopen_s(&ip, ofn.lpstrFile, L"r");

	if (ip == NULL) {
		m_status.AddString(L"Error opening Data file.");
		m_status.SetCurSel(m_status.GetCount() - 1);
		inSub = 0;
		return;
	}

	i = theApp.ReadDataFile(ip);
	if (i == 1) {
		m_status.AddString(L"Finished reading data file.");
	} else {
		m_status.AddString(L"NOTE- There was an error reading the data file.");
		fclose(ip);
		inSub = 0;
		return;
	}

	((CButton *)GetDlgItem(IDC_SAVE_DATA_CROSSWALK))->EnableWindow(1);

	fclose(ip);
	inSub = 0;

	return;
} // end CSimInfoDlg::OnBnClickedReadData()


void 
CSimInfoDlg::OnBnClickedSaveDataCrosswalk()
{
	// TODO: Add your control notification handler code here
	FILE		*ip = NULL;
	CString		cs1;
	CString		cs2;
	int			itemp = 1;
	int			previ = 0;
	size_t		i = 0;

	ofn.lpstrFilter = TEXT("XML Files *.xml\0*.xml\0All Files *.*\0*.*\0\0");
	ofn.lpstrDefExt = L"XML";
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

	_tcscpy_s(ofnBox, 50, L"Save Data Crosswalk File");
	
	cs1 = simFileName;
	while (itemp > 0) {
		itemp = cs1.Find('.', previ + 1);
		if (itemp > 0) {
			previ = itemp;
		}
	}
	itemp = previ;

	if (itemp <= 0) {  // not found
		cs1 += " DXW.xml";
	} else {
		cs1 = cs1.Left(itemp);
		cs1 += +" DXW.xml";
	}

	ofn.lpstrFile = ofnFile;
	wsprintf(ofn.lpstrFile, L"%s", cs1.GetString());

	bool fnStat = (GetSaveFileName(&ofn)) ? 1 : 0;

	if (ofn.lpstrFile[0] == 0) {
		m_status.AddString(L"Data Crosswalk File name not entered.");
		return;
	}
	if (fnStat == 0) {
		m_status.AddString(L"Error getting save file name!");
		return;
	}

	//cs1.Format(L"Entered file '%ls'", ofn.lpstrFile); // debug string
	//m_status.AddString(cs1);

	_wfopen_s(&ip, ofn.lpstrFile, L"w");

	if (ip == NULL) {
		m_status.AddString(L"Error opening Data Crosswalk File");
		m_status.SetCurSel(m_status.GetCount() - 1);
		return;
	}

	// MAG 240208 update save dir in case it's changed
	wsprintf(ofnDir, L"%s", ofn.lpstrFile);
	i = wcslen(ofnDir);
	for (size_t j = i; j > 0; j--){
		if (ofnDir[j - 1] == '\\') {
			ofnDir[j] = 0;
			break;
		}
	}

	cs2.Format(L"Saving data to file:");//% s.", cs1);
	m_status.AddString(cs2);
	theApp.SaveDataCrosswalkFile(ip);

	fclose(ip);
} // end CSimInfoDlg::OnBnClickedSaveDataCrosswalk()


void 
CSimInfoDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	int		filecount = 0;
	int		itemp = 0;
	int		j = 0;
	int		rv = 1; // holds return value from function calls, init to no fault
	size_t	i = 0;
	size_t	si = 0;
	TCHAR	filename[MAX_PATH] = {};
	char	tfn[2 * MAX_PATH] = {};
	CButton *cbv = NULL;

	std::vector<std::basic_string<TCHAR>>			typeFiles;	// .dat
	std::vector<std::basic_string<TCHAR>>			simFiles;	// .sim
	std::vector<std::basic_string<TCHAR>>			dataFiles;	// .csv
	std::vector<std::basic_string<TCHAR>>			unitFiles;  // .txt each line has unit type to insert, in order- term unit sequence file
	std::vector<std::basic_string<TCHAR>>			unitListFiles;  // .xml each tab has unit to insert, with I/O/P data
	std::vector<std::basic_string<TCHAR>>			outFiles;	// MAG 200805 outfiles produced by modsim- fed into reported values only
	std::vector<std::basic_string<TCHAR>>			smlFiles;	// MAG 211013 add State Move Files sml
	std::vector<std::basic_string<TCHAR>>			ualFiles;	// MAG 220421 add Unit Assignment files ual
	std::vector<std::basic_string<TCHAR>>			srfFiles;	// MAG 221006 state report file (drop list of unit:IO# to request state position report) TODO
	std::vector<std::basic_string<TCHAR>>			nonFiles;
	std::vector<std::basic_string<TCHAR>>::iterator it;
	std::basic_string<TCHAR>						tc;

	CString	cs1;
	CString cs2;
	CString cs3;

	FILE	*ip = NULL;

	// NOTE- files must be analyzed in order DAT, SIM, CSV
	filecount = DragQueryFile(hDropInfo, 0xFFFFFFFF, filename, MAX_PATH); // returns number of files dropped
	cs1.Format(L"Drop files: %d detected.", filecount);
	m_status.AddString(cs1);

	for (i = 0; i < filecount; i++) {
		j = DragQueryFile(hDropInfo, (int)i, filename, MAX_PATH);  // get the name of file i in filename

		cs1.Format(L"Add file to queue: '%s'", filename);
		m_status.AddString(cs1);

		// convert to lower case- do this or put uppercase option in every if statement below
		for ( j = 0; j < wcslen(filename); j++){
			filename[j] = tolower(filename[j]);
		}
	
		// find out what has been dropped- .sim, .dat, or .csv?
		if (wcsstr(filename, L".dat") != NULL) { // NOTE- mirrors ReadTypes sub above
			typeFiles.push_back(filename);
		} else if (wcsstr(filename, L".sim") != NULL) { // NOTE- mirrors ReadSim sub above
			if ((simFiles.size() == 0)&&(unitListFiles.size() == 0)) {
				simFiles.push_back(filename);
			} else {
				nonFiles.push_back(filename);
				m_status.AddString(L"NOTE- One simulation work file is allowed. File will not be analyzed."); // file name appears in prev line
			}
		} else if (wcsstr(filename, L".csv") != NULL) { // NOTE- mirrors ReadData sub above
			dataFiles.push_back(filename);
		} else if (wcsstr(filename, L".txt") != NULL) { // NOTE- mirrors ReadData sub above
			unitFiles.push_back(filename);
		} else if (wcsstr(filename, L".txt") != NULL) { // NOTE- mirrors ReadData sub above
			unitFiles.push_back(filename);
		} else if (wcsstr(filename, L".out") != NULL) { // NOTE- mirrors ReadData sub above
			outFiles.push_back(filename);
		} else if (wcsstr(filename, L".sml") != NULL) { // NOTE- mirrors ReadData sub above
			smlFiles.push_back(filename);
		} else if (wcsstr(filename, L".ual") != NULL) { // NOTE- mirrors ReadData sub above
			ualFiles.push_back(filename);
		} else if (wcsstr(filename, L".srf") != NULL) { // MAG 221006
			srfFiles.push_back(filename);
		} else if (wcsstr(filename, L".xml") != NULL) { // NOTE- mirrors ReadData sub above
			if ((simFiles.size() == 0) && (unitListFiles.size() == 0)) {	// MAG 200413 modify so only one sim AND xml sim file can be read in.
				unitListFiles.push_back(filename);
			} else {
				nonFiles.push_back(filename);
				m_status.AddString(L"NOTE- One simulation work file is allowed. File will not be analyzed."); // file name appears in prev line
			}
		} else {
			nonFiles.push_back(filename);
			m_status.AddString(L"This file type is not used.");
		}
	}
	
	// read Type files
	for(it = typeFiles.begin(); it != typeFiles.end(); it++){
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening Types file! File entry is halted.");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}

		sprintf_s(tfn, MAX_PATH, "%S", it->c_str());
		theApp.SetTypesFileName(tfn);

		cs1.Format(L"Reading Types '%s'...", it->c_str());
		m_status.AddString(cs1);

		rv = theApp.ReadTypeFile(ip);
		if (rv == 0) {
			cs1.Format(L"ERROR- Could not read Type file '%s'. File entry is halted.", filename);
			m_status.AddString(cs1);
			fclose(ip);
			return;
		}
		m_status.AddString(L"Finished reading Types file.");

		cbv = (CButton *)GetDlgItem(IDC_VIEW_TYPES);
		if (cbv != NULL) cbv->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_PRINT_TYPES))->EnableWindow(1);
		m_ins_type.EnableWindow(1);
		m_ins_unit.EnableWindow(1);
		m_ins_button.EnableWindow(1);
		m_ins_sbb.EnableWindow(1);

		fclose(ip);
	}

	j = m_status.GetCount();
	m_status.SetCurSel(j - 1);
	if ((theApp.GetTypesCount() == 0) && (simFiles.size() > 0)) {	// check on types size covers current type file reads and previous type file reads
		// need to set cwd to same as sim file(s) in case types not read
		si = simFiles[0].rfind('\\');
		if (si != std::string::npos) {
			tc = simFiles[0].substr(0, si);
			_wchdir(tc.c_str());
			cs1.Format(L"Change directory to '%s'", tc.c_str());
		}
	}

	// read Sim files- NOTE assuming only one will be entered, subsequent ones will cause reset of units array
	// MAG note- simFiles should be max length 1 here- extra sim files are saved to nonFiles array and not read
	for (it = simFiles.begin(); it != simFiles.end(); it++) {
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening input SIMulation work file!");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}

		// MAG 240208 set initdir to directory sim file is in
		wsprintf(ofnDir, L"%s", it->c_str());
		i = wcslen(ofnDir);
		for (size_t j = i; j > 0; j--) {
			if (ofnDir[j - 1] == '\\') {
				ofnDir[j] = 0;
				break;
			}
		}

		cs1.Format(L"Reading SIMulation work file '%s'...", it->c_str());
		m_status.AddString(cs1);

		rv = theApp.ReadSimFile(ip);
		if (rv == 0) {
			cs1.Format(L"ERROR- Could not read simulation work file '%s'. File entry is halted.", filename);
			m_status.AddString(cs1);
			fclose(ip);
			return;
		}
		m_status.AddString(L"Finished reading SIMulation work file.");

		cbv = (CButton *)GetDlgItem(IDC_VIEW_UNITS);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_VIEW_STATE);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_SAVE_SIM_INFO);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_UNIT_LIST);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_READ_DATA);
		if (cbv != NULL) cbv->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_SAVE_TYPE_CROSSWALK))->EnableWindow(1);

		fclose(ip);
	}

	j = m_status.GetCount();
	m_status.SetCurSel(j - 1);

	// MAG 200110
	// read Unit List insert files (XML)
	for (it = unitListFiles.begin(); it != unitListFiles.end(); it++) {
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening Units file! File entry is halted.");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}

		cs1.Format(L"Reading Unit List from '%s'...", it->c_str());
		m_status.AddString(cs1);

		cs2 = it->c_str();
		j = cs2.ReverseFind('\\');
		if (j != -1) {
			cs3 = cs2.Left(j);
			_wchdir(cs3);
		}

		j = theApp.ReadUnitListFile(ip);

		if (j == 1) {
			// shamelessly copied from ReadSim
			CButton *cbv = (CButton *)GetDlgItem(IDC_VIEW_UNITS);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_VIEW_STATE);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_SAVE_SIM_INFO);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_UNIT_LIST);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_READ_DATA);
			if (cbv != NULL) cbv->EnableWindow(1);
			((CButton *)GetDlgItem(IDC_SAVE_TYPE_CROSSWALK))->EnableWindow(1);

			if (unitdlg->GetInit() == 1) theApp.BuildUnitTree(&unitdlg->m_tree);
			if (statedlg->GetInit() == 1)theApp.BuildStateTree(&statedlg->m_tree);

			cs1.Format(L"Finish Reading Unit List from '%s'...", it->c_str());
			m_status.AddString(cs1);
		} else {
			cs1.Format(L"ERROR: Format error found in Unit List file.");
		}
	}
	
	// read Unit insert files (TXT)
	for (it = unitFiles.begin(); it != unitFiles.end(); it++) {
		if (theApp.GetTypeFromIndex(0) == 0) {
			m_status.AddString(L"Error- types not entered.");
			return;
		}
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening Units file! File entry is halted.");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}

		cs1.Format(L"Reading Units from '%s'...", it->c_str());
		m_status.AddString(cs1);

		j = 0;  // bool for any units inserted successfully
		while (fgets(tfn, 10, ip) != 0)
		{
			if (strlen(tfn) == 0) continue;
			itemp = atoi(tfn);
			if (itemp < 0) continue;
			rv = theApp.InsertUnit(itemp, -1);  // position set to -1 to make sure new ones are appended to units list
			if (rv == 1) j = 1;
		}

		if (j == 1) {
			// shamelessly copied from ReadSim
			CButton *cbv = (CButton *)GetDlgItem(IDC_VIEW_UNITS);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_VIEW_STATE);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_SAVE_SIM_INFO);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_UNIT_LIST);
			if (cbv != NULL) cbv->EnableWindow(1);
			cbv = (CButton *)GetDlgItem(IDC_READ_DATA);
			if (cbv != NULL) cbv->EnableWindow(1);
			((CButton *)GetDlgItem(IDC_SAVE_TYPE_CROSSWALK))->EnableWindow(1);

			if (unitdlg->GetInit() == 1) theApp.BuildUnitTree(&unitdlg->m_tree);
			if (statedlg->GetInit() == 1)theApp.BuildStateTree(&statedlg->m_tree);
		}
	}
	
	// read Data files
	for (it = dataFiles.begin(); it != dataFiles.end(); it++) {
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening input data CSV file!");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}
		cs1.Format(L"Reading input data CSV file '%s'...", it->c_str());
		m_status.AddString(cs1);

		rv = theApp.ReadDataFile(ip);
		if (rv == 0) {
			cs1.Format(L"ERROR- Could not read data file '%s'. File entry is halted.", filename);
			m_status.AddString(cs1);
			fclose(ip);
			return;
		}
		m_status.AddString(L"Finished reading data file.");

		((CButton *)GetDlgItem(IDC_SAVE_DATA_CROSSWALK))->EnableWindow(1);

		fclose(ip);
	}

	// read Unit Assignment List files
	for (it = ualFiles.begin(); it != ualFiles.end(); it++) {
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening input data UAL file!");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}
		cs1.Format(L"Reading Unit Assignment List file '%s'...", it->c_str());
		m_status.AddString(cs1);

		rv = theApp.ReadUALFile(ip);
		if (rv == 0) {
			cs1.Format(L"ERROR- Could not read UAL file '%s'. File entry is halted.", filename);
			m_status.AddString(cs1);
			fclose(ip);
			return;
		}

		if (unitdlg->GetInit() == 1) theApp.BuildUnitTree(&unitdlg->m_tree);
		if (statedlg->GetInit() == 1)theApp.BuildStateTree(&statedlg->m_tree);

		m_status.AddString(L"Finished reading UAL file.");

		fclose(ip);
	}

	// read State Move List files
	for (it = smlFiles.begin(); it != smlFiles.end(); it++) {
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening input data SML file!");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}
		cs1.Format(L"Reading State Move List file '%s'...", it->c_str());
		m_status.AddString(cs1);

		rv = theApp.ReadSMLFile(ip);
		if (rv == 0) {
			cs1.Format(L"ERROR- Could not read SML file '%s'. File entry is halted.", filename);
			m_status.AddString(cs1);
			fclose(ip);
			return;
		}

		if (unitdlg->GetInit() == 1) theApp.BuildUnitTree(&unitdlg->m_tree);
		if (statedlg->GetInit() == 1)theApp.BuildStateTree(&statedlg->m_tree);

		m_status.AddString(L"Finished reading SML file.");

		fclose(ip);
	}

	// read OUT files
	if(dataFiles.size() == 0){
		for (it = outFiles.begin(); it != outFiles.end(); it++) {
			_wfopen_s(&ip, it->c_str(), L"r");

			if (ip == NULL) {
				m_status.AddString(L"Error opening input data OUT file!");
				m_status.SetCurSel(m_status.GetCount() - 1);
				return;
			}
			cs1.Format(L"Reading input data OUT file '%s'...", it->c_str());
			m_status.AddString(cs1);

			rv = theApp.ReadDataOutFile(ip);
			if (rv == 0) {
				cs1.Format(L"ERROR- Could not read OUT file '%s'. File entry is halted.", filename);
				m_status.AddString(cs1);
				fclose(ip);
				return;
			}
			m_status.AddString(L"Finished reading OUT file.");

			((CButton *)GetDlgItem(IDC_SAVE_DATA_CROSSWALK))->EnableWindow(1);

			fclose(ip);
		}
	}


	// read State Report File files
	for (it = srfFiles.begin(); it != srfFiles.end(); it++) {
		_wfopen_s(&ip, it->c_str(), L"r");

		if (ip == NULL) {
			m_status.AddString(L"Error opening input data SRF file!");
			m_status.SetCurSel(m_status.GetCount() - 1);
			return;
		}
		cs1.Format(L"Reading State Report File '%s'...", it->c_str());
		m_status.AddString(cs1);

		rv = theApp.ReadSRFFile(ip);
		if (rv == 0) {
			cs1.Format(L"ERROR- Could not read SRF file '%s'. File entry is halted.", filename);
			m_status.AddString(cs1);
			fclose(ip);
			return;
		}

		m_status.AddString(L"Finished reading SRF file.");

		fclose(ip);
	}

	// review unread files
	if (nonFiles.size() > 0) m_status.AddString(L"NOTE- the following files were not analyzed:");
	for (it = nonFiles.begin(); it != nonFiles.end(); it++) {
		m_status.AddString(it->c_str());
	}

	m_status.AddString(L"Completed analyzing dropped files.");
	
	j = m_status.GetCount();
	m_status.SetCurSel(j - 1);

	CDialogEx::OnDropFiles(hDropInfo);

	return;
}  // end OnDropFiles


// MAG 191218 add function
// Insert selected TYPE into unit list at specified position
void
CSimInfoDlg::OnBnClickedInsButton()
{
	// TODO: Add your control notification handler code here
	//int uType;	// type to be added (as unit)
	//char cps[20];
	int		uUnit = 0;  // index of newly added unit
	int		i = 0;
	CString cs1;

	m_ins_unit.GetWindowText(cs1);
	if (cs1.GetLength() <= 0) {
		m_status.AddString(L"ERROR- position not enteed.");
		return;
	}
	uUnit = StrToIntW(cs1);

	i = m_ins_type.GetCurSel();

	if (i < 0) {
		m_status.AddString(L"ERROR- type not selected.");
		return;
	}

	i = theApp.GetTypeFromIndex(i);
	if (theApp.InsertUnit(i, uUnit) == 1) {
		// shamelessly copied from ReadSim
		CButton *cbv = (CButton *)GetDlgItem(IDC_VIEW_UNITS);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_VIEW_STATE);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_SAVE_SIM_INFO);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_UNIT_LIST);
		if (cbv != NULL) cbv->EnableWindow(1);
		cbv = (CButton *)GetDlgItem(IDC_READ_DATA);
		if (cbv != NULL) cbv->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_SAVE_TYPE_CROSSWALK))->EnableWindow(1);

		if (unitdlg->GetInit() == 1) theApp.BuildUnitTree(&unitdlg->m_tree);
		if (statedlg->GetInit() == 1)theApp.BuildStateTree(&statedlg->m_tree);
	}

	return;
}


void 
CSimInfoDlg::OnBnClickedUnitList()
{
	// TODO: Add your control notification handler code here
	FILE		*ip = NULL;
	CString		cs1;
	CString		ipFileName;
	int			i = 1;
	int			previ = 0;
	BOOL		fnStat = 0;
	time_t		t = time(0);   // get time now
	tm			now = {};
	
	localtime_s(&now,&t);

	try {
		ofn.lpstrFilter = TEXT("XML Files *.xml\0*.xml\0All Files *.*\0*.*\0\0");
		ofn.lpstrDefExt = L"XML";
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

		_tcscpy_s(ofnBox, 50, L"Save Unit List File");

		cs1 = simFileName;
		while (i > 0) {
			i = cs1.Find('.', previ + 1);
			if (i > 0) {
				previ = i;
			}
		}
		i = previ;

		if (i <= 0) {  // extension . not found
			if (cs1.GetLength() > 0) {
				cs1 += " ";
				cs1 += "unit list.xml";
			} else {
				cs1.Format(L"unit list %02d%02d%02d.xml", now.tm_year - 100, now.tm_mon, now.tm_mday);
			}
		} else {
			cs1 = cs1.Left(i);
			cs1 += +" unit list.xml";
		}

		ipFileName = cs1;
		ofn.lpstrFile = ipFileName.GetBuffer(MAX_PATH);// ipFileName.GetLength());

		fnStat = GetSaveFileName(&ofn);
		if (ofn.lpstrFile[0] == 0) {
			m_status.AddString(L"Unit List file name not entered.");
			return;
		}
		if (fnStat == 0) {
			cs1.Format(L"Error getting save file name!");
			return;
		}
		if (wcslen(ofn.lpstrFile) < 3) {
			cs1.Format(L"Error getting save file name- string not returned!");
			return;
		}

		cs1.Format(L"Entered file '%ls'", ofn.lpstrFile);
		m_status.AddString(cs1);

		_wfopen_s(&ip, ofn.lpstrFile, L"w");

		// reset to avoid crash MAG 200113
		ofn.lpstrFile = LPWSTR(ofnFile);
	}
	catch (...) {
		cs1.Format(L"ERROR caught saving unit list.");
	}

	if (ip == NULL) {
		m_status.AddString(L"Error saving Unit List file");
		m_status.SetCurSel(m_status.GetCount() - 1);
		MessageBox(L"Error saving Unit List file", L"File NOT Saved", MB_OK);
		return;
	}

	try {
		if(theApp.SaveUnitListFile(ip) == -1) MessageBox(L"Error saving Unit List file", L"File NOT Saved", MB_OK);
		fclose(ip);
	}
	catch (...) {
		cs1.Format(L"ERROR caught after SaveUnitListFile");
	}

	return;
}  // end CSimInfoDlg::OnBnClickedUnitList()


void 
CSimInfoDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if (theApp.GetSavedState() == 1){
		if (MessageBox(L"There are unsaved changes to the model. Do you want to save them?", L"Model data not saved", MB_YESNO) == IDYES) return;
	}

	CDialogEx::OnCancel();
}


void 
CSimInfoDlg::OnBnClickedMon1()
{
	// TODO: Add your control notification handler code here
	currentMonitor = 0;
	if(dtdlg->GetInit() == 1) OnBnClickedViewTypes();
	if (unitdlg->GetInit() == 1) OnBnClickedViewUnits();
	if (statedlg->GetInit() == 1)OnBnClickedViewState();

	return;
}


void 
CSimInfoDlg::OnBnClickedMon2()
{
	// TODO: Add your control notification handler code here
	currentMonitor = 1;
	if (dtdlg->GetInit() == 1) OnBnClickedViewTypes();
	if (unitdlg->GetInit() == 1) OnBnClickedViewUnits();
	if (statedlg->GetInit() == 1)OnBnClickedViewState();

	return;
}


void 
CSimInfoDlg::OnBnClickedMon3()
{
	// TODO: Add your control notification handler code here
	currentMonitor = 2;
	if (dtdlg->GetInit() == 1) OnBnClickedViewTypes();
	if (unitdlg->GetInit() == 1) OnBnClickedViewUnits();
	if (statedlg->GetInit() == 1)OnBnClickedViewState();

	return;
}


void 
CSimInfoDlg::OnBnClickedSaveUal()
{
	// TODO: Add your control notification handler code here
	FILE		*ip = NULL;
	CString		cs1;
	CString		ipFileName;
	int			i = 1;
	int			previ = 0;
	BOOL		fnStat = 0;
	time_t		t = time(0);   // get time now
	tm			now = {};

	localtime_s(&now, &t);

	try {
		ofn.lpstrFilter = TEXT("UAL Files *.ual\0*.ual\0All Files *.*\0*.*\0\0");
		ofn.lpstrDefExt = L"UAL";
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

		_tcscpy_s(ofnBox, 50, L"Save Unit Assignment List File");

		cs1 = simFileName;
		while (i > 0) {
			i = cs1.Find('.', previ + 1);
			if (i > 0) {
				previ = i;
			}
		}
		i = previ;

		if (i <= 0) {  // extension . not found
			if (cs1.GetLength() > 0) {
				cs1 += " ";
				cs1 += "unit asignment list.ual";
			} else {
				cs1.Format(L"unit assignment list %02d%02d%02d.ual", now.tm_year - 100, now.tm_mon, now.tm_mday);
			}
		} else {
			cs1 = cs1.Left(i);
			cs1 += +" unit assignment list.ual";
		}

		ipFileName = cs1;

		fnStat = GetSaveFileName(&ofn);
		if (ofn.lpstrFile[0] == 0) {
			m_status.AddString(L"Unit Assignment List file name not entered.");
			return;
		}
		if (fnStat == 0) {
			cs1.Format(L"Error getting save file name!");
			return;
		}
		if (wcslen(ofn.lpstrFile) < 3) {
			cs1.Format(L"Error getting save file name- string not returned!");
			return;
		}

		cs1.Format(L"Entered file '%ls'", ofn.lpstrFile);
		m_status.AddString(cs1);

		_wfopen_s(&ip, ofn.lpstrFile, L"w");

		// reset to avoid crash MAG 200113
		//ofn.lpstrFile = LPWSTR(ofnFile);
	}
	catch (...) {
		cs1.Format(L"ERROR caught saving unit assignment list.");
	}

	if (ip == NULL) {
		m_status.AddString(L"Error saving Unit Assignment List file");
		m_status.SetCurSel(m_status.GetCount() - 1);
		MessageBox(L"Error saving Unit List file", L"File NOT Saved", MB_OK);
		return;
	}

	try {
		if (theApp.SaveUALFile(ip) == -1) {
			MessageBox(L"Error saving Unit Assignment List file", L"File NOT Saved", MB_OK);
		}
		else {

			m_status.AddString(L"The Unit Assignment List file has been saved.");
		}
		fclose(ip);
	}
	catch (...) {
		cs1.Format(L"ERROR caught after SaveUALFile");
	}

	return;
} // end CSimInfoDlg::OnBnClickedSaveUal()


void CSimInfoDlg::OnBnClickedMinimizeState()
{
	// TODO: Add your control notification handler code here
	if (MessageBox(L"Continue to compress the state array?", L"Verify compress state array", MB_YESNO) == IDNO) return;
	theApp.MinimizeState();

	if (unitdlg->GetInit() == 1) theApp.BuildUnitTree(&unitdlg->m_tree);
	if (statedlg->GetInit() == 1)theApp.BuildStateTree(&statedlg->m_tree);

}
