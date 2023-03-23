#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)

class CGrudgeWraith final :
	public CMonster
{
private:
	CGrudgeWraith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrudgeWraith(const CGrudgeWraith& rhs);
	virtual ~CGrudgeWraith() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnCollision(CGameObject* pObj)override;
	virtual void EnterCollision(CGameObject* pObj) override;

public:
	void Animation_State(_double TimeDelta);
	void Animation(WRAITHSTATE eType);

	void Use_Skill(_double TimeDelta);
	void Use_Skill_Next(_double TimeDelta);

	void Attack_Go(_double TimeDelta);

private:
	void Run(_double TimeDelta);
	void Hit(const _int & _Damage);

	void Skill01(_double TimeDelta);
	void Skill01_1();
	void Skill01_2();

	void Skill02(_double TimeDelta);
	void Skill03(_double TimeDelta);
	void Skill04(_double TimeDelta);
	void Skill05(_double TimeDelta);
	void Skill07(_double TimeDelta);

public:
	WRAITHINFO Get_Info() { return m_tInfo; }
	void Set_Info(_uint iDamage) { m_tInfo._Hp -= iDamage; }

	_bool Get_Attack() { return m_bAttack; }

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT Add_Coll();

private:
	WRAITHINFO m_tInfo;
	_double m_AnimDuration = 0.0;
	_double m_AnimTimeAcc = 0.0;

	_uint m_iAnimIndex = 0;

	_bool m_bAttack = false;
	_bool m_bWlak = false;

	_float m_SkillDelay = 50.f;

	_bool m_Skill1Pair = false;
	_bool m_SkillNext = false;

	_bool m_bSkill5 = false;
	_bool m_bHit = false;
	_bool m_bGod = false;

	_uint Damage = 0;

public:
	static CGrudgeWraith* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free()override;
};

END