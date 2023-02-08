#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI abstract :	public CGameObject
{
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	class CRenderer* m_pRenderer = { nullptr };
	class CTransform* m_pTransform = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer_Rect = { nullptr };
	class CTexture* m_pTexture = { nullptr };

protected:
	_float m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free()override;
};

END