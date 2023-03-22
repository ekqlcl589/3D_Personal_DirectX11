#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)
class CTargetCamera;

class CTSPlayer final : public CGameObject
{
public:
	enum PARTS {PART_HEAD, PART_HAIR_B, PART_HAIR_F, PART_HAIR_S, PART_HAIR_T, PART_TOP, PART_PANTS, PART_CROCS, PART_CROCSR, PART_END};
	enum WEAPON { WEAPON_SS, WEAPON_SHIELD, WEAPON_KARMA14, WEAPON_END };
	enum DIR {FRONT, BACK, DIR_END };
public:
	enum  
	{ COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };

private:
	CTSPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTSPlayer(const CTSPlayer& rhs);
	virtual ~CTSPlayer() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollision(CGameObject* pObj)override;

public:
	void Key_Input(_double TimeDelta);

	void Animation_State(PLAYERANIMSTATE eType, _double TimeDelta);
	void Animation(TSPLAYERANIM eType, _double TimeDelta);

public:
	PLAYERINFO& Get_Info() { return m_tInfo; }

	_float   Lerp(_float Start, _float End, _double Dist) { return (_float)(((1 - Dist)*Start) + (Dist*End)); }
	_float m_dLerpTime = 0.f;

	_bool Get_Attack() { return m_AttackCheck; }
	_bool Get_DownAttack() { return m_DownAttack; }

public:
	void Hit(const _int& _Damage);
	void Damage(const _int& _Damage);

private:
	void Attack();
	void Attack_Combo(_double TimeDelta);
	void Attack_Special(_double TimeDelta);
	void Attack_Special2(_double TimeDelta);

	void Attack_Go(_double TimeDelta);

	void Jump(_double TimeDelta);
	void Jump_Attack(_double TimeDelta);

	void Dash(_double TimeDelta);
	void DashAttack(_double TimeDelta);

	void CombatWait();

	void Evasion(_double TimeDelta);

	void E_Skill(_double TimeDelta);
	void R_Skill(_double TimeDelta);
	void F_Skill(_double TimeDelta);
	void Rage_Skill(_double TimeDelta);

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	vector<CGameObject*> m_vecParts[PART_END];
	vector<CGameObject*> m_vecWeapon[WEAPON_END];
	DIR m_Dir;

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Add_Weapon();
	HRESULT SetUp_ShaderResources();

private:
	PLAYERINFO m_tInfo;

	deque<_uint> m_qMotion;
	_uint m_iAttackCombo[3];
	_uint m_iAnimIndex = 0;

	_double m_AnimDuration = { 0.0 };
	_double m_AnimTimeAcc = { 0.0 };
	_double m_AnimTickPerSecond = { 0.0 };

	_uint m_animation = { 0 };

	_float m_fGravity = 5.f;

private:
	_bool m_WeaponChange = false;
	_bool m_AttackCheck = false;
	_bool m_ComboCheck = false;
	_bool m_ComboCheck2 = false;

	_bool m_bAnimCheck = false;

	_bool m_Evasion = false;
	_bool m_BackEvasion = false;

	_bool m_bJump = false;
	_bool m_JumpAttack = false;
	_bool m_bFall = false;
	_bool m_bLendiongCheck = false;

	_bool m_bCheck = false;
	_bool m_bSIbal = false;

	_float m_fPower = 0.005f;
	_float m_fTime = 0.f;

	_bool m_Hit = false;
	_double m_HitDelay = 0.0;

	_bool m_NoStraight = false;
	
	_float m_ComboTime = 0.f;

	_bool m_bDeah = false;

	_bool m_bStart = false;

	_bool m_bTest = false;

	_bool m_DownAttack = false;

private:
	_bool m_Eskill = false;
	_bool m_RsKill = false;
	_bool m_FsKill = false;
	_bool m_RagesKill = false;


public:
	static CTSPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END