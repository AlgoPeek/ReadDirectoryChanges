// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

// TODO: reference additional headers your program requires here
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include "Util/auto_release.h"
#include "Util/IoCompletionPort.h"
#include "Util/ThreadSafeQueue.h"

#ifndef OVERRIDE
#if _MSC_VER >= 1600
// Visual Studio 2010 and later support override.
#define OVERRIDE override
#else
#define OVERRIDE
#endif
#endif  // OVERRIDE
