#include "..\Public\CollisionMgr.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Collider.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


HRESULT CCollisionMgr::Tick(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	
	CGameObject* pObj = pInstance->Find_GameObject(3, TEXT("Layer_Player"));

	CGameObject* pObj2 = pInstance->Find_GameObject(3, TEXT("Layer_Monster"));


	RELEASE_INSTANCE(CGameInstance);

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

void CCollisionMgr::OnCollisionEnter(COLLISIONSTATE eType, COLLISIONSTATE eType2) // 사실 이게 OnCollision
{
	//OncollisionStay(eType);

	if (m_CollCheck)
	{
		for (auto& Src : m_mapObj[eType])
		{
			for (auto& Dest : m_mapObj[eType2])
			{
				Src.second->OnCollision(Dest.second);
				Dest.second->OnCollision(Src.second);
				// 한 번 충돌 했다면 충돌 정보를 삭제 해야 하는데 어디서 삭제를 해야하지..?
			}
		}
		m_CollCheck = false;
	}
}

void CCollisionMgr::OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2) // 이게 OnCollisionEnter임 ㅎㅎ;
{
	for (auto& Src : m_mapObj[eType])
	{
		for (auto& Dest : m_mapObj[eType2])
		{
			CCollider* SrcColl = Src.second->Get_Collider();
			CCollider* DestColl = Dest.second->Get_Collider();
			if (nullptr == SrcColl || nullptr == DestColl)
				continue;

			m_bIsColl = SrcColl->Collision(DestColl); // 충돌 정보 저장
			m_CollCheck = m_bIsColl;
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

_bool CCollisionMgr::Is_Colli_Dist(const COLLISIONSTATE & eType, const COLLISIONSTATE & Other) // 거리 비교가 아니라 collider 함수 내에 있는 OnCollide 함수를 통해 충돌 판별
{
	for (auto& Src : m_mapObj[eType])
	{
		for (auto& Dest : m_mapObj[Other])
		{
			CCollider* SrcColl = Src.second->Get_Collider();
			CCollider* DestColl = Dest.second->Get_Collider();
			if (nullptr == SrcColl || nullptr == DestColl)
				continue;

			m_bIsColl = SrcColl->Collision(DestColl); // 충돌 정보 저장
			m_CollCheck = m_bIsColl;
			return m_CollCheck;

		}
	}
	return false;
}

void CCollisionMgr::Check_Collision(COLLISIONSTATE eType, COLLISIONSTATE eType2)
{
	for (auto& Dest : m_mapObj[eType2])
	{
		for (auto& Src : m_mapObj[eType])
		{
			if (Dest.second->Get_Dead())
				Src.second->Erase_Collied(Dest.second);
			if (Src.second->Get_Dead())
				Dest.second->Erase_Collied(Src.second);
			if (Dest.second->Get_Dead() || Src.second->Get_Dead())
				continue;

			if (Is_Colli_Dist(Dest.second->Get_ObjType(), Src.second->Get_ObjType()))
			{

				if (!Dest.second->Check_Collied(Src.second)) // 충돌리스트 추가 
				{
					Dest.second->Add_Collied(Src.second);
					Dest.second->Set_isCollied(true);
					Dest.second->OnCollision(Src.second);
				}

				if (!Src.second->Check_Collied(Dest.second)) // 충돌리스트 추가 
				{
					Src.second->Add_Collied(Dest.second);
					Src.second->Set_isCollied(true);
					Src.second->OnCollision(Dest.second);
				}

			}
			else
			{
				Dest.second->Erase_Collied(Src.second); // 리스트에서 제거 
				Src.second->Erase_Collied(Dest.second);
			}

		}
	}
}

void CCollisionMgr::Free()
{
}


//vector<_uint> collinfo;
//collinfo[0][1]
//
// 충돌id 0 : 플레이어와 몬스터의 충돌
// 충돌id 1 : 플레이어칼과 몬스터 충돌