/*! \file BRSE_JSON.c */
/********************************************************************
 * COPYRIGHT -- B&R
 ********************************************************************
 * Library: BRSE_JSON
 * File: BRSE_JSON.c
 * Author: Florian Damerow
 * Created: March 09, 2011
 *******************************************************************/

#include <bur/plctypes.h>
#include "BRSE_JSON.h"
#include <AsBrStr.h>
#include <string.h>
#include "BRSE_JSON_Main.h"
STRING tmpVal[80];

UDINT ValToString(UDINT pv_adresse, UINT pvType){	
	switch (pvType) { 
		case 1: {
				BOOL *var = (BOOL *)pv_adresse;
				brsitoa((BOOL)*var,(UDINT)&tmpVal);
				break;
				}
		case 2: {
				SINT *var = (SINT *)pv_adresse;
				brsitoa((SINT)*var,(UDINT)&tmpVal);
				break; 
				}
		case 3: {
				INT *var = (INT *)pv_adresse;
				brsitoa((INT)*var,(UDINT)&tmpVal);
				break; 
				}
		case 4: {
				DINT *var = (DINT *)pv_adresse;
				brsitoa((DINT)*var,(UDINT)&tmpVal);
				break; 
				}
		case 5: {
				USINT *var = (USINT *)pv_adresse;
				brsitoa((USINT)*var,(UDINT)&tmpVal);
				break; 
				}
		case 6: {
				UINT *var = (UINT *)pv_adresse;
				brsitoa((UINT)*var,(UDINT)&tmpVal);
				break; 
				}
		case 7: {
				UDINT *var = (UDINT *)pv_adresse;
				brsitoa((UDINT)*var,(UDINT)&tmpVal);
				break; 
				}
		case 8: {
				REAL *var = (REAL *)pv_adresse;
				brsftoa((REAL)*var,(UDINT)&tmpVal);
				break; 
				}
		case 9: {
				STRING *var = (STRING *)pv_adresse;
				STRING *tmpBuffer = (STRING *)&tmpVal;
				strcpy(tmpVal,"\"");
				tmpBuffer++;
				strncpy(tmpBuffer,var,(sizeof(tmpVal)-3));
				strcat(tmpVal,"\"");
				break;
				}
		case 11:{
				DINT *var = (DINT *)pv_adresse;
				STRING *tmpBuffer = (STRING *)&tmpVal;
				strcpy(tmpVal,"\"");
				tmpBuffer++;
				brsitoa((DINT)*var,(UDINT)tmpBuffer);
				strcat(tmpVal,"\"");
				break; 
				}	
		case 12:{ 
				DINT *var = (DINT *)pv_adresse;
				STRING *tmpBuffer = (STRING *)&tmpVal;
				strcpy(tmpVal,"\"");
				tmpBuffer++;
				brsitoa((DINT)*var,(UDINT)tmpBuffer);
				strcat(tmpVal,"\"");
				break; 
				}
		case 14:{
				LREAL *var = (LREAL *)pv_adresse;
				brsftoa((LREAL)*var,(UDINT)&tmpVal);
				break;
			}
		/*case 11:{
				TIME *var = pv_adresse;
				STRING *ptmpValue = &tmpValue;
				strcpy(ptmpValue,"\\/Date(");
				ptmpValue = ptmpValue + 7;		
				itoa((DINT)*var,ptmpValue);
				strcat(ptmpValue,")\\/");
				break;
				}
		case 11:{
				DATE_AND_TIME *var = pv_adresse;
				BRSE_DTF_DateTimeFormat_typ format;
				UDINT error;
				error = BRSE_DTF_CreateFormat("YYYY-MM-DD-hh:mm:ss", &format);
				if(error == 0){
					BRSE_DTF_DTToString((DATE_AND_TIME)*var, &format, tmpValue);
				}
				break;
				} 
		case 12:{ 
				TIME *var = pv_adresse;
				STRING *ptmpValue = &tmpValue;
				strcpy(ptmpValue,"\\/Time(");
				ptmpValue = ptmpValue + 7;		
				itoa((DINT)*var,ptmpValue);
				strcat(ptmpValue,")\\/");
				break;
				}*/
		default: strcpy(tmpVal,"null");
				 break; 
	}
	return (UDINT)tmpVal;
}


