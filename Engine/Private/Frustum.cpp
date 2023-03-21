#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Initialize()
{
	m_vOriginal_Points[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginal_Points[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginal_Points[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginal_Points[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginal_Points[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginal_Points[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginal_Points[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginal_Points[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix			ViewMatrixInv = pPipeLine->Get_TransformMatrixInverse(CPipeLine::TS_VIEW);
	_matrix			ProjMatrixInv = pPipeLine->Get_TransformMatrixInverse(CPipeLine::TS_PROJ);

	RELEASE_INSTANCE(CPipeLine);
	
	for (_uint i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&m_vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vOriginal_Points[i]), ProjMatrixInv));
		XMStoreFloat3(&m_vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ViewMatrixInv));
	}

	Make_Plane(m_vPoints, m_vPlane_InWorld);
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	_float3		vPoints[8];

	for (_uint i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), WorldMatrixInv));
	}

	Make_Plane(vPoints, m_vPlane_InLocal);
}

_bool CFrustum::isIn_WorldSpace(_fvector vPoint, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlane_InWorld[i]), vPoint)))
			return false;
	}

	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vPoint, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlane_InLocal[i]), vPoint)))
			return false;
	}

	return true;
}

void CFrustum::Make_Plane(_In_ const _float3* pPoints, _Out_ _float4* pPlanes)
{
	/* +x */
	XMStoreFloat4(&pPlanes[0], 
		XMPlaneFromPoints(
			XMLoadFloat3(&pPoints[1]), 
			XMLoadFloat3(&pPoints[5]), 
			XMLoadFloat3(&pPoints[6])));

	/* -x */
	XMStoreFloat4(&pPlanes[1],
		XMPlaneFromPoints(
			XMLoadFloat3(&pPoints[4]), 
			XMLoadFloat3(&pPoints[0]), 
			XMLoadFloat3(&pPoints[3])));

	/* +y */
	XMStoreFloat4(&pPlanes[2],
		XMPlaneFromPoints(
			XMLoadFloat3(&pPoints[4]),
			XMLoadFloat3(&pPoints[5]),
			XMLoadFloat3(&pPoints[1])));

	/* -y */
	XMStoreFloat4(&pPlanes[3],
		XMPlaneFromPoints(
			XMLoadFloat3(&pPoints[3]),
			XMLoadFloat3(&pPoints[2]),
			XMLoadFloat3(&pPoints[6])));

	/* +z */
	XMStoreFloat4(&pPlanes[4],
		XMPlaneFromPoints(
			XMLoadFloat3(&pPoints[5]),
			XMLoadFloat3(&pPoints[4]),
			XMLoadFloat3(&pPoints[7])));

	/* -z */
	XMStoreFloat4(&pPlanes[5],
		XMPlaneFromPoints(
			XMLoadFloat3(&pPoints[0]),
			XMLoadFloat3(&pPoints[1]),
			XMLoadFloat3(&pPoints[2])));


}

void CFrustum::Free()
{
}
