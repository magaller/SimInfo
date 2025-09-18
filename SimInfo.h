
// SimInfo.h : main header file for the PROJECT_NAME application
//
#pragma once

#ifndef DEF_SIMINFO_MAIN
#define DEF_SIMINFO_MAIN

#include <bits.h>  // for sort() by greater

#include <string>
#include <vector>
#include <algorithm>

#include "SimInfoDlg.h"
#include "Value.h" // for change value dialog
#include "State Move Dialog.h"
#include "resource.h"		// main symbols

#include "C:\Users\mikeg\xml\mxml\mxml.h"

/*
HVACSIM+ Categories
1 Pressure
2 Mass flow rate
3 Temperature
4 Control
5 Other
6 Energy
7 Power
8 Humidity
*/
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif


#define STATESIZE 400	// length of state array PER CATEGORY in state array structure
#define CSTRINGSTUFF	// this enable additional subroutines in the XML code in mxml.cpp

class StateElement;	// pre-declare class so it can be used prior to definition

// Type data: holds info about one input/output- from one line in Typar.dat in a type description
class CategoryParameter {  // was TypeParameter
public:
	CategoryParameter();
	std::string	label;		// text label from input file
	int GetCategory() { return category; };
	void SetCategory(int cat) { category = cat; };
protected:
	int			category;	// valid range 1-8, 1=pressure, etc.
};

// holds type descriptions, read from typar.dat.  NOT instantiated units from a sim file.
class HVType
{
public:
	HVType();
	~HVType();
	const int			GetType() { return id; };
	const int			GetCountSaved() { return countSaved; };
	const int			GetCountDiffEQ() { return countDiffEQ; };
	const size_t		GetCountIn() { return inputs.size(); };
	const size_t		GetCountOut() { return outputs.size(); };
	const size_t		GetCountModifiers() { return modifiers.size(); };
	const size_t		GetCountParameter() { return parameters.size(); };
	const std::string	GetTypeLabel() { return label; };
	const std::string	GetInputLabel(size_t index) { return inputs[index].label; };
	const std::string	GetOutputLabel(size_t index) { return outputs[index].label; };
	const std::string	GetParameterLabel(size_t index) { return parameters[index].label; };
	const std::string	GetModifier(size_t index) { return modifiers[index]; };
	const int			GetInTree() { return inTree; }; // didn't think about this before- may be the weirdest sub name I've ever created. IsInTree would be inconsistent.

	int		SetType(int val); // sets id
	int		SetLabel(char *label);
	int		SetCountSaved(int val);
	int		SetCountDiffEQ(int val);
	int		SetCountIn(int val);
	int		SetCountOut(int val);
	int		SetCountParameter(int val);
	int		SetCountFileParameter(int val);
	void	SetInTree() { inTree = 1; };// this will only change from 0 to 1;

	const int	GetInputCategory(size_t index);
	const int	GetOutputCategory(size_t index);
	const int	GetCountFileParameter(void) { return countFileParameter; }

	int			AddInput(int type, std::string label);
	int			AddOutput(int type, std::string label);
	int			AddParameter(int type, std::string label);
	void		AddModifier(std::string ips) { while (ips.back() == ' ') ips.pop_back(); modifiers.push_back(ips); return; };

	const int	GetUseCount() { return countUsed; };
	void		AddUseCount() { countUsed++; };
	void		DecUseCount() { countUsed--; if (countUsed < 0) countUsed = 0; };	// MAG 201028 add function
	void		ZeroUseCount() { countUsed = 0; };									// MAG 201028 add function

	void		Reset();

	HTREEITEM	GetTreepos() { return treepos; };
	void		SetTreepos(HTREEITEM tp) { treepos = tp; }; // no error checks, NULL is a valid setting
	// these connect to the i/o/p indexes
	HTREEITEM	GetInputTreeItem(size_t index);
	HTREEITEM	GetOutputTreeItem(size_t index);
	HTREEITEM	GetParamTreeItem(size_t index);

