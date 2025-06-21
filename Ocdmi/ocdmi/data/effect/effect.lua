-- m_emEndCondition 	-- ��������
END_CONDITION_NONE			= 0	-- ���������� ����.  �ܺο��� �����Ǿ߸� �Ѵ�.
END_CONDITION_AUTO 			= 1	-- m_fNextStepTime Ÿ���� �Ǹ� ���ᰡ �ȴ�.


-- m_emScreenEffectType
SCREENEFFECT_NONE			= 0 -- �ƹ��۾��� ���� �ʴ´�. m_strEgpFile �۾��� ����.
SCREENEFFECT_BASE_TEXTURE	= 1	-- SCREENEFFECT Texture ������.


-- example
-- STEP_DATA
--{
--	m_emEndCondition = END_CONDITION_TIME,				-- ���� ����								( default : END_CONDITION_TIME )
--	m_bEgpInFrontOfEye = false;							-- egp ����. ��ũ�� �տ��� ������ �ǵ��� ��	( default : false )
--	m_strEgpFile = "effect.egp",						-- ���� play �� .epg ����					( default : "" )
--	m_emScreenEffectType = SCREENEFFECT_BASE_TEXTURE,	-- type ����.								( default : SCREENEFFECT_NONE )
--	m_strScreenEffectShader = "shader.fx",				-- ScreeenEffect ���� ���̴� Shader			( default : "" )
--	m_strScreenEffectTexture = "texture.dds",			-- ScreeenEffect ���� ���̴� Texture		( default : "" )
--	m_fScreenEffectCreatingTime = 1,					-- ScreeenEffect �� �����ɶ� �Ѱɸ��½ð�	( default : 0 )
--	m_fScreenEffectDestroyingTime = 1,					-- ScreeenEffect �� ����ɶ� �Ѱɸ��½ð�	( default : 0 )
--	m_fNextStepTime = 1,								-- ���� �������� �̵��ϴ� �ð�				( default : 0 )
--}


-- �ϳ��� EFFECT �� STEP_DATA �� �������� �� �ִ�.
-- STEP_DATA ���� ���������� �߻��� �ϰ� ���� STEP_DATA �� �ҷ������� m_fNextStepTime �� �ð��� �Ǿ�߸� �Ѵ�.
-- �ڽ��� STEP_DATA �� ���������� ����ְ�, 
-- �Ҹ��� ���ؼ� END_CONDITION_TIME ������ m_fEndTime ����, END_CONDITION_NONE ������ �ܺο��� EFFECT �������� �����ϴ�.

-- ReleaseD ������ \data\effect ������ effect.lua ������ ������ ������ ( ReleaseD �� ������ ������ �ִ°͸� �д´�. ),
-- Release ������ \data\effect\effect.rcc �������Ͼȿ� ����־����.


EFFECT_LIST =
{
	-- Effect �̸�
	EFFECT_BROKEN_GLASS =
	{
		-- 1��
		-- effect �� ����ͼ� ȭ�鿡 �΋H��.
		{
			m_emEndCondition = END_CONDITION_AUTO,
			m_strEgpFile = "example_screen_throwstone.egp",
			m_fNextStepTime = 0.3,
		},

		-- 2��
		{
			m_emEndCondition = END_CONDITION_AUTO,
			m_bEgpInFrontOfEye = true;
			m_strEgpFile = "example_screen_flagmentstone.egp",
			m_fNextStepTime = 0.1,
		},

		-- 3��
		-- ȭ�鿡 �ణ�� ���� ����.
		-- �ܺο��� ������ �ָ� 0.5�� ������ ���� �����鼭 �����.
		{
			m_emEndCondition = END_CONDITION_NONE,
			m_emScreenEffectType = SCREENEFFECT_BASE_TEXTURE,
			m_strScreenEffectTexture = "pvp_brickFloorBlending01_A.DDS",	-- �ܼ��ϰ� Texture �� ������.
			m_fScreenEffectDestroyingTime = 0.5,
		},

		-- 4��
		-- shader �� ����Ѵ�.
		-- ���� ������ �ֵ��� �Ѵ�.
		-- �ܺο��� ������ �ָ� 0.5�� ������ ���� �����鼭 �����.
		{
			m_emEndCondition = END_CONDITION_NONE,
			m_emScreenEffectType = SCREENEFFECT_BASE_TEXTURE,
			m_strScreenEffectShader = "ScreenBrokenGrass.fx",				-- shader
			m_strScreenEffectTexture = "weaponph_wand01_s.dds",				-- shader �� ���� �ؽ���.
			m_fScreenEffectDestroyingTime = 0.5,
		},
	},

	-- Effect �̸�
	EFFECT_CAMERA_MOVE = 
	{
		-- 1��
		-- END_CONDITION_NONE �̹Ƿ� �ܺο��� ������ ����� �Ѵ�.
		{
			m_emEndCondition = END_CONDITION_NONE,
			m_strEgpFile = "example_camera.egp",
		},
	},


	-- Effect �̸�
	EFFECT_WATER = 
	{
		-- 1��
		{
			m_emEndCondition = END_CONDITION_NONE,
			m_emScreenEffectType = SCREENEFFECT_BASE_TEXTURE,
			m_strScreenEffectTexture = "FabricFlag01_A.DDS",
		},
	},

	-- Effect �̸�
	EFFECT_DUST = 
	{
		-- 1��
		{
			m_emEndCondition = END_CONDITION_NONE,
			m_emScreenEffectType = SCREENEFFECT_BASE_TEXTURE,
			m_strScreenEffectTexture = "fabricWicker01.DDS",
		},
	},
}