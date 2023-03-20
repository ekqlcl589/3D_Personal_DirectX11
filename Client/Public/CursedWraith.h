#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)
class CCursedWraith final :
	public CMonster
{
private:
	CCursedWraith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCursedWraith(const CCursedWraith& rhs);
	virtual ~CCursedWraith() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnCollision(CGameObject* pObj)override;

public:
	void Animation_State(_double TimeDelta);
	void Animation(CURSEDWRAITHSTATE eType);
	void Avoid(_double TimeDelta);

public:
	CURSEDWRAITHINFO Get_Info() { return m_tInfo; }
	void Set_Info(_uint iDamage) { m_tInfo._Hp -= iDamage; }

	_bool Get_Attack() { return m_bAttack; }

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT Add_Coll();

private:
	CURSEDWRAITHINFO m_tInfo;
	_double m_AnimDuration = 0.0;
	_double m_AnimTimeAcc = 0.0;

	_uint m_iAnimIndex = 0;

	_bool m_bAttack = false;
	_bool m_bWlak = false;

public:
	static CCursedWraith* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free()override;
};

END