#pragma once
#ifndef DEF_VALUE_DIALOG
#define DEF_VALUE_DIALOG

#include "afxwin.h"


// Value dialog

class ValueDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ValueDlg)

public:
	ValueDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ValueDlg();
	CString GetValueString() { return m_value_cs; };
	void SetValueString(CString ips) { m_value_cs = ips; };
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VALUE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CEdit m_value_edit;
	CString m_value_cs;
};

#endif // DEF_VALUE_DIALOG