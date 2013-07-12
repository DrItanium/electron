   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.30  02/28/08            */
   /*                                                     */
   /*               STRING FUNCTIONS MODULE               */
   /*******************************************************/

/*************************************************************/
/* Purpose: Contains the code for several string functions   */
/*   including str-cat, sym-cat, str-length, str-compare,    */
/*   upcase, lowcase, sub-string, str-index, eval, and       */
/*   build.                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Barry Cameron                                        */
/*                                                           */
/* Revision History:                                         */
/*      6.23: Correction for FalseSymbol/TrueSymbol. DR0859  */
/*                                                           */
/*      6.30: Added support for UTF-8 strings to str-length, */
/*            str-index, and sub-string functions.           */
/*                                                           */
/*************************************************************/

#define _STRNGFUN_SOURCE_

#include "setup.h"

#if STRING_FUNCTIONS

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <ctype.h>
#include <string.h>

#include "argacces.h"
#include "commline.h"
#include "constrct.h"
#include "cstrcpsr.h"
#include "engine.h"
#include "envrnmnt.h"
#include "exprnpsr.h"
#include "extnfunc.h"
#include "memalloc.h"
#include "prcdrpsr.h"
#include "router.h"
#include "strngrtr.h"
#include "scanner.h"
#include "sysdep.h"

#if DEFRULE_CONSTRUCT
#include "drive.h"
#endif

#include "strngfun.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                    StrOrSymCatFunction(void *,DATA_OBJECT_PTR,unsigned short);

/******************************************/
/* StringFunctionDefinitions: Initializes */
/*   the string manipulation functions.   */
/******************************************/
globle void StringFunctionDefinitions(
  void *theEnv)
  {
#if ! RUN_TIME
   EnvDefineFunction2(theEnv,(char*)"str-cat", 'k', PTIEF StrCatFunction, (char*)"StrCatFunction", (char*)"1*");
   EnvDefineFunction2(theEnv,(char*)"sym-cat", 'k', PTIEF SymCatFunction, (char*)"SymCatFunction", (char*)"1*");
   EnvDefineFunction2(theEnv,(char*)"str-length", 'g', PTIEF StrLengthFunction, (char*)"StrLengthFunction", (char*)"11j");
   EnvDefineFunction2(theEnv,(char*)"str-compare", 'g', PTIEF StrCompareFunction, (char*)"StrCompareFunction", (char*)"23*jji");
   EnvDefineFunction2(theEnv,(char*)"upcase", 'j', PTIEF UpcaseFunction, (char*)"UpcaseFunction", (char*)"11j");
   EnvDefineFunction2(theEnv,(char*)"lowcase", 'j', PTIEF LowcaseFunction, (char*)"LowcaseFunction", (char*)"11j");
   EnvDefineFunction2(theEnv,(char*)"sub-string", 's', PTIEF SubStringFunction, (char*)"SubStringFunction", (char*)"33*iij");
   EnvDefineFunction2(theEnv,(char*)"str-index", 'u', PTIEF StrIndexFunction, (char*)"StrIndexFunction", (char*)"22j");
   EnvDefineFunction2(theEnv,(char*)"eval", 'u', PTIEF EvalFunction, (char*)"EvalFunction", (char*)"11k");
   EnvDefineFunction2(theEnv,(char*)"build", 'b', PTIEF BuildFunction, (char*)"BuildFunction", (char*)"11k");
   EnvDefineFunction2(theEnv,(char*)"string-to-field", 'u', PTIEF StringToFieldFunction, (char*)"StringToFieldFunction", (char*)"11j");
#else
#endif
  }

/****************************************/
/* StrCatFunction: H/L access routine   */
/*   for the str-cat function.          */
/****************************************/
globle void StrCatFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  {   
   StrOrSymCatFunction(theEnv,returnValue,STRING);
  }

/****************************************/
/* SymCatFunction: H/L access routine   */
/*   for the sym-cat function.          */
/****************************************/
globle void SymCatFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  {
   StrOrSymCatFunction(theEnv,returnValue,SYMBOL);
  }

