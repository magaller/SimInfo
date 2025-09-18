// State Move Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "SimInfo.h"
#include "State Move Dialog.h"
#include "afxdialogex.h"


// Value dialog

IMPLEMENT_DYNAMIC(StateMoveDlg, CDialogEx)

StateMoveDlg::StateMoveDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VALUE, pParent)
	, m_value_cs(_T(""))
{

}

StateMoveDlg::~StateMoveDlg()
{
	//	UpdateData(TRUE);
}

void StateMoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VALUE_EDIT, m_value_edit);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_value_cs);
}


BEGIN_MESSAGE_MAP(StateMoveDlg, CDialogEx)
END_MESSAGE_MAP()


// Value message handlers
