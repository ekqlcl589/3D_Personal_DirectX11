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
#include "KeyMgr.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CImGui_Tool)
int CImGui_Tool::iCubeNum = 0;
int CImGui_Tool::iMonsterNum = 0;
int CImGui_Tool::iTileNum = 0;

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
		if(CKeyMgr::GetInstance()->Mouse_Pressing(DIMK_LB))
			Picking();
	}
	else if (true == m_bMonster)
	{
		if (CKeyMgr::GetInstance()->Mouse_Pressing(DIMK_LB))
			MonsterPicking();
	}
	else if(true == m_bTile)
	{
		if (CKeyMgr::GetInstance()->Mouse_Pressing(DIMK_LB))
			TilePicking();
	}
	
	if (CKeyMgr::GetInstance()->Mouse_Pressing(DIMK_RB))
	{
		m_bCheck = false;
		m_bMonster = false;
		m_bTile = false;
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
	ObjectSetting();

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

			ImGui::Separator();

			if (ImGui::MenuItem("TileSave"))
				TileSave();

			ImGui::Separator();

			if (ImGui::MenuItem("TileLoad"))
				TileLoad();

			ImGui::Separator();

			if (ImGui::MenuItem("MeshSave"))
				StaticMeshSave();

			ImGui::Separator();

			if (ImGui::MenuItem("MeshLoad"))
				StaticMeshLoad();

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

		else if (ImGui::BeginMenu("Monster"))
		{
			//ImGui::Text("pos%f", m_pCaculator->Get_PickingState().vRayPos);
			m_bMonster = true;
			ImGui::EndMenu();
		}

		else if (ImGui::BeginMenu("Tile"))
		{
			m_bTile = true;
			ImGui::EndMenu();
		}

		else if (ImGui::BeginMenu("StaticMesh"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();

	}

	ImGui::End();
	return S_OK;
}

_bool CImGui_Tool::Picking()
{
	m_bMonster = false;
	m_bTile = false;

	if (ImGui::IsMousePosValid())
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		m_pCaculator->Picking_OnTerrain(g_hWnd, m_pBuffer_Terain, m_pTransform);
		if (true == m_bCheck)
		{
			m_vCubePos = m_pCaculator->Get_PickingState().vRayPos;
			XMStoreFloat3(&fCubePosition, m_vCubePos);
			CCube::CUBESTATE state;
			ZeroMemory(&m_CubeState, sizeof(CCube::CUBESTATE));

			XMStoreFloat3(&m_CubeState.fPos, m_vCubePos);
			m_CubeState.fScale = fCubeScale;
			m_CubeState.transformDesc.fSpeed = 5.f;
			m_CubeState.transformDesc.fRotation = XMConvertToRadians(10);
			m_CubeState.iCubeNum = iCubeNum;

			if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Cube"), TEXT("Layer_Cube"), &m_CubeState)))
				return E_FAIL;

			RELEASE_INSTANCE(CGameInstance);

			m_vecCubeData.push_back(m_CubeState);
			m_bCheck = m_pCaculator->Get_PickingState().bPicking;
			m_bCheck = false;
			return m_bCheck;
		}
	}

}

