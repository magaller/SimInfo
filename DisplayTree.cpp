// DisplayTree.cpp : implementation file
//

#include "stdafx.h"
#include "SimInfo.h"
#include "DisplayTree.h"
#include "afxdialogex.h"

// DisplayTree dialog

IMPLEMENT_DYNAMIC(DisplayTree, CDialogEx)

DisplayTree::DisplayTree(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DISPLAYTREE, pParent)
{

}

DisplayTree::~DisplayTree()
{
}

void
DisplayTree::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_MINIMIZE, m_closeButton);
}


BEGIN_MESSAGE_MAP(DisplayTree, CDialogEx)
	ON_BN_CLICKED(IDC_MINIMIZE, &DisplayTree::OnBnClickedMinimize)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// DisplayTree message handlers

BOOL
DisplayTree::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	std::string simn;// = theApp.GetSimName();
	CString cs1;

	switch (m_tree.GetType()){
		case 0: 
			simn = theApp.GetTypesName();
			theApp.BuildTypeTree(&m_tree); 
			cs1.Format(L"Types List Display [%S]",simn.c_str());
			SetWindowText(cs1);
			break;
		case 1: 
			simn = theApp.GetSimName();
			theApp.BuildUnitTree(&m_tree); 
			cs1.Format(L"Units Display: Superblock:Block:Unit [%S]",simn.c_str());
			SetWindowText(cs1);
			break;
		case 2: 
			theApp.BuildStateTree(&m_tree);
			SetWindowText(L"State Array Display");
			break;
		default:
			theApp.BuildTypeTree(&m_tree);
			break;
	}

	isInit = 1;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void
DisplayTree::OnBnClickedMinimize()
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
}


void
DisplayTree::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (isInit == 0) return;
	int dx = 29; // height of bottom gap below tree window 

	m_tree.MoveWindow(0, 0, cx, cy - dx);

	m_closeButton.SetWindowPos(this, 0, cy-28, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}
