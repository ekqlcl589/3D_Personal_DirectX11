#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Mgr.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Light_Mgr.h"
#include "CollisionMgr.h"
#include "Font_Mgr.h"
#include "Frustum.h"
#include "Target_Manager.h"
#include "KeyMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Mgr::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Mgr::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pTime_Mgr(CTimer_Manager::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLight_Mgr(CLight_Mgr::GetInstance())
	, m_pFont_Mgr(CFont_Mgr::GetInstance())
	, m_pCollision_Mgr(CCollisionMgr::GetInstance())
	, m_pFrustum{ CFrustum::GetInstance() }
	, m_pTarget_Manager{ CTarget_Manager::GetInstance() }
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTime_Mgr);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pLight_Mgr);
	Safe_AddRef(m_pCollision_Mgr);
	Safe_AddRef(m_pFont_Mgr);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, const GRAPHIC_DESC & GraphicDesc, _uint iNumLevels, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppContextOut)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready_Input_Device(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Mgr(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pPipeLine->Initialize()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pPipeLine)
		return E_FAIL;

	m_pInput_Device->SetUp_InputDev();

	m_pLevel_Manager->Tick_Level(TimeDelta);

	m_pObject_Manager->Tick(TimeDelta);
	m_pPipeLine->Tick(TimeDelta);
									  // 맞음    ,  떄림 
	//m_pCollision_Mgr->OnCollisionEnter(OBJ_PLAYER, OBJ_MONSTER_BODY);
	//m_pCollision_Mgr->OnCollisionEnter(OBJ_PLAYER, OBJ_MONSTER_WEAPONL);
	//m_pCollision_Mgr->OnCollisionEnter(OBJ_PLAYER, OBJ_MONSTER_WEAPONR);
	// 
	//m_pCollision_Mgr->OnCollisionEnter(OBJ_MONSTER_BODY, OBJ_WEAPON_KARMA14);

	if (3 == m_pLevel_Manager->Get_LevelIndex())
	{
		m_pCollision_Mgr->Check_Collision(OBJ_MONSTER_BODY, OBJ_WEAPON_KARMA14);
		m_pCollision_Mgr->Check_Collision(OBJ_MONSTER_BODY, OBJ_PLAYER_RAGESKILL);

		m_pCollision_Mgr->Check_Collision(OBJ_PLAYER, OBJ_MONSTER_WEAPONL);
		m_pCollision_Mgr->Check_Collision(OBJ_PLAYER, OBJ_MONSTER_WEAPONR);
		m_pCollision_Mgr->Check_Collision(OBJ_PLAYER, OBJ_MONSTER_PROJECTILE);
	}

	if (4 == m_pLevel_Manager->Get_LevelIndex())// && CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_O))
	{
		m_pCollision_Mgr->Check_Collision(OBJ_PLAYER, OBJ_MONSTER_BALL); // 플레이어, 공 충돌
		m_pCollision_Mgr->Check_Collision(OBJ_PLAYER, OBJ_MONSTER_SICKLE);
		m_pCollision_Mgr->Check_Collision(OBJ_PLAYER, OBJ_NO_COLL);
		m_pCollision_Mgr->Check_Collision(OBJ_PLAYER, OBJ_MONSTER_BLADE);

		//m_pCollision_Mgr->Check_Collision(OBJ_BOSS1, OBJ_PLAYER_RAGESKILL); 시발 이건 걍 버려 무조건 칼로만 데미지 준다 생각하고 넘어가자 
		//m_pCollision_Mgr->Check_Collision(OBJ_BOSS2, OBJ_PLAYER_RAGESKILL);

		m_pCollision_Mgr->Check_Collision(OBJ_BOSS1, OBJ_WEAPON_KARMA14); // 칼, Cursed 충돌
		m_pCollision_Mgr->Check_Collision(OBJ_BOSS2, OBJ_WEAPON_KARMA14); //칼, Grudge 충돌 인데 왜 터지냐 시발
		m_pCollision_Mgr->Check_Collision(OBJ_MONSTER_BALL, OBJ_WEAPON_KARMA14);
		m_pCollision_Mgr->Check_Collision(OBJ_BOSS1, OBJ_MONSTER_BALL); // 칼, Cursed 충돌

	}
	
	m_pObject_Manager->LateTick(TimeDelta); 

	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pCurrentLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pCurrentLevel);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pPrototypeTag, pLayerTag, pArg);
}

