#include "stdafx.h"
#include "..\Public\PlayerHPBar.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"

#include "Player.h"

CPlayerHPBar::CPlayerHPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerHPBar::CPlayerHPBar(const CPlayerHPBar & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerHPBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHPBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_WorldMatrix,
		XMMatrixScaling(540.f, 20.f, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + 100.f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix,
		XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f));


	return S_OK;
}

void CPlayerHPBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CPlayerHPBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	//_float VertexHpY = 0.f;
	//_float TexHpY = 0.f;
	//CGameInstance* p = GET_INSTANCE(CGameInstance);
	//CGameObject* pPlayer = nullptr;
	//
	//pPlayer = p->Find_Prototype(L"Prototype_GameObject_Player");

	//_float MaxHP = static_cast<CPlayer*>(pPlayer)->Get_Info()._MaxHp;
	//_float HP = static_cast<CPlayer*>(pPlayer)->Get_Info()._Hp;

	//TexHpY = 1.f - HP / MaxHP;
	//VertexHpY = 1 - (2 * TexHpY);

	//m_pVIBuffer_Rect->Set_Buffer(TexHpY, VertexHpY);
	//RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPlayerHPBar::Render()
{
	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHPBar::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HPBar"),
		TEXT("Com_Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHPBar::SetUp_ShaderResource()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CPlayerHPBar * CPlayerHPBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerHPBar * pInstance = new CPlayerHPBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("HPBar Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerHPBar::Clone(void * pArg)
{
	CPlayerHPBar * pInstance = new CPlayerHPBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("HPBar Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerHPBar::Free()
{
	Safe_Release(m_pShaderCom);
	__super::Free();
}