/*! \interface JSON_Parse
	\brief parses a JSON message and writes the content to the structure on the PLC
	
	This function block parses the JSON message and writes the content to the structure on the PLC using PV_xgetadr.\n
	The structure must exist.\n
	\n
	It is possible to parse a long JSON message in several pieces. The message can be split at every point.\n
	It is only possible to parse JSON Objects '{name:content}' \n
	arrays or non objects are NOT supported e.g. '[name:content, name2:content2]' or 'name:content' \n
	\n
	The maximal possible struture name is defined in MAX_STRUCT_NAME_LENGTH_Parse. \n
	That means the whole name e.g. 'Structure.Outer[1].Inner.Test' is limited. \n
	\n
	Entering an object will result in going up with the level by 1 \n
	Entering an array will result in going up with the level by 1 \n
	Entering an struct array will result in going up with the level by 2 \n
	\n
	If pDebug != 0 the whole message will be written there. This will be done with the state for every character.\n
	If pDebug = 0 no Debugging will be performed by the system.\n
	
	If pVariablesNotFound != 0 all Variables found in the Message, but could not be written on the system are listed here. 
	They are represented in a JSON array e.g. '["var1","var2"]'.\n
	If pVariablesNotFound = 0 no output will performed wen variables could not be written.\n
	\n
	The funktion block will be stopped when an error accured.\n
	The function block will not be stopped when a warning accures.\n
	\par Errors:\n
	\arg 1001	invalid JSON in parsestate 0
	\arg 1002	invalid JSON in parsestate 1
	\arg 1003	invalid JSON in parsestate 3
	\arg 1004	invalid JSON in parsestate 4
	\arg 1005	invalid JSON in parsestate 6
	\arg 1006	invalid JSON in parsestate 7
	\arg 1007	Structure depth is deeper than MAX_STRUCT_DEPTH_Parse
	
	\par Warnings:\n
	\arg 3001	data type not supported
	\arg 3002	variable not found
	
	\par General: \n
	\arg 2001	parsing so far successful, but end of JSON message net reached yet
	\arg 0		successfuly finished parsing
	
	\param[in] pMessage Pointer to the JSON Message (String)
	\param[in] pDebug Pointer Debug Buffer (String)
	\param[in] SizeDebug size of the Debug Buffer
	\param[in] pVariablesNotFound Pointer Buffer of VariablesNotFound (String)
	\param[in] SizeVariablesNotFound size of the VariablesNotFound Buffer
	
	\param[out] ErrorInfo contains the Error/Warning/General Codes.
	
*/

