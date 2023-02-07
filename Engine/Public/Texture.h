#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* PTextureFilePath, _uint iNumTexture);
	virtual HRESULT Initialize(void* pArg)override;

	HRESULT SetUp_ShaderResource(class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);
	HRESULT SetUp_ShaderResourceArray(class CShader* pShader, const char* pConstantName);

private:
	vector<ID3D11ShaderResourceView*> m_Texture;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* PTextureFilePath, _uint iNumTexture = 1);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;
};

END