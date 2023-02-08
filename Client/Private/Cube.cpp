#include "stdafx.h"
#include "Cube.h"

#include "GameInstance.h"

CCube::CCube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCube::CCube(const CCube& rhs)
	: CGameObject(rhs)
{
}


HRESULT CCube::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCube::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ZeroMemory(&m_CubeState, sizeof(CUBESTATE));

	memcpy(&m_CubeState, pArg, sizeof m_CubeState);
	// pArg에 있는 정보중 트랜스폼 정보를 m_CubeState에 넘겨서 큐브 좌표 세팅
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CubeState.fPos));
	_vector vRight, vUp, vLook, vPos;

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
	 
	return S_OK;
}

void CCube::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CCube::LateTick(_double TimeDelta)
{

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CCube::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CCube::Add_Components(void)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer_Cube"), (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Cube_Shader"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer_Terrain"), (CComponent**)&m_pVIBuffer_Terrain)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Calculator"),
		TEXT("Com_cacul"), (CComponent**)&m_pCCalculatorCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCube::SetUp_ShaderResource()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}


CCube* CCube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCube* pInstance = new CCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Cube Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCube::Clone(void * pArg)
{
	CCube* pInstance = new CCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Cube Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCube::Free(void)
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Terrain);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCCalculatorCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
