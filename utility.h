   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.24  06/05/06            */
   /*                                                     */
   /*                 UTILITY HEADER FILE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides a set of utility functions useful to    */
/*   other modules. Primarily these are the functions for    */
/*   handling periodic garbage collection and appending      */
/*   string data.                                            */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.24: Renamed BOOLEAN macro type to intBool.         */
/*                                                           */
/*************************************************************/

#ifndef _H_utility
#define _H_utility

#ifndef _H_evaluatn
#include "evaluatn.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

struct callFunctionItem
  {
   char *name;
   void (*func)(void *);
   int priority;
   struct callFunctionItem *next;
   short int environmentAware;
   void *context;
  };

struct callFunctionItemWithArg
  {
   char *name;
   void (*func)(void *,void *);
   int priority;
   struct callFunctionItemWithArg *next;
   short int environmentAware;
   void *context;
  };
  
struct trackedMemory
  {
   void *theMemory;
   struct trackedMemory *next;
   struct trackedMemory *prev;
   size_t memSize;
  };

struct garbageFrame
  {
   short dirty;
   short topLevel;
   struct garbageFrame *priorFrame;
   struct ephemeron *ephemeralSymbolList;
   struct ephemeron *ephemeralFloatList;
   struct ephemeron *ephemeralIntegerList;
   struct ephemeron *ephemeralBitMapList;
   struct ephemeron *ephemeralExternalAddressList;
   struct multifield *ListOfMultifields;
   struct multifield *LastMultifield;
  };

#define UTILITY_DATA 55

struct utilityData
  { 
   struct callFunctionItem *ListOfCleanupFunctions;
   struct callFunctionItem *ListOfPeriodicFunctions;
   short GarbageCollectionLocks;
   short PeriodicFunctionsEnabled;
   short YieldFunctionEnabled;
   void (*YieldTimeFunction)(void);
   struct trackedMemory *trackList;
   struct garbageFrame MasterGarbageFrame;
   struct garbageFrame *CurrentGarbageFrame;
  };

#define UtilityData(theEnv) ((struct utilityData *) GetEnvironmentData(theEnv,UTILITY_DATA))

  /* Is c the start of a utf8 sequence? */
#define IsUTF8Start(ch) (((ch) & 0xC0) != 0x80)
#define IsUTF8MultiByteStart(ch) ((((unsigned char) ch) >= 0xC0) && (((unsigned char) ch) <= 0xF7))
#define IsUTF8MultiByteContinuation(ch) ((((unsigned char) ch) >= 0x80) && (((unsigned char) ch) <= 0xBF))

#ifdef _UTILITY_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#define DecrementGCLocks() EnvDecrementGCLocks(GetCurrentEnvironment())
#define IncrementGCLocks() EnvIncrementGCLocks(GetCurrentEnvironment())
#define RemovePeriodicFunction(a) EnvRemovePeriodicFunction(GetCurrentEnvironment(),a)

   LOCALE void                           InitializeUtilityData(void *);
   LOCALE intBool                        AddCleanupFunction(void *,char *,void (*)(void *),int);
   LOCALE intBool                        EnvAddPeriodicFunction(void *,char *,void (*)(void *),int);
   LOCALE intBool                        AddPeriodicFunction(char *,void (*)(void),int);
   LOCALE intBool                        RemoveCleanupFunction(void *,char *);
   LOCALE intBool                        EnvRemovePeriodicFunction(void *,char *);
   LOCALE char                          *CopyString(void *,char *);
   LOCALE void                           DeleteString(void *,char *);
   LOCALE char                          *AppendStrings(void *,char *,char *);
   LOCALE char                          *StringPrintForm(void *,char *);
   LOCALE char                          *AppendToString(void *,char *,char *,size_t *,size_t *);
   LOCALE char                          *InsertInString(void *,char *,size_t,char *,size_t *,size_t *);
   LOCALE char                          *AppendNToString(void *,char *,char *,size_t,size_t *,size_t *);
   LOCALE char                          *EnlargeString(void *,size_t,char *,size_t *,size_t *);
   LOCALE char                          *ExpandStringWithChar(void *,int,char *,size_t *,size_t *,size_t);
   LOCALE struct callFunctionItem       *AddFunctionToCallList(void *,char *,int,void (*)(void *),
                                                               struct callFunctionItem *,intBool);
   LOCALE struct callFunctionItem       *AddFunctionToCallListWithContext(void *,char *,int,void (*)(void *),
                                                                          struct callFunctionItem *,intBool,void *);
   LOCALE struct callFunctionItem       *RemoveFunctionFromCallList(void *,char *,
                                                             struct callFunctionItem *,
                                                             int *);
   LOCALE void                           DeallocateCallList(void *,struct callFunctionItem *);
   LOCALE struct callFunctionItemWithArg *AddFunctionToCallListWithArg(void *,char *,int,void (*)(void *, void *),
                                                                       struct callFunctionItemWithArg *,intBool);
   LOCALE struct callFunctionItemWithArg *AddFunctionToCallListWithArgWithContext(void *,char *,int,void (*)(void *, void *),
                                                                                  struct callFunctionItemWithArg *,intBool,void *);
   LOCALE struct callFunctionItemWithArg *RemoveFunctionFromCallListWithArg(void *,char *,
                                                                            struct callFunctionItemWithArg *,
                                                                            int *);
   LOCALE void                           DeallocateCallListWithArg(void *,struct callFunctionItemWithArg *);
   LOCALE unsigned long                  ItemHashValue(void *,unsigned short,void *,unsigned long);
   LOCALE void                           YieldTime(void *);
   LOCALE void                           EnvIncrementGCLocks(void *);
   LOCALE void                           EnvDecrementGCLocks(void *);
   LOCALE short                          EnablePeriodicFunctions(void *,short);
   LOCALE short                          EnableYieldFunction(void *,short);
   LOCALE struct trackedMemory          *AddTrackedMemory(void *,void *,size_t);
   LOCALE void                           RemoveTrackedMemory(void *,struct trackedMemory *);
   LOCALE void                           UTF8Increment(char *,size_t *);
   LOCALE size_t                         UTF8Offset(char *,size_t);
   LOCALE size_t                         UTF8Length(char *);
   LOCALE size_t                         UTF8CharNum(char *,size_t);
   LOCALE void                           RestorePriorGarbageFrame(void *,struct garbageFrame *,struct garbageFrame *,struct dataObject *);
   LOCALE void                           CallCleanupFunctions(void *);
   LOCALE void                           CallPeriodicTasks(void *);
   LOCALE void                           CleanCurrentGarbageFrame(void *,struct dataObject *);
#endif




