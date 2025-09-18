// MTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SimInfo.h"
#include "MTreeCtrl.h"
#include "Value.h"

// MTreeCtrl

IMPLEMENT_DYNAMIC(MTreeCtrl, CTreeCtrl)

MTreeCtrl::MTreeCtrl()
{
	type = 0;
	dragStart = 0;
	dragEnd = 0;
	lastSelectedType = 0;
	lastSelectedUnit = 0;
	lastSelectedState = 0;
	shiftDown = 0;
	fontSize = 12;
	SetWindowTheme(this->GetSafeHwnd(), L"Explorer", NULL);
}

MTreeCtrl::~MTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(MTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &MTreeCtrl::OnTvnSelchanged)
	ON_WM_RBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &MTreeCtrl::OnTvnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &MTreeCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()


// MTreeCtrl message handlers


void 
MTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hti = pNMTreeView->itemNew.hItem;
	static int busy = 0;
	// search through type and unit arrays to find what's been clicked
	int i = 0;

	if (busy == 1) {
		return;
	}
	busy = 1;

	switch (type) {
		case 0:		// TYPE window
			lastSelectedType = hti; 
			i = theApp.FindTypeTreePos(hti);
			break;
		case 1:		// UNIT window
			lastSelectedUnit = hti; 
			i = theApp.FindUnitTreePos(hti);  // selects correct type in type window, or not
			break;
		case 2:		// STATE window
			lastSelectedState = hti; 
			i = theApp.FindStateTreePos(hti);
			break;
		default: 
			break;
	}

	busy = 0;
	*pResult = 0;

	return;
}


void
MTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//*pResult = 0;
	CTreeCtrl::OnRButtonUp(nFlags, point);
}


void 
MTreeCtrl::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	// Get the cursor position 
	DWORD dwPos = GetMessagePos();

	// Convert the position into a CPoint
	CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
	CPoint spt;

	spt = pt;

	// Convert to screen location for hittest to work
	ScreenToClient(&spt);

	UINT test = 0;
	HTREEITEM hti = HitTest(spt, &test);

	if (hti != NULL)
	{
		// Was the click actually on an item?
		if (test & TVHT_ONITEM) {
			switch (type) {
			case 1: // UNIT
				//theApp.InsertUnit(hti);
				break;
			case 2: // STATE
				dragStart = hti;
				//theApp.InsertState(hti);
				break;
			default: return;
			}
		}
	}

	*pResult = 0;

	return;
}


void 
MTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// Get the cursor position 
	DWORD dwPos = GetMessagePos();

	// Convert the position into a CPoint
	CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
	CPoint spt;
	int unitIndex = 0;
	HVUnit *hvu = nullptr;

	spt = pt;

	// Convert to screen location for hittest to work
	ScreenToClient(&spt);

	UINT test;
	HTREEITEM hti = HitTest(spt, &test);

	if (hti != NULL){
		// Was the click actually on an item?
		if (test & TVHT_ONITEM) {
			switch (type) {
			case 1: // UNIT
				dragStart = 0;
				break;
			case 2: // STATE
				if (dragStart == 0) break;;
				dragEnd = hti;
				if (theApp.DragState(dragStart, dragEnd) == 1) {
					if (theApp.csdlg->GetUnitdlg()->GetInit() == 1) {
						theApp.BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);

						unitIndex = theApp.GetUnitFromState(dragStart);
						if (unitIndex >= 0) {
							hvu = theApp.GetUnitRef(unitIndex);
							if (hvu != NULL)
								theApp.csdlg->GetUnitdlg()->m_tree.Expand(hvu->GetTreepos(), TVE_EXPAND);
						}
					}
					if (theApp.csdlg->GetStatedlg()->GetInit() == 1) {
						theApp.BuildStateTree(&theApp.csdlg->GetStatedlg()->m_tree);
						theApp.csdlg->GetStatedlg()->m_tree.Expand(dragStart, TVE_EXPAND);
					}
				}
				break;
			default: 
				dragStart = 0;
				break;
			}
		}
	}
	theApp.csdlg->m_status.SetCurSel(theApp.csdlg->m_status.GetCount() - 1);

	CTreeCtrl::OnLButtonUp(nFlags, point);

	return;
}


