#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize()
{
	for (_uint i = 0; i < TS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrixInverse[i], XMMatrixIdentity());
	}
	m_vCamPos = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

void CPipeLine::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < TS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrixInverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
	}
	memcpy(&m_vCamPos, &m_TransformMatrix[TS_VIEW].m[3][0], sizeof (_float4) );
}

_float4x4 CPipeLine::Get_Transformfloat4x4(TRANSFORMSTATE eState)
{
	return m_TransformMatrix[eState];
}

_float4x4 CPipeLine::Get_Transformfloat4x4Inverse(TRANSFORMSTATE eState)
{
	return m_TransformMatrixInverse[eState];
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_TransformMatrix[eState]);
}

_matrix CPipeLine::Get_TransformMatrixInverse(TRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_TransformMatrixInverse[eState]);
}

_float4 CPipeLine::Get_CamPos()
{
	return m_vCamPos;
}

void CPipeLine::Set_TransformMatrix(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
}

void CPipeLine::Free()
{
}

