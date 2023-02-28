#include "..\Public\Font_Mgr.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(CFont_Mgr)

CFont_Mgr::CFont_Mgr()
{
}

HRESULT CFont_Mgr::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontName)
{

	if (nullptr != Find_Font(pFontTag))
		return E_FAIL;

	CCustomFont* pFont = CCustomFont::Create(pDevice, pContext, pFontName);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(pFontTag, pFont);

	return S_OK;
}

void CFont_Mgr::Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & vPosition, _fvector vColor, const _float2 & vScale, _float fRadian, const _float2 & vOrigin)
{
	CCustomFont* pFont = Find_Font(pFontTag);
	if (nullptr == pFont)
		return;

	pFont->Render(pText, vPosition, vColor, fRadian, vOrigin, vScale);
}

CCustomFont * CFont_Mgr::Find_Font(const _tchar * pFontTag)
{
	auto iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTag_Finder(pFontTag));

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

void CFont_Mgr::Free()
{
	for (auto& pair : m_Fonts)
		Safe_Release(pair.second);

	m_Fonts.clear();
}
