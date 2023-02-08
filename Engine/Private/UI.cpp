#include "..\Public\UI.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "PipeLine.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{

	return S_OK;
}

void CUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CUI::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer_Rect->Render();

	return S_OK;
}


void CUI::Free()
{
	__super::Free();
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer_Rect);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
}
