#include "stdafx.h"

#include "ImGui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ToolApp.h"

#include "ImGui_Tool.h"

#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Loading.h"

#include "DynamicCamera.h"
#include "KeyMgr.h"

CToolApp::CToolApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CToolApp::Initialize()
{
	GRAPHIC_DESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eMode = GRAPHIC_DESC::MODE_WIN;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.hWnd = g_hWnd;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, GraphicDesc, LEVEL_END, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject_ForStatic()))
		return E_FAIL;

	//if (FAILED(Open_Level(LEVEL_LOGO)))
	//	return E_FAIL;


	CImGui_Tool::GetInstance()->Initialize_ImGui(m_pDevice, m_pDeviceContext);

	return S_OK;
}

void CToolApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	CKeyMgr::GetInstance()->Update_KeyState();
	CKeyMgr::GetInstance()->Update_MouseState();

	CImGui_Tool::GetInstance()->Tick_ImGui(TimeDelta);

	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CToolApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();


	m_pRenderer->Draw_Renderer();

	CImGui_Tool::GetInstance()->Render_ImGui();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CToolApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT		hr = 0;

	/* 해당 레벨에 접근하기전에 무조건 로딩레벨을 통하게 만들어주겠다.  */
	switch (eLevelID)
	{
	case LEVEL_STATIC:
	case LEVEL_LOADING:
		MSG_BOX("이거아닌데.");
		break;

	default:
		hr = m_pGameInstance->Open_Level(LEVEL_LOADING,
			CLoading::Create(m_pDevice, m_pDeviceContext, eLevelID));
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolApp::Ready_Prototype_Component_ForStatic()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DELARATION::Element, VTXTEX_DELARATION::iNumElements))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CToolApp::Ready_Prototype_GameObject_ForStatic()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"), CDynamicCamera::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}


CToolApp * CToolApp::Create()
{
	CToolApp * pInstance = new CToolApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Fail : CToolApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolApp::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);

	CKeyMgr::GetInstance()->DestroyInstance();
	CImGui_Tool::GetInstance()->DestroyInstance();
	CGameInstance::Release_Engine();
}

