#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollider;

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj) {}

	void KnockBack(_uint iDamage, _double TimeDelta);
	void Hit(_uint iDamage);

	void Set_ObjType(COLLISIONSTATE eObjID) { m_eCollisionState = eObjID; }

	COLLISIONSTATE Get_ObjType() { return m_eCollisionState; }
	CCollider* Get_Collider() { return m_pColliderCom; }

	_bool Get_isColl() { return m_isColl; }
	_bool Get_Dead() { return m_bDead; }
	void Set_Dead() { m_bDead = true; }

	int Set_ObjID(int& iObjID) { return m_iObjID = iObjID; }
	int	Get_ObjID() { return m_iObjID; }

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CCollider* m_pColliderCom = { nullptr };
	COLLISIONSTATE m_eCollisionState = OBJ_END;
	_bool m_isColl = { false };

	_bool m_isCloned = { false };
	_bool m_bDead = { false };

	int	m_iObjID = -1;

protected:
	unordered_map<const _tchar*, class CComponent*>			m_Components;

public:
	/* 내 멤버변수에도 컴포넌트 보관 + 부모의 맵에도 보관한다 */
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

	class CComponent* Find_Component(const _tchar* pComponentTag);
	class CComponent* Get_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free()override;
};

END