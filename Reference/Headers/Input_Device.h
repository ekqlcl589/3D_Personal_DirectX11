#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device :	public CBase
{
	DECLARE_SINGLETON(CInput_Device)

private:
	explicit CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte		Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	
	
	_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID)
	{
		return m_MouseState.rgbButtons[byMouseID];
	}
	
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState)
	{
		return *(((long*)&m_MouseState) + eMoveState);
	}

public:
	HRESULT			Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void			SetUp_InputDev(void);

private:
	_byte			m_byKeyState[256];		// 모든 키보드의 입력 값을 저장하기 위한 배열
	DIMOUSESTATE	m_MouseState;			// 마우스 메세지가 발생했을 때 마우스 정보를 보관하는 구조체

	LPDIRECTINPUT8				m_pInputSDK;
	
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

public:
	virtual void		Free(void);
};

END