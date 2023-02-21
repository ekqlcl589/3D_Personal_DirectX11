#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CPlayer_Body final : public CGameObject
{
public:
	enum PARTS {PART_HEAD, PART_HAIR_B, PART_HAIR_F, PART_HAIR_S, PART_HAIR_T, PART_TOP, PART_PANTS, PART_END};
	enum WEAPON { WEAPON_SS, WEAPON_KARMA14, WEAPON_END };

private:
	CPlayer_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Body(const CPlayer_Body& rhs);
	virtual ~CPlayer_Body() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Key_Input(_double TimeDelta);

	void Animation_State(PLAYERANIMSTATE eType, _double TimeDelta);

public:
	PLAYERINFO& Get_Info() { return m_tInfo; }

public:
	void Hit(const _int& _Damage);
	void Attack();
	void Attack_Combo(_double TimeDelta);

public:
	CTransform* Get_Transform();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	vector<CGameObject*> m_vecParts[PART_END];
	vector<CGameObject*> m_vecWeapon[WEAPON_END];
	
private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Add_Weapon();
	HRESULT SetUp_ShaderResources();

private:
	PLAYERINFO m_tInfo;
	
	_double m_AnimDuration = { 0.0 };
	_double m_AnimTimeAcc = { 0.0 };

	_uint m_animation = { 0 };

private:
	_bool m_AttackCheck = false;
	_bool m_ComboCheck = false;

	_float m_ComboTime = 0.f;

public:
	static CPlayer_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END