	void SetInputTreeItem(size_t index, HTREEITEM hti);
	void SetOutputTreeItem(size_t index, HTREEITEM hti);
	void SetParamTreeItem(size_t index, HTREEITEM hti);
protected:
	//NOTE- all info is from typar file
	int			id;					// type id, # from typar
	std::string	label;				// type label
	int			countSaved;			// number of saved vars
	int			countDiffEQ;		// number of differential equations
	int			countIn;			// number of input variables
	int			countOut;			// number of output variables
	int			countParameter;		// number of parameters
	int			countUsed;			// count of instances of this type is the units list, i.e. how many are used in this sim
	int			inTree;				// bool to indicate if this has been added to the dialog box tree
	int			countFileParameter;	// MAG 200921 for types which use a parameter file (.PAR), this is the count of parameters
	HTREEITEM	treepos;			// pointer to tree
	std::vector<CategoryParameter>	inputs;		// info on inputs
	std::vector<CategoryParameter>	outputs;	// info on outputs
	std::vector<CategoryParameter>	parameters;	// info on parameters
	std::vector<std::string>		modifiers;  // MAG 191004 add this- holds line for DE modifiers IPCHEK, IARGUE, COMPAR, CKVALU

	// id's for i/o/p in Types Display window
	std::vector<HTREEITEM>	htlIn;		// list of pointers to input items
	std::vector<HTREEITEM>	htlOut;		// list of pointers to output items
	std::vector<HTREEITEM>	htlParam;	// list of pointers to parameter items
};	// end HVType


// note maximum of 40 units per block
//		maximum of 20 blocks per superblock
//		maximum of 40 superblocks per simulation
class HVUnit {
public:
	HVUnit();
	int			SetUnit(size_t val);
	const size_t	GetUnit() { return unit; };
	void		IncrUnit() { unit++; };  // MAG 191218 used when inserting a new unit in the middle of the list
	void		DecrUnit() { unit--; };  // MAG 220420 used when deleting a unit in the middle of the list
	void		IncrInput(int i);	// MAG 200220 used when inserting multiple new units
	void		IncrOutput(int i);
	int			SetBlock(size_t val);
	int			SetSuperBlock(size_t val);
	const size_t	GetBlock() { return block; };
	const size_t	GetSuperBlock() { return superBlock; };
	//int		SetType(int val);
	int			SetType(HVType *val);
	
	int			AddInput(int val);
	int			AddOutput(int val);
	int			AddInput(size_t val);
	int			AddOutput(size_t val);
	int			AddParameter(double val);
	void		SetParameter(size_t i, double val);
	int			AddInput(int val, double initVal);
	int			AddOutput(int val, double initVal);
	const double	GetInitInputValue(size_t index);
	const double	GetInitOutputValue(size_t index);
	int			SetInitInputValue(size_t index , double val);
	int			SetInitOutputValue(size_t index, double val);

	// MAG 200220 add to support inserting a new unit in the middle of the list with sequencing
	void SetInputIndex(size_t inp, size_t ind);
	void SetOutputIndex(size_t inp, size_t ind);
	void SetInputBoundary(size_t inp, int boundVal);

	const int		GetType();
	HVType			*GetTypeP() { return myType; };	// Note will return NULL if it is NULL, check for NULL not necessary
	const size_t	GetInput(size_t index);	// returns input[index], offset into category state vector
	const size_t	GetOutput(size_t index);	// returns output[index], offset into category state vector
	const double	GetParameter(size_t index);
	const int		GetInputBoundary(size_t inp);

	const size_t	GetInputCount() { if (myType == NULL) return 0; return myType->GetCountIn(); };
	const size_t	GetOutputCount() { if (myType == NULL) return 0; return myType->GetCountOut(); };
	const size_t	GetParameterCount() { if (myType == NULL) return 0; return myType->GetCountParameter(); };

