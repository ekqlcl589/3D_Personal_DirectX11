#include "..\Public\CollisionMgr.h"
#include "GameObject.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


HRESULT CCollisionMgr::Tick(_double TimeDelta)
{
	if (true == m_bIsColl)
	{
		m_fCollTime = 3.f;
	}
	if (m_fCollTime <= 3.f) // 3초간은 충돌이 일어나지 않음
	{
		m_fCollTime -= (_float)TimeDelta;
		
	}

	return S_OK;
}

HRESULT CCollisionMgr::Add_Collider(COLLISIONSTATE eType, int iNum, CGameObject* pObj)
{
	for (auto& iter = m_mapObj[eType].begin(); iter != m_mapObj[eType].end(); iter++)
	{
		if (iter->first == iNum)
			return E_FAIL;
	}
	m_mapObj[eType].emplace(iNum, pObj);

	return S_OK;
}

void CCollisionMgr::OnCollisionEnter()
{
}

void CCollisionMgr::OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2)
{
	for (auto& Src : m_mapObj[eType])
	{
		for (auto& Dest : m_mapObj[eType2])
		{
			CCollider* SrcColl = Src.second->Get_Collider();
			CCollider* DestColl = Dest.second->Get_Collider();
			if (nullptr == SrcColl || nullptr == DestColl)
				continue;

			m_bIsColl = SrcColl->Collision(DestColl);

			if (m_bIsColl)
			{
				Src.second->OnCollision(Dest.second);
				Dest.second->OnCollision(Src.second);
			}
		}
	}
}

void CCollisionMgr::StandColiision()
{
}

HRESULT CCollisionMgr::Delete_CollideObj(COLLISIONSTATE eObjID, int iNum)
{
	for (auto& iter = m_mapObj[eObjID].begin(); iter != m_mapObj[eObjID].end(); iter++)
	{
		if (iter->first == iNum)
		{
			iter = m_mapObj[eObjID].erase(iter);
			break;
		}
	}
	return S_OK;
}

void CCollisionMgr::Free()
{
}
