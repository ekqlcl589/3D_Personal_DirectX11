#pragma once

namespace Engine
{
	/* 그래픽 디바이스를 초기화하기위해 필요한 데이터들 */
	typedef struct tagGraphic_Desc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		WINMODE			eMode;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		HWND			hWnd;

	}GRAPHIC_DESC;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertex_Position_Color
	{
		XMFLOAT3		vPosition;
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Color_Declaration
	{
		const static unsigned int						iNumElements = 1;
		static D3D11_INPUT_ELEMENT_DESC					Elements[1];
	}VTXPOS_DECLARATION;

	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexUV;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexx_Position_TexCoord_Decleartion
	{
		const static unsigned int iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC Element[2];
	}VTXTEX_DELARATION;

	typedef struct tagVertexCube_TEXTURECUBE
	{
		XMFLOAT3			vPosition;	
		XMFLOAT3			vTexUV;

	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertexCube_Position_TexCoord_Decleartion
	{
		const static unsigned int iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC Element[2];
	}VTXCUBE_DELARATION;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexx_Position_Normal_TexCoord_Decleartion
	{
		const static unsigned int iNumElements = 3;
		static D3D11_INPUT_ELEMENT_DESC Element[3];
	}VTXNORTEX_DELARATION;

	typedef struct tagVertex_Point
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;

	typedef struct tagVertex_Matrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVertex_Instance_Declaration
	{
		const static unsigned int						iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC					Elements[6];
	}VTXINSTANCE_DECLARATION;

	typedef struct ENGINE_DLL tagVertex_Point_Instance_Declaration
	{
		const static unsigned int						iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC					Elements[6];
	}VTXPOINTINSTANCE_DECLARATION;

	typedef struct tagLightDesc
	{
		enum LIGHT_TYPE{ LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHT_END};

		LIGHT_TYPE eLightType = LIGHT_END;

		_float4 vDirection; // 방향

		_float4 vPosition; // 점
		float fRange;

		XMFLOAT4 vDiffuse;
		XMFLOAT4 vAmbient;
		XMFLOAT4 vSpecular;


	}LIGHT_DESC;

	typedef struct tagVertex_NonAnim_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXNONANIMMODEL;

	typedef struct ENGINE_DLL tagVertexx_NonAnim_Model_Decleartion
	{
		const static unsigned int iNumElements = 4;
		static D3D11_INPUT_ELEMENT_DESC Element[4];
	}VTXNONANIMMODEL_DELARATION;

	typedef struct tagVertex_Anim_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices; // 최대 4개 까지 뼈의 순서를 저장 시키는 인덱스 
		XMFLOAT4		vBlendWeights; // 첫 번째 블렌드 인덱스에 해당하는 뼈를 몇 퍼센트 이용할 건지를 담아주는 인자값
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexx_Anim_Model_Decleartion
	{
		const static unsigned int iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC Element[6];
	}VTXANIMMODEL_DELARATION;

	typedef struct tagModelMaterial
	{
		class CTexture* pMaterialTexture[AI_TEXTURE_TYPE_MAX] = { nullptr };
	}MODEL_MATERIAL;


	typedef struct tagKeyFrame
	{
		XMFLOAT3 vScale;
		XMFLOAT4 vRotation;
		XMFLOAT3 vPosition;
		double Time;
	}KEYFRAME;

	enum PLAYERANIMSTATE
	{
		ANIM_IDEL,
		ANIM_RUN,
		ANIM_RUN_L,
		ANIM_RUN_R,
		ANIM_RUN_END,
		ANIM_JUMP,
		ANIM_JUMP_ING,
		ANIM_JUMP_LENDING,
		ANIM_JUMP_ATTACK1,
		ANIM_JUMP_ATTACK2,
		ANIM_JUMP_ATTACK3,
		ANIM_ATTACK,
		ANIM_ATTACK_COMBO1,
		ANIM_ATTACK_COMBO2,
		ANIM_ATTACK_COMBO3,
		ANIM_COMBAT_WAIT,
		ANIM_STUN,
		ANIM_SKILL_E,
		ANIM_SKILL_R,
		ANIM_SKILL_F,
		ANIM_SKILL_RAGE,
		ANIM_END
	};
	
	enum TSPLAYERANIM
	{
		TS_JUMP,
		TS_JUMP_DOWN,
		TS_JUMP_LENDING,
		TS_JUMP_UP,
		TS_JUMP_START,
		TS_COMBAT_WAIT,
		TS_DASH,
		TS_DIE,
		TS_RESPWAN,
		TS_RUN_L,
		TS_RUN_R,
		TS_RUN_END,
		TS_STANSUP_ROLLING,
		TS_STANDUP_ATTACK,
		TS_START,
		TS_STURN_LOOP,
		TS_NOENERGY,
		TS_NOENERGY_RUN,
		TS_WAIT,
		TS_WAIT_HABBIT,
		TS_WALK,
		TS_AIR_COMBO01,
		TS_AIR_COMBO02,
		TS_AIR_COMBO03,
		TS_AIR_COMBO04,
		TS_AIR_COMBO04_LENDING,
		TS_AVOID_ATTACK,
		TS_BASIC_COMBO02_END,
		TS_BASIC_COMBO02_LOOP,
		TS_BASIC_COMBO03_START,
		TS_DASHCOMBO,
		TS_SKILL_GROUNDCRASH,
		TS_SKILL_OUTRAGE_END,
		TS_SKILL_OUTRAGE_START,
		TS_SKILL_PRIERCINGRUSH_START_END,
		TS_RAGESKILL_ARMAGEDDONBLADE,
		TS_RAGESKILL_DOUBLESLASH,
		TS_SKILL_ROCKBREAK,
		TS_SPECIALCOMBO_CRASH,
		TS_SPECIALCOMBO_CRASH_READY,
		TS_FRONT_EVASION,
		TS_BACK_EVASION,
		TS_BASIC_COMBO01,
		TS_BASIC_COMBO02,
		TS_BASIC_COMBO03,


		TS_END
	};

	enum STONGOLEMANIMSTATE
	{
		S_STANDUP_F,
		DOWN_F,
		RTDOWN_F,
		RTSTAND_END,
		RTSTAND_LOOP,
		RTSTAND_START,
		S_START,
		S_RESPAN,
		S_RUN,
		S_WAIT,
		S_SKILL01,
		S_SKILL02,
		S_SKILL04_1,
		S_SKILL04_2,
		S_SKILL04_3,
		S_SKILL05_1,
		S_SKILL05_2,
		S_SKILL05_3,
		S_SKILL07,
		S_SKILL09,
		S_SKILL10_1,
		S_SKILL10_2,
		S_SKILL10_3,
		S_RE_SKILL04_1,
		S_RE_SKILL04_2,
		S_RE_SKILL04_3,

		S_ANIMEND
	};

	enum CREATURESTATE
	{
		C_DIE,
		C_RTDOWN_LOOP,
		C_RTDOWN_START,
		C_RTSTAND_END,
		C_RTSTAND_LOOP,
		C_RTSTAND_START,
		C_SKILL01_1,
		C_SKILL01_2,
		C_SKILL02,
		C_SKILL03_L,
		C_SKILL03_R,
		C_SKILL04,
		C_SKILL05,
		C_SKILL07,
		C_SP_SKILL01_1,
		C_SP_SKILL01_2_1,
		C_SP_SKILL01_2_2,
		C_SP_SKILL01_2_3,
		C_SP_SKILL01_END,
		C_SP_SKILL01_WAIT,
		C_SP_SKILL01_WALK,
		C_SP_SKILL02,
		C_SP_SKILL03,
		C_SP_SKILL03_LOOP,
		C_RTDOWN_ATTACK,
		C_START,
		C_WAIT,
		C_WALK,

		C_ANIM_END
	};

	enum WRAITHSTATE
	{
		G_Air,
		G_Avoid,
		G_Down_B_B,
		G_Down_F_F,
		G_DIE,
		G_RTDown_B_B,
		G_RTDown_F_F,
		G_RTStand_Air_Big_B,
		G_RTStand_Air_Big_F,
		G_RTStand_Air_Fail,
		G_RTStand_Air_Lending,
		G_RTStand_Big_F,
		G_Run,
		G_Skill01_1,
		G_Skill01_2,
		G_Skill01_3,
		G_Skill02_1,
		G_Skill02_2,
		G_Skill02_3,
		G_Skill03_1,
		G_Skill03_2,
		G_Skill03_3,
		G_Skill04_1,
		G_Skill04_2,
		G_Skill04_3,
		G_Skill05_1,
		G_Skill07_1,
		G_StandUp_B,
		G_StandUp_F,
		G_Stun_Loop,
		G_Wait,
		G_Walk,

		G_END
	};

	enum CURSEDWRAITHSTATE
	{
		CW_Air,
		CW_Avoid,
		CW_Run,
		CW_Start,
		CW_Wait,
		CW_Walk,
		CW_Down_F_F,
		CW_DIE,
		CW_RTBLOW_AIR_F,
		CW_RTBLOW_AIR_FALL_F,
		CW_RBBLOW_AIR_LENDING,
		CW_RTBLOW_AIR_R,//->X
		CW_RTBLOW_DOWN_F,
		CW_RTCHASE_BIG_F,
		CW_RTDOWN_AIR_LENDING_F_F,//->중복
		CW_RTDOWN_F_F,//->중복
		CW_RTSTAND_BIG_F,//->중복
		CW_SKILL_01,
		CW_SKILL_02,
		CW_SKILL_03,
		CW_SKILL_04,
		CW_SKILL_05,
		CW_SKILL_07,
		CW_SKILL_09_1,
		CW_SKILL_09_2,
		CW_STANDUP_F,
		CW_STUN_LOOP,
		 
		CW_ANIMEND
	};

	enum COLLISIONSTATE
	{
		OBJ_PLAYER,
		OBJ_WEAPON_SS,
		OBJ_WEAPON_SS1,
		OBJ_BOSS1,
		OBJ_WEAPON_KARMA14,
		OBJ_BOSS2,
		OBJ_MONSTER_WEAPONL,
		OBJ_MONSTER_WEAPONR,
		OBJ_MONSTER_BODY,
		OBJ_MONSTER_BALL,
		OBJ_MONSTER_SICKLE,
		OBJ_MONSTER_BLADE,
		OBJ_NO_COLL,
		OBJ_END
	};

	enum TESTSTATE
	{
		S_S_START,
		S_S_DWON,
		S_S_DIE,
		S_S_RESPWAN,
		S_S_RTDOWN,
		S_S_STAND_END,
		S_S_STAND_LOOP,
		S_S_STAND_START,
		S_S_RUN,
		S_S_SKILL01,
		S_S_SKILL02,
		S_S_SKILL04_1,
		S_S_SKILL04_2,
		S_S_SKILL04_3,
		S_S_SKILL05_1,
		S_S_SKILL05_2,
		S_S_SKILL05_3,
		S_S_SKILL07,
		S_S_SKILL09,
		S_S_SKILL10_1,
		S_S_SKILL10_2,
		S_S_SKILL10_3,
		S_S_STANDUP_F,
		S_S_START2,
		S_S_STURN,
		S_S_WAIT,
		S_S_ANIMEND
	};

	typedef struct tagPlayerInfo
	{
		_uint _Lv;

		_float _MaxHp;
		_float _Hp;
		_float _MaxMp;
		_float _Mp;
		_float _MaxExp;

		_float m_ESkill;
		_float m_RSkill;
		_float m_FSkill;
		_float m_RageSkill;

		_uint _ATK;
		_uint _DEF;

		_bool rSkill;
		_bool fSkill;
		_bool rageSkill;

		TSPLAYERANIM CurrAnim;
		TSPLAYERANIM PrevAnim;

	}PLAYERINFO;

	typedef struct tagMonsterInfo
	{
		_float _MaxHp;
		_float _Hp;
		_uint _ATK;
		_uint _DEF;

		STONGOLEMANIMSTATE CurrAnimState;
		STONGOLEMANIMSTATE prevAnimState;

	}MONSTERINFO;

	typedef struct tagGianticCreatureInfo
	{
		_float _MaxHp;
		_float _Hp;
		_uint _ATK;

		CREATURESTATE CurrAnim;
		CREATURESTATE PrevAnim;

	}CREATUREINFO;


	typedef struct tagGrudgeWraithInfo
	{
		_float _MaxHp;
		_float _Hp;
		_uint _ATK;

		WRAITHSTATE CurrAnim;
		WRAITHSTATE PrevAnim;

	}WRAITHINFO;

	typedef struct tagCursedWraithInfo
	{
		_float _MaxHp;
		_float _Hp;
		_uint _ATK;
		_bool _Hit;

		CURSEDWRAITHSTATE CurrAnim;
		CURSEDWRAITHSTATE PrevAnim;

	}CURSEDWRAITHINFO;

	enum class SHAKE_DIRECTION
	{
		RIGHT,
		UP,
		LOOK,
		DIRECTION_END
	};

}