	HTREEITEM		GetTreepos() { return treepos; };
	void			SetTreepos(HTREEITEM tp) { treepos = tp; }; // no error checks, NULL is a valid setting

	//int GetParameterCount() { return myType->GetCountParameter(); };
	const HTREEITEM GetInputTreeItem(size_t index);
	const HTREEITEM GetOutputTreeItem(size_t index);
	const HTREEITEM GetParamTreeItem(size_t index);
	//HTREEITEM GetStateLinkFromInput(int index);	// TODO- verify correct operation
	//HTREEITEM GetStateLinkFromOutput(int index);	// TODO- verify correct operation

	void SetInputTreeItem(size_t index, HTREEITEM hti);
	void SetOutputTreeItem(size_t index, HTREEITEM hti);
	void SetParamTreeItem(size_t index, HTREEITEM hti);

	void SetLabel(std::string nl) { myLabel = nl; }
	const std::string GetLabel() { return myLabel; };

	void SetNote(std::string nl) { myNote = nl; }
	const std::string GetNote() { return myNote; };

	void Reset();

protected:
	//NOTE- when adding new variables here also add them to the Reset function!
	// MAG 210510 NOTE- unit may NOT be the same as index in array, some sample files have units out of order! Why? A good reason has not been shown.
	//				Units should be accessed by GetUnit, not by array index.
	size_t		unit;		// same as index in array, but might be accessed by pointer -? should be offset by one
	size_t		block;		// id of block
	size_t		superBlock;	// id of superblock
	HVType	*myType;	// pointer to type info
	std::string	myLabel;	// label shown in tree
	std::string	myNote;		// MAG 200121 add this field- note from unit list file, if present

	// NOTE state array is accessible through io index combined with io category from myType, used as indexes in the state[][] array

	// these are indexes into the i/o/p type arrays
	// NOTE input and output indexes are by category, not absolute state indexes
	std::vector<size_t>	inputs;		// input index in category
	std::vector<size_t>	outputs;	// output index in category
	std::vector<double>	parameters;	// parameter values
	std::vector<int>	inputBoundary;	// MAG 200420 '1' indicates the STATE variable connected to the INPUT is a boundary. If ANY of the inputs to a STATE are boundary, the STATE is set to boundary.

	//std::vector<StateElement> inputState;
	//std::vector<StateElement> outputState;
	std::vector<double>	initInputValue;	// MAG 200825 Initial value when created. Only used to seed state array. 
	std::vector<double>	initOutputValue;	// MAG 200825 Initial value when created. Only used to seed state array. 

	HTREEITEM	treepos;

	// id's for i/o/p in Units Display window
	std::vector<HTREEITEM>	htlIn;
	std::vector<HTREEITEM>	htlOut;
	std::vector<HTREEITEM>	htlParam;
};	// end HVUNIT

// holds info for one input/output element of a state array, i.e. an input or output
class StateAccess {
public:
	// note- type info is in StateElement
	size_t unit;	// NOTE- hold unit ID, NOT index to unit array! Unit numbers are not contiguous.
	size_t index;  // unit input or output #, i.e. first input (input 1 (regardless of type)) or third output (output 3).
	HTREEITEM treepos;
};	// end StateAccess

// holds one line of state array
// implement one of these for each state element type (category), i.e. 8 of these
// Note that absolute state vector index will have to be calculated externally
// Note that authoratative data is stored in TYPE and UNIT, changes to those require state array to be rebuilt
class StateElement {
protected:
	size_t						category;	// which category 1-8, i.e. 1=pressure, etc.
	size_t						catIndex;	// index in category of this element, i.e. where in state array
											// NOTE- set index to 1 (leave category=0) to indicate "skipped" elements
											// MAG 200225 NOTE- category is set up to maxCat[#]. Use maxCat[] to determine if a 