/********************************************************/
/* StrOrSymCatFunction: Driver routine for implementing */
/*   the str-cat and sym-cat functions.                 */
/********************************************************/
static void StrOrSymCatFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue,
  unsigned short returnType)
  {
   DATA_OBJECT theArg;
   int numArgs, i, total, j;
   char *theString;
   SYMBOL_HN **arrayOfStrings;
   SYMBOL_HN *hashPtr;
   char *functionName;

   /*============================================*/
   /* Determine the calling function name.       */
   /* Store the null string or the symbol nil as */
   /* the return value in the event of an error. */
   /*============================================*/

   SetpType(returnValue,returnType);
   if (returnType == STRING)
     {
      functionName = (char*)"str-cat";
      SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,(char*)""));
     }
   else
     {
      functionName = (char*)"sym-cat";
      SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,(char*)"nil"));
     }

   /*===============================================*/
   /* Determine the number of arguments as create a */
   /* string array which is large enough to store   */
   /* the string representation of each argument.   */
   /*===============================================*/

   numArgs = EnvRtnArgCount(theEnv);
   arrayOfStrings = (SYMBOL_HN **) gm1(theEnv,(int) sizeof(SYMBOL_HN *) * numArgs);
   for (i = 0; i < numArgs; i++)   
     { arrayOfStrings[i] = NULL; }

   /*=============================================*/
   /* Evaluate each argument and store its string */
   /* representation in the string array.         */
   /*=============================================*/

   total = 1;
   for (i = 1 ; i <= numArgs ; i++)
     {
      EnvRtnUnknown(theEnv,i,&theArg);

      switch(GetType(theArg))
        {
         case STRING:
#if OBJECT_SYSTEM
         case INSTANCE_NAME:
#endif
         case SYMBOL:
           hashPtr = (SYMBOL_HN *) GetValue(theArg);
           arrayOfStrings[i-1] = hashPtr;
           IncrementSymbolCount(hashPtr); 
           break;

         case FLOAT:
           hashPtr = (SYMBOL_HN *) EnvAddSymbol(theEnv,FloatToString(theEnv,ValueToDouble(GetValue(theArg))));
           arrayOfStrings[i-1] = hashPtr;
           IncrementSymbolCount(hashPtr);
           break;

         case INTEGER:
           hashPtr = (SYMBOL_HN *) EnvAddSymbol(theEnv,LongIntegerToString(theEnv,ValueToLong(GetValue(theArg))));
           arrayOfStrings[i-1] = hashPtr;
           IncrementSymbolCount(hashPtr);
           break;

         default:
           ExpectedTypeError1(theEnv,functionName,i,(char*)"string, instance name, symbol, float, or integer");
           SetEvaluationError(theEnv,TRUE);
           break;
        }

      if (EvaluationData(theEnv)->EvaluationError)
        {
         for (i = 0; i < numArgs; i++)
           {
            if (arrayOfStrings[i] != NULL)
              { DecrementSymbolCount(theEnv,arrayOfStrings[i]); }
           }

         rm(theEnv,arrayOfStrings,sizeof(SYMBOL_HN *) * numArgs);
         return;
        }

      total += (int) strlen(ValueToString(arrayOfStrings[i - 1]));
     }

   /*=========================================================*/
   /* Allocate the memory to store the concatenated string or */
   /* symbol, then copy the values in the string array to the */
   /* memory just allocated.                                  */
   /*=========================================================*/

   theString = (char *) gm2(theEnv,(sizeof(char) * total));

   j = 0;
   for (i = 0 ; i < numArgs ; i++)
     {
      gensprintf(&theString[j],"%s",ValueToString(arrayOfStrings[i]));
      j += (int) strlen(ValueToString(arrayOfStrings[i]));
     }

   /*=========================================*/
   /* Return the concatenated value and clean */
   /* up the temporary memory used.           */
   /*=========================================*/

   SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,theString));
   rm(theEnv,theString,sizeof(char) * total);

   for (i = 0; i < numArgs; i++)
     {
      if (arrayOfStrings[i] != NULL)
        { DecrementSymbolCount(theEnv,arrayOfStrings[i]); }
     }

   rm(theEnv,arrayOfStrings,sizeof(SYMBOL_HN *) * numArgs);
  }

