#pragma once

/* Ä«¸Þ¶ó º¼·ý. */
#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

public:
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
	_bool isIn_WorldSpace(_fvector vPoint, _float fRadius);
	_bool isIn_LocalSpace(_fvector vPoint, _float fRadius);


private:
	_float3				m_vOriginal_Points[8];
	_float3				m_vPoints[8];
	_float4				m_vPlane_InWorld[6];
	_float4				m_vPlane_InLocal[6];
	_float4				m_vPlane[6];

private:
	void Make_Plane(_In_ const _float3* pPoints, _Out_ _float4* pPlanes);

public:
	virtual void Free() override;
};

END