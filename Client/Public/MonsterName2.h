#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;

END

BEGIN(Client)

class CMonsterName2 final : public CUI
{
public:
	enum OWNER {OWNER_GOLEM, OWNER_CREATURE, OWNER_WRAITH, OWNER_WRAITH2, OWNER_END};

private:
	CMonsterName2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CMonsterName2(const CMonsterName2& rhs);
	virtual ~CMonsterName2() = default;

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

	_double m_fTimeder = 12.0;

	_bool Dead = false;

	_bool m_bRender = false;

public:
	static CMonsterName2* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free()override;

};

END