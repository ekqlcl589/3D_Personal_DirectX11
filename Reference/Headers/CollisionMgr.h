#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;

class CCollisionMgr :
	public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
private:
	CCollisionMgr();
	virtual ~CCollisionMgr() = default;

public:
	HRESULT Add_Collider(COLLISIONSTATE eType, int iNum, CGameObject* pObj);
	
	void OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2);
	HRESULT Delete_CollideObj(COLLISIONSTATE eObjID, int iNum);

private:
	unordered_map<_uint, CGameObject*> m_mapObj[OBJ_END];

public:
	virtual void Free()override;
};

END