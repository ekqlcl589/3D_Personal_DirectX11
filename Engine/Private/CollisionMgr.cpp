#include "..\Public\CollisionMgr.h"
#include "GameObject.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


HRESULT CCollisionMgr::Tick(_double TimeDelta)
{


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

void CCollisionMgr::OnCollisionEnter(COLLISIONSTATE eType, COLLISIONSTATE eType2)
{
	//OncollisionStay(eType);

	if (true == m_bIsColl)
	{
		for (auto& Src : m_mapObj[eType])
		{
			for (auto& Dest : m_mapObj[eType2])
			{
				Src.second->OnCollision(Dest.second);
				Dest.second->OnCollision(Src.second);
			}
		}
	}
	//else
	//{
	//	for (auto& Src : m_mapObj[eType])
	//	{
	//		for (auto& Dest : m_mapObj[eType2])
	//		{
	//
	//			OncollisionStay(Src.second->Get_ObjType());
	//			OncollisionStay(Dest.second->Get_ObjType());
	//		}
	//	}
	//}

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

			if (true == SrcColl->Collision(DestColl)) // 충돌 정보 저장
				m_bIsColl = true;
			else
				m_bIsColl = false;

		}
	}

}

void CCollisionMgr::OncollisionStay(COLLISIONSTATE eType)
{
	// 한 번 충돌 했다면 
	for (auto& iter : m_mapObj[eType])
		Delete_CollideObj(eType, iter.second->Get_ObjID());

	//for (auto& iter = m_mapObj[eType].begin(); iter != m_mapObj[eType].end(); iter++)
	//{
	//	(*iter).
	//	
	//	if (iter->first == iNum)
	//	{
	//		iter = m_mapObj[eObjID].erase(iter);
	//		break;
	//	}
	//}


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

CGameObject * CCollisionMgr::Find_Collider(const _tchar * pColliderTag, COLLISIONSTATE eType)
{
	//auto iter = find_if(m_mapObj[eType].begin(), m_mapObj[eType].end(), CTag_Finder(pColliderTag));
	//
	//if (iter == m_mapObj[eType].end())
	//	return nullptr;
	//
	//return iter->second;
	return false;
}

void CCollisionMgr::Free()
{
}
