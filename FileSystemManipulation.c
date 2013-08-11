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
    * Neither the name of electron nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Joshua Scoggins BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "clips.h"
#if !WIN_MVC 
#include "FileSystemManipulation.h"
#include <sys/stat.h>
#include <sys/types.h>

void FileSystemManipulationFunctions(void* theEnv) {
    EnvDefineFunction2(theEnv,
            (char*)"mkfifo",
            'b',
            PTIEF MakeFifo,
            (char*)"MakeFifo",
            (char*)"22kki");
    EnvDefineFunction2(theEnv,
            (char*)"mkdir",
            'b',
            PTIEF MakeDirectory,
            (char*)"MakeDirectory",
            (char*)"22kki");
    EnvDefineFunction2(theEnv,
            (char*)"umask",
            'g',
            PTIEF ModifyUmask,
            (char*)"ModifyUmask",
            (char*)"11i");

}

int MakeFifo(void* theEnv) {
    if(mkfifo((const char*)EnvRtnLexeme(theEnv,1),
                (mode_t)EnvRtnLong(theEnv,2)) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int MakeDirectory(void* theEnv) {
    if(mkdir((const char*)EnvRtnLexeme(theEnv,1),
                (mode_t)EnvRtnLong(theEnv,2)) == 0) {
        return 1;
    } else {
        return 0;
    }
}

vlong ModifyUmask(void* theEnv) {
    return (vlong)umask((mode_t)EnvRtnLong(theEnv,1));
}

#endif
