#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollisionMgr :
	public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
private:
	CCollisionMgr();
	virtual ~CCollisionMgr() = default;

public:
	void Update_Collision(_double TimeDelta);


public:
	virtual void Free()override;
};

END