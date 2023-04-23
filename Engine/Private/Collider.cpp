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
	//, m_pBatch(rhs.m_pBatch)
	//, m_pEffect(rhs.m_pEffect)
	//, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	//Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
//	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
//	m_pEffect = new BasicEffect(m_pDevice);
//	m_pEffect->SetVertexColorEnabled(true); // 셰이더에 던짐
//
//	const void* pShaderByteCode = { nullptr };
//	size_t iShaderByteCodeLength = { 0 };
//
//	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);
//
//	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
//		return E_FAIL;
//
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

	_matrix TransformMatrix = XMMatrixIdentity(); // OBB 스케일 ㅄ 이라서 이렇게 함

	switch (m_eType)
	{
	case Engine::CCollider::TYPE_AABB:
		m_pAABB = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		TransformMatrix = matScale * matRotation * matTranslation;
		m_pAABB->Transform(*m_pAABB, TransformMatrix);
		m_pOriginAABB = new BoundingBox(*m_pAABB);
		break;
	case Engine::CCollider::TYPE_OBB:
		m_pOBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		TransformMatrix = matRotation * matTranslation;
		m_pOBB->Transform(*m_pOBB, TransformMatrix);
		XMStoreFloat3(&m_pOBB->Extents, XMLoadFloat3(&m_pOBB->Extents) * XMLoadFloat3(&m_ColliderDesc.vScale));
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
	m_isColl = false;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pOriginAABB->Transform(*m_pAABB, Remove_Rotation(TransformMatrix)); // m_pAABB에 값을 저장
		break;

	case TYPE_OBB:
		m_pOriginOBB->Transform(*m_pOBB, TransformMatrix);
		break;

	case TYPE_SPHERE:
		m_pOriginSphere->Transform(*m_pSphere, TransformMatrix);
		break;

	default:
		break;
	}
}

_bool CCollider::Collision(CCollider * pTargetCollider)
{
	if (TYPE_AABB == m_eType)
	{
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pSphere);
	}

	else if (TYPE_OBB == m_eType)
	{
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pSphere);
	}

	else
	{
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pSphere);
	}

	if (true == m_isColl)
		pTargetCollider->m_isColl = m_isColl;

	return m_isColl;
}

_bool CCollider::Collision_AABB(CCollider * pTargetCollider)
{
	if (nullptr == m_pAABB ||
		nullptr == pTargetCollider->m_pAABB)
		return false;

	_float3		vSourMin, vSourMax;
	_float3		vDestMin, vDestMax;

	vSourMin = Compute_Min();
	vSourMax = Compute_Max();

	vDestMin = pTargetCollider->Compute_Min();
	vDestMax = pTargetCollider->Compute_Max();

	/* 너비 충돌. */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return false;
	/* 높이 충돌. */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return false;

	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
		return false;

	m_isColl = true;
	pTargetCollider->m_isColl = true;

	return true;
}

_bool CCollider::Collision_OBB(CCollider * pTargetCollider)
{
	if (nullptr == m_pOBB ||
		nullptr == pTargetCollider->m_pOBB)
		return false;

	OBBDESC		OBBDesc[2] = {
		Compute_OBB(),
		pTargetCollider->Compute_OBB(),
	};

	_float		fLength[3] = { 0.0f };

	_vector		vCenterDir = XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter);

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			/* 센터를 잇는 벡터를 0번째 박스의 0번째 평행한 축에 투영시킨 길이를 얻어온다. */
			fLength[0] = fabs(XMVectorGetX(XMVector3Dot(vCenterDir, XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fLength[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fLength[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			if (fLength[1] + fLength[2] < fLength[0])
			{
				m_isColl = false;
				return m_isColl;
			}
		}
	}

	return m_isColl = true;
}

_bool CCollider::Collision_SPHERE(CCollider * pTargetCollider)
{
	return _bool();
}

#ifdef _DEBUG

//HRESULT CCollider::Render()
//{
	//m_pContext->IASetInputLayout(m_pInputLayout);
	//
	//CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);
	//
	//m_pEffect->SetWorld(XMMatrixIdentity());
	//m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::TS_VIEW));
	//m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::TS_PROJ));
	//
	//RELEASE_INSTANCE(CPipeLine);
	//
	//m_pEffect->Apply(m_pContext);
	//
	//m_pBatch->Begin();
	//
	//_vector	vColor = m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);
	//
	//switch (m_eType)
	//{
	//case CCollider::TYPE_AABB:
	//	DX::Draw(m_pBatch, *m_pAABB, vColor);
	//	break;
	//case CCollider::TYPE_OBB:
	//	DX::Draw(m_pBatch, *m_pOBB, vColor);
	//	break;
	//case CCollider::TYPE_SPHERE:
	//	DX::Draw(m_pBatch, *m_pSphere, vColor);
	//	break;
	//}
	//
	//m_pBatch->End();

//	return S_OK;
//}
#endif // _DEBUG


_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Result = TransformMatrix;

	Result.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Result.r[0]));
	Result.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Result.r[1]));
	Result.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Result.r[2]));

	return Result;
}

_float3 CCollider::Compute_Min()
{
	if (nullptr == m_pAABB)
		return _float3(0.f, 0.f, 0.f);

	_float3 vMin = { m_pAABB->Center.x - m_pAABB->Extents.x,
		m_pAABB->Center.y - m_pAABB->Extents.y,
		m_pAABB->Center.z - m_pAABB->Extents.z
	};

	return vMin;
}

_float3 CCollider::Compute_Max()
{
	if (nullptr == m_pAABB)
		return _float3(0.f, 0.f, 0.f);

	_float3 vMax = { m_pAABB->Center.x + m_pAABB->Extents.x,
		m_pAABB->Center.y + m_pAABB->Extents.y,
		m_pAABB->Center.z + m_pAABB->Extents.z
	};



	return vMax;
}

CCollider::OBBDESC CCollider::Compute_OBB()
{
	OBBDESC		OBBDesc;
	ZeroMemory(&OBBDesc, sizeof OBBDesc);

	OBBDesc.vCenter = m_pOBB->Center;

	_float3		vPoint[8];
	m_pOBB->GetCorners(vPoint);

	XMStoreFloat3(&OBBDesc.vCenterDir[0], (XMLoadFloat3(&vPoint[2]) - XMLoadFloat3(&vPoint[3])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[1], (XMLoadFloat3(&vPoint[2]) - XMLoadFloat3(&vPoint[1])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[2], (XMLoadFloat3(&vPoint[2]) - XMLoadFloat3(&vPoint[6])) * 0.5f);

	XMStoreFloat3(&OBBDesc.vAlignDir[0], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[0])));
	XMStoreFloat3(&OBBDesc.vAlignDir[1], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[1])));
	XMStoreFloat3(&OBBDesc.vAlignDir[2], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[2])));

	return OBBDesc;
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
	//if (false == m_isCloned)
	//{
	//	Safe_Delete(m_pBatch);
	//	Safe_Delete(m_pEffect);
	//}
	//
	//Safe_Release(m_pInputLayout);
#endif
}
