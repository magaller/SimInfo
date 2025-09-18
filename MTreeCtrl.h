#pragma once

#ifndef DEF_MTREECTRL_DIALOG
#define DEF_MTREECTRL_DIALOG

class StateElement;
class HVUnit;

// MTreeCtrl

class MTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(MTreeCtrl)

public:
	MTreeCtrl();
	virtual ~MTreeCtrl();
	//LOGFONT mylf;		// MAG 230706
	//CFont	mycf;

	int GetType() { return type; };
	void SetType(int i) { if ((i >= 0) && (i <= 2)) type = i; };


	// Set/Get functions only added as needed
	HTREEITEM GetLastSelectedUnit() { return lastSelectedUnit; };
	void SetLastSelectedUnit(HTREEITEM hti) { if (hti != NULL) lastSelectedUnit = hti; };
	void SetLastSelectedState(HTREEITEM hti) { if (hti != NULL) lastSelectedState = hti; };


protected:
	DECLARE_MESSAGE_MAP()
	int type;  // 0 = type, 1 = units, 2 = state
	int shiftDown; //  was the last key down a shift key- boolean
	int fontSize;

	HTREEITEM dragStart;
	HTREEITEM dragEnd;
	HTREEITEM lastSelectedType;	// MAG 200325 add to support keypress detection
	HTREEITEM lastSelectedUnit;	// MAG 200325 add to support keypress detection
	HTREEITEM lastSelectedState;	// MAG 200325 add to support keypress detection
public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//StateElement *GetState(HTREEITEM hti);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


#endif // DEF_MTREECTRL_DIALOG