	std::vector<StateAccess>	inputs;		// where is this used as an input?
	std::vector<StateAccess>	outputs;	// where is this used as an output?
	double						initValue;	// holds the initial value, as set in the .sim file. Also changed if modified through interface.
	int							initValueValid;	// MAG 220111 add this check
	std::vector<double>			timeValue;	// holds time series data, if read in.
	std::vector<int>			timeStep;	// timestamp of datapoint, in seconds. Might be better as CTimeSpan?
	std::vector<size_t>			timeSB;		// superblock of point
	std::vector<size_t>			isReported;	// array of SB in which this is a reported state element- ReportedVars has same data and is authorative
	int							isBoundary;	// bool is this a boundary variable?
	HTREEITEM					treepos;		// link to state description node
	HTREEITEM					treeposInput;	// link to "Input" node
	HTREEITEM					treeposOutput;	// link to "Output" node

public:
	StateElement();
	const size_t	GetCategory() { return category; };
	const size_t	GetCategoryIndex() { return catIndex; };
	void			SetCategory(size_t val) { if (val < 0) return; if (val > 8) return; category = val; };
	void			SetCategoryIndex(size_t val) { if (val < 0) return; catIndex = val; };
	const size_t	GetStateIndex();

	const size_t	GetInputCount() { return inputs.size(); };
	const size_t	GetOutputCount() { return outputs.size(); };

	int				GetInputInfo(size_t index, size_t *unit, size_t *unitIndex);
	int				GetOutputInfo(size_t index, size_t *unit, size_t *unitIndex);

	void			IncrementInputUnit(size_t index);
	void			IncrementOutputUnit(size_t index);

	HVUnit			*GetInputUnit(size_t index);
	HVUnit			*GetOutputUnit(size_t index);
	const size_t	GetInputUnitIndex(size_t index) { if (index < 0) return -1; if (index > inputs.size()) return -1; return inputs[index].index; };
	const size_t	GetOutputUnitIndex(size_t index) { if (index < 0) return -1; if (index > outputs.size()) return -1; return outputs[index].index; };

	void			SetInputTreelink(size_t index , HTREEITEM hti);
	void			SetOutputTreelink(size_t index, HTREEITEM hti);
	HTREEITEM		GetInputTreelink(size_t index);
	HTREEITEM		GetOutputTreelink(size_t index);
	HTREEITEM		GetInputTreelink( size_t unit, size_t index);
	HTREEITEM		GetOutputTreelink(size_t unit, size_t index);

	//void AddInput(StateAccess inp);
	void AddInput(size_t unit, size_t unitIndex);
	void AddOutput(size_t unit, size_t unitIndex);
	void DelInput(size_t index);
	void DelOutput(size_t index);

	void Reset();	// set back to original state

	HTREEITEM	GetTreepos() { return treepos; };
	void		SetTreepos(HTREEITEM tp) { treepos = tp; }; // no error checks, NULL is a valid setting
	HTREEITEM	GetTreeposInput() { return treeposInput; };
	void		SetTreeposInput(HTREEITEM tp) { treeposInput = tp; }; // no error checks, NULL is a valid setting
	HTREEITEM	GetTreeposOutput() { return treeposOutput; };
	void		SetTreeposOutput(HTREEITEM tp) { treeposOutput = tp; }; // no error checks, NULL is a valid setting

	//UINT nodeState;  // used with HTREEITEM, GetItemState, SetItemState, with mask TVIS_EXPANDED

	void			SetInitValue(double ip) { initValue = ip; initValueValid = 1; };
	const double	GetInitValue() { return initValue; };
	const int		GetInitValueValid() { return initValueValid; };

	// MAG 190903 add timeValue code and functionality
	void			AddTimeValue(double ip) { timeValue.push_back(ip); };
	void			AddTimeValue(int ts, double ip, size_t supB) { timeStep.push_back(ts); timeValue.push_back(ip); timeSB.push_back(supB); };
	const double	GetTimeValue(int id);	// id is array index
	const double	GetTimeStep(int id);	// id is array index
	const size_t	GetTimeValueCount() { return timeValue.size(); };
	void			ClearTimeValues() { timeValue.clear(); timeStep.clear(); };

