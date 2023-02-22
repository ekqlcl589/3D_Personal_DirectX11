#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"


CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_pAABB(rhs.m_pAABB)
	, m_pOBB(rhs.m_pOBB)
	, m_pSphere(rhs.m_pSphere)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true); // 셰이더에 던짐

	const void* pShaderByteCode = { nullptr };
	size_t iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

#endif // _DEBUG

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_ColliderDesc, pArg, sizeof m_ColliderDesc);

	_matrix matScale, matRotation, matTranslation;

	matScale = XMMatrixScaling(m_ColliderDesc.vScale.x, m_ColliderDesc.vScale.y, m_ColliderDesc.vScale.z);
	matRotation = XMMatrixRotationX(m_ColliderDesc.vRotation.x) * XMMatrixRotationY(m_ColliderDesc.vRotation.y) * XMMatrixRotationZ(m_ColliderDesc.vCenter.z);
	matTranslation = XMMatrixTranslation(m_ColliderDesc.vCenter.x, m_ColliderDesc.vCenter.y, m_ColliderDesc.vCenter.z);

	_matrix TransformMatrix = matScale * matRotation * matTranslation;

	switch (m_eType)
	{
	case Engine::CCollider::TYPE_AABB:
		m_pAABB = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		m_pAABB->Transform(*m_pAABB, TransformMatrix);
		m_pOriginAABB = new BoundingBox(*m_pAABB);
		break;
	case Engine::CCollider::TYPE_OBB:
		m_pOBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		m_pOBB->Transform(*m_pOBB, TransformMatrix);
		m_pOriginOBB = new BoundingOrientedBox(*m_pOBB);
		break;
	case Engine::CCollider::TYPE_SPHERE:
		m_pSphere = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		m_pSphere->Transform(*m_pSphere, TransformMatrix);
		m_pOriginSphere = new BoundingSphere(*m_pSphere);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pOriginAABB->Transform(*m_pAABB, Remove_Rotation(TransformMatrix)); // m_pAABB에 값을 저장
		break;
	case TYPE_OBB:
		break;
	case TYPE_SPHERE:
		break;
	default:
		break;
	}
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	m_pContext->IASetInputLayout(m_pInputLayout);

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::TS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::TS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB);
		break;
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB);
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere);
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG


_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Result = TransformMatrix;

	Result.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Result.r[0]));
	Result.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Result.r[1]));
	Result.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Result.r[2]));

	return Result;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere);
	Safe_Delete(m_pOriginAABB);
	Safe_Delete(m_pOriginOBB);
	Safe_Delete(m_pOriginSphere);

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);
#endif
}
