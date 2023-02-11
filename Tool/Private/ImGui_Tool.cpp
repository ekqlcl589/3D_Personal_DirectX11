#include "stdafx.h"

#include "ImGui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "..\Public\ImGui_Tool.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Object_Manager.h"
#include "Loading.h"
#include "Terrain.h"
#include "Cube.h"
#include "KeyMgr.h"

IMPLEMENT_SINGLETON(CImGui_Tool)
int CImGui_Tool::iNum = 0;

CImGui_Tool::CImGui_Tool()
{
}

void CImGui_Tool::Initialize_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_pBuffer_Terain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp"));
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	m_pCaculator = CCalculator::Create(m_pDevice, m_pContext);
	
}

void CImGui_Tool::Tick_ImGui(_double TimeDelta)
{
	// 추후 ImGui_Tool 은 Manager 형식으로 사용되고 레벨값에 따라 불리는 툴을 따로 배치해서 
	// 배치되는 툴이 불렸을 때 클릭을 통해 피킹 하는 식으로 사용
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	if (true == m_bCheck)
	{
		//if (CKeyMgr::GetInstance()->Key_Down(VK_RBUTTON))
		if(CKeyMgr::GetInstance()->Key_Down(DIMK_LB))
			Picking();
	}
	else if (true == m_bMonster)
	{
		if (CKeyMgr::GetInstance()->Key_Down(DIMK_LB))
			MonsterPicking();
	}
	else
	{
		m_bCheck = false;
		m_bMonster = false;
	}
	RELEASE_INSTANCE(CGameInstance);

}

void CImGui_Tool::Render_ImGui()
{
	// Start ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	LoadTerrain();
	Setting_Terrain();
	Create_Terrain();
	Create_Cube();


	//bool bDebo = true;
	//ImGui::ShowDemoWindow(&bDebo);

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}


void CImGui_Tool::Setting_Terrain()
{
	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;
	ImGui::GetIO().WantCaptureKeyboard = true;

	ImGui::Begin("Terrain Setting", NULL, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("CubeSave"))
				SaveData();

			ImGui::Separator();

			if (ImGui::MenuItem("CubeLoad"))
				LoadData();

			ImGui::Separator();

			if (ImGui::MenuItem("MonsterSave"))
				MonsterSave();

			ImGui::Separator();

			if (ImGui::MenuItem("MonsterLoad"))
				MonsterLoad();

			ImGui::EndMenu();

		}
		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void CImGui_Tool::Create_Terrain()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Begin("Mouse", NULL, ImGuiWindowFlags_MenuBar);

	POINT Pos;
	if (ImGui::IsMousePosValid())
	{
		Pos.x = io.MousePos.x;
		Pos.y = io.MousePos.y;
		ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);

	}
	ImGui::End();
}

HRESULT CImGui_Tool::Delete_Terrain()
{
	return S_OK;
}

