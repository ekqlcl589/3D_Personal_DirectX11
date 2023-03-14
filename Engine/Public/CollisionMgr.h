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
	HRESULT Tick(_double TimeDelta); // �� ������ ������ �浹 ���� 

	HRESULT Add_Collider(COLLISIONSTATE eType, int iNum, CGameObject* pObj); // �浹ü�� map�� push

	void OnCollisionEnter(COLLISIONSTATE eType, COLLISIONSTATE eType2); // �浹 ���� �Ǻ� 

	void OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2); // �浹 �ߴٸ�

	void OncollisionStay(COLLISIONSTATE eType); // �� �����ӿ� �浹 �ߴٸ� �� ��

	HRESULT Delete_CollideObj(COLLISIONSTATE eObjID, int iNum);

	class CGameObject* Find_Collider(const _tchar* pColliderTag, COLLISIONSTATE eType);

	_bool Is_Colli_Dist(const COLLISIONSTATE& eType, const COLLISIONSTATE& Other);

	void Check_Collision(COLLISIONSTATE eType, COLLISIONSTATE eType2);

private:
	unordered_map<_uint, CGameObject*> m_mapObj[OBJ_END];
	_float m_fCollTime = 0.f; // �浹 �� �� �� �� �ٽ� �浹 x üũ �ϱ� ���� ���� 
	_bool m_bIsColl = false;
	_bool m_CollCheck = false;

public:
	virtual void Free()override;
};

END