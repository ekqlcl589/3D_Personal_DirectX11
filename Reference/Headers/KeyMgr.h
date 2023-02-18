#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CKeyMgr final : public CBase
{
	DECLARE_SINGLETON(CKeyMgr);

private:
	CKeyMgr();
	virtual ~CKeyMgr() = default;

public:
	bool Key_Pressing(_ulong _dwKey);
	bool Key_Down(_ulong _dwKey);
	bool Key_Up(_ulong _dwKey);
	void Update_KeyState();

	bool Mouse_Pressing(MOUSEKEYSTATE _eMouse);
	bool Mouse_Down(MOUSEKEYSTATE _eMouse);
	bool Mouse_Up(MOUSEKEYSTATE _eMouse);
	void Update_MouseState();

private:
	bool m_arrbKeyState[KEY_MAX];
	bool m_arrMouseState[DIMK_END];

private:
	virtual void	Free(void) override;
};

END

