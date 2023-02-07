#include "..\Public\Level_Mgr.h"
#include "Object_Manager.h"

IMPLEMENT_SINGLETON(CLevel_Mgr)

CLevel_Mgr::CLevel_Mgr()
{
}


HRESULT CLevel_Mgr::Open_Level(_uint iLevelIndex, CLevel * pCurrentLevel)
{
	CObject_Manager*		pObject_Manager = CObject_Manager::GetInstance();
	Safe_AddRef(pObject_Manager);

	pObject_Manager->Clear(m_iLevelIndex);

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pCurrentLevel;

	m_iLevelIndex = iLevelIndex;

	Safe_Release(pObject_Manager);

	return S_OK;
}

void CLevel_Mgr::Tick_Level(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(TimeDelta);
}

void CLevel_Mgr::Free()
{
	Safe_Release(m_pCurrentLevel);
}
