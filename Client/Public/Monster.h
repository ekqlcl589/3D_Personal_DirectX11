#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Transform.h"
// 추후 몬스터들의 부모로 사용하고 지금은 테스트 용으로 만들어 둠
BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)

class CMonster  abstract: public CGameObject
{
protected:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };

public:
	typedef struct MonsterState
	{
		_float3 fPos;
		_uint iMonsterNum;
		_float3 fScale;

		CTransform::TRANSFORM_DESC transformDesc;
	}MONSTERSTATE;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual _uint OnDead();
protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	MONSTERSTATE m_MonsterState;
	_uint iId = { 0 };
	void Collision_ToPlayer();
	void OnCollisionEnter();
	void ChaseToPlayer(_double TimeDelta);
	void KnockBack(_double TimeDelta);

	_float3 m_fDistance = { 5.f, 5.f, 5.f };
	_float m_fTagetDistance = 0.f;

	_float3 m_fToTagetPos = { 0.f, 0.f, 0.f };
	_vector m_vTargetPos;

	CGameObject* m_pTarget = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

	_bool m_bPlayerChecck = false;
	_bool m_bCheck = false;
	_bool m_bjump = false;

	MONSTERINFO m_eType;

	_float3 m_fHorizontal_Power = { 1.f, 1.f, 1.f };

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END