/*******************************************/
/* StrLengthFunction: H/L access routine   */
/*   for the str-length function.          */
/*******************************************/
globle long long StrLengthFunction(
  void *theEnv)
  {
   DATA_OBJECT theArg;

   /*===================================================*/
   /* Function str-length expects exactly one argument. */
   /*===================================================*/

   if (EnvArgCountCheck(theEnv,(char*)"str-length",EXACTLY,1) == -1)
     { return(-1LL); }

   /*==================================================*/
   /* The argument should be of type symbol or string. */
   /*==================================================*/

   if (EnvArgTypeCheck(theEnv,(char*)"str-length",1,SYMBOL_OR_STRING,&theArg) == FALSE)
     { return(-1LL); }

   /*============================================*/
   /* Return the length of the string or symbol. */
   /*============================================*/
   
   return(UTF8Length(DOToString(theArg)));
  }

/****************************************/
/* UpcaseFunction: H/L access routine   */
/*   for the upcase function.           */
/****************************************/
globle void UpcaseFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  {
   DATA_OBJECT theArg;
   unsigned i;
   size_t slen;
   char *osptr, *nsptr;

   /*===============================================*/
   /* Function upcase expects exactly one argument. */
   /*===============================================*/

   if (EnvArgCountCheck(theEnv,(char*)"upcase",EXACTLY,1) == -1)
     {
      SetpType(returnValue,STRING);
      SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,(char*)""));
      return;
     }

   /*==================================================*/
   /* The argument should be of type symbol or string. */
   /*==================================================*/

   if (EnvArgTypeCheck(theEnv,(char*)"upcase",1,SYMBOL_OR_STRING,&theArg) == FALSE)
     {
      SetpType(returnValue,STRING);
      SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,(char*)""));
      return;
     }

   /*======================================================*/
   /* Allocate temporary memory and then copy the original */
   /* string or symbol to that memory, while uppercasing   */
   /* lower case alphabetic characters.                    */
   /*======================================================*/

   osptr = DOToString(theArg);
   slen = strlen(osptr) + 1;
   nsptr = (char *) gm2(theEnv,slen);

   for (i = 0  ; i < slen ; i++)
     {
      if (islower(osptr[i]))
        { nsptr[i] = (char) toupper(osptr[i]); }
      else
        { nsptr[i] = osptr[i]; }
     }

   /*========================================*/
   /* Return the uppercased string and clean */
   /* up the temporary memory used.          */
   /*========================================*/

   SetpType(returnValue,GetType(theArg));
   SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,nsptr));
   rm(theEnv,nsptr,slen);
  }

/*****************************************/
/* LowcaseFunction: H/L access routine   */
/*   for the lowcase function.           */
/*****************************************/
globle void LowcaseFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  {
   DATA_OBJECT theArg;
   unsigned i;
   size_t slen;
   char *osptr, *nsptr;

   /*================================================*/
   /* Function lowcase expects exactly one argument. */
   /*================================================*/

   if (EnvArgCountCheck(theEnv,(char*)"lowcase",EXACTLY,1) == -1)
     {
      SetpType(returnValue,STRING);
      SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,(char*)""));
      return;
     }

   /*==================================================*/
   /* The argument should be of type symbol or string. */
   /*==================================================*/

   if (EnvArgTypeCheck(theEnv,(char*)"lowcase",1,SYMBOL_OR_STRING,&theArg) == FALSE)
     {
      SetpType(returnValue,STRING);
      SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,(char*)""));
      return;
     }

   /*======================================================*/
   /* Allocate temporary memory and then copy the original */
   /* string or symbol to that memory, while lowercasing   */
   /* upper case alphabetic characters.                    */
   /*======================================================*/

   osptr = DOToString(theArg);
   slen = strlen(osptr) + 1;
   nsptr = (char *) gm2(theEnv,slen);

   for (i = 0  ; i < slen ; i++)
     {
      if (isupper(osptr[i]))
        { nsptr[i] = (char) tolower(osptr[i]); }
      else
        { nsptr[i] = osptr[i]; }
     }

   /*========================================*/
   /* Return the lowercased string and clean */
   /* up the temporary memory used.          */
   /*========================================*/

   SetpType(returnValue,GetType(theArg));
   SetpValue(returnValue,(void *) EnvAddSymbol(theEnv,nsptr));
   rm(theEnv,nsptr,slen);
  }

