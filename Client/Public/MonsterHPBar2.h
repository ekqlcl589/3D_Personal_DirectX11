#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;

END

BEGIN(Client)

class CMonsterHPBar2 final : public CUI
{
public:
	enum OWNER {OWNER_GOLEM, OWNER_CREATURE, OWNER_WRAITH, OWNER_WRAITH2, OWNER_END};

private:
	CMonsterHPBar2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CMonsterHPBar2(const CMonsterHPBar2& rhs);
	virtual ~CMonsterHPBar2() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResource();

private:
	CShader* m_pShaderCom = { nullptr };

	OWNER m_eOwner;

	_float MaxHP = 0.f;
	_float HP = 0.f;

	_bool Dead = false;

public:
	static CMonsterHPBar2* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free()override;

};

END