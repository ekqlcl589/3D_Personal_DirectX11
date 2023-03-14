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
	HRESULT Tick(_double TimeDelta); // 한 프레임 돌리고 충돌 저장 

	HRESULT Add_Collider(COLLISIONSTATE eType, int iNum, CGameObject* pObj); // 충돌체들 map에 push

	void OnCollisionEnter(COLLISIONSTATE eType, COLLISIONSTATE eType2); // 충돌 여부 판별 

	void OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2); // 충돌 했다면

	void OncollisionStay(COLLISIONSTATE eType); // 전 프레임에 충돌 했다면 안 함

	HRESULT Delete_CollideObj(COLLISIONSTATE eObjID, int iNum);

	class CGameObject* Find_Collider(const _tchar* pColliderTag, COLLISIONSTATE eType);

	_bool Is_Colli_Dist(const COLLISIONSTATE& eType, const COLLISIONSTATE& Other);

	void Check_Collision(COLLISIONSTATE eType, COLLISIONSTATE eType2);

private:
	unordered_map<_uint, CGameObject*> m_mapObj[OBJ_END];
	_float m_fCollTime = 0.f; // 충돌 후 몇 초 간 다시 충돌 x 체크 하기 위한 변수 
	_bool m_bIsColl = false;
	_bool m_CollCheck = false;

public:
	virtual void Free()override;
};

END