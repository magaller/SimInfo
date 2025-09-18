
// SimInfoDlg.h : header file
//

#pragma once

#ifndef DEF_SIMINFODLG_MAIN
#define DEF_SIMINFODLG_MAIN
#include <direct.h>  // for chdir and _getcwd

#include <iostream>
#include <string>
#include <cstddef>
#include <vector>

#include "afxwin.h"
#include "DisplayTree.h"

// CSimInfoDlg dialog
class CSimInfoDlg : public CDialogEx
{
// Construction
public:
	CSimInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMINFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	OPENFILENAME ofn;
	TCHAR ofnDir[MAX_PATH];
	TCHAR ofnFile[1500];
	TCHAR ofnBox[50];
	int ofnInit;

	TCHAR simFileName[MAX_PATH];	// stores simfile name from Read Sim to be used to write the state info file

	std::vector<DISPLAY_DEVICE> monitorList;
	std::vector<DISPLAY_DEVICE> adapterList;
	int							currentMonitor;
	// these are from GetSystemMetrics in init
	int							virtLeft; // coordinate of left side of virtual screen
	int							virtWidth;// width of primary virtual screen (assumes all are the same)
	int							monitorCount; // number of monitors

	DisplayTree *dtdlg;		// types
	DisplayTree* unitdlg;	// units
	DisplayTree* statedlg;	// state array

	CButton m_mon1;
	CButton m_mon2;
	CButton m_mon3;

public:

	DisplayTree* Getdtdlg() { return dtdlg; };		// types
	DisplayTree* GetUnitdlg() {	return unitdlg;	};	// units
	DisplayTree* GetStatedlg() { return statedlg; };// state array

	CListBox m_status;

	afx_msg void OnBnClickedReadTypes();
	afx_msg void OnBnClickedReadSim();
	afx_msg void OnBnClickedViewTypes();
	afx_msg void OnBnClickedViewUnits();
	afx_msg void OnBnClickedViewState();
	void		 EnableTypeButton();

	afx_msg void OnBnClickedPrintTypes();
	afx_msg void OnBnClickedSaveSimInfo();
	afx_msg void OnBnClickedSaveTypeCrosswalk();

	CButton		 m_read_data;
	afx_msg void OnBnClickedReadData();

	afx_msg void OnBnClickedSaveDataCrosswalk();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnBnClickedInsButton();
	
	CComboBox	m_ins_type;
	CEdit		m_ins_unit;
	CButton		m_ins_button;

	afx_msg void OnBnClickedUnitList();
	// superblock : block to insert to (default to previous unit SBB)
	CEdit m_ins_sbb;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedMon1();
	afx_msg void OnBnClickedMon2();
	afx_msg void OnBnClickedMon3();
	afx_msg void OnBnClickedSaveUal();
	afx_msg void OnBnClickedMinimizeState();
};

#endif // DEF_SIMINFO_MAIN
