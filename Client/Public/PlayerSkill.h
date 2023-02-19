#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;

END

BEGIN(Client)

class CPlayerSkill final : public CUI
{
private:
	CPlayerSkill(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayerSkill(const CPlayerSkill& rhs);
	virtual ~CPlayerSkill() = default;

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
	_float3 m_fPosition = { 0.f, 0.f, 0.f };

public:
	static CPlayerSkill* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _float3 fPos);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free()override;

};

END