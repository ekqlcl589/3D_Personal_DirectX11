#include "..\Public\Component_Mgr.h"

IMPLEMENT_SINGLETON(CComponent_Mgr)

CComponent_Mgr::CComponent_Mgr()
{
}

HRESULT CComponent_Mgr::Reserve_Mgr(_uint iNumLevel)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumLevel];

	m_iNumLevels = iNumLevel;

	return S_OK;
}

HRESULT CComponent_Mgr::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponent_Mgr::Clone_Componenet(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(pArg);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

CComponent * CComponent_Mgr::Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pPrototypes || iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTag_Finder(pPrototypeTag));
	
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Mgr::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}
	Safe_Delete_Array(m_pPrototypes);
}
