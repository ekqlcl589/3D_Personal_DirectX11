#include "..\Public\Layer.h"
#include "GameObject.h"


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

HRESULT CLayer::Delete_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	for (auto iter = m_GameObject.begin(); iter != m_GameObject.end();)
	{
		if (pGameObject->Get_Dead())
		{
			Safe_Release(*iter);
			return S_OK;

		}
		else
			iter++;
	}

	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto iter = m_GameObject.begin(); iter != m_GameObject.end();)
	{
		if (nullptr != *iter)
		{
			(*iter)->Tick(TimeDelta);
			iter++;
		}
		else
		{
			iter = m_GameObject.erase(iter);
		}
	}
}

void CLayer::LateTick(_double TimeDelta)
{
	for (auto iter = m_GameObject.begin(); iter != m_GameObject.end();) //¸ó½ºÅÍ Á×À¸¸é ÀÌÅÍ ³Î µÇ¼­ ¸ØÃã 
	{
		if (nullptr != *iter)
		{
			(*iter)->LateTick(TimeDelta);
			iter++;
		}
		else
			return;
	}
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_GameObject.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_Component(pComponentTag);
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

