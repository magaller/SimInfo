#pragma once

#ifndef DEF_STATEMOVE_DLG
#define DEF_STATEMOVE_DLG


#include "afxwin.h"


// copied from Value dialog

class StateMoveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(StateMoveDlg)

public:
	StateMoveDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~StateMoveDlg();

	CString GetValue() { return m_value_cs; };
	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VALUE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CEdit m_value_edit;
	CString m_value_cs;

	DECLARE_MESSAGE_MAP()
//public:
};

#endif // DEF_STATEMOVE_DLG
