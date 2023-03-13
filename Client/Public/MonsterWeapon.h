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
class CMonsterWeapon final :
	public CGameObject
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };

public:
	enum WEAPONTYPE { WEAPON_MONSTER_L, WEAPON_MONSTER_R, WEAPON_MONSTER_BODY, WEAPON_END };
	typedef struct tagWeaponDesc
	{
		CBone* pBonePtr;
		_float4x4 matParentLocal;
		CTransform* pParentTransform;
		WEAPONTYPE WeaponType;
	}WEAPONDESC;

private:
	CMonsterWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterWeapon(const CMonsterWeapon& rhs);
	virtual ~CMonsterWeapon() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj, _bool* pColl)override;

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

public:
	WEAPONDESC m_Weapon;
	_float4x4 m_WorldMatrix;

public:
	static CMonsterWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;

};

END