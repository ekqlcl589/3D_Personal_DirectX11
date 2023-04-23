#pragma once

/* 클라이언트에서 사용하기위해 정의된 데이터. */

/* namespace : Client공간. */
/* Client공간안에 정의된 g_iWinSizeX , g_iWinSizeY */
namespace Client {
	static const unsigned int		g_iWinSizeX = 1280;
	static const unsigned int		g_iWinSizeY = 720;

	enum LEVELID {LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_GAMEPLAY2, LEVEL_LOBBY, LEVEL_END};
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#include "SoundMgr.h"

using namespace Client;
