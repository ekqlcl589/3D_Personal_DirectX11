#include "stdafx.h"

#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Calulator.h"
#include "Texture.h"
#include "Terrain.h"
#include "Cube.h"

#include "Player.h"
#include "Player_Body.h"
#include "Hair.h"
#include "PlayerTop.h"

#include "Weapon.h"

#include "Monster.h"

#include "TestTile.h"

#include "PlayerHPBar.h"
#include "PlayerMPBar.h"
#include "PlayerSkill.h"
#include "PlayerRageSkill.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadEntry(void* pArg) //전역함수는 지역함수의 변수를 받아올 수 없어서 Get함수로 인자를 받아옴 
{
	CLoader* pLoader = ((CLoader*)pArg);

	/*쓰레드에서 그려져야 할 것들을 임계영역으로 진입시킴*/
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
	CoInitializeEx(nullptr, 0); // 스레드에 사용되는 컴객체들 초기화 

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
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGroud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;
	//ㅅㅂ 경로 확인 잘 합시다

	lstrcpy(m_szLoadingText, TEXT("정점버퍼를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다."));

#pragma endregion

#pragma region GAMEOBJECTS
	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중."));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//Snow
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
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

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GUI_Common_gauge_Fill_01_Red_ver2.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GUI_Common_gauge_Fill_01_Blue_ver2.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RageSkill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GUI_Slot_Big_RageSkill_ver2.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Skill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GUI_Slot_Skill_ver2.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Round"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GUI_Round_BG_ver2.png")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("정점버퍼를 로딩중입니다."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델를 로딩중입니다."));

	_matrix		LocalMatrix = XMMatrixIdentity();
	LocalMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *XMMatrixRotationZ(XMConvertToRadians(270.0f));

	/* For.Prototype_Component_Model_Player */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Head/Head.fbx", CModel::MODEL_NONANIM, LocalMatrix))))
		return E_FAIL;

	_matrix		LocalMatrix2 = XMMatrixIdentity();
	LocalMatrix2 = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Body"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Body/Test2.fbx", CModel::MODEL_ANIM, LocalMatrix2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_SS"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Weapon/SS/Sword_NonAnim.fbx", CModel::MODEL_NONANIM, LocalMatrix2))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TestMonster */
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Boss/Ancient_Ston_Golem.fbx", CModel::MODEL_ANIM, LocalMatrix))))
		return E_FAIL;*/

#pragma region HairModel
	_matrix		LocalMatrix3 = XMMatrixIdentity();
	LocalMatrix3 = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));

	_matrix		LocalMatrix5 = XMMatrixIdentity();
	LocalMatrix5 = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, -1.f, 0.f) * XMMatrixRotationZ(XMConvertToRadians(270.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hair_Back"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Head/Hair/Hair3.fbx", CModel::MODEL_NONANIM, LocalMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hair_Front"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Head/Hair/h_f.fbx", CModel::MODEL_NONANIM, LocalMatrix5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hair_Side"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Head/Hair/h_s.fbx", CModel::MODEL_NONANIM, LocalMatrix5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hair_Tail"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Head/Hair/h_t.fbx", CModel::MODEL_NONANIM, LocalMatrix5))))
		return E_FAIL;

#pragma endregion HairModel

#pragma region 애니메이션 옷 파츠

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Top"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Parts/Top/Top2.fbx", CModel::MODEL_NONANIM, LocalMatrix))))
		return E_FAIL;

#pragma endregion 애니메이션 옷 파츠

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tile"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Tile/Test.fbx", CModel::MODEL_NONANIM, LocalMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다."));
	/* For.Prototype_Component_Terrain_Shader */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Terrain_Shader"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/shader_VtxTerrain.hlsl"), VTXNORTEX_DELARATION::Element, VTXNORTEX_DELARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/shader_VtxModel.hlsl"), VTXNONANIMMODEL_DELARATION::Element, VTXNONANIMMODEL_DELARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DELARATION::Element, VTXANIMMODEL_DELARATION::iNumElements))))
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

	lstrcpy(m_szLoadingText, TEXT("충돌체를 로딩중입니다."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중."));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube"),
		CCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Body"),
		CPlayer_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hair"),
		CHair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Top"),
		CPlayerTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HPBar"),
		CPlayerHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MPBar"),
		CPlayerMPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RageSkill"),
		CPlayerRageSkill::Create(m_pDevice, m_pContext))))
		return E_FAIL; 

	_float3 fE = { 765.f, 170.f, 0.f };
	_float3 fR = { 690.f, 170.f, 0.f };
	_float3 fF = { 615.f, 170.f, 0.f };

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_E"),
		CPlayerSkill::Create(m_pDevice, m_pContext, fE))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_R"),
		CPlayerSkill::Create(m_pDevice, m_pContext, fR))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_F"),
		CPlayerSkill::Create(m_pDevice, m_pContext, fF))))
		return E_FAIL;

	// Test Tile
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TestTile"),
		CTestTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

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
