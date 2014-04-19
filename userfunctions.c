   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*               CLIPS Version 6.24  04/21/06          */
   /*                                                     */
   /*                USER FUNCTIONS MODULE                */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.24: Created file to seperate UserFunctions and     */
/*            EnvUserFunctions from main.c.                  */
/*                                                           */
/*************************************************************/

/***************************************************************************/
/*                                                                         */
/* Permission is hereby granted, free of charge, to any person obtaining   */
/* a copy of this software and associated documentation files (the         */
/* "Software"), to deal in the Software without restriction, including     */
/* without limitation the rights to use, copy, modify, merge, publish,     */
/* distribute, and/or sell copies of the Software, and to permit persons   */
/* to whom the Software is furnished to do so.                             */
/*                                                                         */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS */
/* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT   */
/* OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY  */
/* CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES */
/* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN   */
/* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF */
/* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.          */
/*                                                                         */
/***************************************************************************/

#include "clips.h"
#include <wiringPi.h>
#include <piGlow.h>
void UserFunctions(void);
void EnvUserFunctions(void *);
void ElectronGlowOne(void*);
void ElectronGlowLeg(void*);
void ElectronGlowRing(void*);
void ElectronGlowDelay(void*);
/*********************************************************/
/* UserFunctions: Informs the expert system environment  */
/*   of any user defined functions. In the default case, */
/*   there are no user defined functions. To define      */
/*   functions, either this function must be replaced by */
/*   a function with the same name within this file, or  */
/*   this function can be deleted from this file and     */
/*   included in another file.                           */
/*********************************************************/
void UserFunctions() {   }
/***********************************************************/
/* EnvUserFunctions: Informs the expert system environment */
/*   of any user defined functions. In the default case,   */
/*   there are no user defined functions. To define        */
/*   functions, either this function must be replaced by   */
/*   a function with the same name within this file, or    */
/*   this function can be deleted from this file and       */
/*   included in another file.                             */
/***********************************************************/
void EnvUserFunctions(void *theEnv) {
   /* initialization */
   wiringPiSetupSys();
   piGlowSetup(0);
   /* functions */
   EnvDefineFunction2(theEnv, "electron-glow-one", 'v', PTIEF ElectronGlowOne, "ElectronGlowOne", "33i");
   EnvDefineFunction2(theEnv, "electron-glow-leg", 'v', PTIEF ElectronGlowLeg, "ElectronGlowLeg", "22i");
   EnvDefineFunction2(theEnv, "electron-glow-ring", 'v', PTIEF ElectronGlowRing, "ElectronGlowRing", "22i");
   EnvDefineFunction2(theEnv, "electron-glow-delay", 'v', PTIEF ElectronGlowDelay, "ElectronGlowDelay", "11i");
}

void ElectronGlowOne(void* theEnv) {
   int leg, ring, intensity;
   leg = EnvRtnLong(theEnv, 1);
   if(leg < 0 || leg > 2) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Target leg out of range\n");
      return;
   }
   ring = EnvRtnLong(theEnv, 2);
   if(ring < 0 || ring > 5) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Target ring out of range\n");
      return;
   }
   intensity = EnvRtnLong(theEnv, 3);
   if(intensity < 0 || intensity > 255) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Intensity out of range\n");
      return;
   }
   piGlow1(leg, ring, intensity);
}

void ElectronGlowLeg(void* theEnv) {
   int leg, intensity;
   leg = EnvRtnLong(theEnv, 1);
   if(leg < 0 || leg > 2) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Target leg out of range\n");
      return;
   }
   intensity = EnvRtnLong(theEnv, 2);
   if(intensity < 0 || intensity > 255) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Intensity out of range\n");
      return;
   }
   piGlowLeg(leg, intensity);
}

void ElectronGlowRing(void* theEnv) {
   int ring, intensity;
   ring = EnvRtnLong(theEnv, 1);
   if(ring < 0 || ring > 5) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Target ring out of range\n");
      return;
   }
   intensity = EnvRtnLong(theEnv, 2);
   if(intensity < 0 || intensity > 255) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Intensity out of range\n");
      return;
   }
   piGlowRing(ring, intensity);
}

void ElectronGlowDelay(void* theEnv) {
   int amount;
   amount = EnvRtnLong(theEnv, 1);
   if(amount < 0) {
      EnvPrintRouter(theEnv, WERROR, "ERROR: Delay amount out of range\n");
      return;
   }
   delay(amount);
}
