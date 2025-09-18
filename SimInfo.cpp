
// SimInfo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SimInfo.h"
#include "C:\Users\mikeg\xml\mxml\mxml.h"
#include <numeric>
//#include <amp_graphics.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//#include "SimInfoDlg.h"

// CSimInfoApp

BEGIN_MESSAGE_MAP(CSimInfoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CSimInfoApp construction

CSimInfoApp::CSimInfoApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSimInfoApp object

CSimInfoApp theApp;

// CSimInfoApp initialization

BOOL 
CSimInfoApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// MAG add some initialization
	std::vector<StateElement>	sev;
	std::vector<int>			sei;
	std::size_t i = 0;
	std::size_t stz = 0;

	for (i = 0; i < 9; i++){
		state.push_back(sev); // create state vectors
		maxCat[i] = 0;
	}

	// only resize 1-8
	for (i = 1; i < 9; i++){
		state[i].resize(STATESIZE);
		state[i][stz].SetCategory((int)i);
	}

	categoryLabels.push_back("NA");  // filler for spot zero
	categoryLabels.push_back("Pressure");
	categoryLabels.push_back("Mass flow rate");
	categoryLabels.push_back("Temperature");
	categoryLabels.push_back("Control");
	categoryLabels.push_back("Other");
	categoryLabels.push_back("Energy");
	categoryLabels.push_back("Power");
	categoryLabels.push_back("Humidity");

	unsavedChanges = 0;

	CSimInfoDlg dlg;
	m_pMainWnd = &dlg;
	csdlg = &dlg;

	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
} // end InitInstancew


int 
CSimInfoApp::DoBGStuff(void)
{
	LONG lIdle = 0;
	MSG msg;  // MAG added for message handling code below
	int bDoingBackgroundProcessing;  // this too

	try {
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!AfxGetApp()->PumpMessage())
			{
				bDoingBackgroundProcessing = FALSE;
				::PostQuitMessage(0);
				break;
			}
		}
	}
	catch (...) {
		return(0);
	}
	// let MFC do its idle processing
	try {
		while (AfxGetApp()->OnIdle(lIdle++));  		// Perform some background processing here 
	}
	catch (...) {
		return(0);
	}
	return(1);
}


// MAG add function FixString.
// Purpose: Removes extra spaces from input strings, front and back.
// Returns:	char pointer to location in ips- should be first non-space character.
// Parameters:
//    ips: pointer to null terminated char array to be modified
char 
*CSimInfoApp::FixString(char *ips) 
{
	char	*cp = nullptr;
	int		i = 0;
	size_t	j = 0;

	if (ips == nullptr) return ips;

	// check for leading spaces, advance pointer accordingly
	cp = ips;
	while (*cp == ' ') cp++;
	if (*cp == '\'') cp++;

	// check for spaces on the end of the array
	j = strlen(cp) - 1;// minus one due to \n
	if (cp[j - 1] == '\'') {
		cp[j - 1] = 0;
		j--;
	}
	while (cp[j - 1] == ' ') {
		cp[j - 1] = 0;
		j--;
	}

	// change commas to semicolons, to work better with CSV files
	for (i = 0; i < strlen(cp); i++){
		if (cp[i] == ',') 
			cp[i] = ';';
	}

	return cp;
}


// MAG add function GetCategoryLabel.
// Purpose: Returns the label for the given category, or "".
// Returns: std::string with content, or "" on error.
// Parameters:
//    index: the index inthe categoryLabels array to retrieve
std::string 
CSimInfoApp::GetCategoryLabel(size_t index) 
{
	if (categoryLabels.size() == 0) return "";
	if (index < 1) return categoryLabels[0];
	if (index >= categoryLabels.size()) return categoryLabels[0];
	
	return categoryLabels[index];
}


// MAG add function GetTypeLabel.
// Purpose: Returns the label for the given Type, or "".
// Returns: std::string with content, or "" on error.
// Parameters:
//    typeID: the index for the type requested 
std::string
CSimInfoApp::GetTypeLabel(int typeID)
{
	std::vector<HVType>::iterator hvi;

	if (typeID < 0) return "";
	if (types.size() == 0) return "";
	if (typeID > types.back().GetType()) return "";

	for ( hvi = types.begin(); hvi != types.end(); hvi++){
		if (hvi->GetType() == typeID) {
			return hvi->GetTypeLabel();
		}
	}

	return "";
}


// MAG add function SetTypeFileName.
// Purpose: Returns the label for the given Type, or "".
// Returns: std::string with content, or "" on error.
void
CSimInfoApp::SetTypesFileName(char *ipf)
{
	typeName = ipf;
}


// MAG add function GetTypeRef.
// Purpose: Returns pointer for the given Type, or NULL.
// Returns: Pointer to HVType class with info, or NULL on error.
HVType
*CSimInfoApp::GetTypeRef(int typeID)
{
	std::vector<HVType>::iterator hvi;

	if (typeID < 0) return NULL;
	if (types.size() == 0) return NULL;
	if (typeID > types.back().GetType()) return NULL;

	for (hvi = types.begin(); hvi != types.end(); hvi++) {
		if (hvi->GetType() == typeID) {
			return hvi._Ptr;
		}
	}

	return NULL;
}


// MAG add function GetUnitRef. Shamelessly copied from GetTypeRef
// Purpose: Returns pointer for the given Unit, or NULL.
// Returns: Pointer to HVUnit class with info, or NULL on error.
// NOTE- input is unit array, 
HVUnit
*CSimInfoApp::GetUnitRef(size_t typeID)
{
	std::vector<HVUnit>::iterator hvi;

	if (typeID < 0) return NULL;
	if (units.size() == 0) return NULL;
	if (typeID >= units.size()) return NULL;

	return &units[typeID];
}


// MAG 200306
// hti must be an input or output info node from the state window.
// returns: index of the unit indicated by hti
int 
CSimInfoApp::GetUnitFromState(HTREEITEM hti)
{
	size_t	ioc = 0;
	size_t	stvar = 0;
	size_t	stateIndex = 0;

	for (size_t i = 1; i < state.size(); i++) {  // scan through categories, should be in array 1-8
		for (size_t j = 0; j < state[i].size(); j++) {
			if (state[i][j].GetTreepos() == hti) return -1;	// unit node
			if (state[i][j].GetTreeposInput() == hti) return -1;	// input node
			if (state[i][j].GetTreeposOutput() == hti) return -1;	// output node

			ioc = state[i][j].GetInputCount();
			for (std::size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetInputTreelink(k) == hti) { // found it!
					state[i][j].GetInputInfo(k, &stvar, &stateIndex);
					return (int) stvar;
				}
			}

			ioc = state[i][j].GetOutputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetOutputTreelink(k) == hti) { // found it!
					state[i][j].GetOutputInfo(k, &stvar, &stateIndex);
					return (int) stvar;
				}
			}
		}
	}

	// If not found, return error.
	return -1;
}


// MAG 200406
// Takes link (HTREEITEM) to item in unit tree (must be input or output item) and returns link to corresponding state item, or 0 on error
HTREEITEM 
CSimInfoApp::GetStateLinkFromUnitLink(HTREEITEM unitID) 
{
	size_t	ioc = 0;
	int		foundUnit = -1;
	int		hitCategory = -1;

	for (size_t i = 0; i < units.size(); i++) {  // scan through categories, should be in array 1-8
		if (units[i].GetTreepos() == unitID) return 0;	// unit node

		ioc = units[i].GetInputCount();
		for (size_t k = 0; k < ioc; k++) {
			if (units[i].GetInputTreeItem(k) == unitID) { // found it!
				hitCategory = units[i].GetTypeP()->GetInputCategory(k);
				return state[hitCategory][units[i].GetInput(k)].GetTreepos();
			}
		}

		ioc = units[i].GetOutputCount();
		for (size_t k = 0; k < ioc; k++) {
			if (units[i].GetTreepos() == unitID) { // found it!
				hitCategory = units[i].GetTypeP()->GetOutputCategory(k);
				return state[hitCategory][units[i].GetOutput(k)].GetTreepos();
			}
		}
	}

	return 0;
}


/*int CSimInfoApp::GetStateIDFromUnitLink(HTREEITEM unitID) {
	size_t ioc;
	int foundUnit = -1;
	int hitCategory = -1;

	for (size_t i = 1; i < units.size(); i++) {  // scan through categories, should be in array 1-8
		if (units[i].treepos == unitID) return -1;	// unit node
		//if (units[i].treeposInput == unitID) return -1;	// input node
		//if (units[i].treeposOutput == unitID) return -1;	// output node

		ioc = units[i].GetInputCount();
		for (size_t k = 0; k < ioc; k++) {
			if (units[i].GetInputTreeItem(k) == unitID) { // found it!
				hitCategory = units[i].GetTypeP()->GetInputCategory(k);
				return state[hitCategory][units[i].GetInput[k]].treepos;
				//return units[i].GetStateLinkFromInput(k);
			}
		}

		ioc = units[i].GetOutputCount();
		for (size_t k = 0; k < ioc; k++) {
			if (units[i].treepos == unitID) { // found it!
				hitCategory = units[i].GetTypeP()->GetOutputCategory(k);
				return state[hitCategory][units[i].GetOutput[k]].treepos;
				//return units[i].GetStateLinkFromOutput(k);
			}
		}
	}
	return 0;

	return 0;
}*/


// MAG add function ReadTypeFile.
// Purpose: Enters the contents of a Type file
// Returns: 1 for success, 0 for error.
// MAG 191218 also fills drop-down for adding units
int
CSimInfoApp::ReadTypeFile(FILE *ip)
{
	char readline[500] = {};	// input from file, to be parsed
	char errorText[500] = {};
	char typeDesc[100] = {};
	char *cp = nullptr;		// character pointer, used for parsing char arrays
	CString cs1;
	std::string ss1;
	int nSaved = 0;
	int nDiffEQ = 0;
	int nIn = 0;
	int nOut = 0;
	int nParam = 0;
	int typeID = 0;
	int iread = 0;
	int linesRead = -1;  // set at -1 due to offset between when incremented and when used in print statements
	int icount = 0;
	int newTypeCount = 0;
	size_t i = 0;
	size_t j = 0;
	HVType newType;
	HVType *hvt = nullptr;
	std::vector<std::string> errorLines;

	if (ip == nullptr) {
		return 0;
	}

	if (units.size() > 0) {  // a simulation work file has already been read. Delete previous unit and state data
		csdlg->m_status.AddString(L"NOTE- Deleting previous simulation data.");
		units.clear();
		for (std::vector<HVType>::iterator hvt = types.begin(); hvt != types.end(); hvt++) hvt->ZeroUseCount();
		for (i = 1; i < 9; i++){
			for (j = 0; j < STATESIZE; j++){
				if (state[i][j].GetCategory() > 0) {
					state[i][j].Reset();
				}
			}
			state[i][0].SetCategory(i);
			maxCat[i] = 0;
		}
	}

	// start reading the file
	while (fgets(readline,500,ip) != NULL){
		linesRead++;

		// The types of lines indicated do not need further processing
		if (strchr(readline, '*') != NULL) continue;
		if (strchr(readline, '#') != NULL) continue;
		if (strlen(readline) < 5 ) continue;
		if (readline[0] == '\n') continue;

		// read type and description
		// typical line: "  1  'Fan or pump                                                             '"
		// Note extra spaces are found in input files
		sscanf_s(readline, "%d %n\n", &typeID, &iread);
		strcpy_s(typeDesc, 100, &readline[iread]);

		if (typeID == 0){
			sprintf_s(errorText,"ERROR (L%d)- find unit number 0", linesRead);
			cs1 = errorText;
			theApp.csdlg->m_status.AddString(cs1);
			errorLines.push_back(errorText);
			//errorCount++;
		}

		cp = FixString(typeDesc);

		newType.SetType(typeID);
		newType.SetLabel(cp);

		// typical line: "  0  0  4  3 12              ! numbers of saved, diff. eq., xin, out, par"
		fgets(readline, 500, ip);	// read saved, diff, in, out, param
		linesRead++;
		sscanf_s(readline, " %d %d %d %d %d", &nSaved, &nDiffEQ, &nIn, &nOut, &nParam);
		if( (nIn < 1)&& (nOut < 1) && (nParam < 1) ){
			cs1.Format(L"FATAL ERROR in DAT file! Format does not match component model information file.");
			theApp.csdlg->m_status.AddString(cs1);
			return 0;
		}

		cs1.Format(L"Read type %03d: %S  nsav %d  ndiff %d  nIn %d  nOut %d  nPar %d",typeID, typeDesc, nSaved, nDiffEQ, nIn, nOut, nParam);
		theApp.csdlg->m_status.AddString(cs1);

		newType.SetCountSaved(nSaved);
		newType.SetCountDiffEQ(nDiffEQ);
		newType.SetCountIn(nIn);
		newType.SetCountOut(nOut);
		newType.SetCountParameter(nParam);

		// read input lines
		for (i = 0; i < nIn; i++){
			// typical line: "  2  'mass flow rate of fluid                                                 '"
			fgets(readline, 500, ip);	// read type and label
			linesRead++;
			sscanf_s(readline, "%d %n\n", &typeID, &iread);
			strcpy_s(typeDesc, 100, &readline[iread]);
			cp = FixString(typeDesc);

			cs1.Format(L"--Input %d Type %d Desc '%S", (int)(i + 1), typeID, cp);
			csdlg->m_status.AddString(cs1);

			newType.AddInput(typeID, cp);
		}

		// read spacer line- line with a '#' so separate sections
		i = 0;
		fgets(readline, 500, ip);	// read type and label
		linesRead++;

		// read any extra lines
		while (readline[0] != '#') {
			sprintf_s(errorText,"NOTE (L%d)- FORMAT ERROR IN COMPONENT MODEL INFORMATION FILE- unexpected line.",linesRead);
			cs1 = errorText;
			theApp.csdlg->m_status.AddString(cs1);
			errorLines.push_back(errorText);
			fgets(readline, 500, ip);	// read type and label
			linesRead++;
			i++;
			if (i > 10) {
				sprintf_s(errorText,"FATAL ERROR in TYPE %d- check format in COMPONENT MODEL INFORMATION FILE", newType.GetType());
				cs1 = errorText;
				theApp.csdlg->m_status.AddString(cs1);
				//errorLines.push_back(errorText);
				return 0;
			}
		}

		// read output lines
		for (i = 0; i < nOut; i ++) {
			// typical line: "  1  'inlet pressure                                                          '"
			fgets(readline, 500, ip);	// read type and label
			linesRead++;
			if (readline[0] == '#') {
				fgets(readline, 500, ip);	// read type and label again- skip spacer line
				linesRead++;
			}

			sscanf_s(readline, "%d %n\n", &typeID, &iread);
			strcpy_s(typeDesc, 100, &readline[iread]);
			cp = FixString(typeDesc);

			cs1.Format(L"--Output %d Type %d Desc '%S", (int)(i + 1), typeID, cp);
			csdlg->m_status.AddString(cs1);

			newType.AddOutput(typeID, cp);
		}

		// read modifier line, if present.
		i = 0;
		fgets(readline, 500, ip);	// read type and label
		linesRead++;
		while(readline[0] != '#') {
			newType.AddModifier(readline);
			fgets(readline, 500, ip);	// load up '# ' seperator line
			linesRead++;
		}

		// check for extra lines
		while (readline[0] != '#') {
			theApp.csdlg->m_status.AddString(L"NOTE- FORMAT ERROR IN TYPAR.DAT- unexpected line:");
			sprintf_s(errorText,"-> %s", readline);
			cs1 = errorText;
			theApp.csdlg->m_status.AddString(cs1);
			errorLines.push_back(errorText);
			fgets(readline, 500, ip);	// read type and label
			linesRead++;
			i++;
			if (i > 10) {
				sprintf_s(errorText,"FATAL ERROR in TYPE %d- check format in TYPAR.DAT", newType.GetType());
				cs1 = errorText;
				theApp.csdlg->m_status.AddString(cs1);
				errorLines.push_back(errorText);
				return 0;
			}
		}

		// read parameter lines
		for (i = 0; i < nParam; i ++) {
			// typical format : "  1  '1st pressure coefficient                                                '"
			fgets(readline, 500, ip);	// read type and label
			linesRead++;
			if (readline[0] == '#') {
				fgets(readline, 500, ip);	// read type and label again- skip spacer line
				linesRead++;
			}

			sscanf_s(readline, "%d %n\n", &typeID, &iread);
			cp = strstr(&readline[iread], "     ");
			if (cp != NULL) {
				*cp = 0;
			}
			strcpy_s(typeDesc, 100, &readline[iread]);
		
			cp = FixString(typeDesc);

			cs1.Format(L"--Parameter %d Type %d Desc '%S", (int)(i + 1), typeID, cp);
			csdlg->m_status.AddString(cs1);

			newType.AddParameter(typeID, cp);
		}

		// read spacer line
		i = 0;
		fgets(readline, 500, ip);	// line should start with '*' and is probably a bunch of them
		linesRead++;
		while (readline[0] != '*') {  // read up to 10 lines searching for spacer line
			cp = fgets(readline, 500, ip);
			linesRead++;
			if (cp == nullptr) break; // end of file
			i++;
			if (i > 10) {
				cs1.Format(L"FATAL ERROR in TYPE %d- check format in TYPAR.DAT", newType.GetType());
				sprintf_s(errorText, "NOTE (L%d) - FATAL ERROR IN TYPE %d- check format in TYPAR.DAT.", linesRead,newType.GetType());
				cs1 = errorText;
				theApp.csdlg->m_status.AddString(cs1);
				errorLines.push_back(errorText);
				return 0;
			}
		}

		typeID = newType.GetType();

		icount = GetTypeFileParameterCount(typeID);
		newType.SetCountFileParameter(icount);

		// check if this type exists already
		ss1 = GetTypeLabel(typeID);
		if (ss1.length() > 0) {
			cs1.Format(L"NOTE- Type %d is already recorded.", typeID);
			csdlg->m_status.AddString(cs1);
			hvt = GetTypeRef(typeID);
			if (hvt == nullptr) continue;
			if (newType.GetCountIn() != hvt->GetCountIn()){
				csdlg->m_status.AddString(L"ERROR in TYPE input- Different number of inputs than previously entered.");
				errorLines.push_back("ERROR in TYPE input- Different number of inputs than previously entered.");
			}
			if (newType.GetCountOut() != hvt->GetCountOut()) {
				csdlg->m_status.AddString(L"ERROR in TYPE output- Different number of outputs than previously entered.");
				errorLines.push_back("ERROR in TYPE output- Different number of outputs than previously entered.");
			}
			if (newType.GetCountParameter() != hvt->GetCountParameter()) {
				csdlg->m_status.AddString(L"ERROR in TYPE parameters- Different number of parameters than previously entered.");
				errorLines.push_back("ERROR in TYPE parameters- Different number of parameters than previously entered.");
			}
		} else {	// found a new unrecorded type, yay
			types.push_back(newType);
			newTypeCount++;		// informative when reading multiple type files
			// add line to types drop-down
			cs1.Format(L"%d", newType.GetType());
			csdlg->m_ins_type.AddString(cs1);
		}

		newType.Reset();
	}  // end loop reading in types

	cs1.Format(L"Completed entering types file, find %zu Types.", types.size());
	csdlg->m_status.AddString(cs1);

	cs1.Format(L"NOTE- found %d new types and %zu errors in file.", newTypeCount, errorLines.size());
	csdlg->m_status.AddString(cs1);

	for (std::string& strname: errorLines){
		cs1 = CString(strname.c_str());
		csdlg->m_status.AddString(cs1);
	}

	csdlg->m_status.SetCurSel(csdlg->m_status.GetCount() - 1);
	
	if (csdlg->Getdtdlg()->GetInit() == 1) BuildTypeTree(&(csdlg->Getdtdlg()->m_tree));

	if (csdlg->m_ins_type.GetCount() > 0) csdlg->m_ins_type.SetCurSel(0);

	return 1;
} // end ReadTypeFile


// MAG 190701
// MAG add function StateVec.
// Purpose: convert single state index to category divided state used internally
// Returns: Pointer to desired StateElement, or NULL
StateElement 
*CSimInfoApp::StateVec(int vecID)
{
	size_t vr = 0;
	size_t category = 0;

	if (vecID < 0) return NULL;	// case of vecID too large is handled by loop

	vr = vecID;
	for (category = 1; category < 9; category++) {
		if (vr <= maxCat[category]) return( &state[category][vr]);	// Found it!
		vr -= maxCat[category];
	}

	return NULL;	// nope.
}


StateElement
* CSimInfoApp::StateVec(size_t vecID)
{
	int iid = (int)vecID;

	return StateVec(iid); // range checking done in subroutine
}


// MAG 211013
// MAG add function StateVec(typeID, vecID).
// Purpose: convert type index and state index to find state element pointer
// Returns: Pointer to desired StateElement, or NULL
StateElement
* CSimInfoApp::StateVec(int typeID, int vecID)
{
	int category = 0;

	if ((typeID < 1) || (typeID >= 9)) return NULL;	// nope.

	if (vecID < 0) return NULL;	// nope.
	if (vecID >= maxCat[typeID]) return NULL;	// nope.

	return(&state[typeID][vecID]);	// Found it!	
}


// MAG 190702
// Subroutine: StateVecSetValue
// Purpose: Write a value to state array based on single state index
// Based on StateVec()
// Returns: 1 for success, 0 for failure (out of range)
int 
CSimInfoApp::StateVecSetValue(size_t vecID, double val)
{
	size_t			vr = 0;
	size_t			category = 0;
	size_t			unitID = 0;
	size_t			unitIndex = 0;
	StateElement*	myse = nullptr;
	HVUnit*			myhv = nullptr;
	size_t			i = 0;

	if (vecID < 0) return 0;

	vr = vecID;
	for (category = 1; category < 9; category++) {
		if (vr <= maxCat[category]) {
			myse = &state[category][vr];
			myse->SetInitValue(val);
			for (i = 0; i < myse->GetOutputCount(); i++) {
				myhv = myse->GetOutputUnit(i);
				if (myhv == nullptr) continue;
				myse->GetOutputInfo(i, &unitID, &unitIndex);
				myhv->SetInitOutputValue(unitIndex, val);
			} 
			for (size_t i = 0; i < myse->GetInputCount(); i++){
				myhv = myse->GetInputUnit(i);
				if (myhv == nullptr) continue;
				myse->GetInputInfo(i, &unitID, &unitIndex);
				myhv->SetInitInputValue(unitIndex, val);
			}
			return 1;
		}
		vr -= maxCat[category];
	}

	return 0;
}


// MAG 200825
int
CSimInfoApp::StateVecSetValue(size_t cat, size_t index, double val)
{
	size_t	ioc = 0;
	HVUnit	*hvu = nullptr;

	if (cat < 0) return 0;
	if (cat > 8) return 0;
	if (index < 0) return 0;
	if (index > maxCat[cat]) return 0;

	state[cat][index].SetInitValue(val);

	// set init value in associated unit links
	for (size_t i = 0; i < state[cat][index].GetInputCount(); i++){
		hvu = state[cat][index].GetInputUnit(i);
		ioc = state[cat][index].GetInputUnitIndex(i);
		hvu->SetInitInputValue(ioc, val);
	}
	for (size_t i = 0; i < state[cat][index].GetOutputCount(); i++){
		hvu = state[cat][index].GetOutputUnit(i);
		ioc = state[cat][index].GetOutputUnitIndex(i);
		hvu->SetInitOutputValue(ioc, val);
	}

	return 1;
}


// MAG 200724
// Subroutine: StateVecSetBoundary
// Purpose: Write boundary status to state array based on single state index
// Based on StateVec()
// Returns: 1 for success, 0 for failure (out of range)
int
CSimInfoApp::StateVecSetBoundary(size_t vecID, int val)
{
	size_t vr = 0;
	size_t category = 0;

	if (vecID < 0) return 0;

	vr = vecID;
	for (category = 1; category < 9; category++) {
		if (vr <= maxCat[category]) {
			state[category][vr].SetBoundary(val);
			return 1;
		}
		vr -= maxCat[category];
	}

	return 0;
}


int
CSimInfoApp::StateVecSetBoundary(size_t cat, size_t index, int val)
{
	size_t ioc = 0;
	HVUnit *hvu = nullptr;

	if (cat < 0) return 0;
	if (cat > 8) return 0;
	if (index < 0) return 0;
	if (index > maxCat[cat]) return 0;

	state[cat][index].SetBoundary(val);

	// set boundary value in associated unit links
	for (size_t i = 0; i < state[cat][index].GetInputCount(); i++){
		hvu = state[cat][index].GetInputUnit(i);
		if (hvu == nullptr) continue;
		ioc = state[cat][index].GetInputUnitIndex(i);
		hvu->SetInputBoundary(ioc, val);
	}
	//NOTE- outputs do not have boundary set

	return 1;
}


// MAG 200724
// Subroutine: StateVecSetReported
// Purpose: Write reported status to state array based on single state index
// Based on StateVec()
// Returns: 1 for success, 0 for failure (out of range)
int
CSimInfoApp::StateVecSetReported(size_t vecID, size_t sb, int val)
{
	size_t vr = 0;
	size_t category = 0;

	if (vecID < 0) return 0;
	if (sb < 1) return 0;
	//if (sb > reportedVars.size()) return 0;

	vr = vecID;
	for (category = 1; category < 9; category++) {
		if (vr <= maxCat[category]) {
			StateVecSetReported(category, vr, sb, val);
			return 1;
		}
		vr -= maxCat[category];
	}

	return 0;
}


//sb is superblock
int
CSimInfoApp::StateVecSetReported(size_t cat, size_t index, size_t sb, int val)
{
	size_t stateIndex = 0;
	std::vector<int>::iterator vit;

	if (cat < 0) return 0;
	if (cat > 8) return 0;
	if (index < 0) return 0;
	if (index > maxCat[cat]) return 0;
	if (sb < 1) return 0;

	state[cat][index].SetReported(sb, val);
	stateIndex = state[cat][index].GetStateIndex();
	
	if (val > 0) {	// adding value
		vit = std::find(reportedVars[sb - 1].begin(), reportedVars[sb - 1].end(), stateIndex);
		if ((reportedVars[sb - 1].size() == 0) || (vit == reportedVars[sb - 1].end())) {
			reportedVars[sb - 1].push_back((int) stateIndex);
			std::sort(reportedVars[sb - 1].begin(), reportedVars[sb - 1].end());
		} /*else { // it's already in the array! This happens because some methods (i.e. report category) are not discriminate about if elements are already reported.
		  vr = vr;	// for debug only
		  }*/
		if (reportedInterval[sb - 1] == 0) reportedInterval[sb - 1] = 60;
	}
	else { // if the element is in the array, remove it
		vit = std::find(reportedVars[sb - 1].begin(), reportedVars[sb - 1].end(), stateIndex);
		if (vit != reportedVars[sb - 1].end()) reportedVars[sb - 1].erase(vit);
	}

	return 1;
}


// MAG 190702
// Subroutine: StateVecAddTimeValue
// Purpose: write a time value to state array based on single state index. Called when entering a data file.
// Based on StateVec()
int 
CSimInfoApp::StateVecAddTimeValue(size_t vecID, double val)
{
	size_t vr = 0;
	size_t category = 0;

	if (vecID < 0) return 0;

	vr = vecID;
	for (category = 1; category < 9; category++) {
		if (vr <= maxCat[category]) {
			state[category][vr].AddTimeValue(val);
			return 1;
		}
		vr -= maxCat[category];
	}

	return 0;
}


int
CSimInfoApp::StateVecAddTimeValue(size_t vecID, int timeStep, double val, size_t superBlockID)
{
	size_t vr = 0;
	size_t category = 0;

	if (vecID < 0) return 0;

	vr = vecID;
	for (category = 1; category < 9; category++) {
		if (vr <= maxCat[category]) {
			state[category][vr].AddTimeValue(timeStep, val, superBlockID);
			return 1;
		}
		vr -= maxCat[category];
	}

	return 0;
}


// MAG 190702
// Subroutine: StateVecClearTimeValue
// Purpose: clear a time value to state array based on single state index. Called when entering a data file.
// Based on StateVec()
int
CSimInfoApp::StateVecClearTimeValue(size_t vecID)
{
	size_t vr = 0;
	int category = 0;

	if (vecID < 0) return 0;

	vr = vecID;
	for (category = 1; category < 9; category++) {
		if (vr <= maxCat[category]) {
			state[category][vr].ClearTimeValues();
			return 1;
		}
		vr -= maxCat[category];
	}

	return 0;
}


// MAG 200722 set entire category reporting value
void
CSimInfoApp::StateVecSetCategoryReporting(size_t cat, int status)
{	
	int			setStat = 0;
	CString		cs1;
	size_t		sb = 1;
	HTREEITEM	hti;
	int			i = 0;
	int			setCount = 0;

	if (cat < 1) return;
	if (cat > state.size()) return;
	if (status < -1) return;
	if (status > 1) return;
	if (maxCat[cat] == 0) return;

	// MAG 201111 when selecting category, use last selected SB from unit list to assign reporting SB.
	hti = csdlg->GetUnitdlg()->m_tree.GetLastSelectedUnit();
	if (hti > 0) {
		i = FindUnitTreeArrPos(hti);
		sb = units[i].GetSuperBlock();
	}

	// determine if we're setting or erasing
	if(status == -1){
		setStat = state[cat][1].IsReported(sb) == 1 ? 0 : 1;  // flip flop from reported state of first state element
	} else {
		setStat = status;
	}

	for (int i = 1;i <= maxCat[cat];i++) {
		//state[cat][i].SetReported(setStat);
		if ((setStat == 1)&&(state[cat][i].GetInputCount() == 0) && (state[cat][i].GetOutputCount() == 0)) continue;  // don't set write on unused state array members, can remove write
		StateVecSetReported(state[cat][i].GetStateIndex(), sb, setStat);
		cs1 = BuildStateTreeLabel(&state[cat][i]);
		csdlg->GetStatedlg()->m_tree.SetItemText(state[cat][i].GetTreepos(), cs1);
		setCount++;
	}

	cs1.Format(L"Set reporting status for %d state elements in category %zu to %d SB %zu", setCount, cat, setStat, sb);
	csdlg->m_status.AddString(cs1);

	return;
} // end StateVecSetCategoryReporting


// MAG create subroutine
// Subroutine: ReadDataFile
// Purpose: Enters historical data from a CSV file and assigns it to elements of the state array.
// Returns: 1 for success, 0 for error.
int 
CSimInfoApp::ReadDataFile(FILE *ip)
{
	char	readline[10000] = {};// MAG Dual duct is about 5600 characters/line, leaves some room for growth
	char	*cp = nullptr;
	size_t	i = 0;
	int		j = 1;
	int		hasSS=0;// bool =1 if this is a statestack file, which has two more starting columns (iunit, and type)
	int		lineCount = 0;
	int		lineInterval = 0;
	double	jd = 0;
	size_t	stateSize = 0;// = std::accumulate(maxCat, maxCat + (8 * sizeof(int)), 0);
	CString cs1;

	if (ip == nullptr) {
		csdlg->m_status.AddString(L"ERROR- Invalid file name entered in ReadDataFile! Data has not been read.");  // ok invalid pointer but try explaining that to the user!
		return 0;
	}

	if (units.size() == 0) {
		csdlg->m_status.AddString(L"ERROR- The simulation work file has not been entered. Data can not be entered.");
		return 0;
	}

	fgets(readline, 10000, ip);  // read off index line
	if (strstr(readline, "iunit") != NULL) hasSS = 1;
	for (i = 1; i <= 8; i++) {
		stateSize += maxCat[i];
	}
	for (i = 1; i <= stateSize; i++){
		StateVecClearTimeValue(i);
	}

	if (reportedInterval.size() > 0) {
		if (reportedInterval[0] == 0) reportedInterval[0] = 10;
		lineInterval = reportedInterval[0];
	} else {
		lineInterval = 10;
	}
	
	while (fgets(readline,10000,ip) != NULL) {
		cp = strchr(readline, ',');
		if (cp == nullptr) continue;
		cp++;

		if (hasSS) {  // read off extra columns
			cp = strchr(cp, ',');
			if (cp == nullptr) continue;
			cp++;
			cp = strchr(cp, ',');
			if (cp == nullptr) continue;
			cp++;
		}
		j = 1;
		while ((cp != NULL) && (*cp != '\0')) {
			while (*cp == ' ') cp++;
			jd = atof(cp);
			StateVecAddTimeValue(j, lineCount*lineInterval, jd, 1);
			while ((*cp != ' ') && (*cp != '\0')) cp++; // move past number just read
			j++;
		}
		lineCount++;
	}

	cs1.Format(L"Entered %d lines from data file.", lineCount);
	theApp.csdlg->m_status.AddString(cs1);

	j = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel(j - 1);

	return 1;
} // end ReadDataFile


