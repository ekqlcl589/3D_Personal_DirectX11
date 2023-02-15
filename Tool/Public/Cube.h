#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class CVIBuffer_Cube;
class CVIBuffer_Terrain;
class CShader;
class CTransform;
class CTexture;
class CRenderer;
class CCalculator;

END

BEGIN(Tool)

class CCube final : public CGameObject
{
public:
	typedef struct tagCubeState
	{
		_float3 fPos;
		_int iCubeNum;
		_float3 fScale;

		CTransform::TRANSFORM_DESC transformDesc;
	}CUBESTATE;

private:
	CCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCube(const CCube& rhs);
	virtual ~CCube() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render(void) override;

	void Debug();

public:
	CUBESTATE Get_CubeState() { return m_CubeState; }

private:
	HRESULT	Add_Components(void);
	HRESULT SetUp_ShaderResource();

private:
	CVIBuffer_Cube* m_pBufferCom = nullptr;
	CVIBuffer_Terrain* m_pVIBuffer_Terrain = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CCalculator* m_pCCalculatorCom = nullptr;

private:
	CUBESTATE m_CubeState;
	_uint iId = { 0 };
	_vector m_vPos;

public:
	static CCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;

	virtual void Free() override;

};

END