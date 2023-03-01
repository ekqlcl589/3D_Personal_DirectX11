#include "..\Public\CollisionMgr.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


HRESULT CCollisionMgr::Add_Collider(const _tchar * pColliderTag, COLLISIONSTATE eType)
{
	if (nullptr != Find_Collider(pColliderTag, eType))
		return E_FAIL;

	CCollider* pCollider = Find_Collider(pColliderTag, eType);

	m_Coll[eType].emplace(pColliderTag, pCollider);


	return S_OK;
}

void CCollisionMgr::Update_Collision(_double TimeDelta, COLLISIONSTATE eType)
{

}

void CCollisionMgr::Render_Collider()
{
}

CCollider * CCollisionMgr::Find_Collider(const _tchar * pColliderTag, COLLISIONSTATE eType)
{
	auto iter = find_if(m_Coll[eType].begin(), m_Coll[eType].end(), CTag_Finder(pColliderTag));

	if (iter == m_Coll[eType].end())
		return nullptr;

	return iter->second;
}

void CCollisionMgr::Free()
{
	for (_uint i = 0; i < OBJ_END; i++)
	{
		for (auto& pair : m_Coll[i])
			Safe_Release(pair.second);

		m_Coll[i].clear();

	}
	Safe_Delete_Array(m_Coll);
}
