#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Transform.h"
// ���� ���͵��� �θ�� ����ϰ� ������ �׽�Ʈ ������ ����� ��
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

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };

protected:
	MONSTERSTATE m_MonsterState;
	_uint iId = { 0 };
	void Collision_ToPlayer();
	void ChaseToPlayer();

	_float3 m_fDistance = { 5.f, 5.f, 5.f };
	_float m_fDis = 5.f;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END