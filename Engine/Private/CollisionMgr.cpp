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

void CCollisionMgr::OnCollisionEnter(COLLISIONSTATE eType, COLLISIONSTATE eType2) // ��� �̰� OnCollision
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
				// �� �� �浹 �ߴٸ� �浹 ������ ���� �ؾ� �ϴµ� ��� ������ �ؾ�����..?
			}
		}
		m_CollCheck = false;
	}
}

void CCollisionMgr::OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2) // �̰� OnCollisionEnter�� ����;
{
	for (auto& Src : m_mapObj[eType])
	{
		for (auto& Dest : m_mapObj[eType2])
		{
			CCollider* SrcColl = Src->Get_Collider();
			CCollider* DestColl = Dest->Get_Collider();
			if (nullptr == SrcColl || nullptr == DestColl)
				continue;

			m_bIsColl = SrcColl->Collision(DestColl); // �浹 ���� ����
			m_CollCheck = m_bIsColl;
			m_bIsColl = false;

		}
	}

}

void CCollisionMgr::OncollisionStay(COLLISIONSTATE eType)
{
	// �� �� �浹 �ߴٸ� 
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
_bool CCollisionMgr::Is_Colli_Dist(const COLLISIONSTATE & eType, const COLLISIONSTATE & Other) // �Ÿ� �񱳰� �ƴ϶� collider �Լ� ���� �ִ� OnCollide �Լ��� ���� �浹 �Ǻ�
{
for (auto& Src : m_mapObj[eType])
{
for (auto& Dest : m_mapObj[Other])
{
CCollider* SrcColl = Src->Get_Collider();
CCollider* DestColl = Dest->Get_Collider();
if (nullptr == SrcColl || nullptr == DestColl)
continue;

m_bIsColl = SrcColl->Collision(DestColl); // �浹 ���� ����
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

	m_bIsColl = SrcColl->Collision(DestColl); // �浹 ���� ����
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

			// �浹�� ���
			if (Is_Colli_Dist(Dest, Src))
			{
				// ������ �浹�� ���
				if (Src->Check_Collied(Dest))
				{
					// ������ �浹�� - �̳༮�� �浹 ���� �� ó���ϴ� �� !!

					Dest->OnCollision(Src);
					Src->OnCollision(Dest);
				}
				else
				{
					// ������ �浹 ���� - ���⿡�� ó�� �浹�� ���� ȣ���ϴ� �Լ��� �ҷ��� !

					Dest->Add_Collied(Src);
					Dest->Set_isCollied(true);
					Dest->EnterCollision(Src);

					Src->Add_Collied(Dest);
					Src->Set_isCollied(true);
					Src->EnterCollision(Dest);
				}
			}
			// �浹�� ����
			else
			{
				// ����Ʈ�� ������ ���� Leave ȣ��
				// GameObject�� m_vecColl �ȿ� ��� ������ ��
				Dest->Erase_Collied(Src); // ����Ʈ���� ���� 
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
// �浹id 0 : �÷��̾�� ������ �浹
// �浹id 1 : �÷��̾�Į�� ���� �浹