// MAG create subroutine
// Subroutine: ReadDataOutFile
// Purpose: Enters historical data from a OUT file and assigns it to REPORTED elements of the state array.
// Returns: 1 for success, 0 for error.
int
CSimInfoApp::ReadDataOutFile(FILE *ip)
{
	char	readline[10000] = {};// MAG Dual duct is about 5600 characters/line, leaves some room for growth
	char	*cp = nullptr;
	size_t	i = 0;
	size_t	j = 1;
	double	jd = 0;
	size_t	stateSize = 0;// = std::accumulate(maxCat, maxCat + (8 * sizeof(int)), 0);
	size_t	reportedCount = 0;
	CString cs1;
	int		hasSS = 0;// bool =1 if this is a statestack file, which has two more starting columns (iunit, and type)
	int		lineTime = 0;	// time read from input file
	size_t	sbID = 0;	// superblock ID
	int		lineRead = 0;	// number of vars read so far on line

	if (ip == nullptr) {
		csdlg->m_status.AddString(L"ERROR- Invalid file name entered in ReadDataOutFile! Data has not been read.");  // ok invalid pointer but try explaining that to the user!
		return 0;
	}

	if (units.size() == 0) {
		csdlg->m_status.AddString(L"ERROR- The simulation work file has not been entered. Data can not be entered.");
		return 0;
	}
	// TODO- update subroutine for OUT files
	//fgets(readline, 10000, ip);  // read off first SUPERBLOCK line
	for (i = 0; i <= 8; i++) {
		stateSize += maxCat[i];
	}
	for (i = 1; i <= stateSize; i++) {
		StateVecClearTimeValue(i);
	}
	lineRead = 0;

	while (fgets(readline, 10000, ip) != NULL) {
		// read superblock and time
		if (strlen(readline) < 12) continue;
		if (strncmp(readline, "SUPERBLOCK", 10) != 0) break;
		//if (atoi(&readline[11]) != 1) continue;

		cp = &readline[10];
		sbID = atoi(cp);	// this should read off superblock number
		cp++;
		cp++; // move past superblock number
		lineTime = atoi(cp);
		reportedCount = reportedVars[sbID - 1].size();
		
		j = 0;	// j tracks values read
		while (j < reportedCount) {
			if (fgets(readline, 1000, ip) == nullptr) break;
			cp = &readline[0];
			while (*cp == ' ') cp++;
			lineRead++;
			while ((cp != NULL) && (*cp != '\0') && (*cp != '\n')) {
				jd = atof(cp);
				if (j > reportedVars[sbID - 1].size()) {
					cs1.Format(L"ERROR- mismatch between file and saved reported variables. Read stopped on line %d.",lineRead);  // manually entered reported elements not updated in reportedVars?
					theApp.csdlg->m_status.AddString(cs1);
					return 0;
				}
				StateVecAddTimeValue(reportedVars[sbID-1][j], lineTime, jd, sbID);
				while ((*cp != ' ') && (*cp != '\0')) cp++; // move past number just read
				j++;
				while (*cp == ' ') cp++;
			}
		}
	}

	cs1.Format(L"Entered %d lines from data file.", lineRead);
	theApp.csdlg->m_status.AddString(cs1);

	j = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel((int)j - 1);

	return 1;
} // end ReadDataOutFile


// MAG create subroutine
// Subroutine: ReadSimFile
// Purpose: Enters the simulation work file.
// Returns: 1 for success, 0 for error.
// MAG Note- this is very long, consider separating?
int 
CSimInfoApp::ReadSimFile(FILE *ip)
{
	char				readline[500] = {};	// input from file, to be parsed
	char				readline2[500] = {};	// input from file, saved if read early
	char				*cp = nullptr;
	int					ncread = 0;	// number of chars read
	int					nSBlocks = 0;	// # of superblocks
	CString				cs1;
	CString				cs2;
	std::string			ss1;
	std::stringstream	st1;
	//FILE				*opstat = nullptr;	// used to mirror output to status window
	std::vector<CString> noteList;  // running list of more important info

	int		nBlocks = 0;	// # of blocks in current superblock
	int		nUnits = 0;	// # of unit in current block

	// indexes for loops
	size_t	iSBlock = 0;
	size_t	iBlock = 0;
	size_t	iUnit = 0;
	
	int		unitNumber = 0;
	//int	unitIndex = 0;
	int		typeNumber = 0;
	size_t	i = 0;
	size_t	j = 0;
	size_t	k = 0;
	int		itemp = 0;
	int		rv = 0;  // for return values
	size_t	maxInputs = 0;
	float	jf = 0;
	double	jd = 0;
	size_t	si = 0;
	HVUnit	myhv;
	HVType	*myht = nullptr;
	StateElement *mse = nullptr;

	// summary of work file varibles
	int		countSB = 0;
	int		countB = 0;
	int		countUnits = 0;
	int		countBoundary = 0;
	int		countReported = 0;
	int		prevSB = -1;
	int		prevB = -1;
	int		prevUnit = -1;
	//int		categories[9];
	//int		svCount = 0;	// state variable count
	size_t	paramCount = 0;
	size_t	stateCount = 0;
	int		linesRead = 0;
	int		resetLabel = 0;	// update window label if this is not the first sim file to be read
	int		exitError = 0; // MAG 211109 exitError=1 indicates an error from which input should not continue. 
	int		stateIndex = 0;

	CTime	ctnow = CTime::GetCurrentTime();
	FILE	*opf = nullptr;
	//FILE	*opf = nullptr;
	std::vector<int> indexSkipped;
	std::vector<int> indexSkipCat;// store category index when skipped
	TCHAR tjunk[MAX_PATH] = {};

	if (ip == nullptr) {
		return 0;
	}

	if (units.size() > 0) {
		csdlg->m_status.AddString(L"Deleting previous simulation data.");	// NOTE types data is preserved here.
		resetLabel = 1;
		units.clear();
		reportedInterval.clear();
		for (std::vector<HVType>::iterator hvt = types.begin(); hvt != types.end(); hvt++) hvt->ZeroUseCount();
		if(csdlg->GetUnitdlg()->GetInit()) BuildUnitTree(&csdlg->GetUnitdlg()->m_tree);
		for (i = 1; i < 9; i++) {
			for (j = 0; j < STATESIZE; j++) {
				if (state[i][j].GetCategory() > 0) {
					state[i][j].Reset();
				}
			}
			state[i][0].SetCategory(i);
			maxCat[i] = 0;
		}
		for (i = 0;i < reportedVars.size();i++) {
			reportedVars[i].clear();
		}
		if (csdlg->GetStatedlg()->GetInit()) BuildStateTree(&csdlg->GetStatedlg()->m_tree);
	}

	if (types.size() == 0) {
		csdlg->m_status.AddString(L"NOTE- TYPE data not entered- using default TYPAR.DAT");
		_wgetcwd(tjunk, MAX_PATH);

		_wfopen_s(&opf, L"typar.dat", L"r");

		if (opf == nullptr) {
			csdlg->m_status.AddString(L"ERROR- Could not open type file TYPAR.DAT.  SIM file was not read.");
			return 0;
		}

		if (theApp.ReadTypeFile(opf) == 0) {
			csdlg->m_status.AddString(L"ERROR- Problem found entering TYPE data. SIM file was not entered.");
			return 0;
		}
		theApp.SetTypesFileName("typar.dat");
		csdlg->EnableTypeButton();
	}

	fgets(readline, 500, ip); // header line
	simName = readline;
	if (simName.back() == '\n') simName.pop_back();
	while ((simName.back() == ' ')&&(simName.length() > 3)) simName.pop_back();
	if (simName.length() <= 3) simName = "SimNoName";
	linesRead++;
	cs1 = readline;
	csdlg->m_status.AddString(cs1);

	fgets(readline, 500, ip); // superblock count
	linesRead++;
	nSBlocks = atoi(readline);
	cs1.Format(L"Find %d Superblocks", nSBlocks);
	csdlg->m_status.AddString(cs1);
	if ((nSBlocks < 1) || (nSBlocks > 40)) {
		cs1.Format(L"ERROR (L%04d)- number of superblocks (%d) is outside allowed range (1-40).",linesRead, nSBlocks);
		csdlg->m_status.AddString(cs1);
		csdlg->m_status.AddString(L"There may be a configuration error. Please review the input file.");
		return 0;
	}

	fgets(readline, 500, ip); // Error Tol. line
	linesRead++;
	cs1.Format(L"NOTE- Error Tol. : %S", readline);
	csdlg->m_status.AddString(cs1);

	readline2[0] = 0;

	for (iSBlock = 0; (iSBlock < nSBlocks)&&(exitError == 0); iSBlock++) {	// cycle through SuperBlocks
		fgets(readline, 500, ip); // blocks in superblock line
		linesRead++;
		nBlocks = atoi(readline);

		if ((nBlocks < 1) || (nBlocks > 20)) {
			cs1.Format(L"ERROR (L%04d)- number of blocks (%d) in superblock %d is outside allowed range (1-20).", linesRead, nBlocks, nSBlocks);
			csdlg->m_status.AddString(cs1);
			csdlg->m_status.AddString(L"There may be a configuration error. Please review the input file.");
			exitError = 1;
			continue;
		}

		cs1.Format(L"Start SB %zu with %d blocks.", iSBlock + 1, nBlocks);
		csdlg->m_status.AddString(cs1);

		for (iBlock = 0; (iBlock < nBlocks) && (exitError == 0); iBlock++) {	// cycle through Blocks in this SuperBlock
			fgets(readline, 500, ip); // units in block line
			linesRead++;
			nUnits = atoi(readline);

			if ((nUnits < 1) || (nUnits > 40)) {
				cs1.Format(L"ERROR (L%04d)- number of units (%zu) in block %d is outside allowed range (1-40).", linesRead, nUnits, iBlock);
				csdlg->m_status.AddString(cs1);
				csdlg->m_status.AddString(L"There may be a configuration error. Please review the input file.");
				exitError = 1;
				continue;
			}

			cs1.Format(L"Start Block %zu with %d units.", iBlock + 1, nUnits);
			csdlg->m_status.AddString(cs1);

			for (iUnit = 0; (iUnit < nUnits) && (exitError == 0); iUnit++){	// cycle through Units in this Block
				myhv.SetBlock(iBlock + 1);
				myhv.SetSuperBlock(iSBlock + 1);
				fgets(readline, 500, ip); // unit #, type #
				linesRead++;
				sscanf_s(readline, " %d %d", &unitNumber, &typeNumber);

				ss1 = GetTypeLabel(typeNumber);
				cs1.Format(L"Enter Unit %03d Type %03d %S", unitNumber, typeNumber,ss1.c_str());
				csdlg->m_status.AddString(cs1);
				myhv.SetUnit(unitNumber);
				if (units.size() > 1) {
					if (units.back().GetUnit() + 1 != unitNumber) {// non-sequential unit numbering, probably a configuration error
						cs1.Format(L"NOTE- Non-sequential unit numbering found at unit # %d", unitNumber);
						noteList.push_back(cs1);
					}
				}
				// find pointer to type
				for (std::vector<HVType>::iterator ii = types.begin(); ii != types.end(); ii++){
					if (ii->GetType() == typeNumber) {
						myhv.SetType(ii._Ptr);
						break;
					}
				}
				if (myhv.GetType() == 0){ // UH OH
					cs1.Format(L"ERROR (L%04d)- In SIM file unit %d- Type %d used but not specified in DAT file. SIM file entry was stopped.",linesRead, unitNumber, typeNumber);
					theApp.csdlg->m_status.AddString(cs1);
					noteList.push_back(cs1);
					csdlg->m_status.AddString(L"There may be a configuration error. Please review the input file.");
					noteList.push_back(L"There may be a configuration error. Please review the input file.");
					units.clear();
					for (std::vector<HVType>::iterator hvt = types.begin(); hvt != types.end(); hvt++) hvt->ZeroUseCount();
					exitError = 1;
					continue;
				}

				// set label for unit
				i = myhv.GetTypeP()->GetUseCount();
				myhv.GetTypeP()->AddUseCount();
				st1.str("");
				if (i < 26) {
					st1 << typeNumber << (char)('a' + i);
				} else {
					st1 << typeNumber << (char)('a' + i%26) << (int)i/26;
				}
				ss1 = st1.str();
				myhv.SetLabel(ss1);

				// read inputs
				fgets(readline, 500, ip); // unit #, type #
				linesRead++;
				cp = &readline[0];
				for ( i = 0; ((i < myhv.GetInputCount())&&(cp != NULL)); i++){
					while (*cp == ' ') cp++;
					if (*cp == '\n') {
						fgets(readline, 500, ip);
						linesRead++;
						cp = &readline[0];
					}
					sscanf_s(cp, "%zd%n", &j, &ncread);
					if (j == 0) {
						cs1.Format(L"NOTE- using 0 for input index.");  // debug for breakpoint
					}
					myhv.AddInput(j);
					cp += ncread;
				}
				if (strlen(cp) > 2) {
					cs1.Format(L"extra input remains- difference in number from typar?");
				}

				// read outputs
				if (myhv.GetOutputCount() > 0) {
					fgets(readline, 500, ip); // unit #, type #
					linesRead++;
					cp = &readline[0];
					for (i = 0; ((i < myhv.GetOutputCount())&&(cp != NULL)); i++) {
						while (*cp == ' ') cp++;
						if (*cp == '\n') {
							fgets(readline, 500, ip);
							linesRead++;
							cp = &readline[0];
						}
						sscanf_s(cp, "%zd%n", &j, &ncread);
						myhv.AddOutput(j);
						cp += ncread;
					}
					if (strlen(cp) > 2) {
						cs1.Format(L"extra output remains- difference in number from typar?");
					}
				}

				// read parameters
				if (myhv.GetParameterCount() > 0) {
					fgets(readline, 500, ip); // unit #, type #
					linesRead++;
					cp = &readline[0];
					paramCount = myhv.GetParameterCount();
					for (i = 0; i < paramCount; i++) {
						while (*cp == ' ') cp++;
						if (*cp == '\n') {
							fgets(readline, 500, ip);
							linesRead++;
							if (strchr(readline, '(') != NULL) {
								cs1.Format(L"ERROR- Unit %d missing parameter value %d (line %d). Setting to 0.",myhv.GetUnit(),i+1,linesRead-1);
								noteList.push_back(cs1);
								strcpy_s(readline2, 500, readline);
								myhv.AddParameter(0);
								continue;
							}
							cp = &readline[0];
						}
						sscanf_s(cp, "%f%n", &jf, &ncread);
						myhv.AddParameter(jf);
						cp += ncread;
					}
					if (strlen(cp) > 2) {
						cs1.Format(L"extra parameter remains- difference in number from typar?");
					}
				}

				units.push_back(myhv);
				myhv.Reset();
			}	// end cycling through units in block
		}	// end for blocks
	}// end for iSBlock

	cs1.Format(L"Finish entering %d Units from Type file.", units.size());
	csdlg->m_status.AddString(cs1);

	if (exitError == 1) {
		cs1.Format(L"NOTE: an unrecoverable error was encountered while reading units.");
		csdlg->m_status.AddString(cs1);
		noteList.push_back(cs1);
	}

	size_t ioCat;
	std::vector<HVUnit>::iterator hvi;
	
	if (exitError == 0) {
		// fill in state vector by parsing list of units
		for (hvi = units.begin(); hvi != units.end(); hvi++) {
			if (hvi->GetUnit() <= 0) continue;

			// parse inputs
			si = hvi->GetInputCount();
			if ((si > maxInputs) && (hvi->GetOutputCount() > 0)) maxInputs = si;
			for (size_t i = 0; i < si; i++) {
				//get cat index for input
				j = hvi->GetInput(i);

				// get type for input
				myht = hvi->GetTypeP();
				ioCat = myht->GetInputCategory(i); // get category of input
				state[ioCat][j].AddInput(hvi->GetUnit(), i);
				state[ioCat][j].SetCategory(ioCat);
				state[ioCat][j].SetCategoryIndex(j);
				state[ioCat][j].SetInitValue(0);
				if (maxCat[ioCat] < j) maxCat[ioCat] = j;
			}

			// parse outputs
			si = hvi->GetOutputCount();
			for (size_t i = 0; i < si; i++) {
				//get cat index for output
				j = hvi->GetOutput(i);

				// get type for output
				myht = hvi->GetTypeP();
				ioCat = myht->GetOutputCategory(i); // get category of output
				// need state[type][typeIndex]
				state[ioCat][j].AddOutput(hvi->GetUnit(), i);
				state[ioCat][j].SetCategory(ioCat);
				state[ioCat][j].SetCategoryIndex(j);
				if (maxCat[ioCat] < j) maxCat[ioCat] = j;
			}

			// Also set types to BOLD in type tree, if build
			if (hvi->GetTypeP()->GetTreepos() > 0) 	csdlg->Getdtdlg()->m_tree.SetItemState(hvi->GetTypeP()->GetTreepos(), TVIS_BOLD, TVIS_BOLD);	// MAG 200921 add bold to used types
		}

		// MAG 200225 scan and set category for unused interstitial states
		for (int cati = 1; cati <= 8; cati++) {
			for (int indi = 1; indi <= maxCat[cati]; indi++) {
				if (state[cati][indi].GetCategory() == 0) state[cati][indi].SetCategory(cati);
				state[cati][indi].SetCategoryIndex(indi);
			}
		}

		for (i = 0; i < 9; i++){
			stateCount += maxCat[i];
		}

		// Read initial values, set into state array
		j = 0; // current index id, must remain < state array size

		// read in initial values for state array
		while (j < stateCount) {
			fgets(readline, 500, ip); // init var x (up to 5)
			linesRead++;
			cp = &readline[0];
			while ((cp != NULL) && (*cp != '\0') && (j < stateCount)) {
				while (*cp == ' ') cp++;
				jd = atof(cp);
				StateVecSetValue(j + 1, jd);
				while ((*cp != ' ') && (*cp != '\0')) cp++; // move past number just read
				j++;
			}
		}

		// read boundary variables
		fgets(readline, 500, ip); // init var x (up to 5)
		linesRead++;
		rv = atoi(readline);
		j = 0;
		HVUnit* thv;
		while (j < rv) {
			fgets(readline, 500, ip); // init var x (up to 5)
			linesRead++;
			cp = &readline[0];
			while ((cp != NULL) && (*cp != '\0') && (j < rv)) {
				while (*cp == ' ') cp++;
				maxInputs = atoi(cp);
				mse = StateVec(maxInputs);
				if (mse != NULL) mse->SetBoundary(1);
				if (mse->GetInputCount() > 0) {	// set boundary in unit array
					thv = mse->GetInputUnit(0);
					if (thv != NULL) {
						thv->SetInputBoundary(mse->GetInputUnitIndex(0), 1);
						countBoundary++;
					}
				}
				while ((*cp != ' ') && (*cp != '\0')) cp++; // move past number just read
				j++;
			}
		}
		reportedVars.resize(nSBlocks);

		// read reported var and intervals
		for (iSBlock = 0; iSBlock < nSBlocks; iSBlock++) {
			do {
				fgets(readline, 500, ip); // read # of reported var and interval for SB ##
				linesRead++;
			} while (strlen(readline) < 2);
			rv = atoi(readline);
			cp = &readline[5];
			while (*cp == ' ') cp++;
			//jd = atof(cp);
			itemp = atoi(cp);
			reportedInterval.push_back(itemp);
			j = 0;
			k = 0; // count of lines read for state index- will need to read off that many lines for cat and cat index after

			while (j < rv) {
				fgets(readline, 500, ip); // init var x (up to 5)
				linesRead++;
				k++;
				cp = &readline[0];
				while ((cp != NULL) && (*cp != '\0') && (j < rv)) {
					while (*cp == ' ') cp++;
					maxInputs = atoi(cp);
					mse = StateVec(maxInputs);
					if (mse == nullptr) {
						cs1.Format(L"ERROR (%04d)- reported variable index exceeds state array size.", linesRead);
						csdlg->m_status.AddString(cs1);
						break;
					}
					StateVecSetReported(mse->GetStateIndex(), iSBlock + 1, 1);// MAG 201105 isBlock was +1, not sure why?
					countReported++;
					while ((*cp != ' ') && (*cp != '\0')) cp++; // move past number just read
					j++;
				}
			}

			// read off category and category index lines, they're redundant
			for (j = 0; j < 2 * k; j++) {
				fgets(readline, 500, ip); // read # of reported var and interval for SB ##
				linesRead++;
			}
		}
		// next lines (usally 0's) are for frozen variables, one for each SB- can skip as done reading

		csdlg->m_status.AddString(L"Completed processing units- analyzing...");

		// check for dual+ output asssignments
		for (i = 1; i <= 8; i++) { // scan through types
			for (j = 0; j < STATESIZE; j++) {
				if (state[i][j].GetOutputCount() > 1) {
					cs1.Format(L"NOTE- State %02d type %d has %02d input assignments and %02d output assignments.", j, i, state[i][j].GetInputCount(), state[i][j].GetOutputCount());
					noteList.push_back(cs1);
				}
			}
		}

		csdlg->m_status.AddString(L"Completed analyzing Simulation Work File.");

		if (csdlg->GetUnitdlg()->GetInit() == 1) {// MAG added 201028
			if (resetLabel == 1) {
				cs1.Format(L"Units Display: Superblock:Block:Unit [%S]", simName.c_str());
				csdlg->GetUnitdlg()->SetWindowText(cs1);
			}
			BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);
		}
		if (csdlg->GetStatedlg()->GetInit() == 1) {// MAG added 201028
			BuildStateTree(&theApp.csdlg->GetStatedlg()->m_tree);
		}

		csdlg->m_status.AddString(L"Summary of Simulation Work File");
		for (hvi = units.begin(); hvi != units.end(); hvi++) {
			if (hvi->GetSuperBlock() != prevSB) {
				countSB++;
				prevSB = (int) hvi->GetSuperBlock();
				prevB = -1;
			}
			if (hvi->GetBlock() != prevB) {
				prevB = (int) hvi->GetBlock();
				countB++;
			}
			countUnits++;

		}
		cs1.Format(L"SB: %d ::  BL: %d ::  Units: %d", countSB, countB, countUnits);
		csdlg->m_status.AddString(cs1);

		cs1.Format(L"State Element by Category: %zu %zu %zu %zu %zu %zu %zu %zu", maxCat[1], maxCat[2], maxCat[3], maxCat[4], maxCat[5], maxCat[6], maxCat[7], maxCat[8]);
		csdlg->m_status.AddString(cs1);

		// MAG TODO- this might be redundant with other code
		//for (i = 1; i <= 8; i++) {
		//	svCount += categories[i];
		//}

		cs1.Format(L"State Variables: %zu", stateCount);
		csdlg->m_status.AddString(cs1);

		cs1.Format(L"Boundary Variable Count: %d  Reported Variable Count: %d", countBoundary, countReported);
		csdlg->m_status.AddString(cs1);

		cs1.Format(L"Reporting interval by SB: ");
		for (i = 0; i < reportedInterval.size(); i++){
			//cs2.Format(L" %d: %3.0f ", i + 1, reportedInterval[i]);
			cs2.Format(L" %zu: %3d ", i + 1, reportedInterval[i]);
			cs1 += cs2;
		}
		csdlg->m_status.AddString(cs1);
	}	// end if exitError == 0

	if (noteList.size() > 0) {
		csdlg->m_status.AddString(L"IMPORTANT NOTES:");
		for (i = 0; i < noteList.size(); i++)
		{
			csdlg->m_status.AddString(noteList[i]);
		}
	}

	itemp = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel(itemp - 1);

	unsavedChanges = 0;

	csdlg->m_status.AddString(L"Finished reading sim file.");

	return 1;
} // end ReadSimFile


