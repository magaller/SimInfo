#pragma once

#ifndef DEF_DIAGRAM_DIALOG
#define DEF_DIAGRAM_DIALOG

// Diagram dialog

class Diagram : public CDialogEx
{
	DECLARE_DYNAMIC(Diagram)

public:
	Diagram(CWnd* pParent = NULL);   // standard constructor
	virtual ~Diagram();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIAGRAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
#endif // DEF_DIAGRAM_DIALOG