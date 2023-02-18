#include "..\Public\KeyMgr.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CKeyMgr);

CKeyMgr::CKeyMgr()
{
	ZeroMemory(&m_arrbKeyState, sizeof(bool) * KEY_MAX);
}

bool CKeyMgr::Key_Pressing(_ulong _dwKey)
{
	_bool result = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_ubyte bKey = (_ubyte)_dwKey;
	if (pGameInstance->Get_DIKeyState(bKey) & 0x80)
		result = true;

	Safe_Release(pGameInstance);
	return result;
}

bool CKeyMgr::Key_Down(_ulong _dwKey)
{
	_bool result = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_ubyte bKey = (_ubyte)_dwKey;
	if (!m_arrbKeyState[bKey] && (pGameInstance->Get_DIKeyState(bKey) & 0x80))
	{
		m_arrbKeyState[bKey] = true;
		result = true;
	}

	Safe_Release(pGameInstance);
	return result;
}

bool CKeyMgr::Key_Up(_ulong _dwKey)
{
	_bool result = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_ubyte bKey = (_ubyte)_dwKey;
	if (m_arrbKeyState[bKey] && !(pGameInstance->Get_DIKeyState(bKey) & 0x80))
	{
		m_arrbKeyState[bKey] = false;
		result = true;
	}

	Safe_Release(pGameInstance);
	return result;
}

void CKeyMgr::Update_KeyState()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (_ubyte i = 0; i < KEY_MAX; ++i)
	{
		if (m_arrbKeyState[i] && !(pGameInstance->Get_DIKeyState(i) & 0x80))
			m_arrbKeyState[i] = false;
		else if (!m_arrbKeyState[i] && (pGameInstance->Get_DIKeyState(i) & 0x80))
			m_arrbKeyState[i] = true;
	}
	Safe_Release(pGameInstance);
}

bool CKeyMgr::Mouse_Pressing(MOUSEKEYSTATE _eMouse)
{
	_bool result = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIMouseState(_eMouse) & 0x80)
		result = true;

	Safe_Release(pGameInstance);
	return result;
}

bool CKeyMgr::Mouse_Down(MOUSEKEYSTATE _eMouse)
{
	_bool result = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (!m_arrMouseState[_eMouse] && (pGameInstance->Get_DIMouseState(_eMouse) & 0x80))
	{
		m_arrMouseState[_eMouse] = true;
		result = true;
	}

	Safe_Release(pGameInstance);
	return result;
}

bool CKeyMgr::Mouse_Up(MOUSEKEYSTATE _eMouse)
{
	_bool result = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_arrMouseState[_eMouse] && !(pGameInstance->Get_DIMouseState(_eMouse) & 0x80))
	{
		m_arrMouseState[_eMouse] = false;
		result = true;
	}

	Safe_Release(pGameInstance);
	return result;
}

void CKeyMgr::Update_MouseState()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (_int i = 0; i < DIMK_END; ++i)
	{
		if (m_arrMouseState[i] && !(pGameInstance->Get_DIMouseState((MOUSEKEYSTATE)i) & 0x80))
			m_arrMouseState[i] = false;
		else if (!m_arrMouseState[i] && (pGameInstance->Get_DIMouseState((MOUSEKEYSTATE)i) & 0x80))
			m_arrMouseState[i] = true;
	}

	Safe_Release(pGameInstance);
}

void CKeyMgr::Free(void)
{
}