#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CAncient_StonGolem final : public CMonster
{
private:
	CAncient_StonGolem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAncient_StonGolem(const CAncient_StonGolem& rhs);
	virtual ~CAncient_StonGolem() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollision(CGameObject* pObj, _bool* pColl)override;

public:
	HRESULT Add_Coll();

	void Set_AnimationState(STONGOLEMANIMSTATE eType);
	_uint Set_State(_double TimeDelta);

public:
	void Run(_double TimeDelta);
	void Set_Skill01(_double TimeDelta);
	void Set_Skill02(_double TimeDelta);
	//void Set_Skill03(_double TimeDelta);
	void Set_Skill04(_double TimeDelta);
	void Set_Skill05(_double TimeDelta);
	//void Set_Skill06(_double TimeDelta);
	void Set_Skill07(_double TimeDelta);
	//void Set_Skill08(_double TimeDelta);
	void Set_Skill09(_double TimeDelta);
	//void Set_Skill10(_double TimeDelta);

public:
	HRESULT Add_Effect();
	void Set_Time();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_uint m_iAnimIndex = 0;
	_bool m_bAttack = false;
	_bool m_bRespwan = false;
	_float m_f = 0.0f;

	queue<_uint> m_AnimIndex;

	_bool m_bStart = false;

	_double m_fTime = 0.0;
	_bool m_bAttackTime = false;


	_double m_AnimDuration = { 0.0 };
	_double m_AnimTimeAcc = { 0.0 };

	_bool m_bSkill4 = false;

	_double m_CoolTime = 0.0;

public:
	static CAncient_StonGolem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free()override;

	STONGOLEMANIMSTATE m_CurrAnim;
	STONGOLEMANIMSTATE m_PrevAnim;
};

END