/********************************************/
/* StrCompareFunction: H/L access routine   */
/*   for the str-compare function.          */
/********************************************/
globle long long StrCompareFunction(
  void *theEnv)
  {
   int numArgs, length;
   DATA_OBJECT arg1, arg2, arg3;
   long long returnValue;

   /*=======================================================*/
   /* Function str-compare expects either 2 or 3 arguments. */
   /*=======================================================*/

   if ((numArgs = EnvArgRangeCheck(theEnv,(char*)"str-compare",2,3)) == -1) return(0L);

   /*=============================================================*/
   /* The first two arguments should be of type symbol or string. */
   /*=============================================================*/

   if (EnvArgTypeCheck(theEnv,(char*)"str-compare",1,SYMBOL_OR_STRING,&arg1) == FALSE)
     { return(0L); }

   if (EnvArgTypeCheck(theEnv,(char*)"str-compare",2,SYMBOL_OR_STRING,&arg2) == FALSE)
     { return(0L); }

   /*===================================================*/
   /* Compare the strings. Use the 3rd argument for the */
   /* maximum length of comparison, if it is provided.  */
   /*===================================================*/

   if (numArgs == 3)
     {
      if (EnvArgTypeCheck(theEnv,(char*)"str-compare",3,INTEGER,&arg3) == FALSE)
        { return(0L); }

      length = CoerceToInteger(GetType(arg3),GetValue(arg3));
      returnValue = strncmp(DOToString(arg1),DOToString(arg2),
                            (STD_SIZE) length);
     }
   else
     { returnValue = strcmp(DOToString(arg1),DOToString(arg2)); }

   /*========================================================*/
   /* Return Values are as follows:                          */
   /* -1 is returned if <string-1> is less than <string-2>.  */
   /*  1 is return if <string-1> is greater than <string-2>. */
   /*  0 is returned if <string-1> is equal to <string-2>.   */
   /*========================================================*/

   if (returnValue < 0) returnValue = -1;
   else if (returnValue > 0) returnValue = 1;
   return(returnValue);
  }

/*******************************************/
/* SubStringFunction: H/L access routine   */
/*   for the sub-string function.          */
/*******************************************/
globle void *SubStringFunction(
  void *theEnv)
  {
   DATA_OBJECT theArgument;
   char *tempString, *returnString;
   size_t start, end, i, j, length;
   void *returnValue;

   /*===================================*/
   /* Check and retrieve the arguments. */
   /*===================================*/

   if (EnvArgCountCheck(theEnv,(char*)"sub-string",EXACTLY,3) == -1)
     { return((void *) EnvAddSymbol(theEnv,(char*)"")); }

   if (EnvArgTypeCheck(theEnv,(char*)"sub-string",1,INTEGER,&theArgument) == FALSE)
     { return((void *) EnvAddSymbol(theEnv,(char*)"")); }

   if (CoerceToLongInteger(theArgument.type,theArgument.value) < 1)
     { start = 0; }
   else
     { start = (size_t) CoerceToLongInteger(theArgument.type,theArgument.value) - 1; }

   if (EnvArgTypeCheck(theEnv,(char*)"sub-string",2,INTEGER,&theArgument) == FALSE)
     {  return((void *) EnvAddSymbol(theEnv,(char*)"")); }

   if (CoerceToLongInteger(theArgument.type,theArgument.value) < 1)
     { return((void *) EnvAddSymbol(theEnv,(char*)"")); }
   else
     { end = (size_t) CoerceToLongInteger(theArgument.type,theArgument.value) - 1; }

   if (EnvArgTypeCheck(theEnv,(char*)"sub-string",3,SYMBOL_OR_STRING,&theArgument) == FALSE)
     { return((void *) EnvAddSymbol(theEnv,(char*)"")); }
   
   tempString = DOToString(theArgument);
   
   /*================================================*/
   /* If parameters are out of range return an error */
   /*================================================*/
   
   length = UTF8Length(tempString);
   
   if (end > length)
     { end = length; }

   /*==================================*/
   /* If the start is greater than the */
   /* end, return a null string.       */
   /*==================================*/

   if ((start > end) || (length == 0))
     { return((void *) EnvAddSymbol(theEnv,(char*)"")); }

   /*=============================================*/
   /* Otherwise, allocate the string and copy the */
   /* designated portion of the old string to the */
   /* new string.                                 */
   /*=============================================*/

   else
     {
      start = UTF8Offset(tempString,start);
      end = UTF8Offset(tempString,end + 1) - 1;
      
      returnString = (char *) gm2(theEnv,(unsigned) (end - start + 2));  /* (end - start) inclusive + EOS */
      for(j=0, i=start;i <= end; i++, j++)
        { *(returnString+j) = *(tempString+i); }
      *(returnString+j) = '\0';
     }

   /*========================*/
   /* Return the new string. */
   /*========================*/

   returnValue = (void *) EnvAddSymbol(theEnv,returnString);
   rm(theEnv,returnString,(unsigned) (end - start + 2));
   return(returnValue);
  }

