#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CBone;
class CCollider;

END

BEGIN(Client)

class CMonsterSickle final :
	public CGameObject
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };

	enum WEAPONTYPE { WEAPON_MONSTER_SICKLE, WEAPON_END };

	typedef struct tagWeaponDesc
	{
		CBone* pBonePtr;
		_float4x4 matParentLocal;
		CTransform* pParentTransform;
		WEAPONTYPE WeaponType;

	}WEAPONDESC;

private:
	CMonsterSickle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterSickle(const CMonsterSickle& rhs);
	virtual ~CMonsterSickle() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj)override;
	virtual void EnterCollision(CGameObject* pObj) override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool m_bStart = { false };

private:
	_float HitTime = 0.f;
	_bool m_bColl = false;

	_bool m_bTakeHit = false;
	_bool m_PlayerRSkill = false;
	_bool m_PlayerFSkill = false;
	_bool m_PlayerRageSkill = false;

private:
	_bool Dead = false;
	_float Damage = 0.f;
public:
	WEAPONDESC m_Weapon;
	_float4x4 m_WorldMatrix;

public:
	static CMonsterSickle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END