// MAG create subroutine
// Subroutine: SaveSimInfoFile
// Purpose: Saves an XML spreadsheet with worksheets with model info:
// 1: Unit List- Lists units, superblock, block, description, count of in/out/params, values of parameters
// 2: State Array- Lists category, indices, and label for entire state array
// 3: HVACGEN Stream Units- Stream input to HVACGEN to create simulation work file. Copy col A to HVACGEN input. Stream 1/2.
// 4: HVACGEN Stream Init Values- Stream input to HVACGEN to fill out init values, boundary, etc. Stream 2/2.
// 5+: Worksheet for each category. Shows output/inputs/init value for each state element.
// Returns: 1 for success, 0 for error.
int 
CSimInfoApp::SaveSimInfoFile(FILE *ip)
{
	mxml mymx;
	xml_element *xe1 = nullptr;	//
	xml_element *xwb = nullptr;	// the workbook
	xml_element *xws = nullptr;	// the worksheet
	xml_element *xtable = nullptr;// the table
	xml_element *xcol = nullptr;	// the column
	xml_element *xrow = nullptr;	// the current row
	xml_element *xcell = nullptr;	// the current cell
	xml_element *xdata = nullptr;
	xml_props   *xp1 = nullptr;
	char ops[400] = {};
	char lineLabel[400] = {};
	char labelString[400] = {};
	//char matchString[400] = {};	// MAG used to hold string for =match...
	int stateIndex=0;		// index to state vector
	int catIndex=0;		// index in category
	char catNames[][30] = { "Pressure","Mass Flow Rate","Temperature","Control","Other","Energy","Power","Humidity" };
	int	catReported = 0;
	size_t unit = 0;
	size_t maxparams = 0;
	size_t unitIndex = 0;
	int ioType = 0;
	int itemp = 0;
	HVType *hvt = nullptr;
	HVUnit *hvu = nullptr;
	std::string ss1;
	StateElement *sep = nullptr;

	if (ip == nullptr) {
		theApp.csdlg->m_status.AddString(L"Error saving Simulation Information file- NULL file pointer.");
		return -1;
	}

	// MAG 221025 removed from here, this should be done when changes are made
	//if (unsavedChanges) {  // MAG added 220503 to ensure state is in correct shape (i.e. no blanks at end of category)
	//	BuildState();
	//	BuildStateTree(NULL);
	//}

	theApp.csdlg->m_status.AddString(L"Saving Simulation Information file....");
	xe1 = mymx.addElement(mymx.getRoot(),"?xml");
	xp1 = mymx.addprop(xe1, "version", "1.0");
	xp1 = mymx.addprop(xe1, "encoding", "UTF-8");

	xe1 = mymx.addElement(mymx.getRoot(), "?mso-application");
	xp1 = mymx.addprop(xe1, "progid", "Excel.Sheet");

	xwb = mymx.addElement(mymx.getRoot(), "Workbook");
	xp1 = mymx.addprop(xwb, "xmlns","urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:x", "urn:schemas-microsoft-com:office:excel");
	xp1 = mymx.addprop(xwb, "xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:html", "https://www.w3.org/TR/html401/");
	
	// *********************
	// Add Units List sheet
	xws = mymx.createElement(xwb, "Worksheet");
	xp1 = mymx.addprop(xws, "ss:Name", "Unit List");
	xtable = mymx.createElement(xws, "Table");
	xcol = mymx.createElement(xtable, "Column");
	xp1 = mymx.addprop(xcol, "ss:Index", "1");
	xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
	xp1 = mymx.addprop(xcol, "ss:Width", "110");
	xrow = mymx.createElement(xtable, "Row");  // title row

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "SBlock");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Block");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");
	
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Unit");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Type");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Description");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Label");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Note");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "In");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Out");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Params");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	//loop through units list to find max params
	for (size_t i = 0; i < units.size(); i++) {
		if (units[i].GetParameterCount() > maxparams) maxparams = units[i].GetParameterCount();
	}

	for (size_t i = 0; i < maxparams; i++){
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", (int)i+1);
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");
	}

	//loop through units list
	for (size_t i = 0; i < units.size(); i++){
		xrow = mymx.createElement(xtable, "Row");  // row for this unit

		xcell = mymx.createElement(xrow, "Cell");  // superblock
		xdata = mymx.createElement(xcell, "Data", units[i].GetSuperBlock());
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");  // block
		xdata = mymx.createElement(xcell, "Data", units[i].GetBlock());
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");  // unit
		xdata = mymx.createElement(xcell, "Data", (int)i+1);
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", units[i].GetType());
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		ss1 = units[i].GetTypeP()->GetTypeLabel();
		sprintf_s(ops, "%s", ss1.c_str());
		xcell = mymx.createElement(xrow, "Cell"); // description (filled from TYPAR.dat)
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		ss1 = units[i].GetLabel();
		sprintf_s(ops, "%s", ss1.c_str());
		xcell = mymx.createElement(xrow, "Cell");	// label
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		ss1 = units[i].GetNote();
		sprintf_s(ops, "%s", ss1.c_str());
		xcell = mymx.createElement(xrow, "Cell");	// Note (Optionally filled by user)
		xdata = mymx.createElement(xcell, "Data",ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", units[i].GetInputCount());
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", units[i].GetOutputCount());
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", units[i].GetParameterCount());
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		// print out parameter values
		for (size_t j = 0; j < units[i].GetParameterCount(); j++){
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", units[i].GetParameter(j));
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");
		}
	}

	// *********************
	// STATE array summary - MAG add 190906
	xws = mymx.createElement(xwb, "Worksheet");
	xp1 = mymx.addprop(xws, "ss:Name", "State Array");
	xtable = mymx.createElement(xws, "Table");
	xcol = mymx.createElement(xtable, "Column");
	xp1 = mymx.addprop(xcol, "ss:Index", "1");
	xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
	xp1 = mymx.addprop(xcol, "ss:Width", "110");
	xrow = mymx.addElement(xtable, "Row");  // title row

	xcell = mymx.createElement(xrow, "Cell");	// Location in state array
	xdata = mymx.createElement(xcell, "Data","Location");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");	// Category in state array
	xdata = mymx.createElement(xcell, "Data", "Category");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");	// Category indices 
	xdata = mymx.createElement(xcell, "Data", "Indices");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");	// label
	xdata = mymx.createElement(xcell, "Data", "Label");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	stateIndex = 1;  // used as location counter for this loop
	for (size_t iCat = 0; iCat < 8; iCat++){
		for (size_t iInd = 1; iInd <= maxCat[iCat + 1]; iInd++)
		{
			sep = &state[iCat + 1][iInd];

			xrow = mymx.createElement(xtable, "Row");  // row for this unit

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", stateIndex);  // location in state array
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", sep->GetCategory());
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", sep->GetCategoryIndex());
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			ss1 = "";
			xcell = mymx.createElement(xrow, "Cell");	//label
			if (sep->GetOutputCount() > 0) {
				sep->GetOutputInfo(0, &unit, &unitIndex);
				hvt = units[unit - 1].GetTypeP();
				if (hvt != NULL) {
					ss1 = hvt->GetOutputLabel(unitIndex);
				}
			} else if(sep->GetInputCount() > 0) {
				sep->GetInputInfo(0, &unit, &unitIndex);
				hvt = units[unit - 1].GetTypeP();
				if (hvt != NULL) {
					ss1 = hvt->GetInputLabel(unitIndex);
				}
			}
			else ss1 = "NA";
			sprintf_s(ops, "%s", ss1.c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			stateIndex++;
		}
	}


	// *********************
	// Add HVACGEN stream sheet MAG add 190926
	xws = mymx.createElement(xwb, "Worksheet");
	xp1 = mymx.addprop(xws, "ss:Name", "HVACGEN Stream Units");
	xtable = mymx.createElement(xws, "Table");
	xcol = mymx.createElement(xtable, "Column");
	xp1 = mymx.addprop(xcol, "ss:Index", "1");
	xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
	xp1 = mymx.addprop(xcol, "ss:Width", "110");
	
	// add 'cr' and 'si' lines
	xrow = mymx.createElement(xtable, "Row");  // new row for cr
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "cr");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "create ...");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // new row for si
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "si");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "... a SIM");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	//loop through units list
	for (size_t i = 0; i < units.size(); i++) {
		hvu = &units[i];

		xrow = mymx.createElement(xtable, "Row");  // row for unit type #
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", hvu->GetType());
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");
		sprintf_s(ops, "Add type '%s'", units[i].GetTypeP()->GetTypeLabel().c_str());
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		// cycle through inputs
		for (size_t j = 0; j < hvu->GetInputCount(); j++) {
			xrow = mymx.createElement(xtable, "Row");  // row for input
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", hvu->GetInput(j));
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "Input %zd '%s'",j+1, units[i].GetTypeP()->GetInputLabel(j).c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}

		// cycle through outputs
		for (size_t j = 0; j < hvu->GetOutputCount(); j++) {
			xrow = mymx.createElement(xtable, "Row");  // row for output
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", hvu->GetOutput(j));
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "Output %zd '%s'",j+1, units[i].GetTypeP()->GetOutputLabel(j).c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}

		// cycle through parameters
		for (size_t j = 0; j < hvu->GetParameterCount(); j++) {
			xrow = mymx.createElement(xtable, "Row");  // row for parameter
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", hvu->GetParameter(j));
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "Param %zd '%s'",j+1, units[i].GetTypeP()->GetParameterLabel(j).c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}

		// determine if continuing in superblock
		if (i == units.size() - 1) { // last unit, end input
			xrow = mymx.createElement(xtable, "Row");  // row for n
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "n");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more units?");  // continue entering units?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xrow = mymx.createElement(xtable, "Row");  // row for n
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "n");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more blocks?");	// continue entering blocks?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xrow = mymx.createElement(xtable, "Row");  // row for n
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "n");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more superblocks?");	// continue entering superblocks?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			// will proceed to enter name, etc
			// after this exit loop
		}
		else if (units[i].GetSuperBlock() != units[i + 1].GetSuperBlock()) {// check for end of superblock
			xrow = mymx.createElement(xtable, "Row");  // row for n
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "n");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more units?");  // continue entering units?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xrow = mymx.createElement(xtable, "Row");  // row for n
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "n");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more blocks?");	// continue entering blocks?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xrow = mymx.createElement(xtable, "Row");  // row for y - continue entering superblocks
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "y");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more superblocks?");	// continue entering superblocks?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			// will proceed to enter number of next type
			// after this return to top of loop
		}
		else if (units[i].GetBlock() != units[i + 1].GetBlock()) {// check for end of block, same superblock
			xrow = mymx.createElement(xtable, "Row");  // row for n
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "n");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more units?");  // continue entering units?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xrow = mymx.createElement(xtable, "Row");  // row for y - continue entering blocks
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "y");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more blocks?");	// continue entering blocks?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
			// after this return to top of loop
		}
		else { // add next unit to same block
			xrow = mymx.createElement(xtable, "Row");  // row for output
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "y");
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", "more units?");  // continue entering units?
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			// after this return to top of loop
		}
	}

	// add sim name
	if (simName.length() == 0) { // MAG 200122 add check for simName
		CTime cts = CTime::GetCurrentTime();
		sprintf_s(ops, "mzvav%02d%02d%02d", cts.GetYear()-2000, cts.GetMonth(), cts.GetDay());
		simName = ops;
	}
	sprintf_s(ops, "%s", simName.c_str());
	xrow = mymx.createElement(xtable, "Row");  // row for title
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data",ops );
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", ops);
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	// add use default values (y)
	xrow = mymx.createElement(xtable, "Row");  // row for output
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "y");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Use default values");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	// add final row to notify of end
	xrow = mymx.createElement(xtable, "Row");  // row for final output
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "NOTE- End of input stream. Move to next worksheet. Do not copy this line.");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	// END FIRST STREAM WORKSHEET

	// enter initial values stream on new sheet
	xws = mymx.createElement(xwb, "Worksheet");
	xp1 = mymx.addprop(xws, "ss:Name", "HVACGEN Stream Init Values");
	xtable = mymx.createElement(xws, "Table");
	xcol = mymx.createElement(xtable, "Column");
	xp1 = mymx.addprop(xcol, "ss:Index", "1");
	xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
	xp1 = mymx.addprop(xcol, "ss:Width", "110");

	// no header, just get into it
	for (size_t iCat = 0; iCat < 8; iCat++) {
		for (size_t iInd = 1; iInd <= maxCat[iCat + 1]; iInd++){
			sep = &state[iCat + 1][iInd];

			xrow = mymx.createElement(xtable, "Row");  // row for this value

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", sep->GetInitValue());  // location in state array
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "%zd:%02zd", sep->GetCategory(), sep->GetCategoryIndex());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			if (sep->GetOutputCount() > 0) {
				sep->GetOutputInfo(0, &unit, &unitIndex);
				ss1 = units[unit - 1].GetTypeP()->GetOutputLabel(unitIndex);
				sprintf_s(ops, "%s", ss1.c_str());
			} else if (sep->GetInputCount() > 0) { 
				sep->GetInputInfo(0, &unit, &unitIndex); 
				ss1 = units[unit - 1].GetTypeP()->GetInputLabel(unitIndex);
				sprintf_s(ops, "%s", ss1.c_str());
			} else sprintf_s(ops, "NA");
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}
	}

	int catBoundaryCount = 0;
	//stateIndex = 0;  // here use to keep track of previous category index count, to get abs. state array position
	// Add boundary variables, reporting interval info, and other remaining items to same worksheet
	// NOTE boundary variables are entered into HVACGEN by category- index is category index
	for (size_t iCat = 1; iCat < 9; iCat++) {
		catBoundaryCount = 0;
		if (maxCat[iCat] == 0) continue;  // skip printing blank for categories with no items
		for (size_t iInd = 1; iInd <= maxCat[iCat]; iInd++) {
			sep = &state[iCat][iInd];
			if (sep->IsBoundary() == 0) continue; // skip non-boundary vars

			xrow = mymx.createElement(xtable, "Row");  // row for this value

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", sep->GetCategoryIndex());  // location in category array
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			// find a label
			if (sep->GetOutputCount() > 0) {
				sep->GetOutputInfo(0, &unit, &unitIndex);
				ss1 = units[unit - 1].GetTypeP()->GetOutputLabel(unitIndex);
				sprintf_s(ops, "%s", ss1.c_str());
			}
			else if (sep->GetInputCount() > 0) {
				sep->GetInputInfo(0, &unit, &unitIndex);
				ss1 = units[unit - 1].GetTypeP()->GetInputLabel(unitIndex);
				sprintf_s(ops, "%zu:%zu %s", iCat,iInd,ss1.c_str());
			}
			else sprintf_s(ops, "NA");	// NOTE this should never happen for a boundary variable
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			if(iInd == 1){// start category, print label
				sprintf_s(ops, "START boundary category %s", categoryLabels[sep->GetCategory()].c_str());
				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", ops);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");
			}
			catBoundaryCount++;
		}
		// MAG 220920 if entire category is boundary, there is no blank line to continue to next category
		if (catBoundaryCount == maxCat[iCat]) continue;

		// print blank line after each category to end it
		xrow = mymx.createElement(xtable, "Row");  // row for blank
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		// add second blank cell to offset BLANK notice
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		sprintf_s(ops, "BLANK to end boundary category %s", categoryLabels[iCat].c_str());
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");
	}

	//
	// Reporting Intervals entry
	//
	// seed STATE array with reporting info
	//for (size_t i = 0; i < reportedVars.size(); i++) {  // i is also current superblock
	//	for (size_t j = 0;j < reportedVars[i].size();j++) {
	//		StateVec(reportedVars[i][j])->SetReported(i + 1); // SB are listed 1-X
	//	}
	//}

	for (size_t i = 0; i < reportedInterval.size(); i++) {  // i is also current superblock
		xrow = mymx.createElement(xtable, "Row");  // row for interval value
		xcell = mymx.createElement(xrow, "Cell");
		if(reportedVars[i].size() == 0)xdata = mymx.createElement(xcell, "Data", 0);	// MAG 200902 set interval size to zero if there's no elements reported
		else xdata = mymx.createElement(xcell, "Data", reportedInterval[i]);
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");
		sprintf_s(ops, "Interval for superblock %zd", i+1);
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		// cycle through categories to find entries reported in this superblock
		for (size_t iCat = 1; iCat <= 8; iCat++) {
			if (maxCat[iCat] == 0) continue;  // skip printing blank for categories with no items

			catReported = 0;

			for (size_t iInd = 1; iInd <= maxCat[iCat]; iInd++) {
				sep = &state[iCat][iInd];
				if (sep->IsReported(i+1) == 0) continue; // skip non-reported vars

				catReported++;
				xrow = mymx.createElement(xtable, "Row");  // row for this value

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", sep->GetCategoryIndex());  // location in category array
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				// find a label
				if (sep->GetOutputCount() > 0) {
					sep->GetOutputInfo(0, &unit, &unitIndex);
					ss1 = units[unit - 1].GetTypeP()->GetOutputLabel(unitIndex);
					sprintf_s(ops, "%s", ss1.c_str());
				} else if (sep->GetInputCount() > 0) {
					sep->GetInputInfo(0, &unit, &unitIndex);
					ss1 = units[unit - 1].GetTypeP()->GetInputLabel(unitIndex);
					sprintf_s(ops, "%s", ss1.c_str());
				} else sprintf_s(ops, "NA");	// NOTE this should never happen for a boundary variable

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", ops);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				if (iInd == 1) {// start category, print label
					sprintf_s(ops, "START reporting %s for superblock %zd", categoryLabels[sep->GetCategory()].c_str(),i+1);
					xcell = mymx.createElement(xrow, "Cell");
					xdata = mymx.createElement(xcell, "Data", ops);
					xp1 = mymx.addprop(xdata, "ss:Type", "String");
				}
			}

			//stateIndex += maxCat[iCat];
			// MAG 200810 NOTE- if all elements in a category are reported, the blank line between categories is not present
			if (catReported < maxCat[iCat]) {
				// print blank line after each category to end it
				xrow = mymx.createElement(xtable, "Row");  // row for this value
				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", "");
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				// add second blank cell to offset BLANK notice
				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", "");
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				xcell = mymx.createElement(xrow, "Cell");
				sprintf_s(ops, "BLANK to end reporting category %s", categoryLabels[iCat].c_str());
				xdata = mymx.createElement(xcell, "Data", ops);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");
			}
		}

		// print end of sb settings
		xrow = mymx.createElement(xtable, "Row");  // row for freezing variables value
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "0");
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");
		sprintf_s(ops, "Freezing option for this superblock: 0, 1, or 2.");
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xrow = mymx.createElement(xtable, "Row");  // row for variable input scan option value
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "0");
		xp1 = mymx.addprop(xdata, "ss:Type", "Number");

		xcell = mymx.createElement(xrow, "Cell");
		sprintf_s(ops, "Variable input scan option for this superblock: 0 or 1.");
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");
	}

	// enter file name
	xrow = mymx.createElement(xtable, "Row");  // row for title
	xcell = mymx.createElement(xrow, "Cell");
	sprintf_s(ops, "%s", simName.c_str());
	xdata = mymx.createElement(xcell, "Data", ops);
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Filename");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	// enter view code to make .model file
	xrow = mymx.createElement(xtable, "Row");  // row for command vi
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "vi");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "View ...");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for command si
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "si");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "... sim");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for title (read)
	xcell = mymx.createElement(xrow, "Cell");
	sprintf_s(ops, "%s", simName.c_str()); // same as title above
	xdata = mymx.createElement(xcell, "Data", ops);
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Filename");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for command al
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "al");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Save ALL simulation information");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for command y
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "y");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Save model setup");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for command n
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "n");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Not default name");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for output file name
	xcell = mymx.createElement(xrow, "Cell");
	sprintf_s(ops, "%s.txt", simName.c_str()); // same as title above, but add .txt
	xdata = mymx.createElement(xcell, "Data", ops);
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Full file name");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for blank
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Blank to continue");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for END
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "en");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "ENd to exit HVACGEN");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for blank
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Enter to continue- exits HVACGEN");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for Start Slimcon
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "..\\slimcon");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Start slimcon- adjust path if necessary");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for input file name
	xcell = mymx.createElement(xrow, "Cell");
	sprintf_s(ops, "%s", simName.c_str()); // same as title above
	xdata = mymx.createElement(xcell, "Data", ops);
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "input sim file name");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for allow algebraic variables- default NO
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Enter to continue with default");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xrow = mymx.createElement(xtable, "Row");  // row for enter to exit
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Enter to exit- this is the final line.");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");
	stateIndex = 1; // need to reset this

	// *********************
	// Add Boundy sheets
	//print boundary file (xxx.bnd) info- will need to be copied to .bnd file but this is a start
	CString boundaryData;
	xml_element *xrt1;  // element for time 1
	xml_element *xrt2;  // element for time 2
	xml_element *xrt3;  // element for time 3
	xml_element *xrt4;  // element for time 4

	xws = mymx.createElement(xwb, "Worksheet");
	xp1 = mymx.addprop(xws, "ss:Name", "Boundary Data");
	xtable = mymx.createElement(xws, "Table");
	xcol = mymx.createElement(xtable, "Column");
	xp1 = mymx.addprop(xcol, "ss:Index", "1");
	xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
	xp1 = mymx.addprop(xcol, "ss:Width", "110");
	xrow = mymx.createElement(xtable, "Row");
	xrt1 = mymx.createElement(xtable, "Row");
	xrt2 = mymx.createElement(xtable, "Row");
	xrt3 = mymx.createElement(xtable, "Row");
	xrt4 = mymx.createElement(xtable, "Row");

	// add cells for header row
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Time");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	// add time cells for next rows
	xcell = mymx.createElement(xrt1, "Cell");
	xdata = mymx.createElement(xcell, "Data", "0");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrt2, "Cell");
	xdata = mymx.createElement(xcell, "Data", "3600");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrt3, "Cell");
	xdata = mymx.createElement(xcell, "Data", "36000");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrt4, "Cell");
	xdata = mymx.createElement(xcell, "Data", "72000");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	// print title row labels
	for (size_t iCat = 1; iCat <= 8; iCat++) {
		if (maxCat[iCat] == 0) continue;  // skip printing blank for categories with no items
		for (size_t iInd = 1; iInd <= maxCat[iCat]; iInd++) {
			if (state[iCat][iInd].IsBoundary() == 0) continue;
			xcell = mymx.createElement(xrow, "Cell");
			if (state[iCat][iInd].GetInputCount() > 0) {
				state[iCat][iInd].GetInputInfo(0, &unit, &unitIndex);
				ss1 = units[unit - 1].GetTypeP()->GetInputLabel(unitIndex);
				sprintf_s(ops, "%s", ss1.c_str());
			}
			else sprintf_s(ops, "NA");	// NOTE this should never happen for a boundary variable

			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			// add data to time rows
			xcell = mymx.createElement(xrt1, "Cell");
			xdata = mymx.createElement(xcell, "Data", state[iCat][iInd].GetInitValue());
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrt2, "Cell");
			xdata = mymx.createElement(xcell, "Data", state[iCat][iInd].GetInitValue());
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrt3, "Cell");
			xdata = mymx.createElement(xcell, "Data", state[iCat][iInd].GetInitValue());
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrt4, "Cell");
			xdata = mymx.createElement(xcell, "Data", state[iCat][iInd].GetInitValue());
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}
	}

	// *********************
	// Add Category sheets
	// loop through for each variable type and create tables
	for (size_t iCat = 0; iCat < 8; iCat++){
		xws = mymx.createElement(xwb, "Worksheet");
		sprintf_s(ops, "%s (%zd)", catNames[iCat], iCat+1);
		xp1 = mymx.addprop(xws, "ss:Name", ops);
		xtable = mymx.createElement(xws, "Table");
		xcol = mymx.createElement(xtable, "Column");
		xp1 = mymx.addprop(xcol, "ss:Index","1");
		xp1 = mymx.addprop(xcol, "ss:AutoFitWidth","0");
		xp1 = mymx.addprop(xcol, "ss:Width","110");
		xrow = mymx.createElement(xtable, "Row");
		// add cells for header row
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data","stateIndex");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "categoryIndex");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "description");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "Init Value");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "OPunit");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");	// MAG 191008 add unit#
		xdata = mymx.createElement(xcell, "Data", "OPunit#");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "OPunitIndex");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "OPtype");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "IPunit");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");	// MAG 191008 add unit#
		xdata = mymx.createElement(xcell, "Data", "IPunit#");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "IPunitIndex");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "IPtype");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		catIndex = 0;

		int rowCount = 0;
		int catCount = 0;
		for (std::vector<StateElement>::iterator ti = state[iCat+1].begin(); ti != state[iCat+1].end(); ti++) {
			// make sure we're using this element
			//if ((ti->GetCategory() == 0) && (ti->GetCategoryIndex() == 0)) continue;
			if (catCount > maxCat[iCat + 1]) {
				continue;
			} else {
				catCount++;
			}
			// start new row for data
			xrow = mymx.createElement(xtable, "Row");
			rowCount++;
			// get info to print in row
			unit = -1;
			unitIndex = -1;
			ioType = -1;
			ss1 = "NA";

			// get description (col 3)
			if (ti->GetOutputCount() > 0) {
				ss1 = ti->GetOutputInfo(0, &unit, &unitIndex);
				hvt = units[unit - 1].GetTypeP();
				if (hvt != NULL) {
					ss1 = hvt->GetOutputLabel(unitIndex);
					ioType = hvt->GetType();
				}
				//else {
				//	ss1 = "NA";
				//}
			} else if (ti->GetInputCount() > 0) {  // get just the label from the inputs, if any
				ss1 = ti->GetInputInfo(0, &unit, &unitIndex);
				hvt = units[unit - 1].GetTypeP();
				if (hvt != NULL) {
					ss1 = hvt->GetInputLabel(unitIndex);
				}
				// these must be reset, they have input info not output info
				unit = -1;
				unitIndex = -1;
				//else {
				//	ss1 = "NA";
				//}
			} else {	// no inputs, no outputs- just filler. Print stateIndex, catID, and NA
				xcell = mymx.createElement(xrow, "Cell");
				if (ti == state[iCat + 1].begin()) {
					xdata = mymx.createElement(xcell, "Data", "-");
					xp1 = mymx.addprop(xdata, "ss:Type", "String");
				} else {
					xdata = mymx.createElement(xcell, "Data", stateIndex);
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");
					stateIndex++;
				}

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", catIndex);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");
				catIndex++;

				sprintf_s(ops, "%s", ss1.c_str());
				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", "NA");
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				continue;
			}

			//if (unitIndex >= 0) unitIndex++;  // NOTE- up to here unitIndex is zero-based, change to 1-based for printing, unless it's -1;
			sprintf_s(ops, "%s", ss1.c_str());
			sprintf_s(lineLabel, "%s", ss1.c_str());  // preserved for IP
			// IP specific info
			for (size_t i = 0; i < ti->GetOutputCount(); i++) {
				if (i > 0) {  // print subsequent OPs on their own shiny NEW row
					xrow = mymx.createElement(xtable, "Row");
					ss1 = ti->GetOutputInfo(i, &unit, &unitIndex);
					hvt = units[unit - 1].GetTypeP();
					if (hvt != NULL) {
						ss1 = hvt->GetOutputLabel(unitIndex);
					}
					sprintf_s(ops, "%s", ss1.c_str());
				}
				if (unitIndex >= 0) unitIndex++;  // NOTE- up to here unitIndex is zero-based, change to 1-based for printing, unless it's -1;
				
				// add cells for index/description
				xcell = mymx.createElement(xrow, "Cell");
				if (ti == state[iCat + 1].begin()) {
					xdata = mymx.createElement(xcell, "Data", "-");
					xp1 = mymx.addprop(xdata, "ss:Type", "String");
				} else {
					xdata = mymx.createElement(xcell, "Data", stateIndex);
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");
				}

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", catIndex);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				xcell = mymx.createElement(xrow, "Cell"); // description
				xdata = mymx.createElement(xcell, "Data", ops);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				xcell = mymx.createElement(xrow, "Cell");	// init value
				xdata = mymx.createElement(xcell, "Data", ti->GetInitValue());
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				// OP specific info - MAG 190905 update to use label
				ss1 = units[unit - 1].GetLabel();
				sprintf_s(labelString, "%s", ss1.c_str());
				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", labelString);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				//sprintf_s(matchString, "=MATCH(RC[-1],'Unit List'!R[%d]C[:$F$%d,0)", -1*(rowCount-1), units.size() + 1);-line not finished
				// MAG TODO- low priority to update this, just putting # for now
				xcell = mymx.createElement(xrow, "Cell");	// MAG 191008 add unit#
				//xp1 = mymx.addprop(xcell, "ss:Formula", matchString);
				xdata = mymx.createElement(xcell, "Data", unit);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", unitIndex);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data",(int) iCat+1);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");
			}
			
			if (ti->GetOutputCount() == 0) { // need to print labels, etc. for first IP line if not printed by OP
				 // add cells for index/description
				xcell = mymx.createElement(xrow, "Cell");
				if (ti == state[iCat + 1].begin()) {
					xdata = mymx.createElement(xcell, "Data", "0");
					xp1 = mymx.addprop(xdata, "ss:Type", "String");
				} else {
					xdata = mymx.createElement(xcell, "Data", stateIndex);
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");
				}

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", catIndex);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				ss1 = ti->GetInputInfo(0, &unit, &unitIndex);
				hvt = units[unit - 1].GetTypeP();
				if (hvt != NULL) {
					ss1 = hvt->GetInputLabel(unitIndex);
					sprintf_s(ops, "%s", ss1.c_str());
					//ioType = hvt->GetType();
				}
				xcell = mymx.createElement(xrow, "Cell");	// description
				xdata = mymx.createElement(xcell, "Data", ops);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				xcell = mymx.createElement(xrow, "Cell");	// init value
				xdata = mymx.createElement(xcell, "Data", ti->GetInitValue());
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				// MAG 200121 add check for input count and adding 4 blank lines
				if (ti->GetInputCount() == 0)
				{
					continue;
				}

				// add four blank lines to skip inputs past unused output columns
				xcell = mymx.createElement(xrow, "Cell"); xdata = mymx.createElement(xcell, "Data", ""); xp1 = mymx.addprop(xdata, "ss:Type", "String");
				xcell = mymx.createElement(xrow, "Cell"); xdata = mymx.createElement(xcell, "Data", ""); xp1 = mymx.addprop(xdata, "ss:Type", "String");
				xcell = mymx.createElement(xrow, "Cell"); xdata = mymx.createElement(xcell, "Data", ""); xp1 = mymx.addprop(xdata, "ss:Type", "String");
				xcell = mymx.createElement(xrow, "Cell"); xdata = mymx.createElement(xcell, "Data", ""); xp1 = mymx.addprop(xdata, "ss:Type", "String");
			}

			// IP specific info
			for (size_t i = 0; i < ti->GetInputCount(); i++){
				unit = -1;
				unitIndex = -1;
				ioType = -1;
				ss1 = "NA";

				if (i > 0) {  // print subsequent IPs on their own shiny NEW row
					xrow = mymx.createElement(xtable, "Row");
					rowCount++;  // MAG 191008 TODO check that all new rows are counted
					// add cells for index/description- same as above
					xcell = mymx.createElement(xrow, "Cell");
					if (ti->GetCategoryIndex() == 0) {
						xdata = mymx.createElement(xcell, "Data", "0");
					}
					else {
						xdata = mymx.createElement(xcell, "Data", stateIndex);
					}
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");

					xcell = mymx.createElement(xrow, "Cell");
					xdata = mymx.createElement(xcell, "Data", catIndex);
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");

					xcell = mymx.createElement(xrow, "Cell");
					xdata = mymx.createElement(xcell, "Data", lineLabel);
					xp1 = mymx.addprop(xdata, "ss:Type", "String");

					xcell = mymx.createElement(xrow, "Cell");	// init value
					xdata = mymx.createElement(xcell, "Data", ti->GetInitValue());
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");

					// four blank cells for OP
					xcell = mymx.createElement(xrow, "Cell");
					xdata = mymx.createElement(xcell, "Data");
					xp1 = mymx.addprop(xdata, "ss:Type", "String");

					xcell = mymx.createElement(xrow, "Cell");
					xdata = mymx.createElement(xcell, "Data");
					xp1 = mymx.addprop(xdata, "ss:Type", "String");

					xcell = mymx.createElement(xrow, "Cell");
					xdata = mymx.createElement(xcell, "Data");
					xp1 = mymx.addprop(xdata, "ss:Type", "String");

					xcell = mymx.createElement(xrow, "Cell");
					xdata = mymx.createElement(xcell, "Data");
					xp1 = mymx.addprop(xdata, "ss:Type", "String");
				}

				ss1 = ti->GetInputInfo(i, &unit, &unitIndex);
				hvt = units[unit - 1].GetTypeP();
				if (hvt != NULL) {
					ss1 = hvt->GetInputLabel(unitIndex);
					ioType = hvt->GetType();
				}

				if (unitIndex >= 0) unitIndex++;  // NOTE- up to here unitIndex is zero-based, change to 1-based for printing, unless it's -1;
												  // OP specific info - MAG 190905 update to use label
				ss1 = units[unit-1].GetLabel();
				sprintf_s(labelString, "%s", ss1.c_str());
				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", labelString);
				//xdata = mymx.createElement(xcell, "Data", unit);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");

				//sprintf_s(matchString, "=MATCH(I%d,'Unit List'!$F$2:$F$%d,0)", rowCount+1, units.size() + 1);
				//NOTE-see comment on previous MATCH
				xcell = mymx.createElement(xrow, "Cell");	// MAG 191008 add unit#
				xdata = mymx.createElement(xcell, "Data", unit);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");
				
				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data", unitIndex);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");

				xcell = mymx.createElement(xrow, "Cell");
				xdata = mymx.createElement(xcell, "Data",(int) iCat+1);
				xp1 = mymx.addprop(xdata, "ss:Type", "Number");
			}
			if(ti->GetCategoryIndex() > 0) stateIndex++;
			catIndex++;
		}
	}  // end for loop through categories

	mymx.print_token(mymx.getRoot(), ip);

	itemp = theApp.csdlg->m_status.GetCount();
	theApp.csdlg->m_status.DeleteString(itemp - 1);
	theApp.csdlg->m_status.AddString(L"The Simulation Information file has been saved.");

	itemp = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel(itemp - 1);

	unsavedChanges = 0;

	return 1;
} // end SaveSimInfoFile


// MAG Saves a new typar file to default name with date stamp
void 
CSimInfoApp::SaveTypeFile()
{
	FILE *op = nullptr;
	//int iTypes;
	//int iSubs;
	int i = 0;
	CTime tnow = CTime::GetCurrentTime();
	//char opName[MAX_PATH] = {};
	std::string ss1;
	std::vector<HVType>::iterator ti;
	CString cs1;
	wchar_t opw[MAX_PATH] = {};
	wchar_t	*cwdreturn = nullptr;

	cs1.Format(L"TYPAR %02d%02d%02d.dat", tnow.GetYear(), tnow.GetMonth(), tnow.GetDay());
	//op = fopen(opName, "w");
	_wfopen_s(&op, cs1, L"w");
	
	if(_tgetcwd(opw, MAX_PATH) == nullptr) { // MAG this should never happen...
		csdlg->m_status.AddString(L"Error opening TYPAR output file.");
		return;
	}

	if (op == nullptr) {
		csdlg->m_status.AddString(L"Error opening TYPAR output file.");
		return;
	}

	fprintf(op, "***** TYPAR file saved by SimInfo %02d//%02d//%02d*****\n",tnow.GetYear(), tnow.GetMonth(), tnow.GetDay());
	for (ti = types.begin(); ti != types.end(); ti++) {
		ss1 = ti->GetTypeLabel();
		fprintf(op, "%d\t'%s'\n", ti->GetType(), ss1.c_str());
		fprintf(op, "%d %d %zd %zd %zd\t!numbers of saved, diff.eq., xin, out, parameters\n", ti->GetCountSaved(), ti->GetCountDiffEQ(),
			ti->GetCountIn(), ti->GetCountOut(), ti->GetCountParameter());
		// print input IDs
		for (int i = 0; i < ti->GetCountIn(); i++) {
			fprintf(op, "%d '%s'\n", ti->GetInputCategory(i), ti->GetInputLabel(i).c_str());
		}
		fprintf(op, "#\n");
		// print output IDs
		for (int i = 0; i < ti->GetCountOut(); i++) {
			fprintf(op, "%d '%s'\n", ti->GetOutputCategory(i), ti->GetOutputLabel(i).c_str());
		}
		for (int i = 0; i < ti->GetCountModifiers(); i++) {
			fprintf(op, "%s\n", ti->GetModifier(i).c_str());
		}
		fprintf(op, "#\n");
		// print parameter IDs
		for (int i = 0; i < ti->GetCountParameter(); i++) {
			fprintf(op, "%d '%s'\n",i, ti->GetParameterLabel(i).c_str());
		}
		fprintf(op, "***************************************\n");
	}

	csdlg->m_status.AddString(L"Type file has been saved.");

	i = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel(i - 1);

	fclose(op);
} // end SaveTypeFile


// builds tree for view window
// basics copied from Center->BuildHVACTree
// builds the tree for Types
void
CSimInfoApp::BuildTypeTree(CTreeCtrl *tree)
{
	TV_ITEM item;
	TV_INSERTSTRUCT insert;
	std::string ss1;
	std::wstring ws1;
	size_t st = 0;
	size_t i = 0;
	HTREEITEM titem = nullptr;
	HTREEITEM ioitem = nullptr;
	HTREEITEM instanceitem = nullptr;

	item.hItem = nullptr;
	item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_TEXT | TVIF_STATE;
	item.stateMask = TVIS_EXPANDED;
	item.state = 0;
	item.cchTextMax = 40;
	item.iImage = 0;
	item.iSelectedImage = 0;
	item.cChildren = 1;
	item.lParam = 0;
	insert.hParent = nullptr;
	insert.hInsertAfter = TVI_LAST;
	insert.item = item;
	wchar_t mtext[100] = {};
	CString cs1;

	insert.item.pszText = &mtext[0];

	for (std::vector<HVType>::iterator ti = types.begin(); ti != types.end(); ti++) {
		if (ti->GetInTree() == 1) continue;  // used when entering multiple types files
		ss1 = ti->GetTypeLabel();
		i = ti->GetType();
		cs1.Format(L"%zu: %S", i, ss1.c_str());
		
		insert.item.pszText = cs1.GetBuffer();

		titem = tree->InsertItem(&insert);
		if (ti->GetUseCount() > 0) tree->SetItemState(titem, TVIS_BOLD,TVIS_BOLD); // MAG 200918 add bold to used types
		ti->SetTreepos(titem);
		
		st = ti->GetCountIn();
		cs1.Format(L"Inputs (%zu)", st);
		ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
		for (i = 0; i < st; i++) {
			ss1 = ti->GetInputLabel(i);
			cs1.Format(L"%zu: Cat. %d: %S", i+1, ti->GetInputCategory(i), ss1.c_str());
			instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
			ti->SetInputTreeItem(i, instanceitem);
		}

		st = ti->GetCountOut();
		cs1.Format(L"Outputs (%zu)", st);
		ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
		for (i = 0; i < st; i++){
			ss1 = ti->GetOutputLabel(i);
			cs1.Format(L"%zu: Cat. %d: %S",i+1, ti->GetOutputCategory(i), ss1.c_str());
			instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
			ti->SetOutputTreeItem(i, instanceitem);
		}

		st = ti->GetCountParameter();
		cs1.Format(L"Parameters (%zu)", st);
		ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
		for (i = 0; i < st; i++) {
			ss1 = ti->GetParameterLabel(i);
			cs1.Format(L"%zu: Label: %S", i+1, ss1.c_str());
			instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
			ti->SetParamTreeItem(i, instanceitem);
		}
		ti->SetInTree();
	}

	return;
}	// end BuildTypeTree


// MAG 200724
// Creates labels for Unit tree
// Type of label is determined by iop
// iop = 0 Print top level unit label
// iop = 1 Print label for input indicated by index
// iop = 2 Print label for output indicated by index
// iop = 3 Print label for parameter indicated by index
// iop = -1 Print label for Input node
// iop = -2 Print label for Output node
CString
CSimInfoApp::BuildUnitTreeLabel(std::vector<HVUnit>::iterator ti, int iop, size_t index)
{
	for (size_t i = 0; i < units.size(); i++)
	{
		if (ti->GetUnit() == units[i].GetUnit()) {
			return BuildUnitTreeLabel(&units[i], iop, index);
		}
	}

	return CString("NA");
}