	const int		IsReported() { return (isReported.size() > 0)?1:0; };
	const size_t	ReportedCount() { return isReported.size(); };
	const int		IsReported(size_t index);
	const size_t	GetReportedIndex(size_t index) { if (index < 0) return 0; if (isReported.size() < 0) return 0; return(isReported[index]); };
	void			SetReported(size_t sbVal, int val);
	const int		IsBoundary() { return isBoundary; };
	int				SetBoundary(int val) {
						if ((val < 0) || (val > 1)) { return 0; }
						else { isBoundary = val; return 1; };
					}
};	// end StateElement

// CSimInfoApp:
// See SimInfo.cpp for the implementation of this class
//

// MAG 190611 holds info for one unit. Used for unit input crosswalk.
typedef struct pUnitCW {
	std::string label;	// unit label
	std::vector<std::string> ilabels;  // holds info on unit providing data
} UnitCW;

// MAG 190611 holds the list of instantiated units for a type. A vector of this type holds the entire list.
typedef struct pTypeCW {
	int id;
	std::string					desc;
	std::vector<UnitCW>			unitList;
	std::vector<std::string>	ilabels;  // holds text for label type
} TypeCW;

// MAG 191126 create struct
// struct DFNfile - holds all info from a .dfn (model definition) file
typedef struct pDFNfile {
	std::string			title;
	int					nstate; // numer of state variables
	int					nsblok;	// number of SBS, state blocks
	std::vector<int>	nsupers;	// # of blocks in each SB
	std::vector<double> state;	// vector of state variable initial values
	std::vector<int>	ndent;	// state variable identification vector
	std::vector<int>	nunits;	// # of units in each block B
	std::vector<int>	njsslv;	// # of simultaneous eqs in each SB
	std::vector<int>	njsolv;	// # of simultaneous eqs in each block
	std::vector<int>	isuper;	// array of block numbers in each SB
	std::vector<std::vector<int>>	iblock; //array of unit numbers in each block - index 1 is block#, index 2 is unit index in block
	std::vector<int>	iunits;	//array of type #'s for each unit
	std::vector<int>	nin;	// number of inputs to unit u
	std::vector<std::vector<int>>	inui;	// array of input connections for unit u
	std::vector<int>	nout;	// number of inputs to unit u
	std::vector<std::vector<int>>	outui;	// array of input connections for unit u
	std::vector<std::vector<int>>	jssolv;	// array of variables solved simultaneously within each sb (between blocks)
	std::vector<int>	nde;	// # of differential eqs in unit U
	std::vector<int>	inde;	// inde(u,i): de index for the ith de in unit U
	std::vector<int>	idevar;	// variable index for de #d
	std::vector<int>	isaved;	// index of first saved var. for unit U
	std::vector<int>	jpar;	//  jpar(u): index of first parameter for unit U
} DFNfile;


class CSimInfoApp : public CWinApp
{
public:
	CSimInfoApp();

// Overrides
public:
	virtual BOOL InitInstance();
	int ReadTypeFile(FILE *ip);
	void SaveTypeFile();
	int ReadSimFile(FILE *ip);
	int SaveSimInfoFile(FILE *op);
	int SaveUnitCrosswalkFile(FILE *op);
	int SaveDataCrosswalkFile(FILE *op);
	CSimInfoDlg *csdlg;
	ValueDlg	*vdlg;
	int ReadDataFile(FILE *ip);
	int ReadDataOutFile(FILE *ip);	// MAG add 200805
	int SaveUnitListFile(FILE *op);	// MAG add 200109
	int SaveUALFile(FILE* op);		// MAG add 220422
	int ReadUnitListFile(FILE *op);	// MAG add 200109
	int ReadSMLFile(FILE* ip);		// MAG add 211013
	int ReadUALFile(FILE* ip);		// MAG add 220421
	int ReadSRFFile(FILE* ip);		// MAG add 221006
	int DeleteUnit(size_t unitIndex);	// MAG add 220419
	int DeleteUnit(HTREEITEM unitIndex);	// MAG add 220419
	