void JSON_Parse(struct JSON_Parse* inst)
{
	STRING *message = (STRING *)inst->pMessage;
	STRING *pBuff = (STRING *)inst->pMessage;
	UDINT pBuffOld = 0;
	inst->ErrorInfo = 0;
	while((UDINT)pBuff<((UDINT)message+strlen(message)) && (inst->ErrorInfo < 1000 || inst->ErrorInfo >=2000)){
		switch(inst->parsestate){
			case 0:
				if(*pBuff == '{'){
					inst->parsestate = 1;
					inst->parselevel++;
					pBuff++;
					if(inst->pVariablesNotFound != 0 && inst->SizeVariablesNotFound > 2){
						strcpy((STRING *)inst->pVariablesNotFound,"[");
					}
				}
				else if(*pBuff == ' '){
					pBuff++;
				}
				else{
					inst->ErrorInfo = 1001; /*invalid JSON*/
					return;	
				}
				break;
				
			case 1:
				if(*pBuff == '"'){
					inst->parsestate = 2;
					pBuff++;
				}
				else if(*pBuff == ' '){
					pBuff++;
				}
				else{
					inst->ErrorInfo = 1002; /*invalid JSON*/
					return;	
				}
				break;
			
			case 2:
				if(*pBuff != '"'){
					inst->pvName[inst->parselevel][inst->j] = *pBuff;
					inst->j++;
					pBuff++;
				}
				else {
					inst->pvName[inst->parselevel][inst->j] = '\0';
					inst->j = 0;
					inst->parsestate = 3;
					pBuff++;	
				}
				break;
				
			case 3:
				if(*pBuff == ':'){
					inst->parsestate = 4;
					pBuff++;
				}
				else if(*pBuff == ' '){
					pBuff++;
				}
				else{
					inst->ErrorInfo = 1003; /*invalid JSON*/	
					return;
				}
				break;
				
			case 4:
				if(*pBuff == '{'){
					inst->parsestate = 1;
					inst->parselevel++;
					pBuff++;
				}
				else if(*pBuff == '['){
					/*strcat(inst->pvName[inst->parselevel],"[");
					strcat(inst->pvName[inst->parselevel],"0");
					strcat(inst->pvName[inst->parselevel],"]");*/
					inst->isArray[inst->parselevel] = 1;
					inst->parselevel++;
					pBuff++;
				}
				else if(*pBuff == '\"'){
					inst->Buffer[inst->j] = *pBuff;
					pBuff++;
					inst->j++;
					inst->parsestate = 41;
				}
				else if(*pBuff != '{' && *pBuff != ' ' && *pBuff != '['){ /*maybe better to check for " or - or number*/
					inst->parsestate = 5;
				}
				else if(*pBuff == ' '){
					pBuff++;
				}
				else{
					inst->ErrorInfo = 1004; /*invalid JSON*/
					return;	
				}
				break;
			
			case 41: /* is in String */
				if(*pBuff == '\"'){
					inst->Buffer[inst->j] = *pBuff;
					inst->j++;
					inst->Buffer[inst->j] = '\0';
					inst->j=0;
					pBuff++;
					inst->parsestate = 51;
				}
				else if(*pBuff == '\\'){
					inst->Buffer[inst->j] = *pBuff;
					inst->j++;
					pBuff++;
					inst->Buffer[inst->j] = *pBuff;
					inst->j++;
					pBuff++;
				}
				else{
					inst->Buffer[inst->j] = *pBuff;
					inst->j++;
					pBuff++;
				}
				break;
				
			case 5:
				if(*pBuff != '}' && *pBuff != ' ' && *pBuff != ',' && *pBuff != ']'){
					inst->Buffer[inst->j] = *pBuff;
					inst->j++;
					pBuff++;
				}
				else{
					inst->Buffer[inst->j] = '\0';
					inst->j=0;
					inst->parsestate = 51;
				}
				break;
				
			case 51: /*write variable*/
				memset((UDINT *)(inst->fullName),0,sizeof(inst->fullName));
				{
					UINT i = 1;
					UDINT adresse;
					UDINT datalen;
					UDINT pvType;
					UDINT pvLength;
					UINT pvStructMembers;
					STRING tmpStr[5];
					while(i <= inst->parselevel && (sizeof(inst->fullName)-strlen(inst->fullName)-7) > strlen(inst->pvName[i])){
						strcat(inst->fullName,inst->pvName[i]);
						if(inst->isArray[i] == 1){
							strcat(inst->fullName,"[");
							brsitoa(inst->pvIndex[i],(UDINT)&tmpStr);
							strcat(inst->fullName,tmpStr);
							strcat(inst->fullName,"]");
						}
						if(i < inst->parselevel && inst->isArray[i] == 0){
							strcat(inst->fullName,".");
						}
						i++;
					}
					inst->ErrorInfo = PV_ninfo((STRING *)&inst->fullName, &pvType, &pvLength, &pvStructMembers);
					if(inst->ErrorInfo == 0){
						inst->ErrorInfo = PV_xgetadr((STRING *)&inst->fullName, &adresse, &datalen);
					}
					else{
						inst->ErrorInfo = 3002; /* variable not found */
					}
					if(inst->ErrorInfo == 0){ 
						STRING *Buffer = (STRING *)&inst->Buffer;
						if(*Buffer == '\"'){
							Buffer++;
							if(inst->Buffer[strlen(Buffer)] == '\"'){
								inst->Buffer[strlen(Buffer)] = '\0';
							}
						}
						if(strstr(inst->Buffer,"null") == 0){
							switch(pvType){
								case 0:{
									DINT *adr = (DINT *)adresse;
									*adr = (DINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 1:{
									BOOL *adr = (BOOL *)adresse;
									if(brsatoi((UDINT)Buffer) != 0){
										*adr = 1;
									}
									else{
										*adr = 0;
									}
									break;
									}
								case 2:{
									SINT *adr = (SINT *)adresse;
									*adr = (SINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 3:{
									INT *adr = (INT *)adresse;
									*adr = (INT)brsatoi((UDINT)Buffer);
									break;
									}
								case 4:{
									DINT *adr = (DINT *)adresse;
									*adr = (DINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 5:{
									USINT *adr = (USINT *)adresse;
									*adr = (USINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 6:{
									UINT *adr = (UINT *)adresse;
									*adr = (UINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 7:{
									UDINT *adr = (UDINT *)adresse;
									*adr = (UDINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 8:{
									REAL *adr = (REAL *)adresse;
									*adr = (REAL)brsatof((UDINT)Buffer);
									break;
									}
								case 9:{
									STRING *adr = (STRING *)adresse;
									strncpy(adr, Buffer, pvLength-1);
									break;
									}
								case 11:{
									DINT *adr = (DINT *)adresse;
									*adr = (DINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 12:{
									DINT *adr = (DINT *)adresse;
									*adr = (DINT)brsatoi((UDINT)Buffer);
									break;
									}
								case 14:{
										LREAL *adr = (LREAL *)adresse;
										*adr = (LREAL)brsatof((UDINT)Buffer);
										break;
									}
								/*case 11:{
									DINT *adr = adresse;
									STRING *Buffer = &inst->Buffer;
									Buffer = strstr(inst->Buffer,")\\/");
									if(Buffer != 0){
										*Buffer = '\0';
										Buffer = strstr(inst->Buffer,"\\/Date(");
										if(Buffer != 0){
											Buffer = Buffer + 7;
										}
									}
									*adr = (DINT)atoi(Buffer);
									break;
									}
								case 12:{
									DINT *adr = adresse;
									STRING *Buffer = &inst->Buffer;
									Buffer = strstr(inst->Buffer,")\\/");
									if(Buffer != 0){
										*Buffer = '\0';
										Buffer = strstr(inst->Buffer,"\\/Time(");
										if(Buffer != 0){
											Buffer = Buffer + 7;
										}
									}
									*adr = (DINT)atoi(Buffer);
									break;
									}*/
								
								default:{
									inst->ErrorInfo = 3001;
									break;
									}
							}
						}
					}
					else{
						inst->ErrorInfo = 3002; /* variable not found */
						if(inst->pVariablesNotFound != 0 && inst->SizeVariablesNotFound != 0){
							if((inst->SizeVariablesNotFound - strlen((STRING *)inst->pVariablesNotFound)-2) > strlen(inst->fullName)+3){
								if(strlen((STRING *)inst->pVariablesNotFound) > 2){
									strcat((STRING *)inst->pVariablesNotFound,",");
								}
								strcat((STRING *)inst->pVariablesNotFound,"\"");
								strcat((STRING *)inst->pVariablesNotFound,inst->fullName);
								strcat((STRING *)inst->pVariablesNotFound,"\"");
							}
						}
					}
					if(*pBuff == ',' && inst->isArray[inst->parselevel-1] == 1){
						pBuff++;
						inst->pvIndex[inst->parselevel-1]++;
						inst->parsestate = 4;
					}
					else{
						inst->parsestate = 6;
					}
				}
				break;
				
			case 6:
				if(*pBuff == '}'){
					inst->parsestate = 6;
					inst->pvIndex[inst->parselevel-1] = 0;
					inst->parselevel--;
					pBuff++;
				}
				else if(*pBuff == ']'){
					inst->parsestate = 6;
					inst->pvIndex[inst->parselevel-1] = 0;
					inst->parselevel--;
					inst->isArray[inst->parselevel] = 0;
					pBuff++;
				}
				else if(*pBuff == ','){
					inst->parsestate = 7;
					inst->pvIndex[inst->parselevel-1]++;
					pBuff++;
				}
				else if(*pBuff == ' '){
					pBuff++;
				}
				else{
					inst->ErrorInfo = 1005; /*invalid JSON*/	
					return;
				}
				break;
				
			case 7:
				if(*pBuff == '{'){
					inst->parselevel++;
					inst->parsestate = 1;
					pBuff++;
				}
				else if(*pBuff == '\"'){
					inst->parsestate = 1;
				}
				else if(*pBuff == ' '){
					pBuff++;
				}
				else{
					inst->ErrorInfo = 1006; /*invalid JSON*/	
					return;
				}
		}
		
		/* DEBUGGING */
		if(inst->pDebug != 0){
			if(pBuffOld != (UDINT)pBuff){
				STRING BUFFER2[4];
				STRING BUFFER3[2];
				if((inst->SizeDebug - strlen((STRING *)inst->pDebug) - 1) > 7){
					if(inst->parsestateold != inst->parsestate){
						inst->parsestateold = inst->parsestate;
						brsitoa((DINT)inst->parsestate,(UDINT)&BUFFER2);
						strcat((STRING *)inst->pDebug,"(");
						strcat((STRING *)inst->pDebug,BUFFER2);
						strcat((STRING *)inst->pDebug,")");
					}
					BUFFER3[0] = *pBuff;
					BUFFER3[1] = 0;
					strcat((STRING *)inst->pDebug,BUFFER3);
				}
			}
			pBuffOld = (UDINT)pBuff;
		}
	}
	/* Error Handling */
	if(inst->parselevel > MAX_STRUCT_DEPTH_Parse){
		inst->ErrorInfo = 1007;
	}
	if(inst->ErrorInfo >= 1000 && inst->ErrorInfo  < 2000){
		inst->parselevel = 0;
		inst->parsestate = 0;
		memset(inst->pvIndex,0,sizeof(inst->pvIndex));
		memset(inst->pvName,0,sizeof(inst->pvName));
	}
	else if(inst->parselevel != 0){
		inst->ErrorInfo = 2001; /*not done*/
	}
	else{
		if(inst->pVariablesNotFound != 0){
			strcat((STRING *)inst->pVariablesNotFound,"]");
		}
		inst->parsestate = 0;
	}
}

/*! \interface JSON_Stringify
	\brief converts a PV variable and the whole structure into a JSON Object using PV_ninfo.
	
	This function block converts the whole structure of a PV variable into a JSON Object using PV_ninfo. \n
	The variable name should be the same as using the PV_ninfo function. By that it is possible to access any kind of PV variables.\n
	The output is always a JSON object. e.g. '{"varname": ... }' \n
	\n
	The function block has to be run several times until the Status = 1. \n
	Every time the function block is called it creates the JSON output for one new variable inside the Structure. \n
	If the variable in the structure is an array of a non object type the whole array will be created in Output.\n
	\n
	The maximum Output length is defined by MAX_OUTPUT_LENGTH_Stringify.\n
	The maximum Value of a nun array, non structure variable is 80. So Strings are only supported up the 80 characters.\n
	
	
	\par Status Codes:\n
	\arg 1001 Variable not found
	\arg 1002 Variable value too long
	\arg 1 singify process completed
	\arg 0 singify process not completet yet - run again
	
	\param[in] pStName Pointer to the requested variable name
	
	\param[out] Status returns the Status codes
	\param[out] Output contains the content of one step
	
*/
void JSON_Stringify(struct JSON_Stringify* inst)
{	
	STRING *pBuff = 0;
	STRING strBuff[20];
	inst->TaskDoc.Input.StructDepth = MAX_STRUCT_DEPTH_Stringify;	
	/*gather information of the structure item*/
	memset(inst->tmpStruct,0,sizeof(inst->tmpStruct));
	strcpy(inst->tmpStruct,(STRING *)inst->pStName); 
	
	strcat(inst->tmpStruct, inst->pvName[0]); 
	for( inst->i = 1; inst->i <= inst->pvLevel; inst->i++ ){ 
		if(inst->isArray[inst->i-1] == 0){
			strcat(inst->tmpStruct, ".");
			strcat(inst->tmpStruct, inst->pvName[inst->i]); 
		}
		else{
			inst->isArray[inst->pvLevel] = 1;
			strcat(inst->tmpStruct,"[");
			brsitoa(inst->pvIndex[inst->i-1],(UDINT)&strBuff);
			strcat(inst->tmpStruct,strBuff);
			strcat(inst->tmpStruct,"]");
		}
	}           
	/*Search in PV-list*/ 
	inst->TaskDoc.Output.StatusInfo = PV_ninfo((STRING *)&inst->tmpStruct, &inst->pvType, &inst->pvLength, &inst->pvStructMembers[inst->pvLevel]); 
	if(inst->pvType == 0){
		STRING name[80];
		inst->TaskDoc.Output.StatusSearch = PV_item((STRING *)&inst->tmpStruct, 0, (STRING *)&name);
		if(inst->TaskDoc.Output.StatusSearch != 0){
			inst->pvType = 4;
			inst->pvStructMembers[inst->pvLevel] = 1;
			inst->TaskDoc.Output.StatusSearch = 0;
		}
	}
	if(inst->pvType == 15){
		inst->isArray[inst->pvLevel] = 1;
		strcat(inst->tmpStruct,"[");
		brsitoa(inst->pvIndex[inst->pvLevel],(UDINT)&strBuff);
		strcat(inst->tmpStruct,strBuff);
		strcat(inst->tmpStruct,"]");
		inst->pvLevel = inst->pvLevel + 1;
		inst->TaskDoc.Output.StatusInfo = PV_ninfo((STRING *)&inst->tmpStruct, &inst->pvType, &inst->pvLength, &inst->pvStructMembers[inst->pvLevel]); 
	}
	else{
		inst->isArray[inst->pvLevel] = 0;
	}
	if( inst->TaskDoc.Output.StatusInfo == 0 ){
		inst->TaskDoc.Output.StatusInfo = PV_xgetadr((STRING *)&inst->tmpStruct,&inst->pv_adresse,&inst->data_len);
	}
	if( inst->TaskDoc.Output.StatusInfo == 0 ){ 
	/*variable was found*/ 
		STRING *ValToStr;
		UDINT Size=0;
		memset(inst->tmpValue,0,sizeof(inst->tmpValue));
		if(inst->pvStructMembers[inst->pvLevel]>1){
			strcat(inst->tmpValue,"[");
		}
		ValToStr = (STRING *)ValToString(inst->pv_adresse, inst->pvType);
		if((sizeof(inst->tmpValue)-strlen(inst->tmpValue)-2) > strlen(ValToStr)){
			strcat(inst->tmpValue, ValToStr);
		}
		inst->arrIndex = 1;
		Size = (UDINT)(inst->data_len/inst->pvStructMembers[inst->pvLevel]);
		pBuff = (STRING *)((UDINT)inst->pv_adresse+Size);
		/*build array output */
		while(inst->arrIndex < inst->pvStructMembers[inst->pvLevel]){
			ValToStr = (STRING *)ValToString((UDINT)pBuff, inst->pvType);
			if((sizeof(inst->tmpValue)-strlen(inst->tmpValue)-2) <= strlen(ValToStr)){
				inst->Status = 1002; /*Variable Value too long*/
				return;
			}
			strcat(inst->tmpValue, ",");
			strcat(inst->tmpValue, ValToStr);
			pBuff = pBuff+Size;
			inst->arrIndex++;
		}
		if(inst->pvStructMembers[inst->pvLevel]>1){
			strcat(inst->tmpValue,"]");
		}
		
		/*get inner variable name*/
		pBuff = (inst->tmpStruct)+strlen(inst->tmpStruct);
		while((UDINT)pBuff >= (UDINT)&(inst->tmpStruct) && *pBuff != '.'){
			pBuff--;
		}
		pBuff++;
		/*create JSON output*/
		memset(inst->Output,0,sizeof(inst->Output));
		if((43+strlen(inst->tmpValue)) > sizeof(inst->Output)){
			inst->Status = 1002; /*Variable Value too long*/
			return;
		}
		if(!(inst->isArray[inst->pvLevel-1] == 1 && inst->pvIndex[inst->pvLevel-1] > 0)){
			if((inst->pvLevel == 0 && inst->pvIndex[0] == 0) || (inst->isArray[inst->pvLevel-1] == 1 && (inst->pvLevel-1) == 0) ){
				strcat(inst->Output, "{");
				
			}
			strcat(inst->Output, "\"");
			if( inst->pvLevel == 0){
				strcat(inst->Output, (STRING *)inst->pStName);
			}
			else if(inst->isArray[inst->pvLevel-1] == 1){
				if((inst->pvLevel-1) > 0){
					strcat(inst->Output, inst->pvName[inst->pvLevel-1]);
				}
				else{
					strcat(inst->Output, (STRING *)inst->pStName);
				}
			}else{
				strcat(inst->Output, inst->pvName[inst->pvLevel]);
			}
			strcat(inst->Output, "\":");
			if(inst->isArray[inst->pvLevel-1]){
				strcat(inst->Output, "[");
			}
		}
		if( inst->pvType == 0 ){
			strcat(inst->Output, "{");
		}
		if( inst->pvType != 0 && inst->pvType != 15 ){	
			strcat(inst->Output, inst->tmpValue);			
		}
	
		inst->pvSearchComplete = 0; 

		if( (inst->pvType == 0) && (inst->pvLevel < (inst->TaskDoc.Input.StructDepth-1)) ){ 
			/*variable is a structure and its ok to go look inside it*/ 
			/*ok. proceed to next level.*/ 
			inst->TaskDoc.Output.StatusSearch = PV_item((STRING *)&inst->tmpStruct, inst->pvIndex[inst->pvLevel], (STRING *)&inst->pvName[inst->pvLevel+1]); 
			inst->pvLevel = inst->pvLevel + 1; 
		}
		else if( (inst->pvLevel > 0) || (inst->pvLevel==0 && inst->pvIndex[inst->pvLevel] < (inst->pvStructMembers[inst->pvLevel]))){ /*..in case its 0 we have a problem*/ 
		/*variable is a basic datatype*/ 
			do{ 
				inst->pvContinue = 0;
				if( inst->isArray[inst->pvLevel-1] == 1){
					if( inst->pvIndex[inst->pvLevel-1] < (inst->pvStructMembers[inst->pvLevel-1]-1)){ 
						/*ok. take the next item in prior structure*/ 
						inst->pvIndex[inst->pvLevel-1] = inst->pvIndex[inst->pvLevel-1] + 1;
						inst->pvLevel = inst->pvLevel - 1;
						strcat(inst->Output, ",");
						break; /*..from loop*/ 
					}
					else{
						strcat(inst->Output, "]");
						inst->pvLevel = inst->pvLevel - 1;
						inst->pvIndex[inst->pvLevel] = 0;
						if( inst->pvLevel > 0 ){ 
							inst->pvContinue = 1; 
						}else{ 
							inst->pvSearchComplete = 1; 
							strcat(inst->Output, "}");
							break; 
						}
					}	
				}
				else{
					if( inst->pvIndex[inst->pvLevel-1] < (inst->pvStructMembers[inst->pvLevel-1]-1)){ 
						/*ok. take the next item in prior structure*/ 
						inst->pvIndex[inst->pvLevel-1] = inst->pvIndex[inst->pvLevel-1] + 1;
						strcat(inst->Output, ",");
						break; /*..from loop*/ 
					}else{ 
						/*continue looking in the prior structure if the index is smaller members*/ 			
						inst->pvLevel = inst->pvLevel - 1;
						if(inst->pvLevel>=0){
							strcat(inst->Output, "}");
						}
						inst->pvIndex[inst->pvLevel] = 0; /*this one is used the nest time a structure is found on this level*/ 
						if( inst->pvLevel > 0 ){ 
							inst->pvContinue = 1; 
						}else{ 
							inst->pvSearchComplete = 1; 
							strcat(inst->Output, "}");
							break; 
						}
					}
				}
			}
			while(inst->pvContinue != 0); 

			/*look after following member in prior structure*/ 
			if( inst->pvLevel > 0 ){ 
				strcpy(inst->tmpStruct,(STRING *)inst->pStName); 

				strcat(inst->tmpStruct, inst->pvName[0]); 
				for( inst->i = 1; inst->i <= (inst->pvLevel-1); inst->i++ ){ 
					if(strlen(inst->pvName[inst->i]) != 0){
						strcat(inst->tmpStruct, ".");
						strcat(inst->tmpStruct, inst->pvName[inst->i]); 
					}
				} 
				/*ok. get the next variable*/ 
				inst->TaskDoc.Output.StatusSearch = PV_item((STRING *)&inst->tmpStruct, inst->pvIndex[inst->pvLevel-1], (STRING *)&inst->pvName[inst->pvLevel]); 
			}else if((inst->pvLevel==0 && inst->pvIndex[inst->pvLevel] == (inst->pvStructMembers[inst->pvLevel]))){ 
				/*no more items in top structure*/ 
				inst->pvSearchComplete = 1;
			} 
		}else{ 
			inst->pvSearchComplete = 1; 
		}
		if(inst->pvSearchComplete == 1){
			inst->Status = 1;
		}else{
			inst->Status = 0;
		}
	}else{ 
		 inst->Status = 1001; /*variable not found*/
	}
}


