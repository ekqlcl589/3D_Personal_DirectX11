#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CBone;
END

BEGIN(Client)

class CCrocs final :
	public CGameObject
{
public:
	typedef struct tagCrocsDesc
	{
		CBone* pBonePtr;
		_float4x4 matParentLocal;
		CTransform* pParentTransform;
	}CROCSDESC;

private:
	CCrocs(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrocs(const CCrocs& rhs);
	virtual ~CCrocs() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	CROCSDESC m_HairParts;
	_float4x4 m_WorldMatrix;

public:
	static CCrocs* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END