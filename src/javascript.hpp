/*             DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

// This encapsulates QuickJS, the same Javascript engine used in VCV Prototype.
// This is very experimental, and done in the simplest way that would accomplish my goals,
// since the documentation of quickjs is mostly "Just read the uncommented headers lol".
//
// I plan to keep it this simple until I run into an obvious pain point.
// 
// Jerry Sievert's fork (and advice!) is used: https://github.com/JerrySievert/QuickJS
// See makefile for how to add it to a project.

#pragma once
#include <rack.hpp>

// FIXME: Both forms of this path are broken on OS X and Linux
// #include <quickjs/quickjs.h>
// #include "../dep/QuickJS/quickjs.h"

// QuickJS always throws a warning here, but it works.
#include "quickjs/quickjs.h"

namespace Javascript {

struct Runtime {    
    JSRuntime *runtime = NULL;
    JSContext *context = NULL;
    JSValue argv;
    JSValue globalObject;

    Runtime () {
        runtime = JS_NewRuntime();
        context = JS_NewContext(runtime);

        argv = JS_NewObject(context);
        globalObject = JS_GetGlobalObject(context);
    }

    ~Runtime () {
        JS_FreeValue(context, argv);
        JS_FreeValue(context, globalObject);
        if (context) JS_FreeContext(context);
        if (runtime) JS_FreeRuntime(runtime);
    }

    void evaluateString(std::string script) {
        JSValue evaluatedScript = JS_Eval(context, script.c_str(), script.size(), "Evaluated script", 0);
        JS_FreeValue(context, evaluatedScript);
    }

    const char* readVariableAsChar(const char* variable){
        size_t plen;
        JSValue value = JS_GetPropertyStr(context, globalObject, variable);
        const char *readValue = JS_ToCStringLen(context, &plen, value);
        JS_FreeValue(context, value);
        return readValue;
    }

    int32_t readVariableAsInt32(const char* variable){
        int32_t readValue = 0;
        JSValue value = JS_GetPropertyStr(context, globalObject, variable);
        JS_ToInt32(context, &readValue, value);
        JS_FreeValue(context, value);
        return readValue;
    }


};

} // Javascript
