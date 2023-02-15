#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final :
	public CVIBuffer
{
protected:
	CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Set_Buffer(_float x, _float y);

public:
	static CVIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END