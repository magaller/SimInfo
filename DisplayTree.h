#pragma once

#ifndef DEF_DISPLAY_TREE_DIALOG
#define DEF_DISPLAY_TREE_DIALOG

#include "afxcmn.h"
#include "afxwin.h"
#include "MTreeCtrl.h"

// DisplayTree dialog

class DisplayTree : public CDialogEx
{
	DECLARE_DYNAMIC(DisplayTree)

public:
	DisplayTree(CWnd* pParent = NULL);   // standard constructor
	virtual ~DisplayTree();
	int	GetInit() { return isInit; };
	void SetInit(int i) { isInit = i; };

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAYTREE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	int isInit = 0;

	DECLARE_MESSAGE_MAP()
public:
	MTreeCtrl m_tree;
	virtual BOOL OnInitDialog();
	//int type;  //0=type, 1=units, 2=state
	afx_msg void OnBnClickedMinimize();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_closeButton;
};
#endif // DEF_DISPLAY_TREE_DIALOG