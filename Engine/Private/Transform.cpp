#include "..\Public\Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_float4 vCurrState;

	XMStoreFloat4(&vCurrState, vState);

	memcpy(((_float4*)&m_WorldMatrix + eState), &vCurrState, sizeof(_float3));
}

_float3 CTransform::Get_Scale()
{
	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp = Get_State(CTransform::STATE_UP);
	_vector vLook = Get_State(CTransform::STATE_LOOK);

	return _float3(XMVectorGetX(XMVector3Length(vRight)), XMVectorGetX(XMVector3Length(vUp)), XMVectorGetX(XMVector3Length(vLook)));
}

/* x,y,z로 절대 스케일을 셋팅하겠다. */
void CTransform::Set_Scale(const _float3 & vScale)
{
	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(Get_State(CTransform::STATE_UP)) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * vScale.z);
}

/* x,y,z로 상대 스케일을 셋팅하겠다. */
void CTransform::Scaling(const _float3 & vScale)
{
	Set_State(CTransform::STATE_RIGHT, Get_State(CTransform::STATE_RIGHT) * vScale.x);
	Set_State(CTransform::STATE_UP, Get_State(CTransform::STATE_UP) * vScale.y);
	Set_State(CTransform::STATE_LOOK, Get_State(CTransform::STATE_LOOK) * vScale.z);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	ZeroMemory(&m_TransformDesc, sizeof m_TransformDesc);

	return S_OK;
}

// 트랜스폼이 복제되면, 복제된 객체에 TRANSFORM_DESC이 부여됨 
HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORM_DESC));

	return S_OK;
}

void CTransform::Go_Straight(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeed * TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += -XMVector3Normalize(vRight) * m_TransformDesc.fSpeed * TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	_vector vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeed * TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Back(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += -XMVector3Normalize(vLook) * m_TransformDesc.fSpeed * TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fAngle)
{
	_vector vRight, vUp, vLook;

	_float3 vScale = Get_Scale();

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp =	 XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook =  XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(vAxis, fAngle);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_vector vRight, vUp, vLook;

	vRight = Get_State(CTransform::STATE_RIGHT);
	vUp = Get_State(CTransform::STATE_UP);
	vLook = Get_State(CTransform::STATE_LOOK);

	_matrix RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotation * TimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

}

void CTransform::LookAt(_fvector vTargetPos)
{
	_vector vPos = Get_State(CTransform::STATE_POSITION);
	
	_vector vLook = vTargetPos - vPos;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimitDist)
{
	LookAt(vTargetPos);

	if (fLimitDist <= Compute_Distance(vTargetPos))
		Go_Straight(TimeDelta);
}

_float CTransform::Compute_Distance(_fvector vTargetPos)
{
	return XMVectorGetX(XMVector3Length(vTargetPos - Get_State(CTransform::STATE_POSITION)));
}

HRESULT CTransform::SetUp_ShaderResource(CShader * pShaderCom, const char * pConstantName)
{
	return 	pShaderCom->Set_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext* pContext)
{
	CTransform * pInstance = new CTransform(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CTransform Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform * pInstance = new CTransform(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CTransform Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