CString
CSimInfoApp::BuildUnitTreeLabel(HVUnit *ti, int iop, size_t index)
{
	CString cs1;
	std::string ss1;
	std::string ss2;
	HVType *hvt = nullptr;
	double itemVal = 0;

	//if (ti == nullptr) return CString("NA");
	hvt = ti->GetTypeP();

	switch (iop) {
		case 0: // base node
			ss1 = hvt->GetTypeLabel();
			ss2 = ti->GetLabel();
			cs1.Format(L"Unit %zu:%zu:%zu: %S : %S", ti->GetSuperBlock(), ti->GetBlock(), ti->GetUnit(), ss1.c_str(), ss2.c_str());
			break;
		case 1:	// input
			ss1 = hvt->GetInputLabel(index);
			//itemVal = state[hvt->GetInputCategory(index)][ti->GetInput(index)].GetInitValue();
			itemVal = ti->GetInitInputValue(index);
			cs1.Format(L"%zu: Cat: %zu Index: %zu Label: \"%S\" Init val: %5.3f", index + 1, hvt->GetInputCategory(index), ti->GetInput(index), ss1.c_str(), itemVal);
			break;
		case 2:	// output
			ss1 = hvt->GetOutputLabel(index);
			//itemVal = state[hvt->GetOutputCategory(index)][ti->GetOutput(index)].GetInitValue();
			itemVal = ti->GetInitOutputValue(index);
			cs1.Format(L"%zu: Cat: %zu Index: %zu Label: \"%S\" Init val: %5.3f", index + 1, hvt->GetOutputCategory(index), ti->GetOutput(index), ss1.c_str(), itemVal);
			break;
		case 3:	// parameter
			ss1 = hvt->GetParameterLabel(index);
			itemVal = ti->GetParameter(index);
			if (abs(itemVal) < 0.01) {
				cs1.Format(L"%zu: Label: \"%S\" Value: %5.3e", index + 1, ss1.c_str(), itemVal);
			} else {
				cs1.Format(L"%zu: Label: \"%S\" Value: %5.4f", index + 1, ss1.c_str(), itemVal);
			}
			break;
		case -1:	// Input node
			cs1.Format(L"Inputs (%zu)", ti->GetInputCount());
			break;
		case -2:	// Output node
			cs1.Format(L"Outputs (%zu)", ti->GetOutputCount());
			break;
		case -3:	// Parameter node
			cs1.Format(L"Parameters (%zu)", ti->GetParameterCount());
			break;
		default:
			cs1 = "NA";
			break;
	}

	return cs1;
} // end BuildUnitTreeLabel


// builds the tree for Units
void
CSimInfoApp::BuildUnitTree(CTreeCtrl *tree) 
{
	TV_ITEM item;
	TV_INSERTSTRUCT insert;
	std::string ss1;
	std::string ss2;
	std::wstring ws1;
	size_t st = 0;
	size_t i = 0;
	HTREEITEM titem;
	HTREEITEM ioitem;
	HTREEITEM instanceitem;
	HVType *hvt = nullptr;

	item.hItem = nullptr;

	item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_TEXT | TVIF_STATE;
	item.stateMask = TVIS_EXPANDED;
	item.state = 0;
	item.cchTextMax = 40;
	item.iImage = 0;
	item.iSelectedImage = 0;
	item.cChildren = 1;
	item.lParam = 0;
	insert.hParent = nullptr;
	insert.hInsertAfter = TVI_LAST;
	insert.item = item;
	wchar_t mtext[100] = {};
	CString cs1;
	CString cs2;

	if (tree == nullptr) tree = &theApp.csdlg->GetUnitdlg()->m_tree;

	tree->DeleteAllItems();
	insert.item.pszText = &mtext[0];

	for (std::vector<HVUnit>::iterator ti = units.begin(); ti != units.end(); ti++) {
		hvt = ti->GetTypeP();
		ss1 = hvt->GetTypeLabel();
		ss2 = ti->GetLabel();
		cs1 = BuildUnitTreeLabel(ti, 0, 0);
		insert.item.pszText = cs1.GetBuffer();
		titem = tree->InsertItem(&insert);
		ti->SetTreepos(titem);

		st = ti->GetInputCount();
		cs1 = BuildUnitTreeLabel(ti, -1, 0);
		ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
		for (i = 0; i < st; i++) {
			cs1 = BuildUnitTreeLabel(ti, 1, i);
			instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
			ti->SetInputTreeItem(i,instanceitem);
		}
	
		st = ti->GetOutputCount();
		cs1 = BuildUnitTreeLabel(ti, -2, 0);
		ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
		for (i = 0; i < st; i++) {
			cs1 = BuildUnitTreeLabel(ti, 2, i);
			instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
			ti->SetOutputTreeItem(i, instanceitem);
		}
		
		st = ti->GetParameterCount();
		cs1 = BuildUnitTreeLabel(ti, -3, 0);
		ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
		for (i = 0; i < st; i++) {
			cs1 = BuildUnitTreeLabel(ti, 3, i);
			instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
			ti->SetParamTreeItem(i, instanceitem);
		}
	}

	return;
}  // end BuildUnitTree


// builds the tree for States
// NOTE- this assumes state array has been updated. If changes have been made call BuildState() first.
void
CSimInfoApp::BuildStateTree(CTreeCtrl *tree)
{
	TV_ITEM item = {};
	TV_INSERTSTRUCT insert = {};
	std::string ss1;
	std::wstring ws1;
	size_t st = 0;
	size_t i = 0;
	HTREEITEM titem = nullptr;
	HTREEITEM ioitem = nullptr;
	HTREEITEM instanceitem = nullptr;
	wchar_t mtext[100] = {};
	CString cs1;
	CString sText;
	int stateCounter = 1;
	int catCounter = 0;
	size_t unit = 0;
	size_t unitIndex = 0;
	char catNames[][20] = { "Pressure","Mass Flow Rate","Temperature","Control","Other","Energy","Power","Humidity" };

	if(tree == nullptr) tree = &theApp.csdlg->GetStatedlg()->m_tree;
	if (tree == nullptr) return;	// indicates not init'd

	tree->DeleteAllItems();
	item.hItem = nullptr;

	item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_TEXT | TVIF_STATE;
	item.stateMask = TVIS_EXPANDED;
	item.state = 0;
	item.cchTextMax = 40;
	item.iImage = 0;
	item.iSelectedImage = 0;
	item.cChildren = 1;
	item.lParam = 0;
	insert.hParent = nullptr;
	insert.hInsertAfter = TVI_LAST;
	insert.item = item;

	insert.item.pszText = &mtext[0];
	StateElement *ti;

	for (int cati = 1; cati <= 8; cati++) {
		catCounter = 0;
		for (int cj = 0;cj <= maxCat[cati];cj++){
			if (catCounter > maxCat[cati]) {
				continue;
			} else {
				catCounter++;
			}
			ti = &state[cati][cj];
			//if (ti == state[cati].begin()) continue;
			cs1 = BuildStateTreeLabel(ti);

			insert.item.pszText = cs1.GetBuffer();
			titem = tree->InsertItem(&insert);
			ti->SetTreepos(titem);	// MAG 200224 add this line

			st = ti->GetInputCount();
			cs1.Format(L"Input to (%zu)", st);
			ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
			ti->SetTreeposInput(ioitem);
			for (i = 0; i < st; i++) {
				ss1 = ti->GetInputInfo(i,&unit,&unitIndex);
				cs1.Format(L"%zu: Unit: %zu Type: %zu UnitInp : %zu", i+1 , unit, units[unit-1].GetType(), unitIndex+1);
				instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
				ti->SetInputTreelink(i, instanceitem);
			}

			st = ti->GetOutputCount();
			cs1.Format(L"Output to (%zu)", st);
			ioitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, titem);
			ti->SetTreeposOutput(ioitem);
			for (i = 0; i < st; i++) {
				ss1 = ti->GetOutputInfo(i, &unit, &unitIndex);
				cs1.Format(L"%zu: Unit: %zu Type: %zu UnitOp : %zu", i+1, unit, units[unit-1].GetType(), unitIndex+1);
				instanceitem = tree->InsertItem(cs1.GetBuffer(), 0, 0, ioitem);
				ti->SetOutputTreelink(i, instanceitem);
			}
		}
	}

} // end BuildStateTree


HVUnit
*CSimInfoApp::GetUnitByID(size_t unitID)
{
	if (unitID < 1) return NULL;

	for (std::vector<HVUnit>::iterator iti = units.begin(); iti != units.end(); iti++){
		if (unitID == iti->GetUnit()) return &(*iti);
	}

	return NULL;
}


// // generates the tree label for a state item
CString 
CSimInfoApp::BuildStateTreeLabel(StateElement *ti) 
{
	CString cs1 = CString("NA");
	CString cs2;
	CString valString;
	std::string ss1;
	std::wstring ws1;
	size_t unit=0;
	size_t unitIndex=0;
	HVUnit* hvu = nullptr;
	HVType *hvt = nullptr;
	char catNames[][20] = { "Pressure","Mass Flow Rate","Temperature","Control","Other","Energy","Power","Humidity" };
	size_t stateCounter=0;
	double initVal = 0;
	CString initValString;

	if (ti == nullptr) {
		cs1 = "NA";
		return cs1;
	}

	if (ti->GetInitValueValid()) {
		initVal = ti->GetInitValue();
		if ((abs(initVal) > 0) && (abs(initVal) < 0.1))
			valString.Format(L"; Value: %5.2e", ti->GetInitValue());
		else
			valString.Format(L"; Value %5.2f", ti->GetInitValue());
	}

	// get label from output
	if (ti->GetOutputCount() > 0) {
		ss1 = ti->GetOutputInfo(0, &unit, &unitIndex);
		hvu = GetUnitByID(unit);
		if (hvu != NULL) {
			hvt = hvu->GetTypeP();
			if (valString.GetLength() == 0) {
				initVal = hvu->GetInitOutputValue(unitIndex);
				if((abs(initVal) > 0)&&(abs(initVal) < 0.1)) 
					valString.Format(L"; Value: %5.2e", hvu->GetInitOutputValue(unitIndex));
				else 
					valString.Format(L"; Value: %5.2f", hvu->GetInitOutputValue(unitIndex));
			}
		}
		if ((hvt != NULL)&&(hvt->GetCountOut() > 0)) {
			ss1 = hvt->GetOutputLabel(unitIndex);
		} else {
			ss1 = "NA no output label";// this shouldn't happen
		}
	} else if (ti->GetInputCount() > 0) {	// or get label from input
		ss1 = ti->GetInputInfo(0, &unit, &unitIndex);
		hvu = GetUnitByID(unit);
		if (hvu != NULL) {
			hvt = hvu->GetTypeP();
			if (valString.GetLength() == 0) {
				initVal = hvu->GetInitInputValue(unitIndex);
				if ((abs(initVal) > 0) && (abs(initVal) < 0.1)) 
					valString.Format(L"; Value: %5.2e", hvu->GetInitInputValue(unitIndex));
				else 
					valString.Format(L"; Value: %5.2f", hvu->GetInitInputValue(unitIndex)); }
		}
		if (hvt != NULL) {
			ss1 = hvt->GetInputLabel(unitIndex);
		} else {
			ss1 = "NA no input label"; // this shouldn't happen
		}
	} else {	// or there is no label, might happen often
		ss1 = "NA";
		valString.Format(L"; Value: %5.2f", ti->GetInitValue());
	}

	// MAG 190618 do not advance state counter for category index 0
	if (ti->GetCategoryIndex() == 0) {
		cs1.Format(L"State Array: Start Category %S", catNames[ti->GetCategory() - 1]);
	} else {
		stateCounter = ti->GetStateIndex();
		cs1.Format(L"State Array: %zu; Category: [%zu:%zu]; Input to: %zu; Output to: %zu; Label: %S%s", stateCounter, ti->GetCategory(), ti->GetCategoryIndex(), ti->GetInputCount(), ti->GetOutputCount(), ss1.c_str(), valString);
		if (ti->IsBoundary() > 0) cs1 += " (B)";	// MAG 200324 add this line
		if (ti->IsReported() > 0) {
			cs1 += " (R";	// MAG 200721 add this line
			for (size_t i = 0; i < ti->ReportedCount(); i++){
				cs2.Format(L" %zu", ti->GetReportedIndex(i));
				cs1 += cs2;
			}
			cs1 += ")";
		}
		stateCounter++;
	}

	return cs1;
}


// finds the TYPE referenced by a UNIT.  UNIT is found by searching for matching tree link (hti).
// When the type is found, it is selected in the TYPE display window. The index of the UNIT is returned.
// This subroutine is called when a selection change occurs in a MTreeCtrl
// MAG Note- this is poorly named, and the return value (not used anywhere) is inaccurate-TODO fix
int
CSimInfoApp::FindUnitTreePos(HTREEITEM hti) 
{
	HVType *hvt = nullptr;
	HVUnit* hvu = nullptr;
	//TV_ITEM pitem;	// parent of item, hopefully
	size_t i = 0;
	size_t j = 0;
	int isIOP=0;
	size_t iop=0;
	size_t unit = 0;
	size_t unitIndex; // NOTE- unit is a label [1-X], unitIndex is unit-1
	size_t cat;
	size_t catID;
	HTREEITEM lht = 0; // local htree item

	for (i = 0; i < units.size(); i++){
		// check if clicked on a unit
		hvt = units[i].GetTypeP();
		if (units[i].GetTreepos() == hti) {
			if(hvt->GetTreepos() != NULL) csdlg->Getdtdlg()->m_tree.Select(hvt->GetTreepos(), TVGN_FIRSTVISIBLE);
			unit = units[i].GetUnit();
			break;
		}
		isIOP = 0;
		// check if clicked on an i/o/p of the unit
		for (j = 0; j < units[i].GetInputCount(); j++){
			if(units[i].GetInputTreeItem(j) == hti){
				lht = hvt->GetInputTreeItem(j);
				if (lht != NULL) {
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_FIRSTVISIBLE);
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_CARET);
				}
				iop = j;
				unit = units[i].GetUnit();
				isIOP = 1;
				break;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetOutputCount(); j++) {
			if (units[i].GetOutputTreeItem(j) == hti) {
				lht = hvt->GetOutputTreeItem(j);
				if (lht != NULL){
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_FIRSTVISIBLE);
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_CARET);
				}
				iop = j;
				unit = units[i].GetUnit();
				isIOP = 2;
				break;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetParameterCount(); j++) {
			if (units[i].GetParamTreeItem(j) == hti) {
				lht = hvt->GetParamTreeItem(j);
				if (lht != NULL) {
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_FIRSTVISIBLE);
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_CARET);
				}
				iop = j;
				unit = units[i].GetUnit();
				isIOP = 3;
				break;
			}
		}
	}

	if (hvt == nullptr) return -1;

	// found unit/iop selected, now if iop select location in state array
	if (unit > 0) {
		unitIndex = unit;// -1;// use this for array references
		hvu = GetUnitByID(unitIndex);
		if (hvu != NULL) {
			switch (isIOP) {
			case 1:
				cat = hvt->GetInputCategory(iop);
				catID = hvu->GetInput(iop);
				if (cat == 0) {
					break;
				}
				lht = state[cat][catID].GetInputTreelink(unit, iop);
				break;
			case 2:
				cat = hvt->GetOutputCategory(iop);
				catID = hvu->GetOutput(iop);
				if (cat == 0) {
					break;
				}
				lht = state[cat][catID].GetOutputTreelink(unit, iop);
				break;
			default:
				lht = nullptr; // MAG 200721 parameters have no place in the state array
				break;
			}
		}
		if ((lht != NULL)&&(csdlg->GetStatedlg()->GetInit() == 1)) {
			csdlg->GetStatedlg()->m_tree.Select(lht, TVGN_PARENT);
			csdlg->GetStatedlg()->m_tree.Select(lht, TVGN_CARET);
			csdlg->GetStatedlg()->m_tree.EnsureVisible(lht);
			csdlg->GetStatedlg()->m_tree.SetLastSelectedState(lht);	// MAG 200722 added this line
		}

		return (int) unit;
	}

	return -1;
}	// end FindUnitTreePos


// MAG 201111
// finds the UNIT based on HTREEITEM data
// returns the units[] array index of the selected unit
int
CSimInfoApp::FindUnitTreeArrPos(HTREEITEM hti)
{
	HVType* hvt = nullptr;
	size_t i = 0;
	size_t j = 0;
	int isIOP = 0;
	int iop = 0;
	int unit = 0;
	HTREEITEM lht = 0; // local htree item

	for (i = 0; i < units.size(); i++) {
		// check if clicked on a unit
		hvt = units[i].GetTypeP();
		if (units[i].GetTreepos() == hti) {
			return (int) i;
		}
		isIOP = 0;
		// check if clicked on an i/o/p of the unit
		for (j = 0; j < units[i].GetInputCount(); j++) {
			if (units[i].GetInputTreeItem(j) == hti) {
				return (int) i;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetOutputCount(); j++) {
			if (units[i].GetOutputTreeItem(j) == hti) {
				return (int) i;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetParameterCount(); j++) {
			if (units[i].GetParamTreeItem(j) == hti) {
				return (int) i;
			}
		}
	}

	// not found!
	return -1;
}	// end FindUnitTreeArrPos


	// finds the UNIT referenced by a HTREEITEM.  UNIT is found by searching for matching tree link (hti).
	// When the type is found, the reporting status is flip-flopped based on the current status of first input.
	// This subroutine is called when r or R key is pressed when on a unit in the Units Display
int
CSimInfoApp::ChangeUnitReporting(HTREEITEM hti)
{
	HVType *hvt = nullptr;
	StateElement *pse = nullptr;
	size_t i = 0;
	size_t j = 0;
	int isIOP = 0;
	int iop = 0;
	int unit = -1;
	size_t unitIndex = -1; // NOTE- unit is a label [1-X], unitIndex is unit-1
	size_t ioIndex = -1;
	size_t cat = -1;
	size_t catID = -1;
	int setReport = 0;
	HTREEITEM lht = 0; // local htree item
	CString cs1;
	size_t sb = 1;

	for (i = 0; i < units.size(); i++) {
		// check if clicked on a unit
		hvt = units[i].GetTypeP();
		if (units[i].GetTreepos() == hti) {
			unitIndex = i;
			break;
		}
		isIOP = 0;
		// check if clicked on an i/o/p of the unit
		for (j = 0; j < units[i].GetInputCount(); j++) {
			if (units[i].GetInputTreeItem(j) == hti) {
				unitIndex = i;
				break;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetOutputCount(); j++) {
			if (units[i].GetOutputTreeItem(j) == hti) {
				unitIndex = i;
				break;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetParameterCount(); j++) {
			if (units[i].GetParamTreeItem(j) == hti) {
				return -1; // can't report on parameter
			}
		}
	}

	hti = csdlg->GetUnitdlg()->m_tree.GetLastSelectedUnit();
	if (hti > 0) {
		sb = units[unitIndex].GetSuperBlock();
	}

	if (unitIndex < 0) return -1;

	ioIndex = units[unitIndex].GetInput(0);
	cat = units[unitIndex].GetTypeP()->GetInputCategory(0);
	pse = &state[cat][ioIndex];
	setReport = pse->IsReported() == 0 ? 1 : 0; // flip reporting of first input
	unsavedChanges = 1;

	// found unit, now modify reporting
	for (i = 0; i < units[unitIndex].GetInputCount(); i++) {
		ioIndex = units[unitIndex].GetInput(i);
		cat = units[unitIndex].GetTypeP()->GetInputCategory(i);
		StateVecSetReported(state[cat][ioIndex].GetStateIndex(), sb, setReport);
		cs1 = BuildStateTreeLabel(&state[cat][ioIndex]);
		csdlg->GetStatedlg()->m_tree.SetItemText(state[cat][ioIndex].GetTreepos(),cs1);
	}
	for (i = 0; i < units[unitIndex].GetOutputCount(); i++) {
		ioIndex = units[unitIndex].GetOutput(i);
		cat = units[unitIndex].GetTypeP()->GetOutputCategory(i);
		StateVecSetReported(state[cat][ioIndex].GetStateIndex(), sb, setReport);
		cs1 = BuildStateTreeLabel(&state[cat][ioIndex]);
		csdlg->GetStatedlg()->m_tree.SetItemText(state[cat][ioIndex].GetTreepos(), cs1);
	}

	return unit;
}	// end ChangeUnitReporting


// MAG 200723
// ChangeUnitValue
int
CSimInfoApp::ChangeUnitValue(HTREEITEM hti)
{
	HVType *hvt = nullptr;
	size_t i = 0;
	size_t j = 0;
	int isIOP = 0;
	double newVal = 0;
	size_t iop = 0;
	size_t unit = 0;
	size_t unitIndex = -1; // NOTE- unit is a label [1-X], unitIndex is unit-1
	HTREEITEM lht = 0; // local htree item
	ValueDlg valueDlg;
	CString cs1;
	std::string ss1;

	vdlg = &valueDlg;
	isIOP = 0;
	for (i = 0; (i < units.size())&&(isIOP == 0); i++) {
		// check if clicked on a unit
		unitIndex = i;
		hvt = units[i].GetTypeP();
		if (units[i].GetTreepos() == hti) {
			if (hvt->GetTreepos() != NULL) csdlg->Getdtdlg()->m_tree.Select(hvt->GetTreepos(), TVGN_FIRSTVISIBLE);
			unit = units[i].GetUnit();
			break;
		}
		// check if clicked on an i/o/p of the unit
		for (j = 0; j < units[i].GetInputCount(); j++) {
			if (units[i].GetInputTreeItem(j) == hti) {
				lht = hvt->GetInputTreeItem(j);
				if (lht != NULL) {
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_FIRSTVISIBLE);
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_CARET);
				}
				iop = j;
				unit = units[i].GetUnit();
				isIOP = 1;
				break;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetOutputCount(); j++) {
			if (units[i].GetOutputTreeItem(j) == hti) {
				lht = hvt->GetOutputTreeItem(j);
				if (lht != NULL) {
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_FIRSTVISIBLE);
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_CARET);
				}
				iop = j;
				unit = units[i].GetUnit();
				isIOP = 2;
				break;
			}
		}
		if (isIOP > 0) break;
		for (j = 0; j < units[i].GetParameterCount(); j++) {
			if (units[i].GetParamTreeItem(j) == hti) {
				lht = hvt->GetParamTreeItem(j);
				if (lht != NULL) {
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_FIRSTVISIBLE);
					csdlg->Getdtdlg()->m_tree.Select(lht, TVGN_CARET);
				}
				iop = j;
				unit = units[i].GetUnit();
				isIOP = 3;
				break;
			}
		}
	}

	// error checking...
	if (hvt == nullptr) return -1;
	if (isIOP == 0) return -1;
	if (isIOP == 1) return -1;
	if (isIOP == 2) return -1;
	if (unitIndex == -1) return -1;

	i = valueDlg.DoModal();
	if (i != 1) return -1;

	newVal = _ttof(valueDlg.GetValueString());
	switch (isIOP){ // MAG really need a switch here?
		case 3: // Parameter
			cs1.Format(L"Parameter value changed from %f to %f.", units[unitIndex].GetParameter(iop), newVal);
			csdlg->m_status.AddString(cs1);
			units[unitIndex].SetParameter(iop, newVal);
			unsavedChanges = 1;

			cs1 = BuildUnitTreeLabel(&units[unitIndex], 3, iop);
			// MAG use range checking if needed
			//if (abs(newVal) < 0.01) {
			//	cs1.Format(L"%d: Label: \"%S\" Value: %5.3e", iop + 1, ss1.c_str(), newVal);
			//}
			//else {
			//	cs1.Format(L"%d: Label: \"%S\" Value: %5.4f", iop + 1, ss1.c_str(), newVal);
			//}
			csdlg->GetUnitdlg()->m_tree.SetItemText(hti, cs1);

			break;
		default:
			break;
	}

	return 1;
}  // end ChangeUnitValue


// MAG 200724
// Changes value by accessing state variable. Also updates labels on State and Unit windows.
int
CSimInfoApp::ChangeStateValue(HTREEITEM hti)
{
	int sIndex = 0;  // state index
	StateElement *myse = nullptr;
	int j = 0;
	ValueDlg valueDlg;
	double newVal = 0.0;
	size_t unitId = 0;
	size_t unitIndex = 0;
	HTREEITEM ht2 = nullptr;
	CString cs1;
	INT_PTR ip = 0;

	sIndex = FindStateTreePos(hti);
	myse = theApp.StateVec(sIndex + 1);
	if (myse == nullptr) return 0;

	cs1.Format(L"%f", myse->GetInitValue());
	valueDlg.SetValueString(cs1);

	ip = valueDlg.DoModal();
	if (ip != IDOK) {
		theApp.csdlg->m_status.AddString(L"Note: return without changing value");
		return -1;
	}

	newVal = _ttof(valueDlg.GetValueString());

	myse->SetInitValue(newVal);
	cs1 = BuildStateTreeLabel(myse);
	csdlg->GetStatedlg()->m_tree.SetItemText(myse->GetTreepos(), cs1);

	for (size_t i = 0; i < myse->GetInputCount(); i++) {
		j = myse->GetInputInfo(i, &unitId, &unitIndex);
		if (csdlg->GetUnitdlg()->GetInit() == 1) { // check that unit dialog has been built			
			ht2 = myse->GetInputUnit(i)->GetInputTreeItem(unitIndex);
			cs1 = BuildUnitTreeLabel(myse->GetInputUnit(i), 1, unitIndex);
			csdlg->GetUnitdlg()->m_tree.SetItemText(ht2, cs1);
		}
		myse->GetInputUnit(i)->SetInitInputValue(unitIndex, newVal);	// MAG add 220106
	}
	for (size_t i = 0; i < myse->GetOutputCount(); i++) {
		j = myse->GetOutputInfo(i, &unitId, &unitIndex);
		if (csdlg->GetUnitdlg()->GetInit() == 1) {	// check for unit here instead of around loop so it can still write init value
			ht2 = myse->GetOutputUnit(i)->GetOutputTreeItem(unitIndex);
			cs1 = BuildUnitTreeLabel(myse->GetOutputUnit(i), 2, unitIndex);
			csdlg->GetUnitdlg()->m_tree.SetItemText(ht2, cs1);
		}
		myse->GetOutputUnit(i)->SetInitOutputValue(unitIndex, newVal);	// MAG add 220106
	}

	unsavedChanges = 1;

	// TODO add for outputs after testing

	return 1;
}


int 
CSimInfoApp::MoveStateValue(HTREEITEM hti)
{
	// MAG NOTE- first section copied from FindStateTreePos
	size_t ioc = 0;
	StateElement* myse = nullptr;
	int endUnit=0;
	size_t stvar=0;
	size_t stateIndex = 0;
	int stateCount = 0; // records absolute index of items
	int returnVal = 0;	// use to adjust output if a category index is selected
	StateMoveDlg smd;
	INT_PTR ip = 0;

	// this filters for correct selection of I/O, not category header. hti is then sent as dragstart to DragState() after dragend is selected below.
	for (size_t i = 1; i < state.size(); i++) {  // scan through categories, should be in array 1-8
		for (size_t j = 0; j <= maxCat[i]; j++) {
			//not moving a category header
			if (state[i][j].GetTreepos() == hti) return returnVal;
			if (state[i][j].GetTreeposInput() == hti) return returnVal;
			if (state[i][j].GetTreeposOutput() == hti) return returnVal;

			ioc = state[i][j].GetInputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetInputTreelink(k) == hti) { // found it!
					state[i][j].GetInputInfo(k, &stvar, &stateIndex);
					returnVal = 1;  // for input
					break;
				}
			}
			if (returnVal != 0) break;

			ioc = state[i][j].GetOutputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetOutputTreelink(k) == hti) { // found it!
					state[i][j].GetOutputInfo(k, &stvar, &stateIndex);
					returnVal = 2;	// for output
				}
			}

			if (j > 0) stateCount++; // don't increment for category headers (index == 0)
		}
		if (returnVal != 0) break;
	}
	if (returnVal == 0) return returnVal;

	ip = smd.DoModal();
	if (ip != 1) return 0;

	stvar = _ttoi(smd.GetValue());

	myse = StateVec(stvar); // this does range checking

	if (myse == nullptr) return 0;

	if (DragState(hti, myse->GetTreepos()) > 0) {
		csdlg->m_status.AddString(L"Move successful.");
		unsavedChanges = 1;
	}

	return 1;
}


// finds the TYPE and UNIT referenced by a STATE elemnt.  TYPE and UNIT are found by searching for matching tree link (hti).
// When the type/unit are found, they are selected in the TYPE display window. The index of the STATE is returned.
// This subroutine is called when a selection change occurs in a MTreeCtrl
// Returns: the absolute index of the state (stateCount), regardless of which part of the tree was clicked on.
//			Also opens the state tree to the indicated position.
int
CSimInfoApp::FindStateTreePos(HTREEITEM hti)
{
	HVType *hvt = nullptr;
	HVUnit* hvu = nullptr;
	size_t ioc = 0;
	size_t stvar = 0;
	size_t stateIndex = 0;
	int stateCount = 0; // records absolute index of items
	int returnVal = 0;	// use to adjust output if a category index is selected
	HTREEITEM uht = nullptr;  // unit htreeitem

	for (size_t i = 1; i < state.size(); i++){  // scan through categories, should be in array 1-8
		for (size_t j = 0; j <= maxCat[i]; j++) {
			returnVal = j == 0 ? -1 : stateCount;	// adjust return if checking a category index (j == 0)
			if (state[i][j].GetTreepos() == hti) return returnVal;
			if (state[i][j].GetTreeposInput() == hti) return returnVal;
			if (state[i][j].GetTreeposOutput() == hti) return returnVal;

			ioc = state[i][j].GetInputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetInputTreelink(k) == hti) { // found it!
					state[i][j].GetInputInfo(k, &stvar, &stateIndex);
					hvu = GetUnitByID(stvar);
					uht = hvu->GetInputTreeItem(stateIndex);
					if (uht > 0) csdlg->GetUnitdlg()->m_tree.Select(uht, TVGN_CARET);

					hvt = units[stvar - 1].GetTypeP();
					if (hvt->GetTreepos() != NULL){
						csdlg->Getdtdlg()->m_tree.Select(hvt->GetTreepos(), TVGN_CARET);
						csdlg->Getdtdlg()->m_tree.EnsureVisible(hvt->GetTreepos());
						csdlg->Getdtdlg()->m_tree.SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0); // this and next line should put selected at top
						csdlg->Getdtdlg()->m_tree.EnsureVisible(hvt->GetTreepos());
					}
					return returnVal;
				}
			}

			ioc = state[i][j].GetOutputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetOutputTreelink(k) == hti) { // found it!
					state[i][j].GetOutputInfo(k, &stvar, &stateIndex);
					hvu = GetUnitByID(stvar);
					uht = hvu->GetOutputTreeItem(stateIndex);
					if (uht > 0) csdlg->GetUnitdlg()->m_tree.Select(uht, TVGN_CARET);
					hvt = units[stvar - 1].GetTypeP();
					if (hvt->GetTreepos() != NULL) {
						csdlg->Getdtdlg()->m_tree.Select(hvt->GetTreepos(), TVGN_CARET);
						csdlg->Getdtdlg()->m_tree.EnsureVisible(hvt->GetTreepos());
						csdlg->Getdtdlg()->m_tree.SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0); // this and next line should put selected at top
						csdlg->Getdtdlg()->m_tree.EnsureVisible(hvt->GetTreepos());
					}
					return returnVal;
				}
			}

			if(j > 0) stateCount++; // don't increment for category headers (index == 0)
		}
	}

	// If a something NOT a state index, category header, or input/output label was selected, it will finish here.
	return -1;
}	// end FindStateTreePos


// MAG 200722
// finds the CATEGORY referenced by a STATE element.  CATEGORY is found by searching for matching tree link (hti).
// Returns: the category of the state element, regardless of which part of the tree was clicked on.
int
CSimInfoApp::FindStateTreeCategory(HTREEITEM hti)
{
	size_t ioc = 0;
	int returnVal = 0;	// use to adjust output if a category index is selected

	for (size_t i = 1; i < state.size(); i++) {  // scan through categories, should be in array 1-8
												 //for (size_t j = 0; j < state[i].size(); j++) {
		for (size_t j = 0; j <= maxCat[i]; j++) {
			returnVal = (int) i;	// adjust return if checking a category index (j == 0)
			if (state[i][j].GetTreepos() == hti) return returnVal;
			if (state[i][j].GetTreeposInput() == hti) return returnVal;
			if (state[i][j].GetTreeposOutput() == hti) return returnVal;

			ioc = state[i][j].GetInputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetInputTreelink(k) == hti) { // found it!
					return returnVal;
				}
			}

			ioc = state[i][j].GetOutputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetOutputTreelink(k) == hti) { // found it!
					return returnVal;
				}
			}
		}
	}

	// If a something NOT a state index, category header, or input/output label was selected, it will finish here.
	return -1;
}	// end FindStateTreeCategory


