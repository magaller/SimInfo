#pragma once

#ifndef DEF_TRNS_TYPES
#define DEF_TRNS_TYPES

#include <string>
#include <vector>

// MAG 230322
// Create classes to support reading in and displaying TRNS deck files.
// Goal is to enables state-vector like functionality in GUI to allow tracing input/output paths
// and to export in format like XML SI files
// NOTE- this work is still in progress


class TRNS_Unit
{
public: 
	TRNS_Unit();
private:
	// MAG 230322 TODO add code to use this like a class
	int	unitID; // Unit ##
	int unitType;
	// these lines begin with *$
	std::string unit_name;
	std::string unit_model;
	std::string unit_position;
	std::string unit_layer;
	std::string unit_comment; // some have an extra line of text before the parameters
	
	std::vector<double>			param_values;
	std::vector<std::string>	param_text; // text after comment on each parameter value line, optional?
	std::vector<int[2]>			input_link;	// either 0,0 for none or unit#,output#. -1,-1 if equation
	
	// *** INITIAL INPUT VALUES section
	std::vector<std::string>	input_equation;
	std::vector<std::string>	input_comment; // text after comment on each input line, optional?
	std::vector<int>			input_type; // 0 == link, 1 == equation
	std::vector<double>			input_values; // initial values
	// undetermined- initial input values which are listed by input name, see unit 9 in example

	// LABELS
	std::vector<std::string>	unit_labels; // entire content of line including quotes

	// *** External files section
	std::vector<std::string>	external_files_name;	// content in quotes on ASSIGN line
	std::vector<int>			external_files_id;		// content after quotes on ASSIGN line
	std::vector<std::string>	external_files_comment; // content from line after ASSIGN line

	std::vector<int>			derivative_value;
	std::vector<std::string>	derivative_label;
};


class TRNS_Equation
{
//public:
//	TRNS_Equation();
private:
	// MAG 230322 TODO add code to use this like a class
	std::string					eq_label; // text after "* EQUATIONS"

	//int							eq_count;
	std::vector<std::string>	eq_string;  // count will be determined by length of eq_string

	// these lines begin with *$
	std::string			eq_name;
	std::string			eq_layer;
	std::string			eq_position;
	std::string			eq_unit_number;  // if this is used this may be better as an int
	std::vector<int[2]>	eq_link;	// list of unitID,outputID referenced in eq_string
	std::vector<std::string>	eq_refeq;	// list of other equations referenced in this equation, by matching eq_name
};


class TRNS_Deck
{
public:
	TRNS_Deck();
private:
	// MAG 230322 TODO add code to use this like a class
	int trns_version;

	std::vector<std::string> control_cards; // whatever this is...
	std::vector<std::string> start_stop_step; // entire content of lines in this section
	std::vector<std::string> user_constants; // entire content of lines in this section, demarcated by *$USER_CONSTANTS text

	std::vector<TRNS_Unit>	units;	// list of units defined in deck file

	std::vector<TRNS_Equation>	equations; // list of equations defined in deck file
};

#endif // DEF_TRNS_TYPES