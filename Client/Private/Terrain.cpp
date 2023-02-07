#include "stdafx.h"
#include "..\Public\Terrain.h"

#include "GameInstance.h"
#include "KeyMgr.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	//CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	//
	//if(CKeyMgr::GetInstance()->Key_Down(VK_LBUTTON))
	//	Check_Picking();
	//
	//RELEASE_INSTANCE(CGameInstance);
	// Tool 에서만 적용 시키고 Client 에서는 적용 하면 안 됨 
	__super::Tick(TimeDelta);
}

void CTerrain::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBuffer_Terrain->Render();
	
	return S_OK;
}

_bool CTerrain::Check_Picking()
{
	return m_pCaCom->Picking_OnTerrain(g_hWnd, m_pVIBuffer_Terrain, m_pTransformCom);
}

HRESULT CTerrain::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer_Terrain"), (CComponent**)&m_pVIBuffer_Terrain)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Terrain_Shader"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Calculator"),
		TEXT("Com_cacul"), (CComponent**)&m_pCaCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), (CComponent**)&m_pMultiTextureCom[TEX_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Filter */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		TEXT("Com_Filter"), (CComponent**)&m_pMultiTextureCom[TEX_FILTER])))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResource()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPos(), sizeof(_float4))))
		return E_FAIL;

	// 툴 완성 되면 데이터를 받아 오는걸로 CGamePlay 클래스에서 
	/* For.Com_Brush_Picking */
	if (true == m_pCaCom->Get_PickingState().bPicking)
	{
		_float4 fff;
		XMStoreFloat4(&fff, m_pCaCom->Get_PickingState().vRayPos);
		if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &fff, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
			TEXT("Com_Brush"), (CComponent**)&m_pMultiTextureCom[TEX_BRUSH])))
			return E_FAIL;

		if (FAILED(m_pMultiTextureCom[TEX_BRUSH]->SetUp_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
			return E_FAIL;

		m_pCaCom->Set_PickingState();
	}


	RELEASE_INSTANCE(CGameInstance);

	//if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
	//	return E_FAIL;
	if (FAILED(m_pMultiTextureCom[TEX_DIFFUSE]->SetUp_ShaderResourceArray(m_pShaderCom, "g_DiffuseMultiTexture")))
		return E_FAIL;
	if (FAILED(m_pMultiTextureCom[TEX_FILTER]->SetUp_ShaderResource(m_pShaderCom, "g_FilterTexture", 0)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_Check", &bbbb, sizeof(_bool))))
	//	return E_FAIL;

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Terrain);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCaCom);

	for(_uint i = 0; i < TEX_END; i++)
		Safe_Release(m_pMultiTextureCom[i]);

}
