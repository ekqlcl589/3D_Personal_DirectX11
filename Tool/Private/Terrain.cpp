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

	//if (FAILED(Create_FilterTexture()))
	//	return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	//CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	//
	//if (CKeyMgr::GetInstance()->Key_Down(VK_LBUTTON))
	//	Check_Picking();
	//
	//RELEASE_INSTANCE(CGameInstance);

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

HRESULT CTerrain::Create_FilterTexture() // Tool에서 만들고 클라에는 데이터만 받아서 사용
{
	ID3D11Texture2D* pTexture = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitialData;
	ZeroMemory(&InitialData, sizeof(D3D11_SUBRESOURCE_DATA));

	_ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_ulong)* TextureDesc.Width * TextureDesc.Height);

	for (_uint i = 0; i < TextureDesc.Height; i++)
	{
		for (_uint j = 0; j < TextureDesc.Width; j++)
		{
			_uint iIndex = i * TextureDesc.Width + j;

			pPixel[iIndex] = D3D11COLOR_ARGB(255, 255, 255, 255);
		}
	}

	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = sizeof(_ulong) * TextureDesc.Width;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, nullptr, &m_pFilterSRV)))
		return E_FAIL;

	//D3D11_MAP_WRITE_DSCARD 옵션 시 여기서 피킹을 해서 픽킹된 지점의 픽셀 값을 바꿈
	//정확한 정보는 2.7 노션 참고 
	for (_uint i = 0; i < TextureDesc.Height; ++i)
	{
		for (_uint j = 0; j < TextureDesc.Width; ++j)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			if (true == m_pCaCom->Get_PickingState().bPicking)
			{
				_float4 fArgb = { 0.f, 0.f, 0.f, 0.f };
				XMStoreFloat4(&fArgb, m_pCaCom->Get_PickingState().vRayPos);

				pPixel[iIndex] = (255, fArgb.x, fArgb.y, fArgb.z);
			}
			else
				return E_FAIL;
		}
	}

	D3D11_MAPPED_SUBRESOURCE MappedSubResource; // 안에 데이터를 채워줌

	m_pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, pPixel, sizeof(_ulong)*TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(pTexture, 0);

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture, TEXT("../Bin/Test/TestFilter.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture);

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


	/* For.Com_Brush */
	/*if (true == m_pCaCom->Get_PickingState().bPicking)
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
	}*/

	const LIGHT_DESC* pLightDesc = pGameInstance->Get_Light(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	//if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
	//	return E_FAIL;
	if (FAILED(m_pMultiTextureCom[TEX_DIFFUSE]->SetUp_ShaderResourceArray(m_pShaderCom, "g_DiffuseMultiTexture")))
		return E_FAIL;
	if (FAILED(m_pMultiTextureCom[TEX_FILTER]->SetUp_ShaderResource(m_pShaderCom, "g_FilterTexture", 0)))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_FilterTexture", m_pFilterSRV)))
	//	return E_FAIL;

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

	Safe_Release(m_pFilterSRV);

	Safe_Release(m_pVIBuffer_Terrain);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCaCom);

	for (_uint i = 0; i < TEX_END; i++)
		Safe_Release(m_pMultiTextureCom[i]);
}
