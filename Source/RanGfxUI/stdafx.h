// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.



// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>

#include "../InternalCommonLib/dxstdafx.h"

#include "../enginelib/DxTools/RENDERPARAM.h"
#include "../enginelib/Common/DXInputString.h"
#include "../enginelib/DeviceMan.h"

#include "../InternalCommonLib/dxstdafx.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

#define DEF_IMPROVE_DISPLAY_NAME