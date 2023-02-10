#include "KeyMgr.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CKeyMgr)


CKeyMgr::CKeyMgr()
{
	ZeroMemory(m_bCurKeyState, sizeof(m_bCurKeyState));
	ZeroMemory(m_bPreKeyState, sizeof(m_bPreKeyState));
	ZeroMemory(m_bKeyDown, sizeof(m_bKeyDown));
	ZeroMemory(m_bKeyUp, sizeof(m_bKeyUp));
}


CKeyMgr::~CKeyMgr()
{
}


_bool CKeyMgr::Key_Pressing(_int _iKey)
{
	return m_bCurKeyState[_iKey];
}

_bool CKeyMgr::Key_Down(_int _iKey)
{
	return m_bKeyDown[_iKey];
}

_bool CKeyMgr::Key_Up(_int _iKey)
{
	return m_bKeyUp[_iKey];
}

void CKeyMgr::Key_Update()
{
	// Key ���� ���� �Լ� MainApp Update ��ó�� �ѹ� ȣ�� �ٸ������� ȣ��x
	for (_int i = 0; i < VK_MAX; ++i)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		if (pInstance->Get_DIKeyState(i) & 0x80) //���紭��Ű üũ
			m_bCurKeyState[i] = true;
		else
			m_bCurKeyState[i] = false;


		// �������� ������ ����, ���� ������ ���
		if (!m_bPreKeyState[i] && m_bCurKeyState[i])
			m_bKeyDown[i] = true;
		else
			m_bKeyDown[i] = false;

		// ������ ������ �ְ�, ���� ������ �ʾ��� ���
		if (m_bPreKeyState[i] && !m_bCurKeyState[i])
		{
			m_bKeyUp[i] = true;
		}
		else
			m_bKeyUp[i] = false;


		if (pInstance->Get_DIKeyState(i) & 0x80) //������ ����Űüũ
			m_bPreKeyState[i] = true;
		else
			m_bPreKeyState[i] = false;
		RELEASE_INSTANCE(CGameInstance);
	}
}


/*
void CKeyMgr::Key_Update()
{
// Key ���� ���� �Լ� MainApp Update ��ó�� �ѹ� ȣ�� �ٸ������� ȣ��x
for (_int i = 0; i < VK_MAX; ++i)
{
CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
if (pInstance->Get_DIKeyState(i) & 0x80) //���紭��Ű üũ
m_bCurKeyState[i] = true;
else
m_bCurKeyState[i] = false;


// �������� ������ ����, ���� ������ ���
if (!m_bPreKeyState[i] && m_bCurKeyState[i])
m_bKeyDown[i] = true;
else
m_bKeyDown[i] = false;

// ������ ������ �ְ�, ���� ������ �ʾ��� ���
if (m_bPreKeyState[i] && !m_bCurKeyState[i])
{
m_bKeyUp[i] = true;
}
else
m_bKeyUp[i] = false;


if (GetAsyncKeyState(i) & 0x8000) //������ ����Űüũ
m_bPreKeyState[i] = true;
else
m_bPreKeyState[i] = false;
}
}
*/
void Engine::CKeyMgr::Free(void)
{
}