void 
MTreeCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// Get the cursor position 
	DWORD dwPos = GetMessagePos();

	// Convert the position into a CPoint
	CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
	CPoint spt;

	int changeStatus = 0;

	spt = pt;

	// Convert to screen location for hittest to work
	ScreenToClient(&spt);

	UINT test = 0;
	HTREEITEM hti = HitTest(spt, &test);

	if (hti != NULL){
		// Was the click actually on an item?
		if (test & TVHT_ONITEM) {
			switch (type) {
			case 1: // UNIT
				changeStatus = theApp.InsertUnit(hti);
				break;
			case 2: // STATE
				changeStatus = theApp.InsertState(hti);
				break;
			default: return;
			}
			if (changeStatus == 1) {
				theApp.csdlg->m_status.AddString(L"NOTE- add new State element.");
				theApp.csdlg->m_status.SetCurSel(theApp.csdlg->m_status.GetCount() - 1);

				if (theApp.csdlg->GetUnitdlg()->GetInit() == 1) theApp.BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);
				if (theApp.csdlg->GetStatedlg()->GetInit() == 1) theApp.BuildStateTree(&theApp.csdlg->GetStatedlg()->m_tree);
			}
		}
	}

	CTreeCtrl::OnRButtonDblClk(nFlags, point);

	return;
}


