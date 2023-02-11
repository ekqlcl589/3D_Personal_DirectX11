#include "stdafx.h"

#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Texture.h"
#include "Terrain.h"
#include "Monster.h"
#include "Model.h"
#include "Cube.h"

#include "TestTile.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadEntry(void* pArg) //�����Լ��� �����Լ��� ������ �޾ƿ� �� ��� Get�Լ��� ���ڸ� �޾ƿ� 
{
	CLoader* pLoader = ((CLoader*)pArg);

	/*�����忡�� �׷����� �� �͵��� �Ӱ迵������ ���Խ�Ŵ*/
	EnterCriticalSection(pLoader->Get_CriticalSectionPtr());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;

	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlay();
		break;

	default:
		break;
	}

	LeaveCriticalSection(pLoader->Get_CriticalSectionPtr());

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	CoInitializeEx(nullptr, 0); // �����忡 ���Ǵ� �İ�ü�� �ʱ�ȭ 

	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Components */
#pragma region COMPONENTS
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGroud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;
	//���� ��� Ȯ�� �� �սô�

	lstrcpy(m_szLoadingText, TEXT("�������۸� �ε����Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("�𵨸� �ε����Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("���̴��� �ε����Դϴ�."));

#pragma endregion

#pragma region GAMEOBJECTS
	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε���."));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//Snow
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Show"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png")))))
		return E_FAIL;

	//Terrain
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.dds")))))
	//	return E_FAIL;

	//Multi Terrain
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	//Brush
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png")))))
		return E_FAIL;

	//Filter
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.png")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�������۸� �ε����Դϴ�."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�𵨸� �ε����Դϴ�."));
	_matrix		LocalMatrix = XMMatrixIdentity();
	LocalMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Boss/NonAnim.fbx", CModel::MODEL_NONANIM, LocalMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tile"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Tile/Test.fbx", CModel::MODEL_NONANIM, LocalMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���̴��� �ε����Դϴ�."));


	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/shader_VtxModel.hlsl"), VTXNONANIMMODEL_DELARATION::Element, VTXNONANIMMODEL_DELARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Terrain_Shader */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Terrain_Shader"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/shader_VtxTerrain.hlsl"), VTXNORTEX_DELARATION::Element, VTXNORTEX_DELARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Cube_Shader"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/shader_VtxTerrain.hlsl"), VTXNORTEX_DELARATION::Element, VTXNORTEX_DELARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Calculator"),
		CCalculator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����̺�."));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube"),
		CCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Test Tile
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TestTile"),
		CTestTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader * pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Create Fail CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_hThread);
}
