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
	HRESULT Tick(_double TimeDelta);

	HRESULT Add_Collider(COLLISIONSTATE eType, int iNum, CGameObject* pObj);
	
	void OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2);
	HRESULT Delete_CollideObj(COLLISIONSTATE eObjID, int iNum);

private:
	unordered_map<_uint, CGameObject*> m_mapObj[OBJ_END];
	_float m_fCollTime = 0.f; // 충돌 후 몇 초 간 다시 충돌 x 체크 하기 위한 변수 
	_bool m_bIsColl = false;
	_bool m_CollCheck = false;

public:
	virtual void Free()override;
};

END