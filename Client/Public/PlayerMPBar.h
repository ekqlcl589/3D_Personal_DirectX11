#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;

END

BEGIN(Client)

class CPlayerMPBar final : public CUI
{
private:
	CPlayerMPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayerMPBar(const CPlayerMPBar& rhs);
	virtual ~CPlayerMPBar() = default;

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
	
	_float MaxMP = 0.f;
	_float MP = 0.f;
public:
	static CPlayerMPBar* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free()override;

};

END