_bool CImGui_Tool::MonsterPicking()
{
	m_bCheck = false;
	m_bTile = false;
	CMonster::MONSTERSTATE eState;
	ZeroMemory(&eState, sizeof(CMonster::MONSTERSTATE));

	if (ImGui::IsMousePosValid())
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		m_pCaculator->Picking_OnTerrain(g_hWnd, m_pBuffer_Terain, m_pTransform);

		if (true == m_pCaculator->Get_PickingState().bPicking)
		{
			m_vMonsterPos = m_pCaculator->Get_PickingState().vRayPos;

			XMStoreFloat3(&eState.fPos, m_vMonsterPos);
			eState.fScale = fMonScale;
			eState.iMonsterNum = iMonsterNum; 
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

_bool CImGui_Tool::TilePicking()
{
	m_bCheck = false;
	m_bMonster = false;

	if (ImGui::IsMousePosValid())
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		m_pCaculator->Picking_OnTerrain(g_hWnd, m_pBuffer_Terain, m_pTransform);
		if (true == m_pCaculator->Get_PickingState().bPicking)
		{
			m_vTilePos = m_pCaculator->Get_PickingState().vRayPos;

			CTestTile::TILESTATE TileState;
			ZeroMemory(&TileState, sizeof(CTestTile::TILESTATE));

			XMStoreFloat3(&TileState.fPos, m_vTilePos);
			TileState.iTileNum = iTileNum;
			TileState.fScale = fTileScale;

			if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestTile"), TEXT("Layer_Tile"), &TileState)))
				return E_FAIL;

			RELEASE_INSTANCE(CGameInstance);

			m_vecTileData.push_back(TileState);
			m_bTile = m_pCaculator->Get_PickingState().bPicking;
			return m_bTile;
		}
	}
}

_bool CImGui_Tool::MeshCreate()
{
	return _bool();
}

void CImGui_Tool::ObjectSetting()
{
	ImGui::Begin("Setting");

	// 큐브
	fCubePosition = m_CubeState.fPos;
	_float fCubePositions[3] = { fCubePosition.x, fCubePosition.y, fCubePosition.z };
	ImGui::InputFloat3("CubePosition", fCubePositions);
	_float fCubeScales[3] = { fCubeScale.x, fCubeScale.y, fCubeScale.z };
	ImGui::InputFloat3("CubeScale", fCubeScales);
	ImGui::Text("Cube : %d", iCubeNum);

	//몬스터
	ImGui::Separator();
	XMStoreFloat3(&fMonPosition, m_vMonsterPos);
	_float fMonPositions[3] = { fMonPosition.x, fMonPosition.y, fMonPosition.z };
	ImGui::InputFloat3("MonPosition", fMonPositions);
	_float MonScale[3] = { fMonScale.x, fMonScale.y, fMonScale.z };
	ImGui::InputFloat3("MonScale", MonScale);
	ImGui::Text("Monster : %d", iMonsterNum);

	//타일
	ImGui::Separator();
	XMStoreFloat3(&fTilePosition, m_vTilePos);
	_float fPositionsss[3] = { fTilePosition.x, fTilePosition.y, fTilePosition.z };
	ImGui::InputFloat3("TilePosition", fPositionsss);
	_float TileSclae[3] = { fTileScale.x, fTileScale.y, fTileScale.z };
	ImGui::InputFloat3("TileScale", TileSclae);
	ImGui::Text("Tile : %d", iTileNum);


	ImGui::End();

}

void CImGui_Tool::Delete_Object()
{
	ImGui::Button("Delete");

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

void CImGui_Tool::TileSave()
{
	HANDLE hFile = CreateFile(L"../Data/Tile/Tile.dat", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Tile Save Fail");
		return;
	}

	DWORD dwByte = 0;

	for (auto& iter : m_vecTileData)
	{
		WriteFile(hFile, &iter, sizeof(CTestTile::TILESTATE), &dwByte, nullptr);
	}

	CloseHandle(hFile);
}

void CImGui_Tool::TileLoad()
{
	HANDLE hFile = CreateFile(L"../Data/Tile/Tile.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Monster Load Fail");
		return;
	}

	DWORD dwByte = 0;
	CTestTile::TILESTATE TileData;
	while (true)
	{
		ReadFile(hFile, &TileData, sizeof(CTestTile::TILESTATE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		m_vecTileData.push_back(TileData);
	}

	CloseHandle(hFile);

	for (auto& iter : m_vecTileData)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestTile"), TEXT("Layer_Tile"), &iter);
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImGui_Tool::StaticMeshSave()
{
}

void CImGui_Tool::StaticMeshLoad()
{
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