// MAG
// Purpose: When a TYPE top node is selected, collapse UNITS display and expand UNITS of selected TYPE
// Note: Only when top node is selected, ignore selection of IOP as designed.
int
CSimInfoApp::FindTypeTreePos(HTREEITEM hti)
{
	int typeSelected = 0;
	size_t i = 0;

	for (i = 0; i < types.size(); i++){
		if (types[i].GetTreepos() == hti) {
			typeSelected = types[i].GetType();
			break;
		}
	}

	if (typeSelected == 0)	return -1;
	
	for (i = 0; i < units.size(); i++) {
		if (units[i].GetTreepos() == 0) continue;	// happens when initializing windows
		// check if clicked on a unit
		if (units[i].GetType() == typeSelected) {
			csdlg->GetUnitdlg()->m_tree.Expand(units[i].GetTreepos(), TVE_EXPAND);
		} else {
			csdlg->GetUnitdlg()->m_tree.Expand(units[i].GetTreepos(), TVE_COLLAPSE);
		}
	}
	
	return 1;
}


int
CSimInfoApp::ShowTypeTreeIndex(size_t index)
{
	return csdlg->Getdtdlg()->m_tree.Select(types[index].GetTreepos(),TVGN_CARET);
}


CategoryParameter::CategoryParameter()
{
	category = 0;
}


HVType::HVType()
{
	id = 0;
	countSaved = 0;
	countDiffEQ = 0;
	countIn = 0;
	countOut = 0;
	countFileParameter = 0;
	countParameter = 0;
	countUsed = 0;
	label = "";
	treepos = nullptr;
	inTree = 0;
}


HVType::~HVType()
{
}


int 
HVType::SetType(int val) 
{
	if (val < 0) return 0;
	if (val > 0xFFFF) return 0;
	id = val;

	return 1;
}


void
HVType::Reset()
{
	id = 0;
	countSaved = 0;
	countDiffEQ = 0;
	countIn = 0;
	countOut = 0;
	countParameter = 0;
	countFileParameter = 0;
	countUsed = 0;
	label = "";
	inputs.clear();
	outputs.clear();
	parameters.clear();
	modifiers.clear();
}


int 
HVType::SetLabel(char *newlabel) 
{
	if (newlabel == nullptr) return 0;

	label = newlabel;

	return 1;
}


int	
HVType::SetCountSaved(int val) 
{
	if (val < 0) return 0;
	if (val > 400) return 0;	// MAG 201202 changed from 0xFF. 400 is an artificial limit of this tool. 
	countSaved = val;
	
	return 1;
}


int	
HVType::SetCountDiffEQ(int val) 
{
	if (val < 0) return 0;
	if (val > 0xFF) return 0;
	countDiffEQ = val;

	return 1;
}


int	
HVType::SetCountIn(int val) 
{
	if (val < 0) return 0;
	if (val > 50) return 0;	// MAG 201202 changed from 0xFF. 
	countIn = val;

	return 1;
}


int	
HVType::SetCountOut(int val) 
{
	if (val < 0) return 0;
	if (val > 50) return 0;	// MAG 201202 changed from 0xFF. 
	countOut = val;

	return 1;
}

int	
HVType::SetCountParameter(int val) 
{
	if (val < 0) return 0;
	if (val > 30) return 0;	// MAG 201202 changed from 0xFF. 
	countParameter = val;

	return 1;
}


int
HVType::SetCountFileParameter(int val)
{
	if (val < 0) return 0;
	if (val > 0xFF) return 0; // NOTE- not setting to check if > countParameter in case this is set first
	countFileParameter = val;

	return 1;
}


int
HVType::AddInput(int cat, std::string label) 
{
	CategoryParameter newparam;

	if (cat < 1) return 0;
	if (cat > 8) return 0;
	if (label.length() == 0) return 0;
	newparam.SetCategory(cat);
	newparam.label = label;
	inputs.push_back(newparam);
	htlIn.push_back(0);

	return 1;
}


int 
HVType::AddOutput(int cat, std::string label)
{
	CategoryParameter newparam;

	if (cat < 1) return 0;
	if (cat > 8) return 0;
	if (label.length() == 0) return 0;
	newparam.SetCategory(cat);
	newparam.label = label;
	outputs.push_back(newparam);
	htlOut.push_back(0);

	return 1;
}


int 
HVType::AddParameter(int cat, std::string label)
{
	CategoryParameter newparam;

	if (cat < 1) return 0;
	//if (cat > 8) return 0;
	if (label.length() == 0) return 0;
	newparam.SetCategory(cat);
	newparam.label = label;
	parameters.push_back(newparam);
	htlParam.push_back(0);

	return 1;
}


HTREEITEM
HVType::GetInputTreeItem(size_t index) 
{
	if (index < 0) return 0;
	if (index >= inputs.size()) return 0;

	return htlIn[index];
}


HTREEITEM 
HVType::GetOutputTreeItem(size_t index) 
{
	if (index < 0) return 0;
	if (index >= outputs.size()) return 0;

	return htlOut[index];
}


HTREEITEM 
HVType::GetParamTreeItem(size_t index)
{
	if (index < 0) return 0;
	if (index >= parameters.size()) return 0;

	return htlParam[index];
}


void 
HVType::SetInputTreeItem(size_t index, HTREEITEM hti)
{
	if (index < 0) return;
	if (index >= inputs.size()) return;
	if (hti < 0) return;

	htlIn[index] = hti;
}


void 
HVType::SetOutputTreeItem(size_t index, HTREEITEM hti) 
{
	if (index < 0) return;
	if (index >= outputs.size()) return;
	if (hti < 0) return;

	htlOut[index] = hti;
}


void 
HVType::SetParamTreeItem(size_t index, HTREEITEM hti)
{
	if (index < 0) return;
	if (index >= parameters.size()) return;
	if (hti < 0) return;

	htlParam[index] = hti;
}


const int
HVType::GetInputCategory(size_t index) 
{
	if (index < 0) return 0;
	if (index >= inputs.size()) return 0;

	return(inputs[index].GetCategory());
}


const int
HVType::GetOutputCategory(size_t index)
{
	if (index < 0) return 0;
	if (index >= outputs.size()) return 0;

	return(outputs[index].GetCategory());
}


HVUnit::HVUnit()
{
	unit = 0;
	block = 0;
	superBlock = 0;
	myType = nullptr;
	treepos = nullptr;
}


// Sets unit number, not type
int 
HVUnit::SetUnit(size_t val)
{
	if (val < 0) return 0;
	if (val > 0xFFFF) return 0; // NOTE 40 units PER BLOCK, but unit numbering is NOT per block
	unit = val;
	
	return 1;
}


int 
HVUnit::SetBlock(size_t val)
{
	if (val < 0) return 0;
	if (val > 20) return 0;	// NOTE 20 blocks per superblock
	block = val;
	
	return 1;
}


int
HVUnit::SetSuperBlock(size_t val)
{
	if (val < 0) return 0;
	if (val > 40) return 0;	// NOTE max 40 super blocks per sim
	superBlock = val;
	
	return 1;
}


int
HVUnit::SetType(HVType *val) 
{
	if (val == nullptr) return 0;
	myType = val;

	return 1;
}

// MAG note- adds inputBoundary too
// val is index into state category
int
HVUnit::AddInput(int val) 
{
	if (val < 0) return 0;
	if (val > 400) return 0;	// MAG 201202 changed from 0xFF. 400 is an artificial limit of this tool. 
	inputs.push_back(val);
	inputBoundary.push_back(0);
	htlIn.push_back(0);
	initInputValue.push_back(-999.01);
	
	return 1;
}


int
HVUnit::AddInput(size_t val)
{
	int ival = (int)val;

	return AddInput(ival);
}


// MAG note- adds outputBoundary too
// val is index into state category
int
HVUnit::AddOutput(int val)
{
	if (val < 0) return 0;
	if (val > 400) return 0;	// MAG 201202 changed from 0xFF. 400 is an artificial limit of this tool. 
	outputs.push_back(val);
	htlOut.push_back(0);
	initOutputValue.push_back(-999.02);

	return 1;
}


int
HVUnit::AddOutput(size_t val)
{
	int ival = (int)val;

	return AddOutput(ival);
}


int
HVUnit::AddInput(int val, double initVal)
{
	if (val < 0) return 0;
	if (val > 400) return 0;	// MAG 201202 changed from 0xFF. 400 is an artificial limit of this tool. 
	inputs.push_back(val);
	inputBoundary.push_back(0);
	htlIn.push_back(0);
	initInputValue.push_back(initVal);

	return 1;
}


int
HVUnit::AddOutput(int val, double initVal)
{
	if (val < 0) return 0;
	if (val > 400) return 0;	// MAG 201202 changed from 0xFF. 400 is an artificial limit of this tool. 
	outputs.push_back(val);
	htlOut.push_back(0);
	initOutputValue.push_back(initVal);

	return 1;
}


int 
HVUnit::AddParameter(double val) 
{
	parameters.push_back(val);
	htlParam.push_back(0);
	
	return 1;
}


const double
HVUnit::GetInitInputValue(size_t index)
{
	if (index < 0)
		return -999.1;
	if (initInputValue.size() == 0)
		return -999.1;
	if (initInputValue.size() <= index)
		return -999.1;

	return initInputValue[index];
}


const double
HVUnit::GetInitOutputValue(size_t index)
{
	if (index < 0)
		return -999.2;
	if (initOutputValue.size() == 0) 
		return -999.2;
	if (initOutputValue.size() <= index) 
		return -999.2;

	return initOutputValue[index];
}


int
HVUnit::SetInitInputValue(size_t index, double val)
{
	if (index < 0) 
		return -1;
	if (initInputValue.size() == 0) 
		return -1;
	if (initInputValue.size() <= index) 
		return -1;

	initInputValue[index] = val;

	return 0;
}


int 
HVUnit::SetInitOutputValue(size_t index, double val)
{
	if (index < 0) 
		return -1;
	if (initOutputValue.size() == 0) 
		return -1;
	if (initOutputValue.size() <= index) 
		return -1;

	initOutputValue[index] = val;

	return 0;
}


// Get state index into category, get category from TYPE
const size_t
HVUnit::GetInput(size_t index) 
{
	if (index < 0) return 0;
	if (index >= inputs.size()) return 0;
	
	return inputs[index];
}


const int 
HVUnit::GetInputBoundary(size_t index)
{
	if (index < 0) return 0;
	if (index >= inputBoundary.size()) return 0;

	return inputBoundary[index];
}


const size_t
HVUnit::GetOutput(size_t index) 
{
	if (index < 0) return 0;
	if (index >= outputs.size()) return 0;

	return outputs[index];
}


const double
HVUnit::GetParameter(size_t index)
{
	if (index < 0) return 0;
	if (index >= parameters.size()) return 0;

	return parameters[index];
}


const int
HVUnit::GetType()
{

	if (myType == nullptr) return 0;

	return myType->GetType();
}


void 
HVUnit::Reset()
{
	unit = 0;
	block = 0;
	superBlock = 0;
	myLabel.clear();
	myNote.clear();
	myType = nullptr;
	inputs.clear();
	outputs.clear();
	inputBoundary.clear();
	parameters.clear();
	htlIn.clear();
	htlOut.clear();
	htlParam.clear();
	initInputValue.clear();
	initOutputValue.clear();
}


const HTREEITEM
HVUnit::GetInputTreeItem(size_t index)
{
	if (index < 0) return 0;
	if (index >= inputs.size()) return 0;

	return htlIn[index];
}


const HTREEITEM
HVUnit::GetOutputTreeItem(size_t index)
{
	if (index < 0) return 0;
	if (index >= outputs.size()) return 0;

	return htlOut[index];
}

const HTREEITEM
HVUnit::GetParamTreeItem(size_t index)
{
	if (index < 0) return 0;
	if (index >= parameters.size()) return 0;

	return htlParam[index];
}


/* MAG- Still potentially needed? 
// returns HTREEITEM link to entire state item, not to specific input
HTREEITEM 
HVUnit::GetStateLinkFromInput(int index)
{
	if (index < 0) return 0;
	if (index >= inputs.size()) return 0;
	if (index >= inputState.size()) return 0;

	//statemyType->GetInputCategory(index)

	return inputState[index].treepos;
}

// returns HTREEITEM link to entire state item, not to specific output
HTREEITEM 
HVUnit::GetStateLinkFromOutput(int index)
{
	if (index < 0) return 0;
	if (index >= outputs.size()) return 0;
	if (index >= outputState.size()) return 0;

	return outputState[index].treepos;
}
*/

void
HVUnit::SetInputTreeItem(size_t index, HTREEITEM hti)
{
	if (index < 0) return;
	if (index >= inputs.size()) return;
	if (hti < 0) return;

	htlIn[index] = hti;
}


void
HVUnit::SetOutputTreeItem(size_t index, HTREEITEM hti)
{
	if (index < 0) return;
	if (index >= outputs.size()) return;
	if (hti < 0) return;

	htlOut[index] = hti;
}


void
HVUnit::SetParamTreeItem(size_t index, HTREEITEM hti)
{
	if (index < 0) return;
	if (index >= parameters.size()) return;
	if (hti < 0) return;

	htlParam[index] = hti;
}


void 
HVUnit::IncrInput(int i)
{
	if (i < 0) return;
	if (i > inputs.size() - 1) return;
	inputs[i]++;
}


void 
HVUnit::IncrOutput(int i)
{
	if (i < 0) return;
	if (i > outputs.size() - 1) return;
	outputs[i]++;
}


void
HVUnit::SetInputIndex(size_t inp, size_t index)
{
	if (inp < 0) return;
	if (inp >= inputs.size()) return;
	if (index < 0) return;
	if (index > 400) return; // MAG TODO- check on max size, maybe compare to highest current index in use
	inputs[inp] = index;
}


void
HVUnit::SetInputBoundary(size_t inp, int boundVal)
{
	if (inp < 0) return;
	if (inp >= inputs.size()) return;
	if (boundVal < 0) boundVal = 0;
	if (boundVal > 1) boundVal = 1; // MAG TODO- check on max size, maybe compare to highest current index in use
	inputBoundary[inp] = boundVal;
}


void
HVUnit::SetOutputIndex(size_t inp, size_t index)
{
	if (inp < 0) return;
	if (inp >= outputs.size()) return;
	if (index < 0) return;
	if (index > 400) return; // MAG TODO- check on max size, maybe compare to highest current index in use
	outputs[inp] = index;
}


void
HVUnit::SetParameter(size_t i, double value)
{
	if (i < 0) return;
	if (i >= parameters.size()) return;
	parameters[i] = value;
}


StateElement::StateElement() 
{
	treepos = 0;
	category = 0;
	catIndex = 0;
	initValue = -999;
	initValueValid = 0;
	isBoundary = 0;
	treepos = nullptr;
	treeposInput = nullptr;
	treeposOutput = nullptr;
}


// returns absolute state array index
const size_t
StateElement::GetStateIndex()
{
	size_t id = 0;
	size_t i = 0;

	for (i = 1; i < category;i++) {
		id += theApp.GetCategoryCount(i);
	}
	id += catIndex;

	return id;
}


int 
StateElement::GetInputInfo(size_t index, size_t *unit, size_t *unitIndex)
{
	int valid = 1;

	if (unit == nullptr) valid = 0;
	if (unitIndex == nullptr) valid = 0;
	if (index < 0) valid = 0;
	if (index >= inputs.size()) valid = 0;

	if (valid == 0) {
		// do not modify parameters as shown below, one or more may be NULL
		//*unit = 0;
		//*unitIndex = 0;
		return 0;
	}

	*unit = inputs[index].unit;
	*unitIndex = inputs[index].index;

	return 1;
}


int 
StateElement::GetOutputInfo(size_t index, size_t *unit, size_t *unitIndex)
{
	int valid = 1;

	if (unit == nullptr) valid = 0;
	if (unitIndex == nullptr) valid = 0;
	if (index < 0) valid = 0;
	if (index >= outputs.size()) valid = 0;
	
	if (valid == 0) {
		// do not modify parameters as shown below, one or more may be NULL
		//*unit = 0;
		//*unitIndex = 0;
		return 0;
	}

	*unit = outputs[index].unit;
	*unitIndex = outputs[index].index;

	return 1;
}


void 
StateElement::IncrementInputUnit(size_t index)
{
	if (index < 0) return;
	if (index >= inputs.size()) return;
	inputs[index].unit++;
}


HVUnit 
*StateElement::GetInputUnit(size_t index)
{
	if (index < 0) return NULL;
	if (index >= inputs.size()) return NULL;
	
	return theApp.GetUnitRef(inputs[index].unit - 1);
}


void
StateElement::IncrementOutputUnit(size_t index)
{
	if (index < 0) return;
	if (index >= outputs.size()) return;
	outputs[index].unit++;
}


HVUnit
*StateElement::GetOutputUnit(size_t index)
{
	if (index < 0) return NULL;
	if (index >= outputs.size()) return NULL;
	return theApp.GetUnitRef(outputs[index].unit - 1);
}


void 
StateElement::Reset()
{
	category = 0;
	catIndex = 0;
	inputs.clear();
	outputs.clear();
	initValue = -999.5;
	initValueValid = 0;
	timeValue.clear();
	timeStep.clear();
	timeSB.clear();
	isReported.clear();
	isBoundary = 0;
	treepos = 0;
	treeposInput = 0;
	treeposOutput = 0;
}


void 
StateElement::SetInputTreelink(size_t index, HTREEITEM hti)
{
	if (index < 0) return;
	if (index >= inputs.size()) return;
	if(hti < 0) return;
	inputs[index].treepos = hti;
}


void 
StateElement::SetOutputTreelink(size_t index, HTREEITEM hti)
{
	if (index < 0) return;
	if (index >= outputs.size()) return;
	if (hti < 0) return;
	outputs[index].treepos = hti;
}


HTREEITEM
StateElement::GetInputTreelink(size_t index)
{
	if (index < 0) return 0;
	if (index >= inputs.size()) return 0;

	return inputs[index].treepos;
}


// unit = unit number
// index = unit index, NOT output index
HTREEITEM 
StateElement::GetInputTreelink(size_t unit, size_t index)
{
	std::vector<StateAccess>::iterator sai;

	if (index < 0) return 0;
	if (unit < 0) return 0;
	for (sai = inputs.begin(); sai != inputs.end(); sai++) {
		if ((sai->unit == unit) && (sai->index == index))
			return sai->treepos;
	}

	return 0;
}


HTREEITEM
StateElement::GetOutputTreelink(size_t index)
{
	if (index < 0) return 0;
	if (index >= outputs.size()) return 0;

	return outputs[index].treepos;
}


// unit = unit number
// index = unit index, NOT output index
HTREEITEM 
StateElement::GetOutputTreelink(size_t unit, size_t index)
{
	std::vector<StateAccess>::iterator sai;

	if (index < 0) return 0;
	if (unit < 0) return 0;
	for (sai = outputs.begin(); sai != outputs.end(); sai++) {
		if ((sai->unit == unit) && (sai->index == index))
			return sai->treepos;
	}

	return 0;
}


void
StateElement::AddInput(size_t unit, size_t unitIndex)
{
	StateAccess nsa = {};

	if (unit < 0) return;
	if (unitIndex < 0) return;
	if (unitIndex >= STATESIZE) {
		theApp.csdlg->m_status.AddString(L"ERROR- State Element index exceeds maximum.");
		return;
	}
	nsa.index = unitIndex;
	nsa.unit = unit;
	nsa.treepos = 0;
	inputs.push_back(nsa);

	return;
}


void 
StateElement::AddOutput(size_t unit, size_t unitIndex)
{
	StateAccess nsa = {};

	if (unit < 0) return;
	if (unitIndex < 0) return;
	if (unitIndex >= STATESIZE) {
		theApp.csdlg->m_status.AddString(L"ERROR- State Element index exceeds maximum.");
		return;
	}
	nsa.index = unitIndex;
	nsa.unit = unit;
	nsa.treepos = 0;
	outputs.push_back(nsa);

	return;
}


void 
StateElement::DelInput(size_t index)
{
	if (index < 0) return;
	if (index >= inputs.size()) return;
	inputs.erase(inputs.begin() + index);
}


void 
StateElement::DelOutput(size_t index)
{
	if (index < 0) return;
	if (index >= outputs.size()) return;
	outputs.erase(outputs.begin() + index);
}


const double 
StateElement::GetTimeValue(int id) 
{
	if (id < 0) 
		return -999.61;
	if (id >= timeValue.size()) 
		return -999.61;

	return(timeValue[id]);
}


const double
StateElement::GetTimeStep(int id)
{
	if (id < 0) 
		return -999.62;
	if (id >= timeStep.size()) 
		return -999.62;

	return(timeStep[id]);
}


const int 
StateElement::IsReported(size_t index)
{
	if (isReported.size() == 0) return 0;
	return (std::find(isReported.begin(), isReported.end(), index) != isReported.end());
}

// sbValue is the superblock which we're adding reporting for
// index = -1 -> delete all entries
// index = 0 -> delete sbValue from array
// index = 1 -> add sbValue to array
void 
StateElement::SetReported(size_t sbValue, int index)
{	// value is the superblock which it's reported in
	if ((index < -1)) {
		return; 
	}

	if(index == -1){// deleting array
		while (isReported.size() > 0) isReported.pop_back();
		theApp.SetSavedState(1);
		return;
	}

	if (index == 0){ // deleting item
		std::vector<size_t>::iterator it = std::find(isReported.begin(), isReported.end(), sbValue);
		if(it == isReported.end()) return;
		isReported.erase(it);
		return;
	}

	if(std::find(isReported.begin(), isReported.end(), sbValue) != isReported.end()) return;
	isReported.push_back(sbValue); 
	std::sort(isReported.begin(), isReported.end());
	theApp.SetSavedState(1);

	return; 
}

// MAG 190611
// Purpose: create a list of which units are outputting the inputs to other units
// Creates a spreadsheet with each tab holding data for a type. The inputs for the type are listed
// in column 0. Each subsequent column contains the info for an instance of that type. The info
// is the data on the unit providing the input data as its output, "u##:# text" where ##.# is
// the unit number then the output variable instance (i.e. output #1 or #2). Text describes the unit.
int 
CSimInfoApp::SaveUnitCrosswalkFile(FILE *ip)
{
	int			i=0;
	mxml		mymx;
	xml_element *xe1 = nullptr;
	xml_element *xwb = nullptr;	// the workbook
	xml_element *xws = nullptr;	// the worksheet
	xml_element *xtable = nullptr;// the table
	xml_element *xcol = nullptr;	// the column
	xml_element *xrow = nullptr;	// the current row
	xml_element *xcell = nullptr;	// the current cell
	xml_element *xdata = nullptr;
	xml_props *xp1 = nullptr;
	char ops[300] = {};
	size_t stateIndex = 0;		// index to state vector
	size_t catIndex = 0;		// index in category
	char catNames[][20] = { "Pressure","Mass Flow Rate","Temperature","Control","Other","Energy","Power","Humidity" };
	size_t unit=0;
	size_t unitIndex=0;
	//int ioType;
	size_t opCount=0;
	size_t opUnit=0;
	size_t opIndex=0;
	HVType *hvt = nullptr;
	std::string ss1;
	std::vector<TypeCW> typeList;
	TypeCW				mtl;
	UnitCW				ucw;
	//UnitCW	*pUnit;
	//int unitIndex = 0;
	int cType = 0;  // current type

	if (ip == nullptr) {
		theApp.csdlg->m_status.AddString(L"Error saving Type Crosswalk file- NULL file pointer.");
		return -1;
	}

	theApp.csdlg->m_status.AddString(L"Saving Type Crosswalk file....");
	xe1 = mymx.addElement(mymx.getRoot(), "?xml");
	xp1 = mymx.addprop(xe1, "version", "1.0");
	xp1 = mymx.addprop(xe1, "encoding", "UTF-8");

	xe1 = mymx.addElement(mymx.getRoot(), "?mso-application");
	xp1 = mymx.addprop(xe1, "progid", "Excel.Sheet");

	xwb = mymx.addElement(mymx.getRoot(), "Workbook");
	xp1 = mymx.addprop(xwb, "xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:x", "urn:schemas-microsoft-com:office:excel");
	xp1 = mymx.addprop(xwb, "xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:html", "https://www.w3.org/TR/html401/");

	// fill units structure
	for (std::vector<HVUnit>::iterator ti = units.begin(); ti != units.end(); ti++) {
		mtl.id = -1;
		mtl.ilabels.clear();
		mtl.unitList.clear();
		ucw.ilabels.clear();
		cType = ti->GetType();
		unit = -1; // use as bool for test below
		unitIndex = 0;
		while (typeList.size() > unitIndex) {
			if (typeList[unitIndex].id == cType) {
				unit = unitIndex;
				break;
			}
			unitIndex++;
		}
		hvt = ti->GetTypeP();
		if (unit == -1) {	// adding first instance of this type, fill in type struct
			mtl.id = cType;
			mtl.desc = hvt->GetTypeLabel();
			for (i = 0; i < hvt->GetCountIn(); i++) {
				ss1 = hvt->GetInputLabel(i);
				sprintf_s(ops, "i%02d %s", i+1, ss1.c_str());  // NOTE- i+1 for printing labels
				mtl.ilabels.push_back(ops);
			}
			typeList.push_back(mtl);
			unitIndex = typeList.size() - 1;
		}
		// fill ucw, add unit to list w. unit label
		sprintf_s(ops, "U%zd %s", ti->GetUnit(), hvt->GetTypeLabel().c_str());
		ucw.label = ops;
		for (i = 0; i < ti->GetInputCount(); i++){
			catIndex = hvt->GetInputCategory(i);
			stateIndex = ti->GetInput(i);
			opCount = state[catIndex][stateIndex].GetOutputCount();
			if (opCount == 0) {
				ucw.ilabels.push_back("Boundary");
			} else{ // find output unit and unit index
				state[catIndex][stateIndex].GetOutputInfo(0, &opUnit, &opIndex);
				sprintf_s(ops, "U%02zd:%02zd T%03d", opUnit, opIndex+1, units[opUnit - 1].GetType());
				ucw.ilabels.push_back(ops);			
			}
		}
		typeList[unitIndex].unitList.push_back(ucw);
	}

	// loop through for each variable type and create tables
	for (std::vector<TypeCW>::iterator tc = typeList.begin(); tc != typeList.end(); tc++) {
		xws = mymx.createElement(xwb, "Worksheet");
		sprintf_s(ops, "T%03d", tc->id);
		xp1 = mymx.addprop(xws, "ss:Name", ops);
		xtable = mymx.createElement(xws, "Table");
		xcol = mymx.createElement(xtable, "Column");
		xp1 = mymx.addprop(xcol, "ss:Index", "1");
		xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
		xp1 = mymx.addprop(xcol, "ss:Width", "110");
		xrow = mymx.createElement(xtable, "Row");

		// add type ## and description
		xcell = mymx.createElement(xrow, "Cell");
		sprintf_s(ops, "T%03d- %s", tc->id,tc->desc.c_str());
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		// add cells for header row
		for (i = 0; i < tc->unitList.size(); i++)
		{
			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "%s", tc->unitList[i].label.c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

		}
		// print row label and then data
		for (i = 0; i < tc->ilabels.size(); i++) {
			xrow = mymx.createElement(xtable, "Row");

			// label
			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "%s", tc->ilabels[i].c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			for (size_t j = 0; j < tc->unitList.size(); j++){
				xcell = mymx.createElement(xrow, "Cell");
				sprintf_s(ops, "%s", tc->unitList[j].ilabels[i].c_str());
				xdata = mymx.createElement(xcell, "Data", ops);
				xp1 = mymx.addprop(xdata, "ss:Type", "String");
			}
		}
	} // end for iterator loop

	mymx.print_token(mymx.getRoot(), ip);

	theApp.csdlg->m_status.AddString(L"The Type Crosswalk file has been saved.");

	i = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel(i - 1);

	return 1;
}


// MAG 190903
// Purpose: create a time series data tab for each unit, showing data from inputs and outputs
// Creates a spreadsheet with each tab holding data for a type. The inputs and outputs for the type are the
// column headers. Each column contains time series data as read in.
int
CSimInfoApp::SaveDataCrosswalkFile(FILE *ip)
{
	int			i = 0;
	int			tv = 0;
	int			lc=0;
	mxml		mymx;
	xml_element *xe1 = nullptr;
	xml_element *xwb = nullptr;	// the workbook
	xml_element *xws = nullptr;	// the worksheet
	xml_element *xtable = nullptr;// the table
	xml_element *xcol = nullptr;	// the column
	xml_element *xrow = nullptr;	// the current row
	xml_element *xcell = nullptr;	// the current cell
	xml_element *xdata = nullptr;
	xml_props *xp1 = nullptr;
	char ops[200] = {};
	CString cs1;
	size_t stateIndex = 0;		// index to state vector
	size_t catIndex = 0;		// index in category
	//char catNames[][20] = { "Pressure","Mass Flow Rate","Temperature","Control","Other","Energy","Power","Humidity" };
	size_t unit = 0;
	size_t tvc = 0;	// holds time value count
	HVType *hvt = nullptr;
	std::string ss1;
	std::vector<TypeCW> typeList;
	TypeCW				mtl;
	UnitCW				ucw;
	StateElement		*myse = nullptr;
	int cType = 0;  // current type

	if (ip == nullptr) {
		theApp.csdlg->m_status.AddString(L"Error saving Data Crosswalk file- NULL file pointer.");
		return -1;
	}

	theApp.csdlg->m_status.AddString(L"Saving Data Crosswalk file....");

	xe1 = mymx.addElement(mymx.getRoot(), "?xml");
	xp1 = mymx.addprop(xe1, "version", "1.0");
	xp1 = mymx.addprop(xe1, "encoding", "UTF-8");

	xe1 = mymx.addElement(mymx.getRoot(), "?mso-application");
	xp1 = mymx.addprop(xe1, "progid", "Excel.Sheet");

	xwb = mymx.addElement(mymx.getRoot(), "Workbook");
	xp1 = mymx.addprop(xwb, "xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:x", "urn:schemas-microsoft-com:office:excel");
	xp1 = mymx.addprop(xwb, "xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:html", "https://www.w3.org/TR/html401/");

	for (i = 0; i < reportedVars.size(); i++) {
		if (reportedVars[i].size() > 0) {
			if (reportedVars[i].size() == 0) break;	// MAG 220411 but why is this so?
			myse = StateVec(reportedVars[i][0]);	// MAG NOTE this assumes there's reported variables in SB 0, or if not there's also not in other SB
			break;
		}
	}
	if (myse != NULL) {
		tvc = myse->GetTimeValueCount();
	} else {
		tvc = state[1][1].GetTimeValueCount();	// saved from CSV file
	}

	// loop through for each unit and create tables
	for (std::vector<HVUnit>::iterator ti = units.begin(); ti != units.end(); ti++) {
		cs1.Format(L"Saving unit %zu...", ti->GetUnit());
		theApp.csdlg->m_status.AddString(cs1);
		DoBGStuff();

		xws = mymx.createElement(xwb, "Worksheet");
		sprintf_s(ops, "U%03zd",ti->GetUnit());	// Unit tab label
		xp1 = mymx.addprop(xws, "ss:Name", ops);
		xtable = mymx.createElement(xws, "Table");
		xcol = mymx.createElement(xtable, "Column");
		xp1 = mymx.addprop(xcol, "ss:Index", "1");
		xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
		xp1 = mymx.addprop(xcol, "ss:Width", "110");
		xrow = mymx.createElement(xtable, "Row");

		// add type ## and description
		xcell = mymx.createElement(xrow, "Cell");
		sprintf_s(ops, "U%03zd T%03d %s", ti->GetUnit(), ti->GetType(),ti->GetTypeP()->GetTypeLabel().c_str());
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		// add IO label row
		hvt = ti->GetTypeP();
		for (i = 0; i < hvt->GetCountIn(); i++){
			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "I%d %s",i+1, hvt->GetInputLabel(i).c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}
		for (i = 0; i < hvt->GetCountOut(); i++) {
			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "O%d %s",i+1, hvt->GetOutputLabel(i).c_str());
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}

		// add data, row by row
		for ( tv = 0; tv < tvc; tv++){  // NOTE- time value count is the same for all state members
			xrow = mymx.createElement(xtable, "Row");
			
			// cell for first column
			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "%d", tv);
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "Number");

			for (i = 0; i < hvt->GetCountIn(); i++) {
				xcell = mymx.createElement(xrow, "Cell");
				catIndex = hvt->GetInputCategory(i);
				stateIndex = ti->GetInput(i);
				sprintf_s(ops, "%f", state[catIndex][stateIndex].GetTimeValue(tv));
				xdata = mymx.createElement(xcell, "Data", ops);
				// check for value "-inf"
				//if(strstr(ops,"inf") != NULL)
				if (ops[0] > 57) // MAG 211026 update to check for anything that's not a number (ascii 48-57) or negative sign (ascii 45)
					xp1 = mymx.addprop(xdata, "ss:Type", "String");
				else
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");
			}
			for (i = 0; i < hvt->GetCountOut(); i++) {
				xcell = mymx.createElement(xrow, "Cell");
				catIndex = hvt->GetOutputCategory(i);
				stateIndex = ti->GetOutput(i);
				sprintf_s(ops, "%f", state[catIndex][stateIndex].GetTimeValue(tv));
				xdata = mymx.createElement(xcell, "Data", ops);
				// check for value "-inf"
				//if (strstr(ops, "inf") != NULL)
				if (ops[0] > 57) // MAG 211026 update to check for anything that's not a number (ascii 48-57) or negative sign (ascii 45)
					xp1 = mymx.addprop(xdata, "ss:Type", "String");
				else
					xp1 = mymx.addprop(xdata, "ss:Type", "Number");
			}
		}
		lc++;
		//if(lc > 20) break;  // test for debugging
		i = theApp.csdlg->m_status.GetCount();
		theApp.csdlg->m_status.DeleteString(i - 1);
	} // end for iterator loop

	mymx.print_token(mymx.getRoot(), ip);

	i = theApp.csdlg->m_status.GetCount();
	theApp.csdlg->m_status.DeleteString(i - 1);
	
	theApp.csdlg->m_status.AddString(L"The Data Crosswalk file has been saved.");

	i = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel(i - 1);

	return 1;
}


