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

class CWeapon abstract : public CGameObject
{

protected:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };

public:
	enum WEAPONTYPE {WEAPON_SWORD, WEAPON_SHILED, WEAPON_END};
	typedef struct tagWeaponDesc
	{
		CBone* pBonePtr;
		_float4x4 matParentLocal;
		CTransform* pParentTransform;
		WEAPONTYPE WeaponType;
	}WEAPONDESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

protected:
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

public:
	WEAPONDESC m_Weapon;
	_float4x4 m_WorldMatrix;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free()override;
};

END