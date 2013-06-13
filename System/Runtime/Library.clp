;------------------------------------------------------------------------------
;electron
;Copyright (c) 2013, Joshua Scoggins 
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without
;modification, are permitted provided that the following conditions are met:
;    * Redistributions of source code must retain the above copyright
;      notice, this list of conditions and the following disclaimer.
;    * Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
;    * Neither the name of Joshua Scoggins nor the
;      names of its contributors may be used to endorse or promote products
;      derived from this software without specific prior written permission.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;DISCLAIMED. IN NO EVENT SHALL Joshua Scoggins BE LIABLE FOR ANY
;DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;------------------------------------------------------------------------------
; Library.clp - Defines the init module and loads all other corresponding
; files.
; 
; Written by Joshua Scoggins 
; Started on 3/11/2013
;------------------------------------------------------------------------------
; Define the module
;------------------------------------------------------------------------------
(defmodule Runtime (export ?ALL))
;------------------------------------------------------------------------------
; Load the corresponding types and functions 
;------------------------------------------------------------------------------
(load* "System/Runtime/GenericDeclareFunctions.clp")
(batch* "System/Runtime/DeclareLibraryFunctions.clp")
(batch* "System/Runtime/DeclareApplicationFunctions.clp")
(batch* "System/Runtime/DeclareLogicFunctions.clp")
(batch* "System/Runtime/Object.clp")
(batch* "System/Runtime/Message.clp")
(batch* "System/Runtime/Stage.clp")
(batch* "System/Runtime/SymbolDeclarationFunctions.clp")
(batch* "System/Runtime/DrawableObject.clp")
(batch* "System/Runtime/Screen.clp")
(batch* "System/Runtime/Surface.clp")
(batch* "System/Runtime/Component.clp")
(batch* "System/Runtime/Panel.clp")
;------------------------------------------------------------------------------
