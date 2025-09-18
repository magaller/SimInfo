// Diagram.cpp : implementation file
//

#include "stdafx.h"

#include "SimInfo.h"
#include "Diagram.h"
#include "afxdialogex.h"


// Diagram dialog

IMPLEMENT_DYNAMIC(Diagram, CDialogEx)

Diagram::Diagram(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIAGRAM, pParent)
{

}

Diagram::~Diagram()
{
}

void Diagram::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Diagram, CDialogEx)
END_MESSAGE_MAP()


// Diagram message handlers