/******************************************/
/* StrIndexFunction: H/L access routine   */
/*   for the sub-index function.          */
/******************************************/
globle void StrIndexFunction(
  void *theEnv,
  DATA_OBJECT_PTR result)
  {
   DATA_OBJECT theArgument1, theArgument2;
   char *strg1, *strg2, *strg3;
   size_t i, j;

   result->type = SYMBOL;
   result->value = EnvFalseSymbol(theEnv);

   /*===================================*/
   /* Check and retrieve the arguments. */
   /*===================================*/

   if (EnvArgCountCheck(theEnv,(char*)"str-index",EXACTLY,2) == -1) return;

   if (EnvArgTypeCheck(theEnv,(char*)"str-index",1,SYMBOL_OR_STRING,&theArgument1) == FALSE) return;

   if (EnvArgTypeCheck(theEnv,(char*)"str-index",2,SYMBOL_OR_STRING,&theArgument2) == FALSE) return;

   strg1 = DOToString(theArgument1);
   strg2 = DOToString(theArgument2);

   /*=================================*/
   /* Find the position in string2 of */
   /* string1 (counting from 1).      */
   /*=================================*/

   if (strlen(strg1) == 0)
     {
      result->type = INTEGER;
      result->value = (void *) EnvAddLong(theEnv,(long long) UTF8Length(strg2) + 1LL);
      return;
     }
     
   strg3 = strg2;
   for (i=1; *strg2; i++, strg2++)
     {
      for (j=0; *(strg1+j) && *(strg1+j) == *(strg2+j); j++)
        { /* Do Nothing */ }

      if (*(strg1+j) == '\0')
        {
         result->type = INTEGER;
         result->value = (void *) EnvAddLong(theEnv,(long long) UTF8CharNum(strg3,i));
         return;
        }
     }

   return;
  }

/********************************************/
/* StringToFieldFunction: H/L access routine */
/*   for the string-to-field function.       */
/********************************************/
globle void StringToFieldFunction(
  void *theEnv,
  DATA_OBJECT *returnValue)
  {
   DATA_OBJECT theArg;

   /*========================================================*/
   /* Function string-to-field expects exactly one argument. */
   /*========================================================*/

   if (EnvArgCountCheck(theEnv,(char*)"string-to-field",EXACTLY,1) == -1)
     {
      returnValue->type = STRING;
      returnValue->value = (void *) EnvAddSymbol(theEnv,(char*)"*** ERROR ***");
      return;
     }

   /*==================================================*/
   /* The argument should be of type symbol or string. */
   /*==================================================*/

   if (EnvArgTypeCheck(theEnv,(char*)"string-to-field",1,SYMBOL_OR_STRING,&theArg) == FALSE)
     {
      returnValue->type = STRING;
      returnValue->value = (void *) EnvAddSymbol(theEnv,(char*)"*** ERROR ***");
      return;
     }

   /*================================*/
   /* Convert the string to an atom. */
   /*================================*/

   StringToField(theEnv,DOToString(theArg),returnValue);
  }

/*************************************************************/
/* StringToField: Converts a string to an atomic data value. */
/*************************************************************/
globle void StringToField(
  void *theEnv,
  char *theString,
  DATA_OBJECT *returnValue)
  {
   struct token theToken;

   /*====================================*/
   /* Open the string as an input source */
   /* and retrieve the first value.      */
   /*====================================*/

   OpenStringSource(theEnv,(char*)"string-to-field-str",theString,0);
   GetToken(theEnv,(char*)"string-to-field-str",&theToken);
   CloseStringSource(theEnv,(char*)"string-to-field-str");

   /*====================================================*/
   /* Copy the token to the return value data structure. */
   /*====================================================*/

   returnValue->type = theToken.type;
   if ((theToken.type == FLOAT) || (theToken.type == STRING) ||
#if OBJECT_SYSTEM
       (theToken.type == INSTANCE_NAME) ||
#endif
       (theToken.type == SYMBOL) || (theToken.type == INTEGER))
     { returnValue->value = theToken.value; }
   else if (theToken.type == STOP)
     {
      returnValue->type = SYMBOL;
      returnValue->value = (void *) EnvAddSymbol(theEnv,(char*)"EOF");
     }
   else if (theToken.type == UNKNOWN_VALUE)
     {
      returnValue->type = STRING;
      returnValue->value = (void *) EnvAddSymbol(theEnv,(char*)"*** ERROR ***");
     }
   else
     {
      returnValue->type = STRING;
      returnValue->value = (void *) EnvAddSymbol(theEnv,theToken.printForm);
     }
  }
  
