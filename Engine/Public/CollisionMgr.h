#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollisionMgr :
	public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
private:
	CCollisionMgr();
	virtual ~CCollisionMgr() = default;

public:
	HRESULT Add_Collider(const _tchar* pColliderTag, COLLISIONSTATE eType);
	
	void Update_Collision(_double TimeDelta, COLLISIONSTATE eType);

	void Render_Collider();

private:
	unordered_map<const _tchar*, class CCollider*>* m_Coll = { nullptr };

private:
	class CCollider* Find_Collider(const _tchar* pColliderTag, COLLISIONSTATE eType);

public:
	virtual void Free()override;
};

END