#pragma once

/* Ŭ���̾�Ʈ���� ����ϱ����� ���ǵ� ������. */

/* namespace : Client����. */
/* Client�����ȿ� ���ǵ� g_iWinSizeX , g_iWinSizeY */
namespace Client {
	static const unsigned int		g_iWinSizeX = 1280;
	static const unsigned int		g_iWinSizeY = 720;

	enum LEVELID {LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END};
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
