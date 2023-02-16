#include "stdafx.h"
#include "..\Public\PlayerMPBar.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "Layer.h"
#include "Player.h"

CPlayerMPBar::CPlayerMPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerMPBar::CPlayerMPBar(const CPlayerMPBar & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerMPBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerMPBar::Initialize(void * pArg)
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
		XMMatrixScaling(340.f, 20.f, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + 70.f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix,
		XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f));


	return S_OK;
}

void CPlayerMPBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CPlayerMPBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	_float VertexHpY = 0.f;
	_float TexHpY = 0.f;

	CGameInstance* p = GET_INSTANCE(CGameInstance);
	CGameObject* pPlayer = nullptr;

	//pPlayer = p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"), TEXT("Layer_Player"));
	pPlayer = p->Find_Prototype(TEXT("Prototype_GameObject_Player"));

	_float MaxHP = static_cast<CPlayer*>(pPlayer)->Get_Info()._MaxMp;
	_float HP = static_cast<CPlayer*>(pPlayer)->Get_Info()._Mp;

	if (HP > MaxHP)
		HP = MaxHP;

	if (HP <= 0)
		HP = 0;

	TexHpY = 1.f - HP / MaxHP;
	VertexHpY = 1.f - (2 * TexHpY);

	m_pVIBuffer_Rect->Set_Buffer(TexHpY, VertexHpY);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPlayerMPBar::Render()
{
	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerMPBar::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MPBar"),
		TEXT("Com_Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerMPBar::SetUp_ShaderResource()
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

CPlayerMPBar * CPlayerMPBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerMPBar * pInstance = new CPlayerMPBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("MPBar Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerMPBar::Clone(void * pArg)
{
	CPlayerMPBar * pInstance = new CPlayerMPBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("MPBar Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerMPBar::Free()
{
	Safe_Release(m_pShaderCom);
	__super::Free();
}