#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(aiNodeAnim* pAIChannel, class CModel* pModel);
	void Invalidate_Transform(_double TrackPosition, _uint* pCurrKeyFrame, const vector<class CBone*>& Bones);

	void Linear_Transform(_double TrackPosition, _uint* pCurrKeyFrame, const vector<class CBone*>& Bones, vector<class CChannel*> mn_Test, _bool& bCheck, _double fAnimTick);
	KEYFRAME Get_KeyFrame(_uint iInext) { return m_vecKeyFrame[iInext]; }
	//void StringCom();
	char m_szName[MAX_PATH] = "";
private:
	_uint m_iBoneIndex = { 0 };

	_bool m_bCheck = false;

	_double m_Ratio = 6.0;
	_double m_AnimTic = 0.0;

private: 
	_uint m_iNextKeyFrame = { 0 };
	_uint m_iNumKeyFrames = { 0 };
	 vector<KEYFRAME> m_vecKeyFrame;


public:
	static CChannel* Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free()override;
};

END