/* NOTE:
List of key bindings:
Type Window:
none

Unit Window:
b,B: swap boundary state
s: decrement superblock
S: increment superblock
d: decrement block
D: increment block
r,R: change unit reporting
v,V: change value
x,X: delete unit

State Window
*/
// MAG- this might be better done by handling KeyPress events instead? But this works.
// NOTE- some key bindings are still in progress but are commented out until complete
void 
MTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int				i = 0;
	size_t			countIn = 0;
	size_t			countOut = 0;
	int				bCur = 0;		// current state
	size_t			sb = 1;
	CString			cs1;
	CString			inputLabel;
	StateElement	*myse = nullptr;
	HTREEITEM		curState = nullptr;
	CEdit			*myce = nullptr;
	CFont			*myfont = nullptr;
	CFont			lcfo;

	if (nChar == 16) {
		shiftDown = 1;
		return;
	}

	if (nChar > 'z') { // 'z' == 122 - this keypress could be anything- num pad, F key, other non-character keys...
		//cs1.Format(L"A key was presssed that has no functionality.");
		//theApp.csdlg->m_status.AddString(cs1);
		//theApp.csdlg->m_status.SetCurSel(theApp.csdlg->m_status.GetCount() - 1);
		shiftDown = 0;
		return;
	}
	// TODO: Add your message handler code here and/or call default
	// NOTE: do not return from switch! use break; Set shiftDown=0 if return is required.
	switch (type) {
		case 0: // TYPE
			i = theApp.ASCIIoffset(nChar, shiftDown);// (shiftDown == 1) ? 0 : 32;
			if (i == 0xFF) break;
			// TODO- finish code to change font size when + or - key is hit
			
			switch (nChar) {
			case 107: // keypad + NOTE this is not working properly, llf is not reading the correct font
			case '+':
				//theApp.csdlg->dtdlg->m_tree.GetFont()->GetLogFont(&llf);
				//llf.lfHeight++;
				//lcfo.CreateFontIndirect(&llf);
				if(fontSize > 30) break;
				fontSize++;
				lcfo.CreatePointFont(fontSize, _T("Segoe UI"));
				theApp.csdlg->Getdtdlg()->m_tree.SetFont(&lcfo);
				theApp.csdlg->Getdtdlg()->m_tree.Invalidate();
				break;
			case 109: // keypad -
			case '-':
				//theApp.csdlg->dtdlg->m_tree.GetFont()->GetLogFont(&llf);
				//llf.lfHeight--;
				//lcfo.CreateFontIndirect(&llf);
				if (fontSize <= 6) break;
				fontSize--;
				lcfo.CreatePointFont(fontSize, _T("Segoe UI"));
				theApp.csdlg->Getdtdlg()->m_tree.SetFont(&lcfo);
				theApp.csdlg->Getdtdlg()->m_tree.Invalidate();
				break;
				//	theApp.csdlg->dtdlg->m_tree->
				//	myce = theApp.csdlg->dtdlg->m_tree.GetEditControl();
				//	myfont = myce->GetFont();
				//}
			default:
				if (nFlags & KF_EXTENDED) break;	// catch alt, ctrl, page up/down, arrow keys, etc.
				cs1.Format(L"The '%c' key was presssed, but has no functionality.", nChar + i);
				theApp.csdlg->m_status.AddString(cs1);
			}
			shiftDown = 0;
			break;
		case 1: // UNIT
			if (lastSelectedUnit == 0) break;
			switch (nChar) {
			case 'b':
			case 'B':
				//if ((nChar == 'b') || (nChar == 'B')) {	// swap boundary state. Only set to true if no outputs and 1+ inputs
				i = theApp.FindUnitTreePos(lastSelectedUnit);
				curState = theApp.GetStateLinkFromUnitLink(lastSelectedUnit);
				if (curState > 0) {
					lastSelectedState = curState;
					theApp.csdlg->GetStatedlg()->m_tree.OnKeyDown(nChar, nRepCnt, nFlags);
				}
				break;
			case 's':
			case 'S':
				if (shiftDown) {
					theApp.BumpSuperBlocks(lastSelectedUnit);
				} else {
					theApp.DecrSuperBlocks(lastSelectedUnit);
				}
				break;
			case 'd':
			case 'D':
				if (shiftDown) {
					theApp.BumpBlocks(lastSelectedUnit);
				} else {
					theApp.DecrBlocks(lastSelectedUnit);
				}
				break;
			case 'r':
			case 'R':
				theApp.ChangeUnitReporting(lastSelectedUnit);
				break;
			case 'v':
			case 'V':
				theApp.ChangeUnitValue(lastSelectedUnit);
				break;
			case 'x':
			case 'X':
				if (theApp.DeleteUnit(lastSelectedUnit) == 1) {
					theApp.BuildState();
					theApp.BuildStateTree(NULL);
					theApp.BuildUnitTree(NULL);
				}
				break;
			default:
				i = theApp.ASCIIoffset(nChar, shiftDown);// (shiftDown == 1) ? 0 : 32;
				if (i == 0xFF) break;
				if (nFlags & KF_EXTENDED) break;	// catch alt, ctrl, page up/down, arrow keys, etc.
				cs1.Format(L"The '%c' key was presssed, but has no functionality.", nChar + i);
				theApp.csdlg->m_status.AddString(cs1);
				break;
			}
			
			break;	// end case UNIT
		case 2: // STATE (b=boundary state, r=reporting state, v=value, m=move to state position
			if (lastSelectedState == 0) break;
			switch (nChar) {
				case 'b':
				case 'B':
					//if ((nChar == 'b') || (nChar == 'B'))
				{	// swap boundary state. Only set to true if no outputs and 1+ inputs
					i = theApp.FindStateTreePos(lastSelectedState);
					myse = theApp.StateVec(i + 1);
					if (myse == NULL) break;
					if (myse->GetCategoryIndex() == 0) {
						theApp.csdlg->m_status.AddString(L"Error: can not change status to boundary for category index 0.");
						break;
					}
					curState = myse->GetTreepos();

					bCur = myse->IsBoundary();
					if (bCur > 0) {		// can always set to not-boundary
						theApp.csdlg->m_status.AddString(L"Note: change status to NOT boundary.");
						/*
						cs1 = GetItemText(curState);
						i = cs1.Find(L"(B)",0);
						//if (cs1.Right(3).Compare(L"(B)") == 0) {	// remove (B) ending on label
						if(i > 0){
							// check for (R)
							if (cs1.Find(L"(R)") > 0) { // leave extra space in front of (R)
								cs1.Delete(i, 3);
							} else {					// delete after value, including extra space
								cs1.Delete(i - 1, 4);
							}
							//cs1 = cs1.Left(cs1.GetLength() - 4);	// don't forget the space
							SetItemText(curState, cs1);
						}
						*/
						if (myse->SetBoundary(0) == 1) {
							theApp.SetSavedState(1);
						}
						cs1 = theApp.BuildStateTreeLabel(myse);
						SetItemText(curState, cs1);

						break;
					}
					countIn = myse->GetInputCount();
					countOut = myse->GetOutputCount();
					if (countOut != 0) {
						theApp.csdlg->m_status.AddString(L"Error: can not change status to boundary- outputs detected.");
						break;
					}
					if (countIn == 0) {
						theApp.csdlg->m_status.AddString(L"Error: can not change status to boundary- no inputs detected.");
						break;
					}
					if (myse->SetBoundary(1) == 1) {
						theApp.SetSavedState(1);
						theApp.csdlg->m_status.AddString(L"Note: Change status to boundary.");
					} else {
						theApp.csdlg->m_status.AddString(L"Note: Error changing status to boundary.");
					}

					cs1 = theApp.BuildStateTreeLabel(myse);

					/*cs1 = GetItemText(curState);
					// check for (R)- insert (B) before, or add to end if (R) not found
					j = cs1.Find(L"(R)");
					if (j >= 0) {
						cs1.Insert(j, L"(B)");
					} else {
						cs1 += " (B)";
					}*/
					SetItemText(curState, cs1);

					/*i = _ttoi(cs1.Mid(12));
					// get input 1 label
					myse->GetInputInfo(0, &unit, &unitIndex);
					theApp.units[unit].GetType();

					cs1.Format(L"State Array: %d; Category: [%d:%d]; Input to: %d; Output to: %d; Label: %S; Value: %5.2f (B)",i, myse->GetCategory(), myse->GetCategoryIndex(), myse->GetInputCount(), myse->GetOutputCount(), ss1.c_str(), myse->GetInitValue());
					//inputLabel =
					*/
					break;
				}
				// MAG 200720 add reporting capability
				case 'r':
				case 'R':
					//if ((nChar == 'r') || (nChar == 'R'))
				{	// swap reporting state. Currently only sets to SB 1
					i = theApp.FindStateTreePos(lastSelectedState);
					myse = theApp.StateVec(i + 1);
					if (myse == NULL) break;
					if (myse->GetCategoryIndex() == 0) {
						// NOTE- there is no state element for category headers. Here myse points to generic unused state.
						i = theApp.FindStateTreeCategory(lastSelectedState);
						theApp.StateVecSetCategoryReporting(i, -1);  // flip-flop from reporting state of first element
						theApp.csdlg->m_status.AddString(L"Note: change reporting status for selected category.");
						break;
					}

					// MAG 201112 TODO- need to determine SB of selected Unit to determine SB to send to IsReported(int)
					curState = theApp.csdlg->GetUnitdlg()->m_tree.lastSelectedUnit;
					if (curState != 0) {
						i = theApp.FindUnitTreeArrPos(curState);
						sb = theApp.GetUnitSBlock(i);
					}

					if (myse->IsReported(sb) == 0) {
						theApp.StateVecSetReported(myse->GetStateIndex(), sb, 1);
						theApp.csdlg->m_status.AddString(L"Note: change status to reported.");
					}
					else {
						theApp.StateVecSetReported(myse->GetStateIndex(), sb, 0);
						theApp.csdlg->m_status.AddString(L"Note: change status to NOT reported.");
					}
					cs1 = theApp.BuildStateTreeLabel(myse);
					SetItemText(myse->GetTreepos(), cs1);
				}
				break;
				case 'v':
				case 'V':
					theApp.ChangeStateValue(lastSelectedState);
					break;
				case 'm':
				case 'M':
					if (theApp.MoveStateValue(lastSelectedState) == 1) {
						if (theApp.csdlg->GetUnitdlg()->GetInit() == 1) {
							theApp.BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);

							int unitIndex = theApp.GetUnitFromState(dragStart);
							if (unitIndex >= 0) {
								HVUnit* hvu = theApp.GetUnitRef(unitIndex);
								if (hvu != NULL)	theApp.csdlg->GetUnitdlg()->m_tree.Expand(hvu->GetTreepos(), TVE_EXPAND);
							}
						}
						if (theApp.csdlg->GetStatedlg()->GetInit() == 1) {
							theApp.BuildStateTree(&theApp.csdlg->GetStatedlg()->m_tree);
							theApp.csdlg->GetStatedlg()->m_tree.Expand(dragStart, TVE_EXPAND);
						}
					}

					break; // end case M
				}
				break;	// end case STATE
		default:	// this would represent an error condition
			i = theApp.ASCIIoffset(nChar, shiftDown);// (shiftDown == 1) ? 0 : 32;
			if (i == 0xFF) break;
			if (nFlags & KF_EXTENDED) break;	// catch alt, ctrl, page up/down, arrow keys, etc.
			cs1.Format(L"The '%c' key was presssed, but has no functionality.", nChar + i);
			theApp.csdlg->m_status.AddString(cs1);
			break;
	} // end case type

	shiftDown = 0;
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	theApp.csdlg->m_status.SetCurSel(theApp.csdlg->m_status.GetCount() - 1);

	return;
}	// end OnKeyDown


void
MTreeCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	NMTVCUSTOMDRAW *pcd = (NMTVCUSTOMDRAW   *)pNMHDR;
	switch (pcd->nmcd.dwDrawStage){
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;

		case CDDS_ITEMPREPAINT:
			HTREEITEM   hItem = (HTREEITEM)pcd->nmcd.dwItemSpec;

			if (this->GetSelectedItem() == hItem){
				pcd->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
				pcd->clrTextBk = GetSysColor(COLOR_HIGHLIGHT);
			}

			*pResult = CDRF_DODEFAULT;// do not set *pResult = CDRF_SKIPDEFAULT
			break;
		//default:
		//	break;
	}

	return;
}