void CImGui_Tool::LoadTerrain()
{
	ImGui::Begin("Terrain Setting", NULL, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Terrain"))
		{
			if (ImGui::MenuItem("Create"))
				if (FAILED(Open_Level(LEVEL_GAMEPLAY)))
					return;

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();

}

HRESULT CImGui_Tool::Create_Cube()
{
	ImGui::Begin("Terrain Setting", NULL, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Cube"))
		{
			//ImGui::Text("pos%f", m_pCaculator->Get_PickingState().vRayPos);
			m_bCheck = true;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Monster"))
		{
			//ImGui::Text("pos%f", m_pCaculator->Get_PickingState().vRayPos);
			m_bMonster = true;
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();

	}

	ImGui::End();
	return S_OK;
}

_bool CImGui_Tool::Picking()
{
	if (ImGui::IsMousePosValid())
	{
		m_bMonster = false;
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		m_pCaculator->Picking_OnTerrain(g_hWnd, m_pBuffer_Terain, m_pTransform);
		//피킹 횟수를 체크 해서 횟수 만큼 free에서 루프를 돌려서 해제시킨다?
		if (true == m_pCaculator->Get_PickingState().bPicking)
		{
			m_vPos = m_pCaculator->Get_PickingState().vRayPos;

			CCube::CUBESTATE state;
			ZeroMemory(&state, sizeof(CCube::CUBESTATE));

			XMStoreFloat3(&state.fPos, m_vPos);

			state.transformDesc.fSpeed = 5.f;
			state.transformDesc.fRotation = XMConvertToRadians(10);
			state.iCubeNum = iNum++;
			//state.fScale = 툴에서 정한값;


			if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Cube"), TEXT("Layer_Cube"), &state)))
				return E_FAIL;

			RELEASE_INSTANCE(CGameInstance);

			m_vecCubeData.push_back(state);
			m_bCheck = m_pCaculator->Get_PickingState().bPicking;
			return m_pCaculator->Get_PickingState().bPicking;
		}
	}

}

_bool CImGui_Tool::MonsterPicking()
{
	if (ImGui::IsMousePosValid())
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		m_bCheck = false;
		m_pCaculator->Picking_OnTerrain(g_hWnd, m_pBuffer_Terain, m_pTransform);

		if (true == m_pCaculator->Get_PickingState().bPicking)
		{
			m_vPos = m_pCaculator->Get_PickingState().vRayPos;

			CMonster::MONSTERSTATE eState;
			ZeroMemory(&eState, sizeof(CMonster::MONSTERSTATE));

			XMStoreFloat3(&eState.fPos, m_vPos);
			//eState.fScale;
			eState.iMonsterNum = iNum++; // 이거 지금 큐브랑 같이 쓰는데 나중에 수정 ㄱㄱ
			eState.transformDesc.fSpeed = 5.f;
			eState.transformDesc.fRotation = 0.f;

			if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster"), TEXT("Layer_Monster"), &eState)))
				return E_FAIL;

			RELEASE_INSTANCE(CGameInstance);

			m_vecMonsterData.push_back(eState);
			m_bMonster = m_pCaculator->Get_PickingState().bPicking;
			return m_pCaculator->Get_PickingState().bPicking;

		}
	}
}

HRESULT CImGui_Tool::Open_Level(LEVELID eLevelID)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pInstance)
		return E_FAIL;

	HRESULT		hr = 0;

	/* 해당 레벨에 접근하기전에 무조건 로딩레벨을 통하게 만들어주겠다.  */
	switch (eLevelID)
	{
	case LEVEL_STATIC:
	case LEVEL_LOADING:
		break;

	default:
		hr = pInstance->Open_Level(LEVEL_LOADING,
			CLoading::Create(m_pDevice, m_pContext, eLevelID));
	}

	if (FAILED(hr))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


void CImGui_Tool::SaveData()
{
	HANDLE hFile = CreateFile(L"../Data/Cube/Cube.dat", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Cube Save Fail");
		return;
	}

	DWORD dwByte = 0;

	for (auto& iter : m_vecCubeData)
	{
		WriteFile(hFile, &iter, sizeof(CCube::CUBESTATE), &dwByte, nullptr);
	}
	
	CloseHandle(hFile);
}

void CImGui_Tool::LoadData()
{
	HANDLE hFile = CreateFile(L"../Data/Cube/Cube.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Cube Load Fail");
		return;
	}

	DWORD dwByte = 0;
	CCube::CUBESTATE CubeData;
	while (true)
	{
		ReadFile(hFile, &CubeData, sizeof(CCube::CUBESTATE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		m_vecCubeData.push_back(CubeData);
	}

	CloseHandle(hFile);

	for (auto& iter : m_vecCubeData)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Cube"), TEXT("Layer_Cube"), &iter);
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImGui_Tool::MonsterSave()
{
	HANDLE hFile = CreateFile(L"../Data/Monster.dat", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Monster Save Fail");
		return;
	}

	DWORD dwByte = 0;

	for (auto& iter : m_vecMonsterData)
	{
		WriteFile(hFile, &iter, sizeof(CMonster::MONSTERSTATE), &dwByte, nullptr);
	}

	CloseHandle(hFile);

}

void CImGui_Tool::MonsterLoad()
{
	HANDLE hFile = CreateFile(L"../Data/Monster.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Monster Load Fail");
		return;
	}

	DWORD dwByte = 0;
	CMonster::MONSTERSTATE MonsterData;
	while (true)
	{
		ReadFile(hFile, &MonsterData, sizeof(CMonster::MONSTERSTATE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		m_vecMonsterData.push_back(MonsterData);
	}

	CloseHandle(hFile);

	for (auto& iter : m_vecMonsterData)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster"), TEXT("Layer_Monster"), &iter);
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImGui_Tool::Free()
{

	Safe_Release(m_pBuffer_Terain);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCaculator);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::GetInstance()->DestroyInstance();
}
