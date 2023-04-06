#include "..\Public\BlendObject.h"
#include "PipeLine.h"

CBlendObject::CBlendObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlendObject::CBlendObject(const CBlendObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlendObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBlendObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CBlendObject::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CBlendObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBlendObject::Compute_CamDistance(_fvector vWorldPos)
{
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	m_fCameraDistance = XMVectorGetX(XMVector3Length(vWorldPos - XMLoadFloat4(&pPipeLine->Get_CamPos())));

	RELEASE_INSTANCE(CPipeLine);
}


void CBlendObject::Free()
{
	__super::Free();

}
