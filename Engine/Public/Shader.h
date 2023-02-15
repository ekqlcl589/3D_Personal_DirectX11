#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final :	public CComponent
{
public:
	typedef struct tagPass
	{
		ID3D11InputLayout* pInputLayout = nullptr;
		ID3DX11EffectPass* pPass = nullptr;
	}PASS;

private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg)override;

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT Set_Matrix(const char* pConstantName, const _float4x4* pMatrix);
	HRESULT Set_MatrixArray(const char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT Set_RawValue(const char* pConstantName, const void* pData, _uint iSize);
	HRESULT Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Set_ShaderResourceViewArray(const char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTexture);

private:
	ID3DX11Effect* m_pEffect = { nullptr };

private:
	_uint m_iNumPasses = { 0 };
	PASS* m_pPasses = { nullptr };

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElements);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;
};

END