// MAG 200224 add function
int
CSimInfoApp::InsertUnit(HTREEITEM hti)	// MAG 191218 add function
{
	size_t i = 0;
	size_t j = 0;

	for (i = 0; i < units.size(); i++) {
		// check if clicked on a unit
		if (units[i].GetTreepos() == hti) {
			return InsertUnit(units[i].GetType(), i+2, units[i].GetSuperBlock(), units[i].GetBlock());  // NOTE- insert at location + 2
		}
		// check if clicked on an i/o/p of the unit
		for (j = 0; j < units[i].GetInputCount(); j++) {
			if (units[i].GetInputTreeItem(j) == hti) {
				return InsertUnit(units[i].GetType(), i+2, units[i].GetSuperBlock(), units[i].GetBlock());
			}
		}
		for (j = 0; j < units[i].GetOutputCount(); j++) {
			if (units[i].GetOutputTreeItem(j) == hti) {
				return InsertUnit(units[i].GetType(), i+2, units[i].GetSuperBlock(), units[i].GetBlock());
			}
		}
		for (j = 0; j < units[i].GetParameterCount(); j++) {
			if (units[i].GetParamTreeItem(j) == hti) {
				return InsertUnit(units[i].GetType(), i+2, units[i].GetSuperBlock(), units[i].GetBlock());
			}
		}
	}

	return 0;  // missed it?
}


// MAG 201112 add function
// Return Value: 0 == Error, > 0 SuperBlock ##
size_t
CSimInfoApp::GetUnitSBlock(size_t index)
{
	if (index < 0) return 0;
	if (index >= units.size()) return 0;
	return units[index].GetSuperBlock();
}


// MAG 200921 add function
// Purpose: to return the number of nFile parameters for specific types. 
// This information is not in a types file, and is tediously extracted here from source code.
int
CSimInfoApp::GetTypeFileParameterCount(int type) {
	switch (type)
	{
		case 401: return  1;
		case 402: return  1;
		case 403: return  1;
		case 404: return  1;
		case 411: return  1;
		case 412: return  1;
		case 413: return  1;
		case 414: return  1;
		case 441: return  8;
		case 471: return  8;
		case 480: return  6;
		case 481: return  8;
		case 482: return  6;
		case 483: return  1;
		case 484: return  2;
		case 485: return  9;
		case 486: return  7;
		case 488: return  2;
		case 489: return  7;
		case 490: return 10;
		case 492: return 11;
		case 493: return  7;
		case 496: return 10;
		case 497: return  9;
		default: break;	// other types have no file parameter count
	}

	return 0;
}


// MAG 191218 add function
// MAG 200413 NOTE- if position < 0, set to add unit to end of units array
int 
CSimInfoApp::InsertUnit(int itype, size_t position, size_t mySBlock , size_t myBlock )	// MAG 191218 add function
{
	HVUnit	myhv;
	HVUnit	*hvp = nullptr;
	HVType *myht = nullptr;
	bool	fit = 0;
	int		i = 0;
	int oc = 0;
	std::vector<HVUnit>::iterator itu;
	std::vector<HVUnit>::iterator itu2;
	std::vector<HVUnit> tUnits;
	CString cs1;
	CString cs2;
	char cc1[20] = {};
	size_t pBlock = 0;
	size_t pSBlock = 0;
	size_t piCat = 0;
	size_t piIndex = 0;

	/* NOTE inputs to set:
	int		unit;  // same as index in array, but might be accessed by pointer
	int		superBlock;
	int		block;
	HVType	*myType;
	std::string	myLabel;

	// these are indexes into the i/o/p type arrays
	// NOTE input and output indexes are by category, not absolute state indexes
	std::vector<int>	inputs;		
	std::vector<int>	outputs;
	std::vector<double>	parameters;

	// id's for i/o/p in Units Display window
	std::vector<HTREEITEM>	htlIn;
	std::vector<HTREEITEM>	htlOut;
	std::vector<HTREEITEM>	htlParam;
	*/

	if (position < 0) position = units.size()+1;
	if (position > units.size()) position = units.size()+1;

	if (itype < 0) {
		csdlg->m_status.AddString(L"ERROR- type not selected.");
		csdlg->m_status.SetCurSel(csdlg->m_status.GetCount() - 1);
		return 0;
	}
	// set itype to the actual type#
	if (mySBlock < 1) mySBlock = 1;
	if (myBlock < 1) myBlock = 1;

	if (mySBlock > 20) mySBlock = 20;
	if (myBlock > 40) myBlock = 40;

	// get pointer to type (myhv)
	for (std::vector<HVType>::iterator itt = types.begin(); itt != types.end(); itt++){
		if (itt->GetType() == itype) {
			myhv.SetType(itt._Ptr);
			// set inputs
			for (i = 0; i < itt->GetCountIn(); i++){
				myhv.AddInput(1);
			}
			for (i = 0; i < itt->GetCountOut(); i++) {
				oc = itt->GetOutputCategory(i);
				myhv.AddOutput(maxCat[oc] + 1);
				maxCat[oc]++;
			}
			for (i = 0; i < itt->GetCountParameter(); i++) {
				myhv.AddParameter(0);
			}
			i = itt->GetUseCount();
			sprintf_s(cc1, "%03d%c", itype, 'a' + i);
			if (i < 26) {
				sprintf_s(cc1, "%03d%c", itype, 'a' + i);
			}
			else {
				sprintf_s(cc1, "%03d%c%d", itype, 'a' + i%26, (int)i / 26);
			}

			myhv.SetLabel(cc1);
			itt->AddUseCount();
			fit = 1;
			break;
		}
	}

	if (fit == 0) {
		csdlg->m_status.AddString(L"ERROR: Could not find indicated type.");
		return 0;
	}

	// insert to proper location, adjust unit numbers
	if (units.size() == 0) { // first unit!
		myhv.SetBlock(myBlock);
		myhv.SetSuperBlock(mySBlock);
		myhv.SetUnit(1);
		units.push_back(myhv);
		hvp = &units[0];
	} else {
		if (position > units.size()) {	// adding to end of list, very sensible...
			hvp = &units[units.size() - 1];
			pBlock = hvp->GetBlock();
			pSBlock = hvp->GetSuperBlock();
			myhv.SetBlock(pBlock > myBlock?pBlock:myBlock);
			myhv.SetSuperBlock(pSBlock > mySBlock ? pSBlock : mySBlock);
			myhv.SetUnit(hvp->GetUnit() + 1);
			units.push_back(myhv);
			hvp = &units.back();
		} else {
			if (position == 0) position = 1;
			// point HVP to unit in spot myhv will take over
			hvp = &units[position-1];

			pBlock = hvp->GetBlock();
			pSBlock = hvp->GetSuperBlock();
			myhv.SetBlock(pBlock > myBlock ? pBlock : myBlock);
			myhv.SetSuperBlock(pSBlock > mySBlock ? pSBlock : mySBlock);
			myhv.SetUnit(hvp->GetUnit() );

			// CHANGE hvp to point to unit BEFORE where myhv will be inserted.
			if (position > 1) hvp = &units[position - 2];

			// check if previous unit is same type- initiate sequential insertion if it is (i.e. put all input and output after prev ones, bump up higher count indexes)
			if (hvp->GetType() == itype) {
				// insert new states
				for (i = 0;i < hvp->GetOutputCount();i++) {
					piCat = hvp->GetTypeP()->GetOutputCategory(i);
					piIndex = hvp->GetOutput(i);
					if (InsertState(piCat, piIndex) == 0) {	// only insert state for new outputs
						csdlg->m_status.AddString(L"ERROR- could not insert state for new unit. The model may be in an unstable state.");
					}
				}
				// add inputs/outputs after all states have been inserted
				for (i = 0;i < hvp->GetInputCount();i++) {
					//piCat = hvp->GetTypeP()->GetInputCategory(i);
					piIndex = hvp->GetInput(i);
					// MAG don't insert for new inputs, they will be matched up to an output
					myhv.SetInputIndex(i, piIndex);  // write inputs to same index as previous, user should adjust manually
				}
				for (i = 0;i < hvp->GetOutputCount();i++) {
					piIndex = hvp->GetOutput(i);
					myhv.SetOutputIndex(i, piIndex + 1);
				}

				// copy parameter values to new unit
				for (i = 0; i < hvp->GetParameterCount(); i++)
				{
					myhv.SetParameter(i, hvp->GetParameter(i));
				}
			}

			units.insert(units.begin() + (position-1), myhv);
			for (itu = units.begin() + position  ; itu != units.end(); itu++) {
				itu->IncrUnit();
			}
			hvp = &units[position];
		}
	}

	// update State array
	BuildState();
	//AddToState(hvp);

	unsavedChanges = 1;

	if(csdlg->Getdtdlg()->GetInit()) csdlg->Getdtdlg()->m_tree.SetItemState(myhv.GetTypeP()->GetTreepos(), TVIS_BOLD, TVIS_BOLD);	// MAG 200918 add bold to used types
	myht = hvp->GetTypeP();
	cs2 = myht->GetTypeLabel().c_str();
	cs1.Format(L"NOTE- Add unit TYPE %d \"%s\" unit array position %zu",itype,cs2, position);
	csdlg->m_status.AddString(cs1);
	csdlg->m_status.SetCurSel(csdlg->m_status.GetCount() - 1);
	if((hvp != NULL)&&(hvp->GetTreepos() > 0)&&(csdlg->GetUnitdlg()->GetInit() == 1)) csdlg->GetUnitdlg()->m_tree.EnsureVisible(hvp->GetTreepos()); // param count indicates if valid unit

	return 1;
}	// end CSimInfoApp::InsertUnit


// MAG 200224 add function
// Purpose: inserts an empty space in the state array, bumps up references to state array above the new one
// Note: will search for unused index above new one, and only increment references between the new space and the unused index (filling the unused index)
// Note: myIndex is category index, not absolute index
// Note: MAG 200407 if myIndex < 0, adds new index to end of indicated category
// MAG 200421 uncomment BuildState at end
int
CSimInfoApp::InsertState(size_t myCategory, size_t myIndex)
{
	int myCeil = -1; // do not increment above this index (-1 for not used)
	int i2 = 0;
	std::vector<HVUnit>::iterator itu2;

	if (myCategory < 1) return 0;
	if (myCategory > 8) return 0;
	if (myIndex < 0) myIndex = maxCat[myCategory]; // value of 0 when clicked on category summary (1st line)
	if (myIndex > maxCat[myCategory]) return 0;

	// NOTE- myIndex should never be higher than maxCat[#] as higher indexes are not displayed
	if (myIndex == maxCat[myCategory]) {	// bumping one on to the end of the category
		maxCat[myCategory]++;
		state[myCategory][myIndex+1].SetCategory(myCategory);
		state[myCategory][myIndex+1].SetCategoryIndex(myIndex+1);
		return 1;
	}

	myCeil = (int) maxCat[myCategory];
	/* MAG 200417 TODO? temp comment out- doesn't work well when adding to end of category
	for (size_t iInd = myIndex+1; iInd <= maxCat[myCategory]; iInd++) {
		if ((state[myCategory][iInd].GetInputCount() == 0) && (state[myCategory][iInd].GetOutputCount() == 0)) {
			myCeil = iInd;
			break;
		}
	}
	*/

	// loop through all units, reassign any variables in category starting higher than index (but below ceiling value) by bumping up one.
	for (itu2 = units.begin(); itu2 != units.end(); itu2++) {
		for (i2 = 0;i2 < itu2->GetInputCount();i2++) {
			if ((itu2->GetTypeP()->GetInputCategory(i2) == myCategory) && (itu2->GetInput(i2) > myIndex) && (itu2->GetInput(i2) <= myCeil)) {	// if it's the same category, and higher than prevUnit->this_input_index...
				itu2->IncrInput(i2);	// increment value once
			}
		}
		for (i2 = 0;i2 < itu2->GetOutputCount();i2++) {
			if ((itu2->GetTypeP()->GetOutputCategory(i2) == myCategory) && (itu2->GetOutput(i2) > myIndex) && (itu2->GetOutput(i2) <= myCeil)) {	// if it's the same category, and higher than prevUnit->this_input_index...
				itu2->IncrOutput(i2);	// increment value once
			}
		}
	}

	//scan through reported variables, increment if needed
	//NOTE reportedVars[Superblock][count of vars in superblock]
	if (reportedVars.size() > 0) {
		for (size_t i = 0; i < reportedVars.size(); i++) {
			for (size_t j = 0; j < reportedVars[i].size(); j++) {
				if (reportedVars[i][j] > myIndex) reportedVars[i][j]++;
			}
		}
	}
	
	BuildState();
	if(csdlg->GetUnitdlg()->GetInit() == 0)  csdlg->GetUnitdlg()->m_tree.EnsureVisible(state[myCategory][myIndex].GetTreepos());

	return 1;
} // end InsertState


int
CSimInfoApp::InsertState(HTREEITEM hti)
{
	size_t ioc = 0;

	for (size_t i = 1; i < state.size(); i++) {  // scan through categories, should be in array 1-8
		for (size_t j = 0; j < state[i].size(); j++) {
			if(state[i][j].GetTreepos() == hti) return InsertState(i, j); // found it!
			if (state[i][j].GetTreeposInput() == hti) return InsertState(i, j); // found it!
			if (state[i][j].GetTreeposOutput() == hti) return InsertState(i, j); // found it!
			ioc = state[i][j].GetInputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetInputTreelink(k) == hti) {	// found it!
					return InsertState(i, j);
				}
			}
			ioc = state[i][j].GetOutputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetOutputTreelink(k) == hti) { // found it!
					return InsertState(i, j);
				}
			}
		}
	}

	return 0;  // did not find hti
}


// MAG 200421 add function
//Purpose: Increment the superblock starting at the unit index provided
int
CSimInfoApp::BumpSuperBlocks(int unitIndex)
{
	CString cs1;
	
	if (unitIndex < 0) return 0;
	if (unitIndex >= units.size()) return 0;
	for (size_t i = unitIndex; i < units.size(); i++){
		units[i].SetSuperBlock(units[i].GetSuperBlock() + 1);
	}
	BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);

	cs1.Format(L"Increment Superblocks starting at Unit %zu", units[unitIndex].GetUnit());
	theApp.csdlg->m_status.AddString(cs1);

	return 1;
}


// MAG 200421 add function
//Purpose: Increment the block starting at the unit index provided- ONLY in the current superblock
int
CSimInfoApp::BumpBlocks(int unitIndex) 
{
	CString cs1;
	size_t curSB = 0;

	if (unitIndex < 0) return 0;
	if (unitIndex >= units.size()) return 0;

	curSB = units[unitIndex].GetSuperBlock();
	for (size_t i = unitIndex; (i < units.size())&&(units[i].GetSuperBlock() == curSB); i++) {
		units[i].SetBlock(units[i].GetBlock() + 1);
	}
	BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);

	cs1.Format(L"Increment Blocks starting at Unit %zu", units[unitIndex].GetUnit());
	theApp.csdlg->m_status.AddString(cs1);

	return 1;
}


int
CSimInfoApp::BumpSuperBlocks(HTREEITEM unitIndex) 
{
	int i = FindUnitTreePos(unitIndex);

	if (i > 0) {
		unsavedChanges = 1;
		return BumpSuperBlocks(i - 1);
	}

	return 0;
}


int
CSimInfoApp::BumpBlocks(HTREEITEM unitIndex)
{
	int i = FindUnitTreePos(unitIndex);

	if (i > 0) {
		unsavedChanges = 1;
		return BumpBlocks(i - 1);
	}

	return 0;
}

// MAG 200421 add function
//Purpose: Decrement the superblock starting at the unit index provided
int
CSimInfoApp::DecrSuperBlocks(int unitIndex)
{
	CString cs1;

	if (unitIndex < 0) return 0;
	if (unitIndex >= units.size()) return 0;
	if (units[unitIndex].GetSuperBlock() == 1) return 0;

	for (size_t i = unitIndex; i < units.size(); i++) {
		units[i].SetSuperBlock(units[i].GetSuperBlock() - 1);
	}
	
	BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);

	cs1.Format(L"Decrement Superblocks starting at Unit %zu", units[unitIndex].GetUnit());
	theApp.csdlg->m_status.AddString(cs1);

	return 1;
}


// MAG 200421 add function
//Purpose: Decrement the block starting at the unit index provided- ONLY in the current superblock
int
CSimInfoApp::DecrBlocks(int unitIndex)
{
	CString cs1;
	size_t curSB = 0;

	if (unitIndex < 0) return 0;
	if (unitIndex >= units.size()) return 0;
	if (units[unitIndex].GetBlock() == 1) return 0;

	curSB = units[unitIndex].GetSuperBlock();
	for (size_t i = unitIndex; (i < units.size()) && (units[i].GetSuperBlock() == curSB); i++) {
		units[i].SetBlock(units[i].GetBlock() - 1);
	}
	BuildUnitTree(&theApp.csdlg->GetUnitdlg()->m_tree);

	cs1.Format(L"Increment Blocks starting at Unit %zu", units[unitIndex].GetUnit());
	theApp.csdlg->m_status.AddString(cs1);

	return 1;
}


int
CSimInfoApp::DecrSuperBlocks(HTREEITEM unitIndex) 
{
	int i = FindUnitTreePos(unitIndex);

	if (i > 0) {
		unsavedChanges = 1;
		return DecrSuperBlocks(i - 1);
	}

	return 0;
}


int
CSimInfoApp::DecrBlocks(HTREEITEM unitIndex) 
{
	int i = FindUnitTreePos(unitIndex);

	if (i > 0) {
		unsavedChanges = 1;
		return DecrBlocks(i - 1);
	}

	return 0;
}


// MAG 200407 add function
// Purpose: Updates state array when adding new unit
int
CSimInfoApp::AddToState(HVUnit *newUnit)
{
	HVType *myht = nullptr;
	bool	fit = 0;
	std::vector<HVUnit> tUnits;
	CString cs1;
	size_t i = 0;
	size_t si = 0;
	int ioCat = 0;
	size_t j = 0;

	if (newUnit == nullptr) return 0;
	if (newUnit->GetTypeP() == nullptr) return 0;	// would indicat that unit is not configured properly

	si = newUnit->GetInputCount();
	for (i = 0; i < si; i++) {
		//get cat index for input
		j = newUnit->GetInput(i);
		if (j >= STATESIZE) {
			cs1.Format(L"ERROR adding input %zu to unit %zu (Type %zu). State array size exceeded. Input item has not been assigned.", 
				i, newUnit->GetUnit(), newUnit->GetType());
			theApp.csdlg->m_status.AddString(cs1);
			break;
		}

		// get type for input
		myht = newUnit->GetTypeP();
		ioCat = myht->GetInputCategory(i); // get category of input
		state[ioCat][j].AddInput(newUnit->GetUnit(), i);
		state[ioCat][j].SetCategory(ioCat);
		state[ioCat][j].SetCategoryIndex(j);
		if (state[ioCat][j].GetInitValueValid() == 0) {
			state[ioCat][j].SetInitValue(newUnit->GetInitInputValue(i));
		} else {
			cs1.Format(L"NOTE: Value is already written.");
		}
		if (maxCat[ioCat] < j) maxCat[ioCat] = j;
		if (newUnit->GetInputBoundary(i) == 1) {
			state[ioCat][j].SetBoundary(1);
		}
	}

	// parse outputs
	si = newUnit->GetOutputCount();
	for (i = 0; i < si; i++) {
		//get cat index for output
		j = newUnit->GetOutput(i);
		if (j >= STATESIZE) {
			cs1.Format(L"ERROR adding output %zu to unit %zu(Type %zu). State array size exceeded. Output item has not been assigned.", i, newUnit->GetUnit(), newUnit->GetType());
			theApp.csdlg->m_status.AddString(cs1);
			break;
		}

		// get type for output
		myht = newUnit->GetTypeP();
		ioCat = myht->GetOutputCategory(i); // get category of output
											// need state[type][typeIndex]
		state[ioCat][j].AddOutput(newUnit->GetUnit(), i);
		state[ioCat][j].SetCategory(ioCat);
		if (state[ioCat][j].GetInitValueValid() == 0) {
			state[ioCat][j].SetInitValue(newUnit->GetInitOutputValue(i));
		}
		state[ioCat][j].SetCategoryIndex(j);
		if (maxCat[ioCat] < j) maxCat[ioCat] = j;
	}

	// MAG 200225 scan and set category for unused interstitial states
	for (int cati = 1; cati <= 8; cati++) {
		for (int indi = 1; indi <= maxCat[cati]; indi++) {	// MAG 200420 added = to comparison, check when run
			if (state[cati][indi].GetCategory() == 0) state[cati][indi].SetCategory(cati);
			state[cati][indi].SetCategoryIndex(indi);
		}
	}

	return 1;
}


// MAG 191219 add function
// Purpose: rebuild the state array after Units are added or after IO assignments have been changed
void
CSimInfoApp::BuildState()
{
	bool	fit = 0;
	int		j = 0;
	CString cs1;
	std::vector<HVUnit>::iterator itu;
	std::vector<HVUnit> tUnits;

	for (int cati = 1; cati <= 8; cati++) {
		for (std::vector<StateElement>::iterator ti = state[cati].begin(); ti != state[cati].end(); ti++) {
			ti->Reset();
		}
		state[cati][0].SetCategory(cati);
		maxCat[cati] = 0;
	}

	//state[1][0].Reset();// MAG TODO was this line for testing?

	for (itu = units.begin(); itu != units.end(); itu++){
		j = AddToState(itu._Ptr);
		if (j == 0) {
			cs1.Format(L"ERROR adding unit %d (Type %d). Unit was not added.",itu->GetUnit(),itu->GetType());
			theApp.csdlg->m_status.AddString(cs1);
			break;
		}
	}

	// MAG 200225 scan and set category for unused interstitial states
	for (int cati = 1; cati <= 8; cati++) {
		for (int indi = 0; indi <= maxCat[cati]; indi++) {
			if (state[cati][indi].GetCategory() == 0) state[cati][indi].SetCategory(cati);
			state[cati][indi].SetCategoryIndex(indi);

			//// MAG 220111 also check for unset states
			//if (state[cati][indi].GetInitValueValid() == 0){
			//	//..get the value from output if available or input
			//	if (state[cati][indi].GetOutputCount() > 0) {
			//
			//	}
			//}
		}
	}

}	// end CSimInfoApp::BuildState


int
CSimInfoApp::SaveUALFile(FILE* op)	// MAG add 220422
{
	std::size_t i = 0;
	std::size_t j = 0;

	if (op == nullptr) {
		theApp.csdlg->m_status.AddString(L"Error saving UAL file- NULL file pointer.");
		return -1;
	}

	theApp.csdlg->m_status.AddString(L"Saving UAL file....");
	for (i = 0; i < units.size(); i++){
		for ( j = 0; j < units[i].GetInputCount(); j++){
			fprintf(op, "U%zd,I%zd,C%d,V%zd\n", units[i].GetUnit(), j+1, units[i].GetTypeP()->GetInputCategory(j), units[i].GetInput(j));
		}
		for (j = 0; j < units[i].GetOutputCount(); j++) {
			fprintf(op, "U%zd,O%zd,C%d,V%zd\n", units[i].GetUnit(), j+1, units[i].GetTypeP()->GetOutputCategory(j), units[i].GetOutput(j));
		}
		// note add option to UAL input to specify dest state ID, i.e. T250:1,I1,S## (or C## to use category index)
	}

	return 1;
}


int 
CSimInfoApp::SaveUnitListFile(FILE *op)	// MAG add 200109
{
	mxml mymx;
	xml_element *xe1 = nullptr;	//
	xml_element *xwb = nullptr;	// the workbook
	xml_element *xws = nullptr;	// the worksheet
	xml_element *xtable = nullptr;// the table
	xml_element *xcol = nullptr;	// the column
	xml_element *xrow = nullptr;	// the current row
	xml_element *xcell = nullptr;	// the current cell
	xml_element *xdata = nullptr;
	xml_props *xp1 = nullptr;
	char ops[400] = {};
	char catNames[][30] = { "Pressure","Mass Flow Rate","Temperature","Control","Other","Energy","Power","Humidity" };
	int stateIndex = 0;		// index to state vector
	int catIndex = 0;		// index in category
	int unit = 0;
	size_t maxparams = 0;
	size_t i = 0;
	std::string ss1;

	if (op == nullptr) {
		theApp.csdlg->m_status.AddString(L"Error saving Unit List file- NULL file pointer.");
		return -1;
	}

	if (unsavedChanges) {  // MAG added 220503 to ensure state is in correct shape (i.e. no blanks at end of category)
		BuildState();
		BuildStateTree(NULL);
	}

	theApp.csdlg->m_status.AddString(L"Saving Units List file....");
	xe1 = mymx.addElement(mymx.getRoot(), "?xml");
	xp1 = mymx.addprop(xe1, "version", "1.0");
	xp1 = mymx.addprop(xe1, "encoding", "UTF-8");

	xe1 = mymx.addElement(mymx.getRoot(), "?mso-application");
	xp1 = mymx.addprop(xe1, "progid", "Excel.Sheet");

	xwb = mymx.addElement(mymx.getRoot(), "Workbook");
	xp1 = mymx.addprop(xwb, "xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:x", "urn:schemas-microsoft-com:office:excel");
	xp1 = mymx.addprop(xwb, "xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
	xp1 = mymx.addprop(xwb, "xmlns:html", "https://www.w3.org/TR/html401/");

	// save values in State array
	xws = mymx.createElement(xwb, "Worksheet");
	xp1 = mymx.addprop(xws, "ss:Name", "State Array");

	xtable = mymx.createElement(xws, "Table");
	xcol = mymx.createElement(xtable, "Column");
	xp1 = mymx.addprop(xcol, "ss:Index", "1");
	xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
	xp1 = mymx.addprop(xcol, "ss:Width", "110");

	xrow = mymx.createElement(xtable, "Row");  // title row
	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Index");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Values");
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	xcell = mymx.createElement(xrow, "Cell");
	xdata = mymx.createElement(xcell, "Data", "Status");	// saves Boundary or Reported status
	xp1 = mymx.addprop(xdata, "ss:Type", "String");

	for (int cati = 1; cati <= 8; cati++) {
		for (std::vector<StateElement>::iterator ti = state[cati].begin(); ti != state[cati].end(); ti++) {
			if (ti->GetCategoryIndex() == 0) continue;
			xrow = mymx.createElement(xtable, "Row");  // new row
			sprintf_s(ops, "State %02d:%02zd", cati, ti->GetCategoryIndex());
			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", ti->GetInitValue());
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "st%s%s", ti->IsBoundary() ? "B" : "", ti->IsReported() ? "R" : "");
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}
	}

	// fill units structure
	for (std::vector<HVUnit>::iterator ti = units.begin(); ti != units.end(); ti++) {

		// *********************
		// Add Units List sheet

		xws = mymx.createElement(xwb, "Worksheet");
		sprintf_s(ops, "Unit %zd", ti->GetUnit());
		xp1 = mymx.addprop(xws, "ss:Name", ops);

		xtable = mymx.createElement(xws, "Table");
		xcol = mymx.createElement(xtable, "Column");
		xp1 = mymx.addprop(xcol, "ss:Index", "1");
		xp1 = mymx.addprop(xcol, "ss:AutoFitWidth", "0");
		xp1 = mymx.addprop(xcol, "ss:Width", "110");

		xrow = mymx.createElement(xtable, "Row");  // title row
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "Type");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", ti->GetType());
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xrow = mymx.createElement(xtable, "Row");  // label
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "Label");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		sprintf_s(ops, "%s", ti->GetLabel().c_str());
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xrow = mymx.createElement(xtable, "Row");  // Notes field
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "Note");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		sprintf_s(ops, "%s", ti->GetNote().c_str());
		if (strlen(ops) == 0) {
			sprintf_s(ops, "%s", ti->GetTypeP()->GetTypeLabel().c_str());
		}
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", ops);
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xrow = mymx.createElement(xtable, "Row");  // Superblock
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "Superblock");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", ti->GetSuperBlock());
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xrow = mymx.createElement(xtable, "Row");  // block
		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", "Block");
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		xcell = mymx.createElement(xrow, "Cell");
		xdata = mymx.createElement(xcell, "Data", ti->GetBlock());
		xp1 = mymx.addprop(xdata, "ss:Type", "String");

		// write input type indexes
		for (i = 0; i < ti->GetInputCount(); i++)
		{
			xrow = mymx.createElement(xtable, "Row");  // new row

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "Input %zd", i+1);
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", ti->GetInput(i));
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}

		// write output type indexes
		for (i = 0; i < ti->GetOutputCount(); i++)
		{
			xrow = mymx.createElement(xtable, "Row");  // new row

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "Output %zd", i+1);
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", ti->GetOutput(i));
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}

		// write parameter values
		for (i = 0; i < ti->GetParameterCount(); i++)
		{
			xrow = mymx.createElement(xtable, "Row");  // new row

			xcell = mymx.createElement(xrow, "Cell");
			sprintf_s(ops, "Parameter %zd", i + 1);
			xdata = mymx.createElement(xcell, "Data", ops);
			xp1 = mymx.addprop(xdata, "ss:Type", "String");

			xcell = mymx.createElement(xrow, "Cell");
			xdata = mymx.createElement(xcell, "Data", ti->GetParameter(i));
			xp1 = mymx.addprop(xdata, "ss:Type", "String");
		}

	}
	mymx.print_token(mymx.getRoot(), op);

	// unit used as generic int here
	unit = theApp.csdlg->m_status.GetCount();
	theApp.csdlg->m_status.DeleteString(unit - 1);
	theApp.csdlg->m_status.AddString(L"The Unit List file has been saved.");

	unit = csdlg->m_status.GetCount();
	csdlg->m_status.SetCurSel(unit - 1);

	xe1 = mymx.getRoot();
	while (xe1->subelement != NULL) {
		mymx.deleteElement(xe1->subelement);
	}

	unsavedChanges = 0;

	return 1;
} // end SaveUnitListFile


