#pragma once
#include "Base.h"

BEGIN(Engine)


class ENGINE_DLL CKeyMgr : public CBase
{
	DECLARE_SINGLETON(CKeyMgr)

private:
	explicit CKeyMgr();
	virtual ~CKeyMgr();

public:
	_bool Key_Pressing (_int _iKey);
	_bool Key_Down(_int _iKey);
	_bool Key_Up (_int _iKey);

	void Key_Update(void);

private:
	_bool m_bCurKeyState[VK_MAX];
	_bool m_bPreKeyState[VK_MAX];
	_bool m_bKeyDown[VK_MAX];
	_bool m_bKeyUp[VK_MAX];

public:
	virtual	void Free(void)	override;
};

END