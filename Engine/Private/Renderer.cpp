#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Mgr.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Mgr::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	_uint		iNumViewport = 1;

	D3D11_VIEWPORT		ViewportDesc;
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	/* 앞으로 사용할 렌더타겟들을 생성한다. */
	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.0f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.0f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.0f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.0f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma region Test
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ForBlur"), ViewportDesc.Width, ViewportDesc.Height, 
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 1.f, 1.f)))) //만약 경찬이 방식이라면  백 버퍼 색이랑 맞춰야 한다 
		return E_FAIL;
	// 타겟 y도 만들어서 해주ㅏㅓ얗 ㅁ
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ForGlow"), ViewportDesc.Width, ViewportDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 1.f, 1.f))))
		return E_FAIL;
#pragma endregion Test

	/* 생성 렌더타겟들을 용도에 따라 묶는다. */
	/* For.MRT_Deferred */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;

#pragma region Test
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurEffect"), TEXT("Target_ForBlur"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_ForGlow"))))
		return E_FAIL;
#pragma endregion Test

	/* For.MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DELARATION::Element, VTXTEX_DELARATION::iNumElements);
	
	m_pBlurShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxBlur.hlsl"), VTXTEX_DELARATION::Element, VTXTEX_DELARATION::iNumElements);
	if (nullptr == m_pShader || nullptr == m_pBlurShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_FullScreenWorldMatrix,
		XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f) *
		XMMatrixTranslation(0.0f, 0.0f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));


#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.0f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.0f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 100.0f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;

#pragma region Test
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ForBlur"), 500.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ForGlow"), 500.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
#pragma endregion Test

#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject || eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObject[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

void CRenderer::Draw_Renderer()
{
	//for (auto& RenderList : m_RenderObject)
	//{
	//	for (auto& pGameObject : RenderList)
	//	{
	//		if (nullptr != pGameObject)
	//			pGameObject->Render();

	//		Safe_Release(pGameObject);
	//	}
	//	RenderList.clear();
	//} 이제 무지성 그리는게 아니라 그룹에 따라 다른 효과를 줘서 그림

	Render_Priority();
	Render_NonAlphaBlend();
	Render_Lights();
	// blur
	
	Render_Blend();
	Render_Blur();
	Render_Glow();

	Render_NonLight();
	Render_AlphaBlend();
	Render_UI();

#ifdef _DEBUG

	Render_DebugGroup();

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	m_pTarget_Manager->Render(TEXT("MRT_Deferred"), m_pShader, m_pVIBuffer);

#pragma region Test
	m_pTarget_Manager->Render(TEXT("MRT_BlurEffect"), m_pShader, m_pVIBuffer);
	m_pTarget_Manager->Render(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer);
#pragma endregion Test

	m_pTarget_Manager->Render(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);
#endif

}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugGroup(CComponent * pComponent)
{
	m_DebugRenderObjects.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Render_DebugGroup()
{
	for (auto& pComponent : m_DebugRenderObjects)
	{
		pComponent->Render();

		Safe_Release(pComponent);
	}

	m_DebugRenderObjects.clear();

	return S_OK;
}
#endif

void CRenderer::Render_Priority()
{
	if (FAILED(m_pTarget_Manager->NoneClerBegin(m_pContext, TEXT("MRT_BlurEffect"))))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_PRIORITY].clear();

	if (FAILED(m_pTarget_Manager->End(m_pContext)))
		return;

}

void CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return;

	/* Diffuse + Normal : 그리고 있는 객체(지형, 플레이어, 몬스터 etc )들로부터 받아오고자하는 정보가 있다. */
	if (FAILED(m_pTarget_Manager->Begin(m_pContext, TEXT("MRT_Deferred"))))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_NONALPHA].clear();

	if (FAILED(m_pTarget_Manager->End(m_pContext)))
		return;
}

