#include "..\Public\Layer.h"



CLayer::CLayer()
{
}


HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObject.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObject)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(TimeDelta);
	}
}

void CLayer::LateTick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObject)
	{
		if (nullptr != pGameObject)
			pGameObject->LateTick(TimeDelta);
	}
}

CLayer * CLayer::Create()
{
	CLayer * pInstance = new CLayer();

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObject)
		Safe_Release(pGameObject);

	m_GameObject.clear();
}

