#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;

END

BEGIN(Client)

class CMonsterHPBar final : public CUI
{
private:
	CMonsterHPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CMonsterHPBar(const CMonsterHPBar& rhs);
	virtual ~CMonsterHPBar() = default;

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

	_float MaxHP = 0.f;
	_float HP = 0.f;
public:
	static CMonsterHPBar* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free()override;

};

END