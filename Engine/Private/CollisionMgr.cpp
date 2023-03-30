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
	/*for (auto& iter = m_mapObj[eType].begin(); iter != m_mapObj[eType].end(); iter++)
	{
	if (iter->first == iNum)
	return E_FAIL;
	}
	m_mapObj[eType].emplace(iNum, pObj);*/

	_bool bExist = false;

	for (unordered_map<_uint, list<CGameObject*>>::iterator iter = m_mapObj.begin(); iter != m_mapObj.end(); ++iter)
	{
		if (iNum == iter->first)
		{
			iter->second.push_back(pObj);
			bExist = true;
		}
	}

	if (!bExist)
	{
		list<CGameObject*> pList;
		pList.push_back(pObj);
		m_mapObj.emplace(iNum, pList);
	}

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
				Src->OnCollision(Dest);
				Dest->OnCollision(Src);
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
			CCollider* SrcColl = Src->Get_Collider();
			CCollider* DestColl = Dest->Get_Collider();
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
		Delete_CollideObj(eType, iter->Get_ObjID());

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
	/*for (auto& iter = m_mapObj[eObjID].begin(); iter != m_mapObj[eObjID].end(); iter++)
	{
	if (iter->first == iNum)
	{
	iter = m_mapObj[eObjID].erase(iter);
	break;
	}
	}*/
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

/*
_bool CCollisionMgr::Is_Colli_Dist(const COLLISIONSTATE & eType, const COLLISIONSTATE & Other) // 거리 비교가 아니라 collider 함수 내에 있는 OnCollide 함수를 통해 충돌 판별
{
for (auto& Src : m_mapObj[eType])
{
for (auto& Dest : m_mapObj[Other])
{
CCollider* SrcColl = Src->Get_Collider();
CCollider* DestColl = Dest->Get_Collider();
if (nullptr == SrcColl || nullptr == DestColl)
continue;

m_bIsColl = SrcColl->Collision(DestColl); // 충돌 정보 저장
m_CollCheck = m_bIsColl;
return m_CollCheck;

}
}
return false;
}
*/

_bool CCollisionMgr::Is_Colli_Dist(CGameObject * pDest, CGameObject * pSour)
{
	if (nullptr == pDest || nullptr == pSour)
		return false;

	CCollider* SrcColl = pSour->Get_Collider();
	CCollider* DestColl = pDest->Get_Collider();

	if (nullptr == SrcColl || nullptr == DestColl)
		return false;

	m_bIsColl = SrcColl->Collision(DestColl); // 충돌 정보 저장
	m_CollCheck = m_bIsColl;

	return m_CollCheck;
}

void CCollisionMgr::Check_Collision(COLLISIONSTATE eType, COLLISIONSTATE eType2)
{
	for (auto& Dest : m_mapObj[eType2])
	{
		for (auto& Src : m_mapObj[eType])
		{
			if (Dest->Get_Dead())
				Src->Erase_Collied(Dest);
			if (Src->Get_Dead())
				Dest->Erase_Collied(Src);
			if (Dest->Get_Dead() || Src->Get_Dead())
				continue;
			
			if (nullptr == Dest || nullptr == Src)
				return;

			// 충돌한 경우
			if (Is_Colli_Dist(Dest, Src))
			{
				// 이전에 충돌한 경우
				if (Src->Check_Collied(Dest))
				{
					// 이전에 충돌함 - 이녀석이 충돌 중일 때 처리하는 것 !!

					Dest->OnCollision(Src);
					Src->OnCollision(Dest);
				}
				else
				{
					// 이전에 충돌 안함 - 여기에서 처음 충돌할 때만 호출하는 함수를 불러와 !

					Dest->Add_Collied(Src);
					Dest->Set_isCollied(true);
					Dest->EnterCollision(Src);

					Src->Add_Collied(Dest);
					Src->Set_isCollied(true);
					Src->EnterCollision(Dest);
				}
			}
			// 충돌을 안함
			else
			{
				// 리스트에 있으면 빼고 Leave 호출
				// GameObject의 m_vecColl 안에 없어서 터지는 듯
				Dest->Erase_Collied(Src); // 리스트에서 제거 
				Src->Erase_Collied(Dest);
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