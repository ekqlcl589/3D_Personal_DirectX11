#pragma once
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CMouse final :
	public CGameObject
{
private:
	CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMouse(const CMouse& rhs);
	virtual ~CMouse() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	_float m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr)override;
	virtual void Free()override;
};

END