// MAG 240721 TODO update to also read ReportChain input files
int 
CSimInfoApp::ReadUnitListFile(FILE *op)	// MAG add 200109
{
	mxml mymx;
	xml_element *xe1 = nullptr;	//
	xml_element *xwb = nullptr;	// the workbook
	xml_element *xrow = nullptr;	// the current row
	xml_element *xcell = nullptr;	// the current cell
	xml_element *xdata = nullptr;
	xml_props	*xp1 = nullptr;
	int		unitType=0;
	size_t	i=0;
	size_t	j=0;
	HVUnit	myhv;
	char	ioType[20] = {};
	int		ioCat = 0;
	size_t	typeCtr = 1;
	int		iCount = 0;	// count of inputs aleady processed, used for offsets
	size_t		oCount = 0;
	int			ioVal = 0;
	size_t		ioValt = 0;
	std::vector<double> stateVals;
	std::vector<std::string> statusVals;
	std::vector<int>	BlockOverride;		// MAG 200824 add Superblock Override feature
	std::vector<int>	SuperBlockOverride;
	int					orFail = 0;			// override fail
	TCHAR		tjunk[MAX_PATH];
	FILE		*opf = nullptr;
	size_t		maxCatLocal[9] = { 0 };	// holds maximum index used for each category NOTE use <= in for loops
	CString		cs1;
	std::vector<int> svi;	// used for adding to reportedVars
	
	if (op == nullptr) {
		csdlg->m_status.AddString(L"ERROR: File name not found.");
		return 0;
	}

	if(mymx.read_xml_file(op) == 0){
		csdlg->m_status.AddString(L"ERROR: Error reading input XML file.");
		return 0;
	}

	//TODO check for and delete prev unit list
	/*
	if (units.size() > 0) {
		csdlg->m_status.AddString(L"NOTE- Deleting previous simulation data.");
		units.clear();
		for (i = 1; i < 9; i++) {
			for (j = 0; j < STATESIZE; j++) {
				if (state[i][j].GetCategory() > 0) {
					state[i][j].Reset();
				}
			}
		}
	}*/
	for (i = 0; i < 9; i++){
		maxCatLocal[i] = maxCat[i];
	}

	if (types.size() == 0) {
		csdlg->m_status.AddString(L"NOTE- TYPE data not entered- using default TYPAR.DAT");
		if (_wgetcwd(tjunk, MAX_PATH) == nullptr) {
			csdlg->m_status.AddString(L"ERROR- Could not open Unit List File.");
			return 0;
		}

		_wfopen_s(&opf, L"typar.dat", L"r");

		if (opf == nullptr) {
			csdlg->m_status.AddString(L"ERROR- Could not open type file TYPAR.DAT.  Unit list file was not read.");
			return 0;
		}

		if (theApp.ReadTypeFile(opf) == 0) {
			csdlg->m_status.AddString(L"ERROR- Problem found entering TYPE data. Unit list file was not entered.");
			return 0;
		}
		theApp.SetTypesFileName("typar.dat");
		csdlg->EnableTypeButton();
	}

	xwb = mymx.element_by_name(mymx.getRoot(), "Worksheet");
	if (xwb == nullptr) {
		xwb = mymx.element_by_name(mymx.getRoot(), "ReportChain");
		if (xwb == nullptr) xwb = mymx.element_by_name(mymx.getRoot(), "reportchain");  // we are very forgiving here
		if (xwb == nullptr) {
			csdlg->m_status.AddString(L"ERROR: Format error in configuration file. Known format (Worksheet/ReportChain) not found.");
			return 0;
		}
		// reading ReportChain 

		// finished with ReportChain code
		return 1;
	}

	// read state values (one worksheet)
	// NOTE values will be read here but applied after all units are read
	xp1 = mymx.getprop(xwb, "ss:Name");	// get link to prop with name data
	if (xp1 == nullptr) {
		csdlg->m_status.AddString(L"ERROR: Format error in configuration file. Worksheet name not found.");
		return 0;
	}

	if (xp1->data.compare(0, 5, "State") == 0) {  // check we have the state tab
		xrow = mymx.element_by_name(xwb, "Row");
		if (xrow != NULL) xrow = xrow->next; // push past title row
		while (xrow != NULL) {
			xcell = mymx.element_by_name(xrow, "Cell");
			xcell = xcell->next;
			xdata = mymx.element_by_name(xcell, "Data");
			stateVals.push_back(std::stod(xdata->data));

			// get the status cell
			xcell = xcell->next;
			if (xcell != NULL) {
				xdata = mymx.element_by_name(xcell, "Data");
				statusVals.push_back(xdata->data);
			} else { 
				statusVals.push_back(std::string(""));
			}
			xrow = xrow->next;
		}
		xwb = xwb->next;
	} else {
		csdlg->m_status.AddString(L"ERROR: Format error in configuration file. State Array worksheet not found. The file was not imported.");
		return 0;
	}

	xp1 = mymx.getprop(xwb, "ss:Name");	// get link to prop with name data
	if (xp1 == nullptr) {
		csdlg->m_status.AddString(L"ERROR: Format error in configuration file. Worksheet name not found.");
		return 0;
	}

	if (xp1->data.compare(0, 5, "Super") == 0) {  // check we have the Superblock Override tab
		xrow = mymx.element_by_name(xwb, "Row");
		if (xrow != NULL) xrow = xrow->next; // push past title row
		while (xrow != NULL) {
			xcell = mymx.element_by_name(xrow, "Cell");// skip past unit number cell
			xcell = xcell->next;

			// get the SuperBlock cell
			xdata = mymx.element_by_name(xcell, "Data");
			if (xdata == nullptr) {
				orFail = 1;
				break;
			}
			ioVal = std::stoi(xdata->data);
			if ((ioVal < 1) || (ioVal > 40)) {
				csdlg->m_status.AddString(L"ERROR: Superblock override setting out of range.");
				orFail = 1;
				break;
			} else {
				SuperBlockOverride.push_back(ioVal);
			}
			while(ioVal > reportedInterval.size()){	// MAG 201202 add this
				reportedInterval.push_back(10);
				reportedVars.push_back(svi);
			}

			// get the Block cell
			xcell = xcell->next;
			if (xcell == nullptr) {
				orFail = 1;
				break;
			}
			xdata = mymx.element_by_name(xcell, "Data");
			if (xdata == nullptr) {
				orFail = 1;
				break;
			}
			ioVal = std::stoi(xdata->data);
			if ((ioVal < 1) || (ioVal > 20)) {
				csdlg->m_status.AddString(L"ERROR: Block override setting out of range.");
				orFail = 1;
				break;
			} else {
				BlockOverride.push_back(ioVal);
				xrow = xrow->next;
			}
		}

		if (orFail == 1) {
			BlockOverride.clear();
			SuperBlockOverride.clear();
			csdlg->m_status.AddString(L"ERROR: Format error in Superblock Override. Original Superblock::block values will be used.");
		}
		xwb = xwb->next;
	}
	
	typeCtr = units.size()+1;  // used as unit index in Unit array
	if (typeCtr == 0) typeCtr++;

	// read unit worksheets
	while (xwb != NULL) {
		// check we have a unit ## worksheet
		xp1 = mymx.getprop(xwb, "ss:Name");	// get link to prop with name data
		if (xp1 == nullptr) {
			csdlg->m_status.AddString(L"ERROR: Format error in configuration file. Unit worksheet name not found.");
			return 0;
		}
		if (xp1->data.compare(0, 4, "Unit") != 0) {  // check we have the state tab
			csdlg->m_status.AddString(L"NOTE: Found unexpected worksheet while reading units.");
			xwb = xwb->next;
			continue;
		}

		myhv.SetUnit(typeCtr);		// set unit
		typeCtr++;

		// read unit type
		xrow = mymx.element_by_name(xwb, "Row");
		if (xrow == nullptr) {  // NOTE- if there's not a format error here, there probably won't be one below either- not checking
			csdlg->m_status.AddString(L"ERROR: Format error in configuration file. Row not found.");
			return 0;
		}

		xcell = mymx.element_by_name(xrow, "Cell");
		xcell = xcell->next;
		xdata = mymx.element_by_name(xcell, "Data");
		unitType = std::stoi(xdata->data);

		// MAG debug test
		//if (unitType == 53) {
		//	i = 0;// for debug breakpoint
		//}

		i = 0; // used as bool
		for (std::vector<HVType>::iterator ii = types.begin(); ii != types.end(); ii++) {
			if (ii->GetType() == unitType) {
				myhv.SetType(ii._Ptr);
				i = 1;
				ii->AddUseCount();
				break;
			}
		}
		if (i == 0) {
			csdlg->m_status.AddString(L"ERROR: Unknown type found in config file. Please enter correct TYPES file and retry.");
			return 0;
		}

		// read label
		xrow = xrow->next;
		xcell = mymx.element_by_name(xrow, "Cell");
		xcell = xcell->next;
		xdata = mymx.element_by_name(xcell, "Data");
		myhv.SetLabel(xdata->data);

		// check for notes field
		if (xrow->next->subelement->subelement->data.compare("Note") == 0) {
			xrow = xrow->next;
			xcell = mymx.element_by_name(xrow, "Cell");
			xcell = xcell->next;
			xdata = mymx.element_by_name(xcell, "Data");
			myhv.SetNote(xdata->data);
		}

		// read superblock
		xrow = xrow->next;
		if (SuperBlockOverride.size() > 0) {
			if (myhv.SetSuperBlock(SuperBlockOverride[0]) < 0) {
				csdlg->m_status.AddString(L"ERROR: Superblock override setting out of range.");
				return 0;
			}
			SuperBlockOverride.erase(SuperBlockOverride.begin());
		} else {
			xcell = mymx.element_by_name(xrow, "Cell");
			xcell = xcell->next;
			xdata = mymx.element_by_name(xcell, "Data");
			myhv.SetSuperBlock(std::stoi(xdata->data));
		}

		// read block
		xrow = xrow->next;
		if (BlockOverride.size() > 0) {
			myhv.SetBlock(BlockOverride[0]);
			BlockOverride.erase(BlockOverride.begin());
		} else {
			xcell = mymx.element_by_name(xrow, "Cell");
			xcell = xcell->next;
			xdata = mymx.element_by_name(xcell, "Data");
			myhv.SetBlock(std::stoi(xdata->data));
		}

		// read inputs, outputs, params
		iCount = 0;
		oCount = 0;
		xrow = xrow->next;
		std::stringstream sstream;

		while (xrow != NULL) {
			xcell = mymx.element_by_name(xrow, "Cell");
			xdata = mymx.element_by_name(xcell, "Data");
			sprintf_s(ioType, "%s", xdata->data.c_str());

			xcell = xcell->next;
			xdata = mymx.element_by_name(xcell, "Data");

			switch (ioType[0])
			{
			case 'i':
			case 'I':
				ioCat = myhv.GetTypeP()->GetInputCategory(iCount);
				sstream = std::stringstream(xdata->data);
				sstream >> ioValt;
				if (ioValt > 0) ioValt += maxCat[ioCat];
				if (ioValt >= STATESIZE) { // uh oh
					cs1.Format(L"ERROR: Unit type %d (#%d) Input %zu exceeds array size (%zd)", myhv.GetType(), myhv.GetTypeP()->GetUseCount(), myhv.GetInputCount(), ioValt);
					theApp.csdlg->m_status.AddString(cs1);
					theApp.csdlg->m_status.AddString(L"NOTE: Input is set to 0 for category. Review before proceeding.");
					ioValt = 0;
				}
				myhv.AddInput(ioValt);
				iCount++;
				break;
			case 'o':
			case 'O':
				ioCat = myhv.GetTypeP()->GetOutputCategory(oCount);
				sstream = std::stringstream(xdata->data);
				sstream >> ioValt;
				if (ioValt == 0) {
					ioValt = 0;  // for breakpoint
				}
				if (ioValt > 0) ioValt += maxCat[ioCat];
				if (ioValt >= STATESIZE) { // uh oh
					cs1.Format(L"ERROR: Unit type %d (#%d) Output %d exceeds array size (%zu)", myhv.GetType(), myhv.GetTypeP()->GetUseCount(), myhv.GetOutputCount(), ioValt);
					theApp.csdlg->m_status.AddString(cs1);
					theApp.csdlg->m_status.AddString(L"NOTE: Output is set to 0 for category. Review before proceeding.");
					ioVal = 0;
				}
				myhv.AddOutput(ioValt);
				oCount++;
				break;
			case 'p':
			case 'P':
				myhv.AddParameter(std::stod(xdata->data));
				break;

			default:
				break;
			}
			xrow = xrow->next;
		}

		// insert at proper location in units array
		iCount = 0;// used as bool for is unit inserted
		for (std::vector<HVUnit>::iterator it = units.begin(); it < units.end(); it++){
			if (myhv.GetSuperBlock() > it->GetSuperBlock()) continue;
			if (myhv.GetSuperBlock() < it->GetSuperBlock()) {
				myhv.SetUnit(it->GetUnit());
				units.insert(it, myhv);
				iCount = 1;
				break;
			}
			if ((myhv.GetSuperBlock() == it->GetSuperBlock()) && (myhv.GetBlock() < it->GetBlock())) {
				units.insert(it, myhv);
				iCount = 1;
				break;
			}
		}

		if(iCount == 0) units.push_back(myhv); // didn't get inserted yet

		myhv.Reset();

		xwb = xwb->next;
	}  // end while reading workbooks loop
	
	// renumber units to be in order
	iCount = 1;// used as unit number
	for (std::vector<HVUnit>::iterator it = units.begin(); it < units.end(); it++){
		it->SetUnit(iCount);
		iCount++;
	}

	BuildState();// MAG 200826 NOTE boundary, reported not being set correctly

	// check if we need to init reportedVars[#SuperBlock][#reported vars in SB] sparse array
	if (reportedVars.size() == 0) {
		j = units.back().GetSuperBlock();

		for (i = 0; i < j; i++){
			reportedVars.push_back(svi);
		}
	}

	if ((reportedVars.size() > 0) && (reportedInterval.size() == 0)) {
		for (i = 0; i < reportedVars.size(); i++){
			reportedInterval.push_back(10);
		}
	}
	
	// set values into state array, using difference in maxCat and maxCatoriginal to determine new state elements, by category
	iCount = 0;  // use as index into stateVals
	oCount = 0;	// use as sum of prev indexes
	// MAG 220518 TOTO: this loop assumes only one element added per category? Fails for larger gaps. Needs to be redone to account for larger gaps.
	for (i = 1; i < 9; i++){	// cycle through categories
		if (i > 1) oCount += maxCat[i - 1];
		for ( j = maxCatLocal[i]; j < maxCat[i]; j++){
			StateVecSetValue(i, j+1, stateVals[iCount]);
			if (statusVals[iCount].find('B') != std::string::npos) StateVecSetBoundary(i, j + 1, 1);
			if (statusVals[iCount].find('R') != std::string::npos) StateVecSetReported(i, j + 1, 1, 1);
			iCount++;
		}
	}

	// MAG 200323 add find boundary variables
	//for (size_t iCat = 1; iCat <= 8; iCat++) {
	//	for (size_t iInd = 1; iInd <= maxCat[iCat]; iInd++)
	//		if (state[iCat][iInd].GetOutputCount() == 0)		// no outputs
	//			if (state[iCat][iInd].GetInputCount() > 0)		// must be used, else it's just an empty slot not a boundary
	//				state[iCat][iInd].SetBoundary(1);
	//}

	xe1 = mymx.getRoot();
	while (xe1->subelement != NULL) {
		mymx.deleteElement(xe1->subelement);
	}

	return 1;
} //end ReadUnitListFile


// MAG 240721 add this function and functionality
// Report chain file contains col elements that indicate how to find value for that column
// unit or type indicators and IO index will point to an element of the state array, which is saved
// state index can also be indicated directly, though this is less flexible
/*
SAMPLE XML FORMAT:
<ReportChain>
	<col id="1"> <!-- id is for user convenience, not read by FPTM -->
		<unit>20</unit>
		<io iotype="0">1</io>  <!-- type can be 0, I, i for input, 1, O, o for output -->
	</col>
	<col id="2">
		<unitType typeid="320" instance="2" />
		<io iotype="O">4</io>
	</col>
	<col stateIndex="15" />  <!-- short and simple  -->
	<file>report chain 1.csv</file>
</ReportChain>
*/
int CSimInfoApp::ReadReportChain(mxml myxml)
{
	// TODO: Add your implementation code here.
	xml_element* xe1 = nullptr;	//
	xml_element* col = nullptr;	// 
	xml_element* subcol = nullptr;	// 
	xml_props* xp1 = nullptr;
	int foundUnitorType = 0; // 1=unit 2=unit type
	int foundUnit = -1;
	int foundType = 0;
	int foundTypeInstance = -1;
	int foundIOtype = -1;
	int foundIOinstance = -1;
	int foundIOcategory = 0;
	int colState = 0;  // store actual state index for current column
	int unitCount = 0;
	std::vector<int>	ReportChainStateCategories;
	std::vector<int>	ReportChainStateIndexes;	// NOTE these are indexes into categories, NOT absolute indexes
	std::string			ReportChainFileName;
	CString cs1;

	xe1 = myxml.element_by_name(myxml.getRoot(), "ReportChain");
	if(xe1 == nullptr) myxml.element_by_name(myxml.getRoot(), "reportchain");
	if (xe1 == nullptr) {
		theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: could not find ReportChain element.");
		return 0;
	}
	col = myxml.element_by_name(xe1, "col");
	while (col != NULL) {
		if (col->name.compare("file") == 0) break; // found the file element, done with columns

		// check for direct state listing
		xp1 = myxml.getprop(col, "stateIndex");
		if (xp1 == nullptr) xp1 = myxml.getprop(col, "stateindex");
		if (xp1 != NULL) {
			colState = atoi(xp1->data.data());
			foundIOcategory = (int) StateVec(colState)->GetCategory();
			foundIOinstance = (int) StateVec(colState)->GetCategoryIndex();
			ReportChainStateCategories.push_back(foundIOcategory);
			ReportChainStateIndexes.push_back(foundIOinstance);
			col = col->next;
			colState = 0;
			break;
		}

		// re-init for current loop
		foundUnitorType = 0;
		foundUnit = -1;	// zero may be a valid value
		foundType = 0;
		foundTypeInstance = -1;
		foundIOtype = -1;
		foundIOinstance = -1;
		foundIOcategory = 0;
		colState = 0;
		unitCount = 0;

		// check for unit
		subcol = myxml.element_by_name(col, "unit");
		if (subcol != NULL) {
			foundUnitorType = 1;
			foundUnit = atoi(subcol->data.data());
		}

		// if needed check for unitType
		if (foundUnitorType == 0) {
			subcol = myxml.element_by_name(col, "unitType");
			if(subcol == nullptr) subcol = myxml.element_by_name(col, "unittype");
			if (subcol == nullptr) {
				theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: col unit was found with no unit or type.");
				col = col->next;
				continue;
			}
			foundUnitorType = 2;

			// get typeid
			xp1 = myxml.getprop(subcol, "typeid");
			if(xp1 == nullptr) xp1 = myxml.getprop(subcol, "typeID");
			if (xp1 == nullptr) xp1 = myxml.getprop(subcol, "TypeID");
			if (xp1 == nullptr) {
				theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: col unit type was found with no type ID.");
				col = col->next;
				continue;
			}
			foundType = atoi(xp1->data.data());

			// get instance
			xp1 = myxml.getprop(subcol, "instance");
			if (xp1 == nullptr) xp1 = myxml.getprop(subcol, "Instance");
			if (xp1 == nullptr) {
				theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: col unit type was found with no IO instance.");
				col = col->next;
				continue;
			}
			foundTypeInstance = atoi(xp1->data.data());

			for (size_t i = 0; i < units.size(); i++) {
				if (units[i].GetType() == foundType) {
					unitCount++;
					if (unitCount == foundTypeInstance) {
						foundUnit = (int)i;
						break;
					}
				}
			}
			if (foundUnit == -1) {
				cs1.Format(L"ReadReportChain STOP ERROR: Could not find indicated unit. (Type%d03:%d)", foundType, foundTypeInstance);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			// unit is found	
		}

		// NOTE unit # is stored in foundUnit, which is indes to unit array

		subcol = myxml.element_by_name(col, "io");
		if (subcol == nullptr) {
			theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: col unit was found with no io element.");
			col = col->next;
			continue;
		}

		// read io type
		xp1 = myxml.getprop(subcol, "iotype");
		if (xp1 == nullptr) xp1 = myxml.getprop(subcol, "ioType");
		if (xp1 == nullptr) xp1 = myxml.getprop(subcol, "IOtype");
		if (xp1 == nullptr) xp1 = myxml.getprop(subcol, "IOType");
		if (xp1 == nullptr) {
			theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: col io was found with no type element.");
			col = col->next;
			continue;
		}
		switch (xp1->data.data()[0]) { // switch on first character
			case '0':
			case 'i':
			case 'I':
				foundIOtype = 0;
				break;
			case '1':
			case 'o':
			case 'O':
				foundIOtype = 1;
				break;
			default:
				theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: col io was found without valid io type.");
				col = col->next;
				continue;
				break;
		}
		foundIOinstance = atoi(subcol->data.data());

		// now find state column and add to list
		// note this finds index into type, not full state index
		switch (foundIOtype) {
			case 0:
				colState = (int) units[foundUnit].GetInput(foundIOinstance);
				foundIOcategory = units[foundUnit].GetTypeP()->GetInputCategory(foundIOinstance);
				break;
			case 1:
				colState = (int) units[foundUnit].GetOutput(foundIOinstance);
				foundIOcategory = units[foundUnit].GetTypeP()->GetOutputCategory(foundIOinstance);
				break;
			default: // this shouldn't be reached
				theApp.csdlg->m_status.AddString(L"ReadReportChain ERROR: col io was found without valid io type.");
				col = col->next;
				continue;
		}
		ReportChainStateIndexes.push_back(colState);
		ReportChainStateCategories.push_back(foundIOcategory);
	}

	return 0;
}  // end ReadReportChain


// MAG 200225 add drag and drop in the State window
// Returns: 0 for error, 1 for success
int
CSimInfoApp::DragState(HTREEITEM dragItem, HTREEITEM dragDest)	// MAG add 200225
{
	StateElement *dragSE = nullptr;
	StateElement *dropSE = nullptr;
	size_t dragio = -1;	// 0 for input, 1 for output
	size_t ioindex = -1;	// index in state for type, i.e. input #3 or output #1.
	size_t dragUnit = 0;		// unit # for which i/o state # is changing
	size_t dragUnitIndex = 0;	// index in unit, i.e. input #2 or output #5 IN UNIT
	size_t dropStateIndex = 0;
	CString cs1;
	size_t ioc = 0;
	
	if (dragItem == 0) return 0;
	if (dragDest == 0) return 0;

	// Find dragItem
	for (size_t i = 1; (i < state.size())&&(dragio == -1); i++) {  // scan through categories, should be in array 1-8
		for (size_t j = 0; (j < state[i].size()) && (dragio == -1); j++) {
			ioc = state[i][j].GetInputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetInputTreelink(k) == dragItem) { // found it!
					dragSE = &state[i][j];
					dragio = 0;
					ioindex = k;
					break;
				}
			}

			ioc = state[i][j].GetOutputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetOutputTreelink(k) == dragItem) { // found it!
					dragSE = &state[i][j];
					dragio = 1;
					ioindex = k;
					break;
				}
			}
		}
	}

	if (dragSE == nullptr) {
		csdlg->m_status.AddString(L"Drag and drop move failed- dragged item not eligible.");
		return 0;
	}

	// Find dest Item
	for (size_t i = 1; (i < state.size()) && (dropSE == nullptr); i++) {  // scan through categories, should be in array 1-8
		for (size_t j = 0; (j < state[i].size()) && (dropSE == nullptr); j++) {
			if ((state[i][j].GetTreepos() == dragDest)|| (state[i][j].GetTreeposInput() == dragDest)|| (state[i][j].GetTreeposOutput() == dragDest)) {// found it!
				dropSE = &state[i][j];
				dropStateIndex = j;
				break;
			}

			ioc = state[i][j].GetInputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetInputTreelink(k) == dragDest) { // found it!
					dropSE = &state[i][j];
					dropStateIndex = j;
					break;
				}
			}

			ioc = state[i][j].GetOutputCount();
			for (size_t k = 0; k < ioc; k++) {
				if (state[i][j].GetOutputTreelink(k) == dragDest) { // found it!
					dropSE = &state[i][j];
					dropStateIndex = j;
					break;
				}
			}
		}
	}

	if (dropSE == nullptr) {
		csdlg->m_status.AddString(L"Drag and drop move failed- drop location not eligible.");
		return 0;
	}

	// check they're the same category
	if (dropSE->GetCategory() != dragSE->GetCategory()) {
		csdlg->m_status.AddString(L"Drag and drop move failed- Move must be to the same category.");
		return 0;
	}

	if(dropSE == dragSE) {
		csdlg->m_status.AddString(L"Drag and drop move stopped- Move was to same State index.");
		return 0;
	}

	// everything seems ok, try to move the item in the state array and the units array
	switch (dragio){
		case 0: // input
			dragSE->GetInputInfo(ioindex, &dragUnit, &dragUnitIndex);
			units[dragUnit - 1].SetInputIndex(dragUnitIndex, dropStateIndex);
			dragSE->DelInput(ioindex);
			dropSE->AddInput(dragUnit, dragUnitIndex);
			cs1.Format(L"Move input from State index %zu to %zu.", dragSE->GetStateIndex(), dropSE->GetStateIndex());
			csdlg->m_status.AddString(cs1);
			break;
		case 1: // output
			dragSE->GetOutputInfo(ioindex, &dragUnit, &dragUnitIndex);
			units[dragUnit - 1].SetOutputIndex(dragUnitIndex, dropStateIndex);
			dragSE->DelOutput(ioindex);
			dropSE->AddOutput(dragUnit, dragUnitIndex);
			cs1.Format(L"Move output from State index %zu to %zu.", dragSE->GetStateIndex(), dropSE->GetStateIndex());
			csdlg->m_status.AddString(cs1);
			break;
		default:
			csdlg->m_status.AddString(L"Drag and drop move stopped- Error with determining I/O type.");
			return 0;
	}

	unsavedChanges = 1;

	return 1;
} // end DragState


// MAG 240522
// Function MinimizeState
// Purpose: Removes empty state elements by copying from the end of each category
void
CSimInfoApp::MinimizeState()
{
	StateElement *minzero = nullptr;	// SE with no items, dest for source
	StateElement* source = nullptr;	// The SE with items to be moved
	size_t	sourceIndex = 0;	// state index for source
	size_t sourceUnit = 0;
	size_t sourceUnitIndex = 0;
	//std::vector<size_t> startCat;//(maxCat, maxCat + sizeof(maxCat) / sizeof(maxCat[0]));
	size_t startCat[9] = {};
	CString cs1;

	memcpy_s(startCat, sizeof(size_t) * 9, maxCat, sizeof(size_t) * 9);
	for (size_t i = 1; (i < state.size()); i++) {  // scan through categories, should be in array 1-8
		for (size_t j = 1; (j < maxCat[i]); j++) {
			if ((state[i][j].GetInputCount() == 0) && (state[i][j].GetOutputCount() == 0)) {  // find an empty state
				// find next highest state element that is not empty
				sourceIndex = j + 1;
				while ((sourceIndex <= maxCat[i]) && (state[i][sourceIndex].GetInputCount() == 0) && (state[i][sourceIndex].GetOutputCount() == 0) ) {
					sourceIndex++;
				}

				//// start from end of category, move down if necessary. This method flips the order of moved elements
				//sourceIndex = maxCat[i];// state[i].size() - 1;
				//while ((state[i][sourceIndex].GetInputCount() == 0) && (state[i][sourceIndex].GetOutputCount() == 0) && (sourceIndex > j)) {
				//	sourceIndex--;
				//}
				
				if (sourceIndex <= maxCat[i]) { // now move sourceIndex to j
					while (state[i][sourceIndex].GetInputCount() > 0) { // scan through inputs
						state[i][sourceIndex].GetInputInfo(0, &sourceUnit, &sourceUnitIndex);
						state[i][sourceIndex].DelInput(0);
						state[i][j].AddInput(sourceUnit, sourceUnitIndex);
						units[sourceUnit-1].SetInputIndex(sourceUnitIndex, j);
					}
					while (state[i][sourceIndex].GetOutputCount() > 0) { // scan through outputs
						state[i][sourceIndex].GetOutputInfo(0, &sourceUnit, &sourceUnitIndex);
						state[i][sourceIndex].DelOutput(0);
						state[i][j].AddOutput(sourceUnit, sourceUnitIndex);
						units[sourceUnit-1].SetOutputIndex(sourceUnitIndex, j);
					}
				}
			}

		}
	}
	BuildState();

	unsavedChanges = 1;
	sourceIndex = 0; // reusing for accumulator
	for (size_t k = 1; k < 9; k++)
	{
		cs1.Format(L"Category %zu: remove %02d state elements", k, startCat[k] - maxCat[k]);
		csdlg->m_status.AddString(cs1);
		sourceIndex += maxCat[k];
	}
	cs1.Format(L"Total state count is now %zu", sourceIndex);
	csdlg->m_status.AddString(cs1);
	csdlg->m_status.SetCurSel(csdlg->m_status.GetCount() - 1);
}  // end MinimizeSize


CString
CSimInfoApp::MakeStateLabel(int stateIndex, int ioType, int ioIndex)	// make updated state label to be used for an input or output
{
	StateElement *myse = StateVec(stateIndex);
	CString cs1;
	//int i;

/*	MAG TODO? obvs not finished, might not be necessary
	cs1 = GetItemText(lastSelected);
	i = _ttoi(cs1.Mid(12));
	// get input 1 label
	myse->GetInputInfo(0, &unit, &unitIndex);
	theApp.units[unit].GetType();

	cs1.Format(L"State Array: %d; Category: [%d:%d]; Input to: %d; Output to: %d; Label: %S; Value: %5.2f (B)", i, myse->GetCategory(), myse->GetCategoryIndex(), myse->GetInputCount(), myse->GetOutputCount(), ss1.c_str(), myse->GetInitValue());
	*/
	cs1 = "NA";

	return cs1;
}


