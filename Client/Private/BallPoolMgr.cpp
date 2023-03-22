#include "stdafx.h"
#include "..\Public\BallPoolMgr.h"
#include "GameInstance.h"
#include "BlackBall.h"

IMPLEMENT_SINGLETON(CBallPoolMgr)

CBallPoolMgr::CBallPoolMgr()
{
}


CGameObject * CBallPoolMgr::Reuse_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	//CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pBall = nullptr;

	if (m_ObjectPool.empty())
	{
		pBall = CBlackBall::Create(pDevice, pContext);
		++m_iCount;
	}
	else
	{
		pBall = m_ObjectPool.front();
		m_ObjectPool.pop_front();
	}

	if (nullptr == pBall)
		return nullptr;

	return pBall;
}

void CBallPoolMgr::Collect_Object(CGameObject * pObj)
{
	if (nullptr == pObj)
		return;

	m_ObjectPool.push_back(pObj);
}

void CBallPoolMgr::Free(void)
{
	for (auto& iter : m_ObjectPool)
	{
		Safe_Release(iter);
	}
	m_ObjectPool.clear();
}

