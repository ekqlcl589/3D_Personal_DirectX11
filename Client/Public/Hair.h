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

class CHair final : public CGameObject
{
public:
	enum HAIRTYPE { HAIR_BACK, HAIR_FRONT, HAIR_SIDE, HAIR_TAIL, HAIR_END };
	typedef struct tagHairDesc
	{
		CBone* pBonePtr;
		_float4x4 matParentLocal;
		CTransform* pParentTransform;
		HAIRTYPE HairType;
	}HAIRDESC;

private:
	CHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHair(const CHair& rhs);
	virtual ~CHair() = default;

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
	HAIRDESC m_HairParts;
	_float4x4 m_WorldMatrix;

public:
	static CHair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END