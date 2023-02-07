#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
	
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_CameraDesc, pArg, sizeof m_CameraDesc);

	m_Transform = CTransform::Create(m_pDevice, m_pContext);

	if (nullptr == m_Transform)
		return E_FAIL;

	m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
	m_Transform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
	m_Transform->Set_TransformDesc(m_CameraDesc.Transform_Desc);

	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	m_pPipeLine->Set_TransformMatrix(CPipeLine::TS_VIEW, m_Transform->Get_WorldMatrixInverse());

	m_pPipeLine->Set_TransformMatrix(CPipeLine::TS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.vFov, m_CameraDesc.vAspect, m_CameraDesc.vNear, m_CameraDesc.vFar));

}

void CCamera::LateTick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}


void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_Transform);
	Safe_Release(m_pPipeLine);

}