// MAG 220421
// Reads State Move List file
// File CSV format: 
// Source index#, I/O#, Dest index#
// t426:2,I1,t350:1,O2 -> moves the first input (I1) from the second Type426 in the unit list to the same state as the second output of the first Type 350.
// u5,I3,u30,O1 -> moves the third input from the 5th item in the unit list to the same state as the first output of the 30th item in the unit list.
// t426:3,I2,u35,O2 -> moves the second input (I2) from the third Type426 in the unit list to the same state as the second output 35th item in the unit list.
// U6,I2,C3,I9 -> moves second input from 6th unit to the state at Category 3, Index 9
// note can mix and match source and dest declaration types except for Category, Index destination
int
CSimInfoApp::ReadUALFile(FILE* ip)
{
	char ips[500];// extra long to accomodate long notes lines
	char* cp1 = nullptr;
	char* cp2 = nullptr;
	char* cp3 = nullptr;
	int sequenceID = -1;

	int ioType = -1;
	int ioID = -1;
	int unitID = -1;
	int typeID = -1;
	int typeCount = -1;

	int sourceUnitID = -1;
	int destUnitID = -1;
	int sourceIOT = -1;	// source IO type, 0=input, 1=output, 2=parameter
	int destIOT = -1;	// dest IO type
	int sourceIOD = -1;	// source IO ID
	int destIOD = -1;	// dest IO ID
	int sourceCategory = -1;	// source category
	int destCategory = -1;	// dest category
	int destCatIndex = -1;
	int maxID = -1;				// max unit IO count, or category size

	CString cs1;
	size_t lineCount = 0;

	if (ip == nullptr) return 0;

	while (fgets(ips, 500, ip) != NULL) {
		sourceUnitID = -1;
		sourceIOT = -1;	// source IO type
		sourceIOD = -1;	// source IO ID
		sourceCategory = -1;	// source category
		
		lineCount++;
		if ((ips[0] == '!') || (ips[0] == '%') || (ips[0] == '#')) {
			cs1 = ips;
			csdlg->m_status.AddString(cs1);
			continue;
		}
		
		if (ips[0] == '\n') continue;
		if(strlen(ips) < 7) {  // NOTE 7 is shortest valid command, i.e. in format "U1,I1,X"
			cs1 = ips;
			csdlg->m_status.AddString(cs1);
			continue;
		}

		cp1 = ips;
		while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++; // read off any empty spaces

		// read source unit
		switch (*cp1) {
			case 'u':	// find by unit number, i.e. U1,I1
			case 'U':
				cp1++;
				sourceUnitID = atoi(cp1)-1; // offset to zero-index
				if ((sourceUnitID < 0) || (sourceUnitID >= units.size())) {
					cs1.Format(L"STOP ERROR: Line %zu Format error. Source unit out of range (%d).", lineCount,sourceUnitID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				break;
			case 't':
			case 'T':
				// find unit by type:instance
				cp1++;
				typeID = atoi(cp1);
				cp3 = cp1;
				cp1 = strchr(cp1, ':');
				cp2 = strchr(cp3, ','); // use to search for fault where :# is missing
				if (cp1 == nullptr) {
					cs1.Format(L"STOP ERROR: Line %zu Could not find type count.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				if (cp2 == nullptr) {
					cs1.Format(L"STOP ERROR: Line %zu Format error.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				if (cp1 > cp2) {
					cs1.Format(L"STOP ERROR: Line %zu Format error, Type missing instance specifier.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				cp1++;
				if (*cp1 >= 'A') { // use format like "T###:A" for first instance of T###
					if (*cp1 >= 'a') {
						typeCount = 1 + *cp1 - 'a';
					}
					else if (*cp1 >= 'A') {
						typeCount = 1 + *cp1 - 'A';
					}
					else {
						cs1.Format(L"STOP ERROR: Line %zu Format error. Unit type out of range (%c).", lineCount, *cp1);
						csdlg->m_status.AddString(cs1);
						return 0;
					}
				}
				else {
					typeCount = atoi(cp1);
				}
				if (typeCount <= 0) {
					cs1.Format(L"STOP ERROR: Line %zu Format error. Unit type out of range (%d).", lineCount, typeCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				ioID = 0; // using for convenience
				unitID = -1;
				for (size_t i = 0; i < units.size(); i++) {
					if (units[i].GetType() == typeID) {
						ioID++;
						if (ioID == typeCount) {
							unitID = (int) i;
							break;
						}
					}
				}
				if (unitID == -1) {
					cs1.Format(L"STOP ERROR: Line %zu Could not find indicated unit. (%S)", lineCount, ips);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				sourceUnitID = unitID; // already at zero index
				// unit is found			
				break;
			default:
				cs1.Format(L"STOP ERROR: Line %zu Format error.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;		
		} // end source switch

		cp1 = strchr(ips, ',');
		if(cp1 == nullptr) {
			cs1.Format(L"STOP ERROR: Line %zu Format error.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}
		cp1++; // move past comma
		while (*cp1 == ' ') cp1++; // read off any empty spaces
		switch (*cp1) { // format [IO][IO ID]
			case 'i':
			case 'I':
				cp1++;
				ioID = atoi(cp1);
				if((ioID < 1)||(ioID > units[sourceUnitID].GetInputCount())) {
					cs1.Format(L"STOP ERROR: Line %zu Source input index out of range. (%s)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				sourceIOD = ioID - 1;
				sourceIOT = 0;
				break;
			case 'o':
			case 'O':
				cp1++;
				ioID = atoi(cp1);
				if ((ioID < 1) || (ioID > units[sourceUnitID].GetOutputCount())) {
					cs1.Format(L"STOP ERROR: Line %zu Source output index out of range. (%s)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				sourceIOD = ioID - 1;
				sourceIOT = 1;
				break;
			case 'p': // MAG 240530 add parameter option
			case 'P':
				cp1++;
				ioID = atoi(cp1);
				if ((ioID < 1) || (ioID > units[sourceUnitID].GetParameterCount())) {
					cs1.Format(L"STOP ERROR: Line %zu Source parameter index out of range. (%s)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				sourceIOD = ioID - 1;
				sourceIOT = 2;
				break;
			default:
				cs1.Format(L"STOP ERROR: Line %zu Format error. Require I or O indication after unit.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
		}

		//***************
		// read dest unit
		//***************
		destIOD = -1;	// dest IO ID
		destIOT = -1;	// dest IO type
		destUnitID = -1;
		destCategory = -1;	// dest category
		destCatIndex = -1;
		
		cp1 = strchr(cp1, ',');
		cp1++; // move past comma
		while ((*cp1 == ' ')||(*cp1 == '\t')) cp1++; // read off any empty spaces

		switch (*cp1) {
			case 'c':
			case 'C':
				cp1++;
				destCategory = atoi(cp1);
				if ((destCategory < 1) || (destCategory > 8)) {
					cs1.Format(L"STOP ERROR: Line %zu Dest category out of range. (%d)", lineCount, destCategory);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				break;
			case 'u':
			case 'U':
				cp1++;
				destUnitID = atoi(cp1)-1; // offset to zero-index
				if ((sourceUnitID < 0) || (sourceUnitID >= units.size())) {
					cs1.Format(L"STOP ERROR: Line %zu Format error. Dest unit out of range (%d).", lineCount, sourceUnitID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				break;
			case 't':
			case 'T':
				// find unit by type:instance
				cp1++;
				typeID = atoi(cp1);
				cp3 = cp1;
				cp1 = strchr(cp1, ':');
				cp2 = strchr(cp3, ','); // use to search for fault where :# is missing
				if (cp1 == nullptr) {
					cs1.Format(L"STOP ERROR: Line %zu Could not find type count.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				if (cp2 == nullptr) {
					cs1.Format(L"STOP ERROR: Line %zu Format error.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				if (cp1 > cp2) {
					cs1.Format(L"STOP ERROR: Line %zu Format error, Type missing instance specifier.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				cp1++;
				if (*cp1 >= 'A') { // use format like "T###:a"
					if (*cp1 >= 'a') {
						typeCount = 1 + *cp1 - 'a';
					} else if (*cp1 >= 'A') {
						typeCount = 1 + *cp1 - 'A';
					} else {
						cs1.Format(L"STOP ERROR: Line %zu Format error. Unit type out of range (%s).", lineCount, cp1);
						csdlg->m_status.AddString(cs1);
						return 0;
					}
				} else {
					typeCount = atoi(cp1);
				}
				if (typeCount <= 0) {
					cs1.Format(L"STOP ERROR: Line %zu Format error. Unit type out of range (%d).", lineCount, typeCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				ioID = 0; // using for convenience
				unitID = -1;
				for (size_t i = 0; i < units.size(); i++) {
					if (units[i].GetType() == typeID) {
						ioID++;
						if (ioID == typeCount) {
							unitID = (int) i;
							break;
						}
					}
				}
				if (unitID == -1) {
					cs1.Format(L"STOP ERROR: Line %zu Could not find indicated unit. (%S)", lineCount, ips);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				destUnitID = unitID; // already at zero-index
				// unit is found			
				break;
			case 'x':
			case 'X':
				if (sourceIOT == 0) {
					units[sourceUnitID].SetInputIndex(sourceIOD, 0);
					cs1.Format(L"NOTE: Unit %d Input %d has been moved to category index 0. (Line %zu)", sourceUnitID + 1, sourceIOD + 1, lineCount);
				} else { 
					units[sourceUnitID].SetOutputIndex(sourceIOD, 0); 
					cs1.Format(L"NOTE: Unit %d Output %d has been moved to category index 0.(Line %zu)", sourceUnitID + 1, sourceIOD + 1, lineCount);
				}
				continue;  // should kick it out to next readline
			default:
				cs1.Format(L"STOP ERROR: Line %zu Format error.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
		} // end dest unit switch

		cp1 = strchr(cp1, ',');
		if (cp1 == nullptr) {
			cs1.Format(L"STOP ERROR: Line %zu Format error.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}
		cp1++; // move past comma
		while (*cp1 == ' ') cp1++; // read off any empty spaces
		switch (*cp1) { // format [IO][IO ID]
			case 'v':
			case 'V':
				cp1++;
				if(destCategory == -1){
					cs1.Format(L"STOP ERROR: Line %zu Dest category index specified without category.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				destCatIndex = atoi(cp1);
				if ((destCatIndex < 0) || (destCatIndex >= maxCat[destCategory])) {
					cs1.Format(L"STOP ERROR: Line %zu Dest category index out of range. (%d)", lineCount, destCatIndex);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				break;
			case 'i':
			case 'I':
				cp1++;
				ioID = atoi(cp1);
				if (destCategory > 0) {
					maxID = maxCat[destCategory];
				} else {
					maxID = units[destUnitID].GetInputCount();
				}
				if ((ioID < 1) || (ioID > maxID)) {
					cs1.Format(L"STOP ERROR: Line %zu Dest input index out of range. (%d)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				destIOD = ioID - 1;
				destIOT = 0;
				break;
			case 'o':
			case 'O':
				cp1++;
				ioID = atoi(cp1);
				if (destCategory > 0) {
					maxID = maxCat[destCategory];
				} else {
					maxID = units[destUnitID].GetOutputCount();
				}
				if ((ioID < 1) || (ioID > maxID)) {
					cs1.Format(L"STOP ERROR: Line %zu Dest output index out of range. (%d)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				destIOD = ioID - 1;
				destIOT = 1;
				break;
			case 'p':
			case 'P':
				cp1++;
				ioID = atoi(cp1);
				if (destCategory > 0) {
					maxID = maxCat[destCategory];
				}
				else {
					maxID = units[destUnitID].GetParameterCount();
				}
				if ((ioID < 1) || (ioID > maxID)) {
					cs1.Format(L"STOP ERROR: Line %zu Dest parameter index out of range. (%d)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				destIOD = ioID - 1;
				destIOT = 2;
				break;
			default:
				cs1.Format(L"STOP ERROR: Line %zu Format error. Require I or O indication after unit.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
		}

		// check that source and dest are in same category
		// lineCount is used as placeholder for dest index
		switch (sourceIOT) {
			case 0:	sourceCategory = units[sourceUnitID].GetTypeP()->GetInputCategory(sourceIOD); break;
			case 1: sourceCategory = units[sourceUnitID].GetTypeP()->GetOutputCategory(sourceIOD); break;
			case 2: sourceCategory = 9; break;
			default: break;  // need error handling here?
		}

		if (destCategory == -1) {  // else it's already set
			switch (destIOT) {
				case 0:
					destCategory = units[destUnitID].GetTypeP()->GetInputCategory(destIOD);
					lineCount = units[destUnitID].GetInput(destIOD);
					break;
				case 1:
					destCategory = units[destUnitID].GetTypeP()->GetOutputCategory(destIOD);
					lineCount = units[destUnitID].GetOutput(destIOD);
					break;
				case 2:
					destCategory = 9; 
					lineCount = -1; // destIOD is used 
					break;
			}
		} else {
			lineCount = destCatIndex;
		}

		if(sourceCategory != destCategory) {
			cs1.Format(L"STOP ERROR (line %zu): Source (%d) and Dest (%d) are in different categories.",lineCount,sourceCategory,destCategory);
			csdlg->m_status.AddString(cs1);
			return 0;
		}

		// have source and dest info, assign source to new dest
		switch (sourceIOT ) {
			case 0:
				units[sourceUnitID].SetInputIndex(sourceIOD,lineCount);
				cs1.Format(L"NOTE: Unit %d Input %d has been moved to category %d index %d. (Line %zu)", sourceUnitID+1, sourceIOD + 1, destCategory, lineCount);
				break;
			case 1:
				units[sourceUnitID].SetOutputIndex(sourceIOD, lineCount);
				cs1.Format(L"NOTE: Unit %d Output %d has been moved to category %d index %d. (Line %zu)", sourceUnitID + 1, sourceIOD + 1,destCategory, lineCount);
				break;
			case 2:
				units[sourceUnitID].SetParameter(sourceIOD, units[destUnitID].GetParameter(destIOD));
				break;
		}
		csdlg->m_status.AddString(cs1);

	}  // end while fgets

	BuildState();  // NOTE if StateTree needs to be rebuilt, do it from calling function

	unsavedChanges = 1;

	return 1;
} // end ReadUALfile


// MAG 221006
// Reads State Report File, then copies indicated data to clipboard
// File SRF format:
// Source index#, I/O#, Dest index#
// t426:2,I1 -> selects the state assigned to the first input (I1) from the second Type426 in the unit list
// u5,I3 -> selects the state assigned to the third input from the 5th item in the unit list
int
CSimInfoApp::ReadSRFFile(FILE* ip)
{
	char ips[500] = {};// extra long to accomodate long notes lines
	char* cp1 = nullptr;
	char* cp2 = nullptr;
	char* cp3 = nullptr;
	int sequenceID = -1;

	int ioType = -1;
	size_t ioID = -1;
	int unitID = -1;
	int typeID = -1;
	int typeCount = -1;
	size_t stateIndex = 0;

	int sourceUnitID = -1;
	int sourceIOT = -1;	// source IO type
	size_t sourceIOD = -1;	// source IO ID
	int sourceCategory = -1;	// source category
	size_t maxID = 0;				// max unit IO count, or category size

	std::string stateList;
	std::string itemText;
	std::string itemComment;

	char		catLabInit[] = { "XPMTCOEWH" };		// first letter of each category label (7:Power -> Watts due to P repeating)
	char		itemOutput[100] = {};	// format print to this, then add to stateList
	HVType* hvt = nullptr;
	HGLOBAL hg = nullptr;
	CString cs1;
	int lineCount = 0;

	if (ip == nullptr) return 0;

	while (fgets(ips, 500, ip) != NULL) {
		sourceUnitID = -1;
		sourceIOT = -1;	// source IO type
		sourceIOD = -1;	// source IO ID
		sourceCategory = -1;	// source category

		lineCount++;
		
		if (strlen(ips) == 0) continue;	// check for blank lines
		
		cp1 = strchr(ips, ',');			// check for lines with no comma (might be just a few blank spaces)
		if (cp1 == nullptr) continue;

		if ((ips[0] == '!') || (ips[0] == '%')) {
			cs1 = ips;
			csdlg->m_status.AddString(cs1);
			continue;
		}
		cp1 = ips;
		while (*cp1 == ' ') cp1++; // read off any empty spaces

		// read source unit
		switch (*cp1) {
			case 'u':
			case 'U':
				cp1++;
				sourceUnitID = atoi(cp1) - 1; // offset to zero-index
				if ((sourceUnitID < 0) || (sourceUnitID >= units.size())) {
					cs1.Format(L"STOP ERROR: Line %d Format error. Source unit out of range (%d).", lineCount, sourceUnitID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				break;
			case 't':
			case 'T':
				// find unit by type:instance
				cp1++;
				typeID = atoi(cp1);
				cp3 = cp1;
				cp1 = strchr(cp1, ':');
				cp2 = strchr(cp3, ','); // use to search for fault where :# is missing
				if (cp1 == nullptr) {
					cs1.Format(L"STOP ERROR: Line %d Could not find type count.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				if (cp2 == nullptr) {
					cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				if (cp1 > cp2) {
					cs1.Format(L"STOP ERROR: Line %d Format error, Type missing instance specifier.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				cp1++;
				if (*cp1 >= 'A') { // use format like "T###:A" for first instance of T###
					if (*cp1 >= 'a') {
						typeCount = 1 + *cp1 - 'a';
					}
					else if (*cp1 >= 'A') {
						typeCount = 1 + *cp1 - 'A';
					}
					else {
						cs1.Format(L"STOP ERROR: Line %d Format error. Unit type out of range (%s).", lineCount, &cp1[0]);
						csdlg->m_status.AddString(cs1);
						return 0;
					}
				}
				else {
					typeCount = atoi(cp1);
				}
				if (typeCount <= 0) {
					cs1.Format(L"STOP ERROR: Line %d Format error. Unit type out of range (%d).", lineCount, typeCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				ioID = 0; // using for convenience
				unitID = -1;
				for (size_t i = 0; i < units.size(); i++) {
					if (units[i].GetType() == typeID) {
						ioID++;
						if (ioID == typeCount) {
							unitID = (int) i;
							break;
						}
					}
				}
				if (unitID == -1) {
					cs1.Format(L"STOP ERROR: Line %d Could not find indicated unit. (%S)", lineCount, ips);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				sourceUnitID = unitID; // already at zero index
				// unit is found			
				break;
			default:
				cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
		} // end source switch

		cp1 = strchr(ips, ',');
		if (cp1 == nullptr) {
			cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}
		cp1++; // move past comma
		while (*cp1 == ' ') cp1++; // read off any empty spaces
			switch (*cp1) { // format [IO][IO ID]
			case 'i':
			case 'I':
				cp1++;
				ioID = atoi(cp1);
				if ((ioID < 1) || (ioID > units[sourceUnitID].GetInputCount())) {
					cs1.Format(L"STOP ERROR: Line %d Source input index out of range. (%zu)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				sourceIOD = ioID - 1;
				sourceIOT = 0;
				break;
			case 'o':
			case 'O':
				cp1++;
				ioID = atoi(cp1);
				if ((ioID < 1) || (ioID > units[sourceUnitID].GetOutputCount())) {
					cs1.Format(L"STOP ERROR: Line %d Source output index out of range. (%zu)", lineCount, ioID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				sourceIOD = ioID - 1;
				sourceIOT = 1;
				break;
			default:
				cs1.Format(L"STOP ERROR: Line %d Format error. Require I or O indication after unit.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
		}

		// check for line comment
		cp3 = strchr(ips, '!');
		if (cp3 == nullptr) {
			cp3 = strchr(ips, '/');
			cp3++;
		}

		if (cp3 != NULL) {
			while (*cp3 == '!') cp3++;
			while (*cp3 == ' ') cp3++;
			itemComment = cp3;
		} else {
			itemComment.clear();
		}

		// find state index, save to string
		hvt = theApp.units[sourceUnitID].GetTypeP();

		if (sourceIOT == 0) {
			ioID = theApp.units[sourceUnitID].GetInput(sourceIOD);
			sourceCategory = hvt->GetInputCategory(sourceIOD);
			stateIndex = state[sourceCategory][ioID].GetStateIndex();
			itemText = hvt->GetInputLabel(sourceIOD);
		} else { 
			ioID = theApp.units[sourceUnitID].GetOutput(sourceIOD); 
			sourceCategory = hvt->GetOutputCategory(sourceIOD);
			stateIndex = state[sourceCategory][ioID].GetStateIndex();
			itemText = hvt->GetOutputLabel(sourceIOD);
		}
		sprintf_s(itemOutput,100,"%zd,! U%03d:%c%02zd - %c%03zd - %s", stateIndex, sourceUnitID+1, (sourceIOT == 0)?'I':'O', sourceIOD+1, catLabInit[sourceCategory], stateIndex, itemText.c_str());
		stateList.append( itemOutput);
		if (itemComment.length() > 0) {
			stateList.append(" ");
			stateList.append(itemComment);
		} else stateList.append("\n");
	}

	// save string to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();

	//HGLOBAL 
	hg = GlobalAlloc(GMEM_MOVEABLE, stateList.size());
	if (!hg) {
		CloseClipboard();
		return 0;
	}

	try {
		memcpy(GlobalLock(hg), stateList.c_str(), stateList.size());
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);
	}
	catch (...) {
		csdlg->m_status.AddString(L"ERROR- contents may not have been copied to clipboard!");
		return 0;
	}

	csdlg->m_status.AddString(L"The state report has been sent to the clipboard.");
	csdlg->m_status.SetCurSel(csdlg->m_status.GetCount() - 1);

	return 1;
}


// MAG 211013
// Reads State Move List file
// File CSV format: 
// Source index#, I/O#, Dest index#
// 100,O1,101 -> move state 100 Output 1 to state 101
// (4,26),I2,(4,30) -> move type 4 #26 to type 4 #30 NOTE mix and match (,) vs state#
// T426:2,I1,T800:1,O3 -> move 1st input from 2nd Type426 unit state assignment -to same state as- third output of 1st Type800 unit - MAG 220420
int 
CSimInfoApp::ReadSMLFile(FILE* ip)
{
	// TODO: Add your implementation code here.
	char ips[500] = {};// extra long to accomodate long notes lines
	char* cp1 = nullptr;
	char* cp2 = nullptr;
	char* cpNote = nullptr;
	int typeID = -1;
	int ioType = -1;
	int sequenceID = -1;
	int ioID = -1;
	int unitID = -1;
	int typeCount = -1;
	StateElement* dragSE = nullptr; // source
	StateElement* dropSE = nullptr;	// dest
	CString cs1;
	size_t dragUnit = 0;		// unit # for which i/o state # is changing
	size_t dragUnitIndex = 0;	// index in unit, i.e. input #2 or output #5 IN UNIT
	size_t dropStateIndex = 0;	// index into state type, i.e. state[?][THIS]
	int lineCount = 0;
	std::vector<int> unitRemoveList;

	if (ip == nullptr) return 0;

	while (fgets(ips, 500, ip) != NULL) {
		lineCount++;
		if ((ips[0] == '!')||(ips[0] == '%')) {
			cs1 = ips;
			csdlg->m_status.AddString(cs1);
			continue;
		}

		if (ips[0] == 'u') {  // remove unit, line format "u###"
			cp1 = ips + 1;
			typeID = atoi(ips);
			if(typeID < 0){
				cs1.Format(L"STOP ERROR: Line %d Format error. Unit removed is less than zero (%d).", lineCount,typeID);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			if (typeID >= units.size()) {
				cs1.Format(L"STOP ERROR: Line %d Format error. Unit removed is out of range (%d/%d).", lineCount, typeID, (int)units.size());
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			unitRemoveList.push_back(typeID);  // hold off on removing until all units are read
			continue;	// MAG 220110 should go to next line in file here.
		}

		cp1 = strchr(ips, ',');
		if (cp1 == nullptr) {// check for commas, if none there is an error
			cs1.Format(L"NOTE: Line %d does not contain position information.", lineCount);
			csdlg->m_status.AddString(cs1);
			continue;
		}

		cpNote = strchr(ips, '!');
		// check for (T,ID) format- see if ( is before ,
		cp1 = strchr(ips, '(');
		cp2 = strchr(ips, ',');
		if ((cpNote != NULL) && (cp2 > cpNote)) {
			cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}
		if ((cp1 != NULL)&&(cp1 < cp2)) { // first one starts with () format
			cp1++; // move past (
			typeID = atoi(cp1);
			if ((typeID < 1) || (typeID > 8)) {
				cs1.Format(L"STOP ERROR: Line %d Category %d is out of range.", lineCount,typeID);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			cp2++;
			sequenceID = atoi(cp2);
			if ((sequenceID < 0) || (sequenceID >= maxCat[typeID])) {
				cs1.Format(L"STOP ERROR: Line %d category %d is out of range (0 - %zu).", lineCount, sequenceID, maxCat[typeID]);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			dragSE = StateVec(typeID, sequenceID);
			if (dragSE == nullptr) {
				cs1.Format(L"STOP ERROR: Line %d Could not find indicated source element (%d,%d).", lineCount, typeID, sequenceID);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			cp1 = strchr(ips, ')');
			if (cp1 == nullptr) {
				cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			cp1++;	// move past )
			cp1++;	// move past ,
			while (*cp1 == ' ') cp1++;
		} else {	// just read vector index
			switch (ips[0]) {
			case 't':
			case 'T':// format: "T426:2"
				// find unit by type:instance
				cp1 = &ips[1];
				typeID = atoi(cp1);
				cp1 = strchr(ips, ':');
				if (cp1 == nullptr) {
					cs1.Format(L"STOP ERROR: Line %d Could not find type count.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				cp1++;
				typeCount = atoi(cp1);
				ioID = -1; // using for convenience
				unitID = -1;
				for (size_t i = 0; i < units.size(); i++){
					if (units[i].GetType() == typeID){
						ioID++;
						if (ioID == typeCount) {
							unitID = (int) i;
							break;
						}
					}
				}
				if(unitID == -1){
					cs1.Format(L"STOP ERROR: Line %d Could not find indicated unit. (%S)", lineCount,ips);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				// unit is found, I/O id and state linkage are found below
				sequenceID = -1;
				dragSE = nullptr;
				break;
			case 'u':
			case 'U':
				break;
			default:
				sequenceID = atoi(ips);
				dragSE = StateVec(sequenceID);	// error/range checking done in sub
				if (dragSE == nullptr) {
					cs1.Format(L"STOP ERROR: Line %d Could not find indicated source element (%d).", lineCount, sequenceID);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				cp1 = strchr(ips, ',');
				cp1++;	// move past ,
				while (*cp1 == ' ') cp1++;
			}
		}

		// read IO id, get HTREEITEM
		switch (*cp1){
			case 'i':
			case 'I':
				ioType = 1;
				break;
			case 'o':
			case 'O':
				ioType = 2;
				break;
			default:
				break;
		}
		if (ioType == -1) { 
			cs1.Format(L"STOP ERROR: Line %d Could not find I/O type '%c'.", lineCount, *cp1);
			csdlg->m_status.AddString(cs1);
			return 0; 
		}

		cp1++;
		ioID = atoi(cp1);
		ioID--; // base 1 (human) vs base 0 counting- must match display number!!!
		if (ioID < 0) {
			cs1.Format(L"STOP ERROR: Line %d I/O instance out of range.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}
		if (dragSE != NULL) {// using state element id to specifiy drag element
			switch (ioType) {
			case 1: // input
				if (ioID >= dragSE->GetInputCount()) {
					cs1.Format(L"STOP ERROR: Line %d I/O instance out of range.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				//sourceHandle = dragSE->GetInputTreelink(ioID);
				break;
			case 2: // output
				if (ioID >= dragSE->GetOutputCount()) {
					cs1.Format(L"STOP ERROR: Line %d I/O instance out of range.", lineCount);
					csdlg->m_status.AddString(cs1);
					return 0;
				}
				//sourceHandle = dragSE->GetOutputTreelink(ioID);
				break;
			default:	// this shouldn't ever happen
				cs1.Format(L"STOP ERROR: Line %d Unknown I/O type.", lineCount);
				return 0;
			}
		}

		// get destination info
		cp2 = strchr(cp1, ',');
		if (cp2 == nullptr) {
			cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}
		if ((cpNote != NULL) && (cp2 > cpNote)) {
			cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}
		cp2++;

		cp1 = strchr(cp2, '(');
		// check for lines like "1,I1,2 ! (note with parenth)"
		if ((cp1 != NULL)&&((cpNote != NULL) && (cp1 < cpNote))){ // dest uses () format
			cp1++; // move past (
			typeID = atoi(cp1);
			if ((typeID < 1) || (typeID > 8)) {
				cs1.Format(L"STOP ERROR: Line %d Category out of range (%d).", lineCount,typeID);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			cp2 = strchr(cp1, ',');
			if (cp2 == nullptr) {
				cs1.Format(L"STOP ERROR: Line %d Format error.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			cp2++;
			sequenceID = atoi(cp2);
			if ((sequenceID < 0) || (sequenceID >= maxCat[typeID])) {
				cs1.Format(L"STOP ERROR: Line %d position %d is out of range (0 - %d).", lineCount, sequenceID, maxCat[typeID]);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
			dropSE = StateVec(typeID, sequenceID);
			if (dropSE == nullptr) {
				cs1.Format(L"STOP ERROR: Line %d Could not identify target.", lineCount);
				return 0;
			}
		} else {	// just read vector index
			sequenceID = atoi(cp2);
			dropSE = StateVec(sequenceID);	// error/range checking done in sub
			if (dropSE == nullptr) {
				cs1.Format(L"STOP ERROR: Line %d Could not find indicated destination element (%d).", lineCount, sequenceID);
				csdlg->m_status.AddString(cs1);
				return 0;
			}
		}

		// error check the move
		if (dropSE->GetCategory() != dragSE->GetCategory()) {
			cs1.Format(L"STOP ERROR: Line %d State move failed- Move must be to the same category.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}

		if (dropSE == dragSE) {
			cs1.Format(L"STOP ERROR: Line %d State move stopped- Move was to same State index.", lineCount);
			csdlg->m_status.AddString(cs1);
			return 0;
		}

		// everything seems ok, try to move the item in the state array and the units array
		dropStateIndex = dropSE->GetCategoryIndex();

		// MAG NOTE this section copied from DragState()
		switch (ioType) {
			case 1: // input
				dragSE->GetInputInfo(ioID, &dragUnit, &dragUnitIndex);
				//dropSE->GetInputInfo(ioindex, &dropUnit, &dropUnitIndex);
				units[dragUnit - 1].SetInputIndex(dragUnitIndex, dropStateIndex);
				dragSE->DelInput(ioID);
				dropSE->AddInput(dragUnit, dragUnitIndex);
				cs1.Format(L"Move input from State index %zu to %zu.", dragSE->GetStateIndex(), dropSE->GetStateIndex());
				csdlg->m_status.AddString(cs1);
				unsavedChanges = 1;
				break;
			case 2: // output
				dragSE->GetOutputInfo(ioID, &dragUnit, &dragUnitIndex);
				units[dragUnit - 1].SetOutputIndex(dragUnitIndex, dropStateIndex);
				dragSE->DelOutput(ioID);
				dropSE->AddOutput(dragUnit, dragUnitIndex);
				cs1.Format(L"Move output from State index %zu to %zu.", dragSE->GetStateIndex(), dropSE->GetStateIndex());
				csdlg->m_status.AddString(cs1);
				unsavedChanges = 1;
				break;
			default:
				cs1.Format(L"STOP ERROR: Line %zu State move stopped- Error with determining I/O type.", lineCount);
				csdlg->m_status.AddString(cs1);
				return 0;
		}

		//unsavedChanges = 1; MAG 220110 test if not needed

	}  // end while fgets (reading file)

	if (unitRemoveList.size() > 0) {
		std::sort(unitRemoveList.begin(), unitRemoveList.end(), std::greater<int>()); // sort list in descending order
		const auto dupes = std::adjacent_find(unitRemoveList.begin(), unitRemoveList.end());
		if(dupes != unitRemoveList.end()) {
			cs1.Format(L"STOP ERROR: Duplicate units in remove list (unit %d).", *dupes);
			csdlg->m_status.AddString(cs1);
			return 0;
		}

		for (std::vector<int>::iterator iit = unitRemoveList.begin(); iit != unitRemoveList.end(); iit++) {
			cs1.Format(L"Delete unit #%d Type %d (%S)", *iit, units[*iit].GetType(), units[*iit].GetTypeP()->GetTypeLabel().c_str());
			units[*iit].GetTypeP()->DecUseCount();
			units.erase(units.begin() + *iit);
		}
		unsavedChanges = 1;
	}

	if (unsavedChanges > 0) BuildState();

	return 1;
} // end ReadSMLFile


int 
CSimInfoApp::DeleteUnit(size_t unitIndex)	// MAG add 220419
{
	CString cs1;

	if (unitIndex < 0) return 0;
	if (unitIndex >= units.size()) return 0;

	cs1.Format(L"Delete unit #%zu Type %d (%S)", unitIndex+1, units[unitIndex].GetType(), units[unitIndex].GetTypeP()->GetTypeLabel().c_str());
	if (MessageBox(NULL, cs1, L"Confirm delete unit?", MB_YESNO) == IDNO) {
		return 0;
	}

	units[unitIndex].GetTypeP()->DecUseCount();
	units.erase(units.begin() + unitIndex);
	theApp.csdlg->m_status.AddString(cs1);
	unsavedChanges = 1;

	// start with next unit (now at unitIndex) and decrement unit value
	for (size_t i = unitIndex; i < units.size(); i++){
		units[i].DecrUnit();
	}

	return 1;
}


int
CSimInfoApp::DeleteUnit(HTREEITEM unitIndex)	// MAG add 220419
{
	for (size_t i = 0; i < units.size(); i++) {
		if (units[i].GetTreepos() == unitIndex) {
			return DeleteUnit(i);
		}
	}

	return  0;
}


// returns the OFFSET from the key pressed to the ascii index for the character when the SHIFT key is pressed. -32 for a to A
// NOTE returns 0xFF as an error value
// NOTE letters are read as upper case, but return value will switch to lower case if SHIFT is unpressed
int 
CSimInfoApp::ASCIIoffset(char inChar, int shiftOn)
{
	if (inChar < 32) return 0xFF;
	if (inChar > 126) return 0xFF;
	if ((inChar >= 'A') && (inChar <= 'Z') && (shiftOn == 0)) return 32;  // convert upper case to lower case
	// Numbers!
	if ((inChar >= '0') && (inChar <= '9') && (shiftOn == 1)) {   // number key presses are sent as numbers even if shift is on
		switch (inChar) {
			case '1': return '!' - '1';
			case '2': return '@' - '2';
			case '3': return '#' - '3';
			case '4': return '$' - '4';
			case '5': return '%' - '5';
			case '6': return '^' - '6';
			case '7': return '&' - '7';
			case '8': return '*' - '8';
			case '9': return '(' - '9';
			case '0': return ')' - '0';
			default:
				return 0xFF;
		}
	}

	if (shiftOn == 0) return 0;

	switch (inChar) {
		case '`': return '~' - '`';
		case ',': return '<' - ',';
		case '.': return '>' - '.';
		case ';': return ':' - ';';
		case '[': return '{' - '[';
		case ']': return '}' - ']';
		case '-': return '_' - '-';
		case '=': return '+' -'=';
		case '\\': return '|' - '\\';
		case '\'': return '\"' - '\'';
	}

	return 0;
};

