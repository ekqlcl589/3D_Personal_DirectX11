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


	void Set_AnimationState(STONGOLEMANIMSTATE eType);
	void Set_State(_double TimeDelta);

	//void Set_Skill01(_double TimeDelta);
	//void Set_Skill02(_double TimeDelta);
	//void Set_Skill03(_double TimeDelta);
	void Set_Skill04(_double TimeDelta);
	void Set_Skill05(_double TimeDelta);
	//void Set_Skill06(_double TimeDelta);
	//void Set_Skill07(_double TimeDelta);
	//void Set_Skill08(_double TimeDelta);
	//void Set_Skill09(_double TimeDelta);
	//void Set_Skill10(_double TimeDelta);


private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_uint m_iAnimIndex = 0;
	_bool m_bAttack = false;

	_float m_f = 0.0f;

	queue<_uint> m_AnimIndex;

public:
	static CAncient_StonGolem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free()override;

	STONGOLEMANIMSTATE m_CurrAnim;
	STONGOLEMANIMSTATE m_PrevAnim;
};

END