HRESULT CGameInstance::Dleate_GameObject(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Dleate_GameObject(iLevelIndex, pLayerTag);
}

CGameObject * CGameInstance::Clone_GameObject_Add_Layer(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject_Add_Layer(pPrototypeTag,  pArg);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

CGameObject * CGameInstance::Find_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(iLevelIndex, pLayerTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Componenet(iLevelIndex, pPrototypeTag, pArg);
}

_float4x4 CGameInstance::Get_Transformfloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transformfloat4x4(eState);
}

_float4x4 CGameInstance::Get_Transformfloat4x4Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();
	return m_pPipeLine->Get_Transformfloat4x4Inverse(eState);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrixInverse(eState);
}

_float4 CGameInstance::Get_CamPos()
{
	if (nullptr == m_pPipeLine)
		return _float4(0.f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPos();
}

_float CGameInstance::Get_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTime_Mgr)
		return 0.f;

	return m_pTime_Mgr->Get_Timer(pTimerTag);
}

void CGameInstance::Set_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTime_Mgr)
		return;

	m_pTime_Mgr->Set_Timer(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTime_Mgr)
		return E_FAIL;
	return m_pTime_Mgr->Ready_Timer(pTimerTag);
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMoveState);
}

HRESULT CGameInstance::Add_Lights(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHT_DESC & LightDesc)
{
	if (nullptr == m_pLight_Mgr)
		return E_FAIL;

	return m_pLight_Mgr->Add_Lights(pDevice, pContext, LightDesc);
}

const LIGHT_DESC * CGameInstance::Get_Light(_uint iIndex)
{
	if (nullptr == m_pLight_Mgr)
		return nullptr;

	return m_pLight_Mgr->Get_Light(iIndex);
}

HRESULT CGameInstance::Add_Collider(COLLISIONSTATE eType, int iNum, CGameObject * pObj)
{
	if(nullptr == m_pCollision_Mgr)
		return E_FAIL;

	return m_pCollision_Mgr->Add_Collider(eType, iNum, pObj);
}

void CGameInstance::OnCollision(COLLISIONSTATE eType, COLLISIONSTATE eType2)
{
	if (nullptr == m_pCollision_Mgr)
		return;

	m_pCollision_Mgr->OnCollision(eType, eType2);
}

HRESULT CGameInstance::Delete_CollideObj(COLLISIONSTATE eObjID, int iNum)
{
	if (nullptr == m_pCollision_Mgr)
		return E_FAIL;

	return m_pCollision_Mgr->Delete_CollideObj(eObjID, iNum);
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontName)
{
	if(nullptr == m_pFont_Mgr)
	return E_FAIL;

	return m_pFont_Mgr->Add_Font(pDevice, pContext, pFontTag, pFontName);
}

void CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & vPosition, _fvector vColor, const _float2 & vScale, _float fRadian, const _float2 & vOrigin)
{
	if (nullptr == m_pFont_Mgr)
		return;

	 m_pFont_Mgr->Render_Font(pFontTag, pText, vPosition, vColor, vScale, fRadian, vOrigin);
}

_bool CGameInstance::isIn_WorldSpace(_fvector vPoint, _float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vPoint, fRadius);
}

HRESULT CGameInstance::Set_ShaderRenderTargetResourceView(CShader * pShader, const _tchar * pTargetTag, const char * pContantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Set_ShaderResourceView(pShader, pTargetTag, pContantName);
}


void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CComponent_Mgr::GetInstance()->DestroyInstance();
	CLevel_Mgr::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CTimer_Manager::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CLight_Mgr::GetInstance()->DestroyInstance();
	CCollisionMgr::GetInstance()->DestroyInstance();
	CFont_Mgr::GetInstance()->DestroyInstance();
	CFrustum::GetInstance()->DestroyInstance();
	CTarget_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTime_Mgr);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pLight_Mgr);
	Safe_Release(m_pCollision_Mgr);
	Safe_Release(m_pFont_Mgr);

	Safe_Release(m_pGraphic_Device);
}