	CString MakeStateLabel(int stateIndex, int ioType, int ioIndex);	// get state label to be used for an input or output

	void BuildTypeTree(CTreeCtrl *tree);
	void BuildUnitTree(CTreeCtrl *tree);
	void BuildStateTree(CTreeCtrl *tree);
	void MinimizeState(void);

	void BuildState();
	int AddToState(HVUnit *newUnit);
	CString BuildStateTreeLabel(StateElement *);
	CString BuildUnitTreeLabel(std::vector<HVUnit>::iterator tree, int iop, size_t index);
	CString BuildUnitTreeLabel(HVUnit *tree, int iop, size_t index);

	int FindUnitTreePos(HTREEITEM hti);
	int FindUnitTreeArrPos(HTREEITEM hti);
	int FindTypeTreePos(HTREEITEM hti);
	int FindStateTreePos(HTREEITEM hti);
	int FindStateTreeCategory(HTREEITEM hti);
	int ChangeUnitReporting(HTREEITEM hti);
	int ChangeUnitValue(HTREEITEM hti);
	int ChangeStateValue(HTREEITEM hti);
	int MoveStateValue(HTREEITEM hti); // MAG 211012 move I/O to new state value using keyboard

	int ShowTypeTreeIndex(size_t index);
	std::string	GetCategoryLabel(size_t index);
	std::string GetTypeLabel(int typeID);
	std::string GetSimName() { return simName; };
	std::string GetTypesName() { return typeName; };
	void SetTypesFileName(char *);
	void SetTypesFileName(std::string ips) { typeName = ips; };

	StateElement *StateVec(int vecid);	// input- position in absolute state vector. Output- pointer to correct StateElement
	StateElement* StateVec(size_t vecid);	// input- position in absolute state vector. Output- pointer to correct StateElement
	StateElement* StateVec(int typeID, int vecID);	// input- position by type and position in type vector. Output- pointer to correct StateElement

	int StateVecSetValue(size_t vecid, double val);	// input- position in absolute state vector. Output- pointer to correct StateElement
	int StateVecSetValue(size_t cat, size_t index, double val);	// cat- category; index- category index; val- value to set
	int StateVecSetBoundary(size_t vecid, int val);	// input- position in absolute state vector. Output- pointer to correct StateElement
	int StateVecSetReported(size_t vecid, size_t sb, int val);	// input- position in absolute state vector. Output- pointer to correct StateElement
	int StateVecSetBoundary(size_t cat, size_t index, int val);	// input- category, and position in category. Output- pointer to correct StateElement
	int StateVecSetReported(size_t cat, size_t index, size_t sb, int val);	// input- category, and position in category. Output- pointer to correct StateElement
	int StateVecAddTimeValue(size_t vecid, double val);
	int StateVecAddTimeValue(size_t vecid, int timeStep, double val, size_t supB);
	int StateVecClearTimeValue(size_t vecid);
	void StateVecSetCategoryReporting(size_t cat, int status);	// MAG 200722 set entire category reporting value
	HVType *GetTypeRef(int typeID);
	HVUnit *GetUnitRef(size_t untID);	// NOTE input is unit array index, NOT unit ID.
	int GetUnitFromState(HTREEITEM hti);
	HTREEITEM GetStateLinkFromUnitLink(HTREEITEM unitID);
	//int GetStateIDFromUnitLink(HTREEITEM unitID);

