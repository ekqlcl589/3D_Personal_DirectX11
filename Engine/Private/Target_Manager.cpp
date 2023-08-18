#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor)
{
	if (nullptr != Find_Target(pTargetTag))
		return E_FAIL;

	CRenderTarget*	pRenderTarget = CRenderTarget::Create(pDevice, pContext, iWidth, iHeight, eFormat, vColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RTVs.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget*	pRenderTarget = Find_Target(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);
	
	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin(ID3D11DeviceContext* pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRenderTargets[8];

	_uint	iNumViews = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumViews++] = pRenderTarget->Get_RTV();
	}

	/* ·»´õÅ¸°ÙµéÀ» ÀåÄ¡¿¡ ¹ÙÀÎµùÇÏ³®. */
	pContext->OMSetRenderTargets(iNumViews, pRenderTargets, m_pDepthStencilView);	

	return S_OK;
}

HRESULT CTarget_Manager::End(ID3D11DeviceContext* pContext)
{	
	pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::NoneClerBegin(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRenderTargets[8];

	_uint	iNumViews = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		//pRenderTarget->Clear();
		pRenderTargets[iNumViews++] = pRenderTarget->Get_RTV();
	}

	/* ·»´õÅ¸°ÙµéÀ» ÀåÄ¡¿¡ ¹ÙÀÎµùÇÏ³®. */
	pContext->OMSetRenderTargets(iNumViews, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::Set_ShaderResourceView(CShader * pShader, const _tchar * pTargetTag, const char * pContantName)
{
	if (nullptr == pShader)
		return E_FAIL;

	CRenderTarget*		pTarget = Find_Target(pTargetTag);
	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Set_ShaderResourceView(pShader, pContantName);
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pTarget = Find_Target(pTargetTag);
	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render(const _tchar * pMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{


	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)	
		pTarget->Render(pShader, pVIBuffer);	

	return S_OK;
}
#endif // _DEBUG

CRenderTarget * CTarget_Manager::Find_Target(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_RTVs.begin(), m_RTVs.end(), CTag_Finder(pTargetTag));

	if (iter == m_RTVs.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;	
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)		
			Safe_Release(pRenderTarget);

		Pair.second.clear();		
	}

	m_MRTs.clear();

	for (auto& Pair : m_RTVs)
		Safe_Release(Pair.second);

	m_RTVs.clear();

	/*Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);*/
}
