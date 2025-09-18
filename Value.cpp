// Value.cpp : implementation file
//

#include "stdafx.h"
#include "SimInfo.h"
#include "Value.h"
#include "afxdialogex.h"


// Value dialog

IMPLEMENT_DYNAMIC(ValueDlg, CDialogEx)

ValueDlg::ValueDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VALUE, pParent)
	, m_value_cs(_T(""))
{

}

ValueDlg::~ValueDlg()
{
//	UpdateData(TRUE);
}

void ValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VALUE_EDIT, m_value_edit);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_value_cs);
}


BEGIN_MESSAGE_MAP(ValueDlg, CDialogEx)
END_MESSAGE_MAP()


// Value message handlers
