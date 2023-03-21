#pragma once

/* 여러가지 정점버퍼들의 부모가 되는 클래스이다. */
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer
{	
protected:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual	HRESULT Initialize_Prototype(_float fWidth, _float fHeight, _float fDepth, _float fMinSpeed, _float fMaxSpeed, _uint iNumInstance);
	virtual	HRESULT Initialize(void* pArg);
	virtual HRESULT Render() override;

public:
	void Update(_double TimeDelta);

private:
	ID3D11Buffer*					m_pVBInstance = { nullptr };
	_uint							m_iStrideInstance = { 0 };
	_uint							m_iNumInstance = { 0 };
	_float*							m_pSpeed = { nullptr };
	_float*							m_pOldHeight = { nullptr };

public:	
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float fWidth, _float fHeight, _float fDepth, _float fMinSpeed, _float fMaxSpeed, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END