#if (! RUN_TIME) && (! BLOAD_ONLY)

/**************************************/
/* EvalFunction: H/L access routine   */
/*   for the eval function.           */
/**************************************/
globle void EvalFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  {
   DATA_OBJECT theArg;

   /*=============================================*/
   /* Function eval expects exactly one argument. */
   /*=============================================*/

   if (EnvArgCountCheck(theEnv,(char*)"eval",EXACTLY,1) == -1)
     {
      SetpType(returnValue,SYMBOL);
      SetpValue(returnValue,EnvFalseSymbol(theEnv));
      return;
     }

   /*==================================================*/
   /* The argument should be of type SYMBOL or STRING. */
   /*==================================================*/

   if (EnvArgTypeCheck(theEnv,(char*)"eval",1,SYMBOL_OR_STRING,&theArg) == FALSE)
     {
      SetpType(returnValue,SYMBOL);
      SetpValue(returnValue,EnvFalseSymbol(theEnv));
      return;
     }

   /*======================*/
   /* Evaluate the string. */
   /*======================*/

   EnvEval(theEnv,DOToString(theArg),returnValue);
  }

/****************************/
/* Eval: C access routine   */
/*   for the eval function. */
/****************************/
#if ALLOW_ENVIRONMENT_GLOBALS
globle int Eval(
  char *theString,
  DATA_OBJECT_PTR returnValue)
  {
   return EnvEval(GetCurrentEnvironment(),theString,returnValue);
  }
#endif
  