void CRenderer::Render_Lights()
{
	/* Shade */
	if (FAILED(m_pTarget_Manager->Begin(m_pContext, TEXT("MRT_LightAcc"))))
		return;

	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_Transformfloat4x4Inverse(CPipeLine::TS_VIEW))))
		return;
	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_Transformfloat4x4Inverse(CPipeLine::TS_VIEW))))
		return;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPos(), sizeof(_float4))))
		return;

	RELEASE_INSTANCE(CPipeLine);
	
	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return;

	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(m_pShader, TEXT("Target_Depth"), "g_DepthTextrue")))
		return;

	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);

	if (FAILED(m_pTarget_Manager->End(m_pContext)))
		return;
}

void CRenderer::Render_Blend()
{
	if (FAILED(m_pTarget_Manager->NoneClerBegin(m_pContext, TEXT("MRT_BlurEffect"))))
		return;


	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return;

	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return;

	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return;

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End(m_pContext)))
		return;
}

void CRenderer::Render_Blur()
{
	//Prototype_Component_Texture_Blur

	// targetForYBlur 비긴 
	//if (FAILED(m_pTarget_Manager->NoneClerBegin(m_pContext, TEXT("MRT_Effect")))) // 원래 주석
	//	return;

	if (FAILED( m_pTarget_Manager->Set_ShaderResourceView(m_pBlurShader, TEXT("Target_ForBlur"), "g_BlurTexture")))
		return;

	if (FAILED(m_pBlurShader->Set_Matrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pBlurShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pBlurShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	//_float fPixelWidth = 1280.f;
	//_float fPixelHeight = 720.f;
	//
	//m_pBlurShader->Set_RawValue("g_Width", &fPixelWidth, sizeof(_float));
	//m_pBlurShader->Set_RawValue("g_Height", &fPixelHeight, sizeof(_float));
	//
	//float blurPower = 0.5f;
	//m_pBlurShader->Set_RawValue("g_BlurPower", &blurPower, sizeof(_float));

	m_pBlurShader->Begin(2);
	m_pVIBuffer->Render();

	// 비긴한 거 엔드 시키고 
	//if (FAILED(m_pTarget_Manager->End(m_pContext))) // 원래 주석
	//	return;

	// 블러 y 함수를 하나 더 파서 
	// 	if (FAILED( m_pTarget_Manager->Set_ShaderResourceView(m_pBlurShader, TEXT("Target_ForBlurY"), "g_BlurTexture")))
	//return;
	if (FAILED(m_pTarget_Manager->NoneClerBegin(m_pContext, TEXT("MRT_Effect")))) // 원래 주석
		return;

	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(m_pBlurShader, TEXT("Target_ForGlow"), "g_GlowTexture")))
		return;

	m_pBlurShader->Begin(3);
	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End(m_pContext))) // 원래 주석
		return;

}

//void CRenderer::Render_BlurY() ㅇ기는 위에서 엔드를 했으니까 백버퍼로 가 있고 
// 

void CRenderer::Render_Glow()
{
	//비긴	
	//if (FAILED(m_pTarget_Manager->Begin(m_pContext, TEXT("MRT_Effect")))) 
	//	return;

	for (auto& pGameObject : m_RenderObject[RENDER_GLOW])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();
	
		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_GLOW].clear();


	//if (FAILED(m_pTarget_Manager->End(m_pContext)))
	//	return;

	//엔드 
	//if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(m_pBlurShader, TEXT("Target_ForGlow"), "g_GlowTexture")))
	//	return;
	//
	//if (FAILED(m_pBlurShader->Set_Matrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
	//	return;
	//if (FAILED(m_pBlurShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return;
	//if (FAILED(m_pBlurShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return;

	//m_pBlurShader->Begin(3);
	//m_pVIBuffer->Render();
}

void CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObject[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_NONLIGHT].clear();
}

void CRenderer::Render_AlphaBlend()
{
	for (auto& pGameObject : m_RenderObject[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_ALPHABLEND].clear();
}

void CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObject[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_UI].clear();
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer * pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Renderer Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pShader);
	Safe_Release(m_pBlurShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTarget_Manager);

}
