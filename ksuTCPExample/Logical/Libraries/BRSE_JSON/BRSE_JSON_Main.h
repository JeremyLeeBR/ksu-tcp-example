/*! \file BRSE_JSON_Main.h */
/*!
	\mainpage BRSE_JSON
	
	\section Introduction
	\brief This library provides the ability to use JSON to write and read variables using normal ASCII string communication.
	
	
	\section Usage
	The library contains two function blocks \ref JSON_Parse and \ref JSON_Stringify.
	
	\ref JSON_Parse takes a JSON string created by \ref JSON_Stringify and interprets it.
	It searches for the variables and updates their values on the PLC.
	
	\ref JSON_Stringify takes an address to a string containing a variable or structure name and creates a JSON string.
	The JSON string contains all the necessary data about the structure or variable so that a parser can reconstruct it.
	It uses a basic JSON structure: {name:content}
	
	An example of a string is the following:
	
	{"PV_var1":[{"Outer1":[{"Middle2":"1234","Middle1":[{"Inner1":"90","Inner2":"22","Inner3":"33"}]}]"Outer2":"567"}]}
	
	The JSON string describes the following structure:
	 
	PV_var1 \n
	...=>Outer1 \n
	......=>Middle2 1234 \n
	......=>Middle1 \n
	.........=>Inner1 90 \n
	.........=>Inner2 22 \n
	.........=>Inner3 33 \n
	...=>Outer2 567
*/

#include "BRSE_JSON.h"
#include <asstring.h>
#include <string.h> /*this will override the asstring functions*/