/*****************************/
/* EnvEval: C access routine */
/*   for the eval function.  */
/*****************************/
globle int EnvEval(
  void *theEnv,
  char *theString,
  DATA_OBJECT_PTR returnValue)
  {
   struct expr *top;
   int ov;
   static int depth = 0;
   char logicalNameBuffer[20];
   struct BindInfo *oldBinds;

   /*======================================================*/
   /* Evaluate the string. Create a different logical name */
   /* for use each time the eval function is called.       */
   /*======================================================*/

   depth++;
   gensprintf(logicalNameBuffer,"Eval-%d",depth);
   if (OpenStringSource(theEnv,logicalNameBuffer,theString,0) == 0)
     {
      SetpType(returnValue,SYMBOL);
      SetpValue(returnValue,EnvFalseSymbol(theEnv));
      depth--;
      return(FALSE);
     }

   /*================================================*/
   /* Save the current parsing state before routines */
   /* are called to parse the eval string.           */
   /*================================================*/

   ov = GetPPBufferStatus(theEnv);
   SetPPBufferStatus(theEnv,FALSE);
   oldBinds = GetParsedBindNames(theEnv);
   SetParsedBindNames(theEnv,NULL);

   /*========================================================*/
   /* Parse the string argument passed to the eval function. */
   /*========================================================*/

   top = ParseAtomOrExpression(theEnv,logicalNameBuffer,NULL);

   /*============================*/
   /* Restore the parsing state. */
   /*============================*/

   SetPPBufferStatus(theEnv,ov);
   ClearParsedBindNames(theEnv);
   SetParsedBindNames(theEnv,oldBinds);

   /*===========================================*/
   /* Return if an error occured while parsing. */
   /*===========================================*/

   if (top == NULL)
     {
      SetEvaluationError(theEnv,TRUE);
      CloseStringSource(theEnv,logicalNameBuffer);
      SetpType(returnValue,SYMBOL);
      SetpValue(returnValue,EnvFalseSymbol(theEnv));
      depth--;
      return(FALSE);
     }

   /*==============================================*/
   /* The sequence expansion operator must be used */
   /* within the argument list of a function call. */
   /*==============================================*/

   if ((top->type == MF_GBL_VARIABLE) || (top->type == MF_VARIABLE))
     {
      PrintErrorID(theEnv,(char*)"MISCFUN",1,FALSE);
      EnvPrintRouter(theEnv,WERROR,(char*)"expand$ must be used in the argument list of a function call.\n");
      SetEvaluationError(theEnv,TRUE);
      CloseStringSource(theEnv,logicalNameBuffer);
      SetpType(returnValue,SYMBOL);
      SetpValue(returnValue,EnvFalseSymbol(theEnv));
      ReturnExpression(theEnv,top);
      depth--;
      return(FALSE);
     }

   /*=======================================*/
   /* The expression to be evaluated cannot */
   /* contain any local variables.          */
   /*=======================================*/

   if (ExpressionContainsVariables(top,FALSE))
     {
      PrintErrorID(theEnv,(char*)"STRNGFUN",2,FALSE);
      EnvPrintRouter(theEnv,WERROR,(char*)"Some variables could not be accessed by the eval function.\n");
      SetEvaluationError(theEnv,TRUE);
      CloseStringSource(theEnv,logicalNameBuffer);
      SetpType(returnValue,SYMBOL);
      SetpValue(returnValue,EnvFalseSymbol(theEnv));
      ReturnExpression(theEnv,top);
      depth--;
      return(FALSE);
     }

   /*====================================*/
   /* Evaluate the expression and return */
   /* the memory used to parse it.       */
   /*====================================*/

   ExpressionInstall(theEnv,top);
   EvaluateExpression(theEnv,top,returnValue);
   ExpressionDeinstall(theEnv,top);

   depth--;
   ReturnExpression(theEnv,top);
   CloseStringSource(theEnv,logicalNameBuffer);

   /*==========================================*/
   /* Perform periodic cleanup if the eval was */
   /* issued from an embedded controller.      */
   /*==========================================*/

   if ((EvaluationData(theEnv)->CurrentEvaluationDepth == 0) && (! CommandLineData(theEnv)->EvaluatingTopLevelCommand) &&
       (EvaluationData(theEnv)->CurrentExpression == NULL))
     { 
      ValueInstall(theEnv,returnValue);
      PeriodicCleanup(theEnv,TRUE,FALSE); 
      ValueDeinstall(theEnv,returnValue);
     }

   if (GetEvaluationError(theEnv)) return(FALSE);
   return(TRUE);
  }

#else

/*************************************************/
/* EvalFunction: This is the non-functional stub */
/*   provided for use with a run-time version.   */
/*************************************************/
globle void EvalFunction(
  void *theEnv,
  DATA_OBJECT_PTR returnValue)
  {
   PrintErrorID(theEnv,(char*)"STRNGFUN",1,FALSE);
   EnvPrintRouter(theEnv,WERROR,(char*)"Function eval does not work in run time modules.\n");
   SetpType(returnValue,SYMBOL);
   SetpValue(returnValue,EnvFalseSymbol(theEnv));
  }

/*****************************************************/
/* EnvEval: This is the non-functional stub provided */
/*   for use with a run-time version.                */
/*****************************************************/
globle int EnvEval(
  void *theEnv,
  char *theString,
  DATA_OBJECT_PTR returnValue)
  {
   PrintErrorID(theEnv,(char*)"STRNGFUN",1,FALSE);
   EnvPrintRouter(theEnv,WERROR,(char*)"Function eval does not work in run time modules.\n");
   SetpType(returnValue,SYMBOL);
   SetpValue(returnValue,EnvFalseSymbol(theEnv));
   return(FALSE);
  }

#endif

#if (! RUN_TIME) && (! BLOAD_ONLY)
/***************************************/
/* BuildFunction: H/L access routine   */
/*   for the build function.           */
/***************************************/
globle int BuildFunction(
  void *theEnv)
  {
   DATA_OBJECT theArg;

   /*==============================================*/
   /* Function build expects exactly one argument. */
   /*==============================================*/

   if (EnvArgCountCheck(theEnv,(char*)"build",EXACTLY,1) == -1) return(FALSE);

   /*==================================================*/
   /* The argument should be of type SYMBOL or STRING. */
   /*==================================================*/

   if (EnvArgTypeCheck(theEnv,(char*)"build",1,SYMBOL_OR_STRING,&theArg) == FALSE)
     { return(FALSE); }

   /*======================*/
   /* Build the construct. */
   /*======================*/

   return(EnvBuild(theEnv,DOToString(theArg)));
  }

