#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>


#ifndef DebugHR()
#define DebugHR(hr)                                                  \
	{                                                                \
    HRESULT result = hr;                                             \
	if(FAILED(result))                                               \
	{                                                                \
		assert(__FILE__, (DWORD)__LINE__, result, L"Error", true);  \
	}                                                                \
	}
#endif


#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

#define SafeDelete(x) { delete x; x = 0; }


#endif