	const size_t GetTypesCount() { return types.size(); };
	const size_t GetUnitsCount() { return units.size(); };
	const size_t GetStateCount() { return state.size(); };  // MAG note- this is worthless.
	const size_t GetCategoryCount(size_t i) {
		if (i < 0) return 0; if (i > state.size()) return 0; return(maxCat[i]);
	};
	int InsertState(size_t category, size_t indexAt);
	int InsertState(HTREEITEM hti);  // finds clicked state item, calls other InsertState

	// Increment SuperBlock or Block
	int BumpSuperBlocks(int unitIndex);
	int BumpSuperBlocks(HTREEITEM unitIndex);
	int BumpBlocks(int unitIndex);
	int BumpBlocks(HTREEITEM unitIndex);

	// Decrement SuperBlock or Block
	int DecrSuperBlocks(int unitIndex);
	int DecrSuperBlocks(HTREEITEM unitIndex);
	int DecrBlocks(int unitIndex);
	int DecrBlocks(HTREEITEM unitIndex);

	int InsertUnit(int type, size_t position, size_t mySBlock=1, size_t myBlock=1);	// MAG 191218 add function
	int InsertUnit(HTREEITEM hti);	// MAG 200224 add function
	const int GetTypeFromIndex(int i) {
		if (i < 0) return 0; if (i >= types.size()) return 0; return types[i].GetType();
	};
	int GetTypeFileParameterCount(int type);
	// MAG 200225 Lets user drag State Input or Output to a new location. dragItem must be an input or output. dragDest could be any place on the state tree.
	int DragState(HTREEITEM dragItem, HTREEITEM dragDest);

	void SetSavedState(int newState) { unsavedChanges = newState > 0 ? 1 : 0; };
	int GetSavedState() { return unsavedChanges; };
	size_t GetUnitSBlock(size_t i);
	HVUnit* GetUnitByID(size_t unitID);

	int ASCIIoffset(char inChar, int shiftOn);  // returns the OFFSET from the key pressed to the ascii index for the character when the SHIFT key is pressed. -32 for a to A

// Implementation

	DECLARE_MESSAGE_MAP()
protected:
	int DoBGStuff(void);				// copied from FDDEA
	std::vector<HVType>			types;	// definitions from typar.dat
	std::vector<HVUnit>			units;	// instantiated in the sim file
	
	// MAG NOTE- state array is reflective of units array, it is not authoratative. If changes are made update units and then rebuild state.
	std::vector<std::vector<StateElement>>	state;	// array of state vectors, one for each type.  filled when sim file is read. NOTE- sized with one extra element to use 1-8, not 0-7
	
	std::vector<std::string>	categoryLabels;

	std::string					simName; // MAG 190926 saved in ReadSimFile
	std::string					typeName; // MAG 191205

	char *FixString(char *ips);	// removes ' and spaces before and after for text labels

	size_t maxCat[9] = { 0 };	// holds maximum index used for each category NOTE use <= in for loops

	int unsavedChanges;	// MAG 200421 boolean to warn user when closing

	// MAG 190926 add below to store data on remainder of work file, to support writing full siminfo streaming
	// MAG 2004078 comment out, use state array indication of boundary insted
	//std::vector<int>			boundaryVars; // list of boundary variables, value is index in state, read in ReadSimFile

	std::vector<std::vector<int>>	reportedVars;// read in ReadSimFile, first index is superblock, second index is just count. Value is absolute index in state array, not type or category NOTE-zero based, SB 1 is at [0]
	std::vector<int>				reportedInterval;// read in ReadSimFile, one element per superblock MAG 210510 change from double to int
	std::vector<int>				frozenVars1;// lines after reported vars in sim work file- read in ReadSimFile
	std::vector<int>				frozenVars2;// read in ReadSimFile

//	std::vector<int>			ReportChainStateIndexes;
public:
	int ReadReportChain(mxml myxml);
};

extern CSimInfoApp theApp;

#endif // DEF_SIMINFO_MAIN