/*****************************/
/* Build: C access routine   */
/*   for the build function. */
/*****************************/
#if ALLOW_ENVIRONMENT_GLOBALS
globle int Build(
  char *theString)
  {
   return EnvBuild(GetCurrentEnvironment(),theString);
  }
#endif
  
/******************************/
/* EnvBuild: C access routine */
/*   for the build function.  */
/******************************/
globle int EnvBuild(
  void *theEnv,
  char *theString)
  {
   char *constructType;
   struct token theToken;
   int errorFlag;

   /*====================================================*/
   /* No additions during defrule join network activity. */
   /*====================================================*/

#if DEFRULE_CONSTRUCT
   if (EngineData(theEnv)->JoinOperationInProgress) return(FALSE);
#endif

   /*===========================================*/
   /* Create a string source router so that the */
   /* string can be used as an input source.    */
   /*===========================================*/

   if (OpenStringSource(theEnv,(char*)"build",theString,0) == 0)
     { return(FALSE); }

   /*================================*/
   /* The first token of a construct */
   /* must be a left parenthesis.    */
   /*================================*/

   GetToken(theEnv,(char*)"build",&theToken);

   if (theToken.type != LPAREN)
     {
      CloseStringSource(theEnv,(char*)"build");
      return(FALSE);
     }

   /*==============================================*/
   /* The next token should be the construct type. */
   /*==============================================*/

   GetToken(theEnv,(char*)"build",&theToken);
   if (theToken.type != SYMBOL)
     {
      CloseStringSource(theEnv,(char*)"build");
      return(FALSE);
     }

   constructType = ValueToString(theToken.value);

   /*======================*/
   /* Parse the construct. */
   /*======================*/
   
   errorFlag = ParseConstruct(theEnv,constructType,(char*)"build");

   /*=================================*/
   /* Close the string source router. */
   /*=================================*/

   CloseStringSource(theEnv,(char*)"build");

   /*=========================================*/
   /* If an error occured while parsing the   */
   /* construct, then print an error message. */
   /*=========================================*/

   if (errorFlag == 1)
     {
      EnvPrintRouter(theEnv,WERROR,(char*)"\nERROR:\n");
      PrintInChunks(theEnv,WERROR,GetPPBuffer(theEnv));
      EnvPrintRouter(theEnv,WERROR,(char*)"\n");
     }

   DestroyPPBuffer(theEnv);

   /*===========================================*/
   /* Perform periodic cleanup if the build was */
   /* issued from an embedded controller.       */
   /*===========================================*/

   if ((EvaluationData(theEnv)->CurrentEvaluationDepth == 0) && (! CommandLineData(theEnv)->EvaluatingTopLevelCommand) &&
       (EvaluationData(theEnv)->CurrentExpression == NULL))
     { PeriodicCleanup(theEnv,TRUE,FALSE); }

   /*===============================================*/
   /* Return TRUE if the construct was successfully */
   /* parsed, otherwise return FALSE.               */
   /*===============================================*/

   if (errorFlag == 0) return(TRUE);

   return(FALSE);
  }
#else
/**************************************************/
/* BuildFunction: This is the non-functional stub */
/*   provided for use with a run-time version.    */
/**************************************************/
globle int BuildFunction(
  void *theEnv)
  {
   PrintErrorID(theEnv,(char*)"STRNGFUN",1,FALSE);
   EnvPrintRouter(theEnv,WERROR,(char*)"Function build does not work in run time modules.\n");
   return(FALSE);
  }

/******************************************************/
/* EnvBuild: This is the non-functional stub provided */
/*   for use with a run-time version.                 */
/******************************************************/
globle int EnvBuild(
  void *theEnv,
  char *theString)
  { 
   PrintErrorID(theEnv,(char*)"STRNGFUN",1,FALSE);
   EnvPrintRouter(theEnv,WERROR,(char*)"Function build does not work in run time modules.\n");
   return(FALSE);
  }
#endif /* (! RUN_TIME) && (! BLOAD_ONLY) */

#endif /* STRING_FUNCTIONS */
