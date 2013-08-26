/*
electron
Copyright (c) 2013, Joshua Scoggins 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "clips.h"
#if !WIN_MVC
#include "shellvar.h"
#include <stdlib.h>
void ShellVariableQueryFunctions(void* theEnv) {
    EnvDefineFunction2(theEnv, 
            (char*)"get-shell-variable",
            'k',
            PTIEF GetShellVariable,
            (char*)"GetShellVariable",
            (char*)"11k");
    EnvDefineFunction2(theEnv,
            (char*)"set-shell-variable",
            'b',
            PTIEF SetShellVariable,
            (char*)"SetShellVariable",
            "33kkki");
    EnvDefineFunction2(theEnv,
            (char*)"unset-shell-variable",
            'b',
            PTIEF UnsetShellVariable,
            (char*)"UnsetShellVariable",
            "11k");
}

void GetShellVariable(void* theEnv, DATA_OBJECT_PTR ret) {
    char* result;
    result = getenv((const char*)EnvRtnLexeme(theEnv, 1));
    if(result != NULL) {
        ret->type = STRING;
        ret->value = EnvAddSymbol(theEnv, result);
    } else {
        ret->type = SYMBOL;
        ret->value = EnvFalseSymbol(theEnv);
    }
}

int SetShellVariable(void* theEnv) {
    int result;
    result = setenv((const char*)EnvRtnLexeme(theEnv,1),
            (const char*)EnvRtnLexeme(theEnv,2),
            (int)EnvRtnLong(theEnv,3));

    if(result != -1) {
        return 1;
    } else {
        return 0;
    }
}

int UnsetShellVariable(void* theEnv) {
    int result;
    result = unsetenv((const char*)EnvRtnLexeme(theEnv,1));

    if(result != -1) {
        return 1;
    } else {
        return 0;
    }
}
#endif
