#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "halls_of_reflection.h"
#include "LFGMgr.h"
#include "Group.h"

#define MAX_ENCOUNTER_TIME                    325 * 1000
enum
{
    /*INTRO - Pre Uther*/
    SAY_JAINA_INTRO_01                 = -1668001,
    SAY_SYLVANA_INTRO_01               = -1668021,
    SAY_JAINA_INTRO_02                 = -1668002,
    SAY_JAINA_INTRO_03                 = -1668003,
    SAY_SYLVANA_INTRO_03               = -1668022,
    SAY_JAINA_INTRO_04                 = -1668004,
    SAY_SYLVANA_INTRO_04               = -1668023,

    /*INTRO - Uther Dialog*/
    SAY_UTHER_A_01                     = -1668005, 
    SAY_UTHER_H_01                     = -1668024,
    SAY_JAINA_02                       = -1668006,
    SAY_SYLVANA_02                     = -1668025,
    SAY_UTHER_A_03                     = -1668007,
    SAY_UTHER_H_03                     = -1668026,
    SAY_JAINA_04                       = -1668008,
    SAY_SYLVANA_04                     = -1668027,
    SAY_UTHER_A_05                     = -1668009,
    SAY_UTHER_H_05                     = -1668028,
    SAY_JAINA_06                       = -1668010,
    SAY_SYLVANA_06                     = -1668029,
    SAY_UTHER_A_07                     = -1668011,
    SAY_UTHER_H_07                     = -1668030,
    SAY_JAINA_08                       = -1668012,
    SAY_SYLVANA_08                     = -1668031,
    SAY_UTHER_A_09                     = -1668013,
    SAY_UTHER_H_09                     = -1668032,
    SAY_JAINA_10                       = -1668014,
    SAY_UTHER_A_11                     = -1668015,
    SAY_UTHER_H_11                     = -1668033,
    SAY_JAINA_12                       = -1668017,
    SAY_SYLVANA_12                     = -1668034,
    SAY_UTHER_A_13                     = -1668016,
    SAY_UTHER_A_14                     = -1668018,
    SAY_JAINA_15                       = -1668019,

    /*INTRO - Lich King Arrive*/
    SAY_UTHER_A_16                     = -1668020,
    SAY_UTHER_H_16                     = -1668035,
    SAY_LICH_KING_17                   = -1668036,
    SAY_LICH_KING_18                   = -1668037,
    SAY_LICH_KING_19                   = -1668038,
    SAY_JAINA_20                       = -1668042,
    SAY_SYLVANA_20                     = -1668043,
    SAY_LICH_KING_A_21                 = -1594473,//
    SAY_LICH_KING_H_21                 = -1594474,//
    SAY_FALRIC_INTRO                   = -1668039,
    SAY_MARWYN_INTRO                   = -1668040,
    SAY_FALRIC_INTRO2                  = -1668041,

    /*INTRO - Pre Escape*/
    SAY_LICH_KING_AGGRO_A              = -1594477,
    SAY_LICH_KING_AGGRO_H              = -1594478,
    SAY_JAINA_AGGRO                    = -1594479,
    SAY_SYLVANA_AGGRO                  = -1594480,

    /*ESCAPE*/
    SAY_JAINA_WALL_01                  = -1594487,
    SAY_SYLVANA_WALL_01                = -1594488,
    SAY_JAINA_WALL_02                  = -1594489,
    SAY_SYLVANA_WALL_02                = -1594490,
    SAY_LICH_KING_WALL_02              = -1594491,
    SAY_LICH_KING_WALL_03              = -1594492,
    SAY_LICH_KING_WALL_04              = -1594493,
    SAY_JAINA_WALL_03                  = -1594494,
    SAY_JAINA_WALL_04                  = -1594495,
    SAY_SYLVANA_WALL_03                = -1594496,
    SAY_SYLVANA_WALL_04                = -1594497,
    SAY_JAINA_ESCAPE_01                = -1594498,
    SAY_JAINA_ESCAPE_02                = -1594499,
    SAY_SYLVANA_ESCAPE_01              = -1594500,
    SAY_SYLVANA_ESCAPE_02              = -1594501,
    SAY_JAINA_TRAP                     = -1594502,
    SAY_SYLVANA_TRAP                   = -1594503,
    SAY_MATHEAS_JAINA                  = -1594505,
    SAY_LICH_KING_END_01               = -1594506,
    SAY_LICH_KING_END_02               = -1594507,
    SAY_LICH_KING_END_03               = -1594508,

    /*SPELLS AND VISUAL EFFECTS*/
    SPELL_TAKE_FROSTMOURNE             = 72729,
    SPELL_FROSTMOURNE_DESPAWN          = 72726,
    SPELL_FROSTMOURNE_SOUNDS           = 70667,
    SPELL_CAST_VISUAL                  = 65633,  //Jaina And Sylavana cast this when summon uther.
    SPELL_BOSS_SPAWN_AURA              = 72712,  //Falric and Marwyn
    SPELL_UTHER_DESPAWN                = 70693,
    SPELL_WINTER                       = 69780,
    SPELL_FURY_OF_FROSTMOURNE          = 70063,
    SPELL_SOUL_REAPER                  = 73797,
    SPELL_RAISE_DEAD                   = 69818,
    SPELL_ICE_PRISON                   = 69708,
    SPELL_ICE_PRISON_VISUAL            = 69708,
    SPELL_DARK_ARROW                   = 70194,
    SPELL_ICE_BARRIER                  = 69787,
	SPELL_EVASION                      = 70188,
    SPELL_DESTROY_ICE_WALL_01          = 69784, //Jaina
    SPELL_DESTROY_ICE_WALL_02          = 70224, 
    SPELL_DESTROY_ICE_WALL_03          = 70225, //Sylvana
    SPELL_SUMMON_ICE_WALL              = 69784,
    SPELL_SYLVANA_JUMP                 = 68339,
    SPELL_SYLVANA_STEP                 = 69087,
    SPELL_SILENCE                      = 69413,
    SPELL_LICH_KING_CAST               = 57561,
    SPELL_FROSTMOURNE_VISUAL           = 73220,
    SPELL_SHIELD_DISRUPTION            = 58291,

    FACTION                            = 2076,
};

static Position LeaderStartPosition = {5577.187f, 2236.003f, 733.012f, 0.0f};

enum achivement
{
ACHIEV_HOR_H                        =4521,
ACHIEV_HOR                          =4518,
ACHIEV_NOT_RETREATING               =4526
};

class npc_uther : public CreatureScript
{
public:
    npc_uther() : CreatureScript("npc_uther") { }

	struct npc_utherAI : public NullCreatureAI
    {
        npc_utherAI(Creature *pCreature) : NullCreatureAI(pCreature) { }

		EventMap events;

		void DoAction(int32 const action)
		{
			switch (action)
			{
				case ACTION_START_FIGHT:
					Talk(0);
					events.Reset();
					events.ScheduleEvent(EVENT_START_QUELDELLAR, 5000);
					break;
				case ACTION_OUTRO_UTHER:
					events.Reset();
					events.ScheduleEvent(EVENT_OUTRO_1, 5000);
					break;
			}
		}

        void UpdateAI(const uint32 diff)
        {
			events.Update(diff);

			while (uint32 e = events.ExecuteEvent())
			{
				switch (e)
				{
					case EVENT_START_QUELDELLAR:
						Talk(1);
						me->SummonCreature(NPC_QUEL_DELLAR, QuelDellarPosition)->AI()->DoAction(ACTION_START_FIGHT);
						break;

					case EVENT_OUTRO_1:
						Talk(2);
						events.ScheduleEvent(EVENT_OUTRO_2, 10000);
						break;
					case EVENT_OUTRO_2:
						Talk(3);
						events.ScheduleEvent(EVENT_OUTRO_3, 7000);
						break;
					case EVENT_OUTRO_3:
						Talk(4);
						events.ScheduleEvent(EVENT_OUTRO_4, 10000);
						break;
					case EVENT_OUTRO_4:
						Talk(5);
						events.ScheduleEvent(EVENT_OUTRO_5, 6000);
						break;
					case EVENT_OUTRO_5:
						me->GetInstanceScript()->SetData(TYPE_QUEL_DALLAR, 0);
						me->DespawnOrUnsummon(100);
						break;
				}
			}
        }
    };
    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_utherAI(pCreature);
    }
};

class npc_quel_dellar : public CreatureScript
{
public:
    npc_quel_dellar() : CreatureScript("npc_quel_dellar") { }

	struct npc_quel_dellarAI : public ScriptedAI
    {
        npc_quel_dellarAI(Creature *pCreature) : ScriptedAI(pCreature) { }

		EventMap events;

		void DoAction(int32 const action)
		{
			switch (action)
			{
				case ACTION_START_FIGHT:
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
					me->GetMotionMaster()->MoveSmoothPath(1, me->GetEntry(), 10.0f, true);
					break;
			}
		}

		void MovementInform(uint32 type, uint32 id)
		{
			if (id == 1)
			{
				events.Reset();
				events.ScheduleEvent(EVENT_START_ATTACK, 2000);
			}
		}

		void JustDied(Unit* killer)
		{
			if (me->ToTempSummon())
				if (Unit* summoner = me->ToTempSummon()->GetSummoner())
					summoner->GetAI()->DoAction(ACTION_OUTRO_UTHER);
		}

        void UpdateAI(const uint32 diff)
        {
			events.Update(diff);

			while (uint32 e = events.ExecuteEvent())
			{
				switch (e)
				{
					case EVENT_START_ATTACK:
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
						me->setFaction(14);
						me->SetReactState(REACT_AGGRESSIVE);
						Talk(0);
						events.ScheduleEvent(EVENT_QUELDELLAR_BLADESTORM, 5000);
						events.ScheduleEvent(EVENT_QUELDELLAR_HEROIC_STRIKE, 1000);
						events.ScheduleEvent(EVENT_QUELDELLAR_MORTAL_STRIKE, 2500);
						break;
					case EVENT_QUELDELLAR_BLADESTORM:
						if (me->getVictim())
							DoCastAOE(SPELL_QUELDELLAR_BLADESTORM);
						events.ScheduleEvent(EVENT_QUELDELLAR_BLADESTORM, 20000);
						break;
					case EVENT_QUELDELLAR_HEROIC_STRIKE:
						if (me->getVictim())
							DoCastVictim(SPELL_QUELDELLAR_HEROIC_STRIKE);
						events.ScheduleEvent(EVENT_QUELDELLAR_HEROIC_STRIKE, 5000);
						break;
					case EVENT_QUELDELLAR_MORTAL_STRIKE:
						if (me->getVictim())
							DoCastVictim(SPELL_QUELDELLAR_MORTAL_STRIKE);
						events.ScheduleEvent(EVENT_QUELDELLAR_MORTAL_STRIKE, 7500);
						break;
				}
			}

			if (UpdateVictim())
				DoMeleeAttackIfReady();
        }
    };
    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_quel_dellarAI(pCreature);
    }
};

class npc_jaina_and_sylvana_HRintro : public CreatureScript
{
public:
    npc_jaina_and_sylvana_HRintro() : CreatureScript("npc_jaina_and_sylvana_HRintro") { }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
    {
        InstanceScript* m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();

        if (!m_pInstance)
            return false;

        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                pPlayer->CLOSE_GOSSIP_MENU();
				if (m_pInstance->GetData(TYPE_EVENT) == IN_PROGRESS || m_pInstance->GetData(TYPE_EVENT) == FAIL)
					CAST_AI(npc_jaina_and_sylvana_HRintroAI, pCreature->AI())->StartEvent(true);
                else
					CAST_AI(npc_jaina_and_sylvana_HRintroAI, pCreature->AI())->StartEvent();
				m_pInstance->SetData(TYPE_EVENT, IN_PROGRESS);
                break;
        }

        if(pPlayer->GetTeam() == ALLIANCE) 
            m_pInstance->SetData(DATA_LIDER, 1);
        else
            m_pInstance->SetData(DATA_LIDER, 2);

        m_pInstance->SetData64(DATA_ESCAPE_LIDER,pCreature->GetGUID());

        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        InstanceScript* m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();

        if(pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu( pCreature->GetGUID());

		if (!m_pInstance->GetData(TYPE_QUEL_DALLAR))
			switch(pCreature->GetEntry())
			{
				case NPC_JAINA:
					pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lady Jaina, we are ready for next mission!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
					break;
				case NPC_SYLVANA: 
					pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lady Sylvanas, we are ready for next mission!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
					break;
			}

        pPlayer->PlayerTalkClass->SendGossipMenu(907,pCreature->GetGUID());
        return true;
    }

    struct npc_jaina_and_sylvana_HRintroAI : public ScriptedAI
    {
        npc_jaina_and_sylvana_HRintroAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
            m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();
            Reset();
        }

        InstanceScript* m_pInstance;

        uint32 StepTimer;
        uint32 Step;
        uint64 m_uiFrostmourneGUID;
        uint64 m_uiMainGateGUID;
        uint64 m_uiFalricGUID;
        uint64 m_uiMarwynGUID;
        Creature* pUther;
        Creature* pLichKing;
        bool Small;

        void Reset()
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            Small = false;
			me->SetVisible(true);
			m_pInstance->SetData(TYPE_PHASE, 0); 
        }

        void StartEvent(bool small = false)
        {
            if(!m_pInstance)
                return;
            m_pInstance->SetData(TYPE_PHASE, 1);
            Step = 1;
            StepTimer = 100;
			Small = small;
        }

        void JumpNextStep(uint32 Time)
        {
            StepTimer = Time;
            Step++;
        }

        void Event()
        {
            switch(Step)
            {
                case 1:
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    JumpNextStep(2000);
                    break;
                case 2:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_INTRO_01, me);
                        JumpNextStep(5000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_SYLVANA_INTRO_01, me);
                        JumpNextStep(8000);
                    }
                    break;
                case 3: 
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_INTRO_02, me);
                        JumpNextStep(5000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                        JumpNextStep(500);
                    break;
                case 4:
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    me->GetMotionMaster()->MovePoint(0, 5307.031f, 1997.920f, 709.341f);
                    JumpNextStep(10000);
                    break;
                case 5:
                    if(Creature* pTarget = me->SummonCreature(NPC_ALTAR_TARGET,5309.374f,2006.788f,711.615f,1.37f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000))
                    {
                        me->SetUInt64Value(UNIT_FIELD_TARGET, pTarget->GetGUID());
                        pTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    }
                    JumpNextStep(1000);
                    break;
                case 6:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_INTRO_03, me);
                        JumpNextStep(5000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_SYLVANA_INTRO_03, me);
                        JumpNextStep(6000);
                    }
                    break;
                case 7:
                    DoCast(me, SPELL_CAST_VISUAL);
                    if(me->GetEntry() == NPC_JAINA)
                        DoScriptText(SAY_JAINA_INTRO_04, me);
                    if(me->GetEntry() == NPC_SYLVANA)
                        DoScriptText(SAY_SYLVANA_INTRO_04, me);
                    JumpNextStep(3000);
                    break;
                case 8:
                    DoCast(me, SPELL_FROSTMOURNE_SOUNDS);
                    m_uiFrostmourneGUID = m_pInstance->GetData64(GO_FROSTMOURNE);
                    if(GameObject* pFrostmourne = m_pInstance->instance->GetGameObject(m_uiFrostmourneGUID))
                    {
                        pFrostmourne->SetGoState(GO_STATE_ACTIVE);
                    }
                    if(me->GetEntry() == NPC_JAINA)
                        JumpNextStep(12000);
                    if(me->GetEntry() == NPC_SYLVANA)
                        JumpNextStep(8000);
                    break;
                case 9:
                    if(Creature* Uther = me->SummonCreature(NPC_UTHER,5308.228f,2003.641f,709.341f,4.17f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,300000))
                    {
                        pUther = Uther;
                        Uther->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
                        me->SetUInt64Value(UNIT_FIELD_TARGET, Uther->GetGUID());
                        if(me->GetEntry() == NPC_JAINA)
                        {
                            DoScriptText(SAY_UTHER_A_01, Uther);
                            JumpNextStep(3000);
                        }
                        if(me->GetEntry() == NPC_SYLVANA)
                        {
                            DoScriptText(SAY_UTHER_H_01, Uther);
                            JumpNextStep(10000);
                        }
                    }
                    break;
                case 10:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_02, me);
                        JumpNextStep(5000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_SYLVANA_02, me);
                        JumpNextStep(3000);
                    }
                    break;
                case 11:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                    {
                        DoScriptText(SAY_UTHER_A_03, pUther);
                        JumpNextStep(7000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA && pUther)
                    {
                        DoScriptText(SAY_UTHER_H_03, pUther);
                        JumpNextStep(6000);
                    }
                    if(Small)
                        Step = 24;
                    break;
                case 12:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_04, me);
                        JumpNextStep(2000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_SYLVANA_04, me);
                        JumpNextStep(5000);
                    }
                    break;
                case 13:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                    {
                        DoScriptText(SAY_UTHER_A_05, pUther);
                        JumpNextStep(10000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA && pUther)
                    {
                        DoScriptText(SAY_UTHER_H_05, pUther); 
                        JumpNextStep(19000);
                    }
                    break;
                case 14:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_06, me);
                        JumpNextStep(6000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_SYLVANA_06, me); 
                        JumpNextStep(2000);
                    }
                    break;
                case 15:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                    {
                        DoScriptText(SAY_UTHER_A_07, pUther); 
                        JumpNextStep(12000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA && pUther)
                    {
                        DoScriptText(SAY_UTHER_H_07, pUther);
                        JumpNextStep(20000);
                    }
                    break;
                case 16:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_08, me); 
                        JumpNextStep(6000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_SYLVANA_08, me);
                        JumpNextStep(3000);
                    }
                    break;
                case 17:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                    {
                        DoScriptText(SAY_UTHER_A_09, pUther);
                        JumpNextStep(12000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA && pUther)
                    {
                        DoScriptText(SAY_UTHER_H_09, pUther);
                        JumpNextStep(11000);
                    }
                    break;
                case 18:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_10, me);
                        JumpNextStep(11000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        JumpNextStep(100);
                    }
                    break;
                case 19:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                    {
                        DoScriptText(SAY_UTHER_A_11, pUther);
                        JumpNextStep(24000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA && pUther)
                    {
                        DoScriptText(SAY_UTHER_H_11, pUther);
                        JumpNextStep(9000);
                    }
                    break;
                case 20:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_12, me);
                        JumpNextStep(2000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_SYLVANA_12, me);
                        JumpNextStep(2100);
                    }
                    break;
                case 21:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                    {
                        DoScriptText(SAY_UTHER_A_13, pUther); 
                        JumpNextStep(5000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        JumpNextStep(100);
                    }
                    break;
                case 22:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                    {
                        DoScriptText(SAY_UTHER_A_14, pUther); 
                        JumpNextStep(12000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        JumpNextStep(100);
                    }
                    break;
                case 23:
                    if(me->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_JAINA_15, me);
                        JumpNextStep(2000);
                    }
                    if(me->GetEntry() == NPC_SYLVANA)
                    {
                        JumpNextStep(100);
                    }
                    break;
                case 24:
                    if(me->GetEntry() == NPC_JAINA && pUther)
                        DoScriptText(SAY_UTHER_A_16, pUther);
                    if(me->GetEntry() == NPC_SYLVANA && pUther)
                        DoScriptText(SAY_UTHER_H_16, pUther);
                    m_uiMainGateGUID = m_pInstance->GetData64(GO_IMPENETRABLE_DOOR);
                    if(GameObject* pGate = m_pInstance->instance->GetGameObject(m_uiMainGateGUID))
                    {
                        pGate->SetGoState(GO_STATE_ACTIVE);
                    }
                    if(Creature* LichKing = me->SummonCreature(NPC_LICH_KING,5362.469f,2062.342f,707.695f,3.97f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,360000))
                    {
                        pLichKing = LichKing;
                        LichKing->setActive(true);
                    }
                    JumpNextStep(1000);
                    break;
                case 25:
                    if(pUther)
                        pUther->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);
                    if(pLichKing)
                    {
                        pLichKing->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pLichKing->AddUnitMovementFlag(MOVEMENTFLAG_WALKING); 
                        pLichKing->GetMotionMaster()->MovePoint(0, 5314.881f, 2012.496f, 709.341f);
                    }
                    JumpNextStep(3000);
                    break;
                case 26:
                    if(GameObject* pGate = m_pInstance->instance->GetGameObject(m_uiMainGateGUID))
                        pGate->SetGoState(GO_STATE_READY);
                    JumpNextStep(7000);
                    break;
                case 27:
                    if(pUther)
                        pUther->CastSpell(pUther, SPELL_UTHER_DESPAWN, false);
                    JumpNextStep(500);
                    break;
                case 28:
                    if(pLichKing)
                        DoScriptText(SAY_LICH_KING_17, pLichKing);
                    JumpNextStep(10000);
                    break;
                case 29:
                    if(pLichKing)
                        DoScriptText(SAY_LICH_KING_18, pLichKing);
                    JumpNextStep(5000);
                    break;
                case 30:
                    if(pLichKing)
                        pLichKing->CastSpell(pLichKing, SPELL_TAKE_FROSTMOURNE, false);
                    if(GameObject* pFrostmourne = m_pInstance->instance->GetGameObject(m_uiFrostmourneGUID))
                        pFrostmourne->SetGoState(GO_STATE_READY);
                    JumpNextStep(1500);
                    break;
                case 31:
                    if(GameObject* pFrostmourne = m_pInstance->instance->GetGameObject(m_uiFrostmourneGUID))
                        pFrostmourne->SetPhaseMask(0, true);
                    if(pLichKing)
                        pLichKing->CastSpell(pLichKing, SPELL_FROSTMOURNE_VISUAL, false);
                    me->RemoveAurasDueToSpell(SPELL_FROSTMOURNE_SOUNDS);
                    JumpNextStep(5000);
                    break;
                case 32:
                    if(pLichKing)
                        DoScriptText(SAY_LICH_KING_19, pLichKing);
                    JumpNextStep(9000);
                    break;
                case 33:
                    m_uiFalricGUID = m_pInstance->GetData64(NPC_FALRIC);
                    m_uiMarwynGUID = m_pInstance->GetData64(NPC_MARWYN);
                    if(Creature* Falric = m_pInstance->instance->GetCreature(m_uiFalricGUID))
                    {
                        Falric->RemoveAllAuras();
                        Falric->SetVisible(true);
                        Falric->CastSpell(Falric, SPELL_BOSS_SPAWN_AURA, false);
                        Falric->GetMotionMaster()->MovePoint(0, 5283.309f, 2031.173f, 709.319f);
                    }
                    if(Creature* Marwyn = m_pInstance->instance->GetCreature(m_uiMarwynGUID))
                    {
                        Marwyn->RemoveAllAuras();
                        Marwyn->SetVisible(true);
                        Marwyn->CastSpell(Marwyn, SPELL_BOSS_SPAWN_AURA, false);
                        Marwyn->GetMotionMaster()->MovePoint(0, 5335.585f, 1981.439f, 709.319f);
                    }
                    if(pLichKing)
                        pLichKing->GetMotionMaster()->MovePoint(0, 5402.286f, 2104.496f, 707.695f);
                    JumpNextStep(600);
                    break;
                case 34:
                    if(Creature* Falric = (Unit::GetCreature(*me, m_uiFalricGUID)))
                        DoScriptText(SAY_FALRIC_INTRO, Falric);
                    if(Creature* Marwyn = (Unit::GetCreature(*me, m_uiMarwynGUID)))
                        DoScriptText(SAY_MARWYN_INTRO, Marwyn);
                    JumpNextStep(3000);
                    break;
                case 35:
                    if(GameObject* pGate = m_pInstance->instance->GetGameObject(m_uiMainGateGUID))
                        pGate->SetGoState(GO_STATE_ACTIVE);
                    if(Creature* Falric = (Unit::GetCreature(*me, m_uiFalricGUID)))
                        DoScriptText(SAY_FALRIC_INTRO2, Falric);
                    m_pInstance->SetData(TYPE_FALRIC, SPECIAL);
                    JumpNextStep(4000);
                    break;
                case 36:
                    if(me->GetEntry() == NPC_JAINA)
                        DoScriptText(SAY_JAINA_20, me);
                    if(me->GetEntry() == NPC_SYLVANA)
                        DoScriptText(SAY_SYLVANA_20, me);
                    me->GetMotionMaster()->MovePoint(0, 5443.880f, 2147.095f, 707.695f);
                    JumpNextStep(4000);
                    break;
                case 37:
                    if(pLichKing)
                    {
                        pLichKing->GetMotionMaster()->MovementExpired(false);
                        pLichKing->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        pLichKing->GetMotionMaster()->MovePoint(0, 5443.880f, 2147.095f, 707.695f);
                    }
                    if(me->GetEntry() == NPC_JAINA && pLichKing)
                        DoScriptText(SAY_LICH_KING_A_21, pLichKing);
                    if(me->GetEntry() == NPC_SYLVANA && pLichKing)
                        DoScriptText(SAY_LICH_KING_H_21, pLichKing);
                    JumpNextStep(8000);
                    break;
                case 38:
                    if(GameObject* pGate = m_pInstance->instance->GetGameObject(m_uiMainGateGUID))
                        pGate->SetGoState(GO_STATE_READY); 
                    JumpNextStep(5000);
                    break;
                case 39:
                    me->SetVisible(false);
                    if(pLichKing)
                        pLichKing->SetVisible(false);
                    m_pInstance->SetData(TYPE_PHASE, 2);  
                    JumpNextStep(1000);
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(!m_pInstance)
                return;

            /*if(m_pInstance->GetData(TYPE_EVENT) == 1
                &&  m_pInstance->GetData64(DATA_ESCAPE_LIDER) == me->GetGUID())
                StartEvent();

            if(m_pInstance->GetData(TYPE_EVENT) == 2
                &&  m_pInstance->GetData64(DATA_ESCAPE_LIDER) == me->GetGUID())
            {
                Small = true;
                StartEvent(true);
            }*/

            if(StepTimer < diff && m_pInstance->GetData(TYPE_PHASE) == 1)
                Event();
            else StepTimer -= diff;

            return;
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_jaina_and_sylvana_HRintroAI(pCreature);
    }

};

Position const IceWallTargetPosition[] =
{
	{ 5546.515137f, 2084.362305f, 734.209045f, 0.0f },
	{ 5499.063965f, 1976.363647f, 739.632690f, 0.0f },
	{ 5436.650391f, 1881.708374f, 754.394165f, 0.0f },
	{ 5324.894043f, 1756.869507f, 773.035217f, 0.0f },
	/*
    { 5547.833f, 2083.701f, 731.4332f, 1.029744f  }, // 1st Icewall
    { 5503.213f, 1969.547f, 737.0245f, 1.27409f   }, // 2nd Icewall
    { 5439.976f, 1879.005f, 752.7048f, 1.064651f  }, // 3rd Icewall
    { 5318.289f, 1749.184f, 771.9423f, 0.8726646f }  // 4th Icewall
	*/
};

class npc_jaina_and_sylvana_HRextro : public CreatureScript
{
public:
    npc_jaina_and_sylvana_HRextro() : CreatureScript("npc_jaina_and_sylvana_HRextro") { }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
    {
        InstanceScript* m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                pPlayer->CLOSE_GOSSIP_MENU();
				pCreature->AI()->DoAction(ACTION_START_FIGHT);
				pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

                if(m_pInstance)
                {
                    m_pInstance->SetData64(DATA_ESCAPE_LIDER_2, pCreature->GetGUID());
                    m_pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);
                    m_pInstance->SetData(TYPE_PHASE, 6);
                }
                return true;
            default:
                return false;
        }
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        InstanceScript*   m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();

        if(!m_pInstance)
            return false;

        if(m_pInstance->GetData(TYPE_LICH_KING) == DONE)
            return false;

        if(pCreature->isQuestGiver())
           pPlayer->PrepareQuestMenu( pCreature->GetGUID());

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lets go!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

        return true;
    }

    struct npc_jaina_and_sylvana_HRextroAI : public npc_escortAI
    {
        npc_jaina_and_sylvana_HRextroAI(Creature *pCreature) : npc_escortAI(pCreature)
        {
            m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();
            Reset();
			pLichKing = NULL;
			EncounterTimed = false;
			SPELL_LK_BLOCK = me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_PRISON : SPELL_DARK_ARROW;
			SPELL_DAMAGE_BLOCK = me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_BARRIER : SPELL_EVASION;
        }

        InstanceScript* m_pInstance;
        Creature* pLichKing;
		EventMap events;

		uint32 SPELL_DAMAGE_BLOCK;
		uint32 SPELL_LK_BLOCK;
		uint32 health;
        uint32 EncounterTime;
		bool EncounterTimed;

		void DoAction(const int32 action = 0)
		{
			switch (action)
			{
				case ACTION_START_PREFIGHT:
					m_pInstance->SetData(TYPE_PHASE, 4);
					me->setFaction(FACTION);
					me->SetReactState(REACT_AGGRESSIVE);
					DoCast(me, SPELL_DAMAGE_BLOCK);
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
					{
						pLichKing = LK;
                        LK->SetPhaseMask(65535, true);
						me->SetInCombatWith(LK);
						LK->SetInCombatWith(me);
						AttackStart(LK);
						LK->AI()->AttackStart(me);
					}
					me->SetHealth(200000);
					health = 200000;
					break;
				case ACTION_TALK_PREFIGHT:
					m_pInstance->SetData(TYPE_PHASE, 5);
					events.ScheduleEvent(EVENT_PREFIGHT_TALK_LK, 100);
					break;
				case ACTION_STOP_PREFIGHT:
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
					{
						pLichKing = LK;
						me->AddAura(SPELL_LK_BLOCK, LK);
						me->SetInCombatWith(LK);
						LK->SetInCombatWith(me);
						AttackStart(LK);
						LK->AI()->AttackStart(me);
						LK->SetReactState(REACT_PASSIVE);
						LK->AttackStop();
						LK->SetTarget(0);
					}
					me->SetReactState(REACT_PASSIVE);
					me->AttackStop();
					me->SetTarget(0);
					me->SetHealth(200000);
					health = 200000;
					me->GetMotionMaster()->MovePoint(0, LeaderStartPosition);
					me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					break;
				case ACTION_START_FIGHT:
					me->SetReactState(REACT_PASSIVE);
					me->RemoveAurasDueToSpell(SPELL_DAMAGE_BLOCK);
					EnterEvadeMode();
					me->SetWalk(false);
					Start(false, true);
					EncounterTime = 110000;
					EncounterTimed = true;
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
					{
						pLichKing = LK;
						LK->RemoveAurasDueToSpell(SPELL_LK_BLOCK);
						LK->AI()->DoAction(ACTION_START_FIGHT);
					}
					break;
				case ACTION_ICE_WALL_DESTROYED:
					me->InterruptNonMeleeSpells(false);
					SetEscortPaused(false);
					break;
				case ACTION_RESET_FIGHT:
					{
						Creature* meClone = me->SummonCreature(me->GetEntry(), 5579.1f, 2234.2f, 733.012f, 5.51f);
						meClone->setFaction(FACTION);
						meClone->AI()->DoAction(ACTION_STOP_PREFIGHT);
						me->DespawnOrUnsummon(100);
					}
					break;
				case ACTION_OUTRO:
					events.Reset();
					m_pInstance->SetData(TYPE_GUNSHIP, IN_PROGRESS);
					events.ScheduleEvent(EVENT_OUTRO_1, 10000);
					break;
			}
		}

		void EnterEvadeMode()
		{
			//me->DeleteThreatList();
			me->CombatStop(true);
		}

		void DamageTaken(Unit* damager, uint32& damage)
		{
			health -= damage;
			ScriptedAI::DamageTaken(damager, damage);
		}

        void JustDied(Unit* pKiller)
        {
			if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
				if (LK != pKiller)
					LK->AI()->DoAction(ACTION_RESET_FIGHT);
        }

        void AttackStart(Unit* who)
        {
            if(!who)
                return;

            if(m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS)
                return;

            npc_escortAI::AttackStart(who);

        }

        void WaypointReached(uint32 id)
        {
            switch(id)
            {
				case 1:
					me->SetHealth(200000);
					break;
                case 3:
                    m_pInstance->SetData(TYPE_ICE_WALL_01, IN_PROGRESS);
					me->SummonCreature(NPC_ICE_WALL, IceWallTargetPosition[0], TEMPSUMMON_TIMED_DESPAWN, 180000);
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
						LK->AI()->DoAction(ACTION_ICE_WALL_1);
                    break;
                case 4:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
					{
                        DoScriptText(SAY_JAINA_WALL_01, me);
						DoCast(SPELL_DESTROY_ICE_WALL_01);
					}
                    else if (me->GetEntry() == NPC_SYLVANA_OUTRO)
					{
                        DoScriptText(SAY_SYLVANA_WALL_01, me);
						DoCast(SPELL_DESTROY_ICE_WALL_02);
					}
                    SetEscortPaused(true);
                    break;
                case 6:
                    m_pInstance->SetData(TYPE_ICE_WALL_02, IN_PROGRESS);
					me->SummonCreature(NPC_ICE_WALL, IceWallTargetPosition[1], TEMPSUMMON_TIMED_DESPAWN, 180000);
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
						LK->AI()->DoAction(ACTION_ICE_WALL_2);
                    break;
                case 8:
                    if(me->GetEntry() == NPC_JAINA_OUTRO)
					{
                        DoScriptText(SAY_JAINA_WALL_02, me);
						DoCast(SPELL_DESTROY_ICE_WALL_01);
					}
                    else if (me->GetEntry() == NPC_SYLVANA_OUTRO)
					{
                        DoScriptText(SAY_SYLVANA_WALL_02, me);
						DoCast(SPELL_DESTROY_ICE_WALL_02);
					}
                    SetEscortPaused(true);
                    break;
                case 9:
                    if(me->GetEntry() == NPC_JAINA_OUTRO)
                        DoScriptText(SAY_JAINA_ESCAPE_01, me);
                    if(me->GetEntry() == NPC_SYLVANA_OUTRO)
                        DoScriptText(SAY_SYLVANA_ESCAPE_01, me);
                    break;
                case 11:
                    m_pInstance->SetData(TYPE_ICE_WALL_03, IN_PROGRESS);
					me->SummonCreature(NPC_ICE_WALL, IceWallTargetPosition[2], TEMPSUMMON_TIMED_DESPAWN, 180000);
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
						LK->AI()->DoAction(ACTION_ICE_WALL_3);
                    break;
                case 12:
                    if(me->GetEntry() == NPC_JAINA_OUTRO)
					{
                        DoScriptText(SAY_JAINA_WALL_03, me);
						DoCast(SPELL_DESTROY_ICE_WALL_01);
					}
                    else if (me->GetEntry() == NPC_SYLVANA_OUTRO)
					{
                        DoScriptText(SAY_SYLVANA_WALL_03, me);
						DoCast(SPELL_DESTROY_ICE_WALL_02);
					}
                    SetEscortPaused(true);
                    break;
                case 13:
                    if(me->GetEntry() == NPC_JAINA_OUTRO)
                        DoScriptText(SAY_JAINA_ESCAPE_02, me);
                    if(me->GetEntry() == NPC_SYLVANA_OUTRO)
                        DoScriptText(SAY_SYLVANA_ESCAPE_02, me);
                    break;
                case 15:
                    m_pInstance->SetData(TYPE_ICE_WALL_04, IN_PROGRESS);
					me->SummonCreature(NPC_ICE_WALL, IceWallTargetPosition[3], TEMPSUMMON_TIMED_DESPAWN, 180000);
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
						LK->AI()->DoAction(ACTION_ICE_WALL_4);
                    break;
                case 16:
                    if(me->GetEntry() == NPC_JAINA_OUTRO)
					{
                        DoScriptText(SAY_JAINA_WALL_04, me);
						DoCast(SPELL_DESTROY_ICE_WALL_01);
					}
                    else if (me->GetEntry() == NPC_SYLVANA_OUTRO)
					{
                        DoScriptText(SAY_SYLVANA_WALL_04, me);
						DoCast(SPELL_DESTROY_ICE_WALL_02);
					}
                    SetEscortPaused(true);
                    break;
                case 19:
                    if(me->GetEntry() == NPC_JAINA_OUTRO)
                        DoScriptText(SAY_JAINA_TRAP, me);
                    if(me->GetEntry() == NPC_SYLVANA_OUTRO)
                        DoScriptText(SAY_SYLVANA_TRAP, me);
					EncounterTimed = false;
                    break;          
                case 20:
                    SetEscortPaused(true);
                    if(me->GetEntry() == NPC_JAINA_OUTRO)
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2HL);
                    if(me->GetEntry() == NPC_SYLVANA_OUTRO)
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
					if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
						LK->AI()->DoAction(ACTION_OUTRO);
					SetEscortPaused(true);
                    break;
            }
        }

        void QuestComplete()
        {
            if(m_pInstance)
            {
                Map::PlayerList const &PlayerList = m_pInstance->instance->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    i->getSource()->KilledMonsterCredit(36955, 0);
                    i->getSource()->KilledMonsterCredit(37554, 0);
                    i->getSource()->KilledMonsterCredit(38211, 0);
					if (i->getSource()->GetGroup())
						sLFGMgr->RewardDungeonDoneFor(sLFGMgr->GetDungeon(i->getSource()->GetGroup()->GetGUID()), i->getSource());
                }
            }
        }

        void UpdateEscortAI(const uint32 diff)
        {
			if (m_pInstance->GetData(TYPE_LICH_KING) != IN_PROGRESS)
				if (UpdateVictim())
					DoMeleeAttackIfReady();

			if (EncounterTimed)
				EncounterTime += diff;

			if (health)
				me->SetHealth(health);

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_PREFIGHT_TALK_LK:
                        if(me->GetEntry() == NPC_JAINA_OUTRO)
                            DoScriptText(SAY_LICH_KING_AGGRO_A, pLichKing);
                        else if(me->GetEntry() == NPC_SYLVANA_OUTRO)
                            DoScriptText(SAY_LICH_KING_AGGRO_H, pLichKing);
						events.ScheduleEvent(EVENT_PREFIGHT_TALK_ME, 12000);
						break;
					case EVENT_PREFIGHT_TALK_ME:
						if(me->GetEntry() == NPC_JAINA_OUTRO)
							DoScriptText(SAY_JAINA_AGGRO, me);
						else if(me->GetEntry() == NPC_SYLVANA_OUTRO)
							DoScriptText(SAY_SYLVANA_AGGRO, me);
						events.ScheduleEvent(EVENT_STOP_PREFIGHT, 500);
						break;
					case EVENT_STOP_PREFIGHT:
						DoAction(ACTION_STOP_PREFIGHT);
						break;
						
					case EVENT_OUTRO_1:
						events.ScheduleEvent(EVENT_OUTRO_2, 5000);
						break;
					case EVENT_OUTRO_2:
						if(GameObject* pCave = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CAVE)))
							pCave->SetGoState(GO_STATE_READY);
						me->CastSpell(me, SPELL_SHIELD_DISRUPTION, false);
						events.ScheduleEvent(EVENT_OUTRO_3, 5000);
					case EVENT_OUTRO_3:
						me->RemoveAurasDueToSpell(SPELL_SILENCE);
						if (Creature* LK = me->GetCreature(*me, m_pInstance->GetData64(BOSS_LICH_KING)))
							LK->SetVisible(false);
						me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
						events.ScheduleEvent(EVENT_OUTRO_4, 3000);
						break;
					case EVENT_OUTRO_4:
						m_pInstance->SetData(TYPE_GUNSHIP, IN_PROGRESS);
						QuestComplete();
						if (m_pInstance)
						{
							if (IsHeroic())
							{
								m_pInstance->DoCompleteAchievement(ACHIEV_HOR_H);
								if (EncounterTime <= MAX_ENCOUNTER_TIME)
									m_pInstance->DoCompleteAchievement(ACHIEV_NOT_RETREATING);
							}
							else
							{
								m_pInstance->DoCompleteAchievement(ACHIEV_HOR);
							}
						}
						events.ScheduleEvent(EVENT_OUTRO_5, 8000);
						break;
					case EVENT_OUTRO_5:
						me->MonsterMoveWithSpeed(5240.66f, 1646.93f, 784.302f, 2.5);
						m_pInstance->SetData(TYPE_LICH_KING, DONE);
						me->SetOrientation(0.68f);
						me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
						me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
						break;
				}
			}
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_jaina_and_sylvana_HRextroAI(pCreature);
    }
};

class npc_lich_king_hr : public CreatureScript
{
public:
    npc_lich_king_hr() : CreatureScript("npc_lich_king_hr") { }

    struct npc_lich_king_hrAI : public ScriptedAI
    {
        npc_lich_king_hrAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
            Reset();
        }

        void Reset()
        {
        }

        void JustDied(Unit* pKiller)
        {
        }

        void AttackStart(Unit* who) 
        { 
            return;
        }

        void UpdateAI(const uint32 diff)
        {
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_lich_king_hrAI(pCreature);
    }
};

enum GeneralEvents
{
    //General
    EVENT_SHIELD                 = 1,
    EVENT_SPIKE                  = 2,
    EVENT_CLONE                  = 3,

    SAY_AGGRO                    = 0,
    SAY_DEATH                    = 1,

    SPELL_SHIELD_THROWN          = 69222, // 73076 on hc
    SPELL_SPIKE                  = 69184, // 70399 on hc
    SPELL_CLONE_NAME             = 57507,
    SPELL_CLONE_MODEL            = 45204,

    // Reflection
    EVENT_BALEFUL_STRIKE         = 1,

    SPELL_BALEFUL_STRIKE         = 69933, // 70400 on hc
    SPELL_SPIRIT_BURST           = 69900, // 73046 on hc
};

class npc_frostworn_general : public CreatureScript
{
public:
    npc_frostworn_general() : CreatureScript("npc_frostworn_general") { }

    struct npc_frostworn_generalAI : public ScriptedAI
    {
        npc_frostworn_generalAI(Creature* creature) : ScriptedAI(creature)
        {
            _instance = me->GetInstanceScript();
            Reset();
        }

        InstanceScript* _instance;
        EventMap _events;

        void Reset() OVERRIDE
        {
            _events.Reset();
            _instance->SetData(TYPE_FROST_GENERAL, NOT_STARTED);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void JustDied(Unit* /*killer*/) OVERRIDE
        {
            Talk(SAY_DEATH);
            _instance->SetData(TYPE_FROST_GENERAL, DONE);
        }

        void EnterCombat(Unit* /*victim*/) OVERRIDE
        {
            Talk(SAY_AGGRO);
            _events.ScheduleEvent(EVENT_SHIELD, 5000);
            _events.ScheduleEvent(EVENT_SPIKE, 14000);
            _events.ScheduleEvent(EVENT_CLONE, 22000);
            _instance->SetData(TYPE_FROST_GENERAL, IN_PROGRESS);
        }

        void MoveInLineOfSight(Unit* pWho) 
        {
            if (!_instance)
                return;

            if (me->getVictim())
                return;

            if (pWho->GetTypeId() != TYPEID_PLAYER
                || _instance->GetData(TYPE_MARWYN) != DONE
                || !me->IsWithinDistInMap(pWho, 20.0f)
                ) return;

            if (Player* pPlayer = (Player*)pWho)
                if (pPlayer->isGameMaster())
                    return;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            AttackStart(pWho);
        }

        void UpdateAI(uint32 const diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 event = _events.ExecuteEvent())
            {
                switch (event)
                {
                    case EVENT_SHIELD:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_SHIELD_THROWN);
                        _events.ScheduleEvent(EVENT_SHIELD, urand(8000, 12000));
                        break;
                    case EVENT_SPIKE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_SPIKE);
                        _events.ScheduleEvent(EVENT_SPIKE, urand(15000, 20000));
                        break;
                    case EVENT_CLONE:
                        SummonClones();
                        _events.ScheduleEvent(EVENT_CLONE, 60000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

        void SummonClones()
        {
            std::list<Unit *> playerList;
            SelectTargetList(playerList, 5, SELECT_TARGET_TOPAGGRO, 0, true);
            for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
            {
                Unit* temp = (*itr);
                Creature* reflection = me->SummonCreature(NPC_REFLECTION, temp->GetPositionX(), temp->GetPositionY(), temp->GetPositionZ(), temp->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
                reflection->SetName(temp->GetName());
                temp->CastSpell(reflection, SPELL_CLONE_NAME, true);
                temp->CastSpell(reflection, SPELL_CLONE_MODEL, true);
                reflection->setFaction(me->getFaction());
                reflection->AI()->AttackStart(temp);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new npc_frostworn_generalAI(creature);
    }
};

class npc_spiritual_reflection : public CreatureScript
{
public:
    npc_spiritual_reflection() : CreatureScript("npc_spiritual_reflection") { }

    struct npc_spiritual_reflectionAI : public ScriptedAI
    {
        npc_spiritual_reflectionAI(Creature *creature) : ScriptedAI(creature)
        {
            Reset();
        }

        EventMap _events;

        void Reset() OVERRIDE
        {
            _events.Reset();
        }

        void EnterCombat(Unit* /*victim*/) OVERRIDE
        {
            _events.ScheduleEvent(EVENT_BALEFUL_STRIKE, 3000);
        }

        void JustDied(Unit* killer) OVERRIDE
        {
            DoCast(killer, SPELL_SPIRIT_BURST);
        }

        void UpdateAI(uint32 const diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (_events.ExecuteEvent())
            {
                case EVENT_BALEFUL_STRIKE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_BALEFUL_STRIKE);
                    _events.ScheduleEvent(EVENT_BALEFUL_STRIKE, urand(3000, 8000));
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new npc_spiritual_reflectionAI(creature);
    }
};

enum TrashSpells
{
    // Ghostly Priest
    SPELL_SHADOW_WORD_PAIN                        = 72318,
    SPELL_CIRCLE_OF_DESTRUCTION                   = 72320,
    SPELL_COWER_IN_FEAR                           = 72321,
    SPELL_DARK_MENDING                            = 72322,

    // Phantom Mage
    SPELL_FIREBALL                                = 72163,
    SPELL_FLAMESTRIKE                             = 72169,
    SPELL_FROSTBOLT                               = 72166,
    SPELL_CHAINS_OF_ICE                           = 72121,
    SPELL_HALLUCINATION                           = 72342,

    // Phantom Hallucination (same as phantom mage + HALLUCINATION_2 when dies)
    SPELL_HALLUCINATION_2                         = 72344,

    // Shadowy Mercenary
    SPELL_SHADOW_STEP                             = 72326,
    SPELL_DEADLY_POISON                           = 72329,
    SPELL_ENVENOMED_DAGGER_THROW                  = 72333,
    SPELL_KIDNEY_SHOT                             = 72335,

    // Spectral Footman
    SPELL_SPECTRAL_STRIKE                         = 72198,
    SPELL_SHIELD_BASH                             = 72194,
    SPELL_TORTURED_ENRAGE                         = 72203,

    // Tortured Rifleman
    SPELL_SHOOT                                   = 72208,
    SPELL_CURSED_ARROW                            = 72222,
    SPELL_FROST_TRAP                              = 72215,
    SPELL_ICE_SHOT                                = 72268,
};

enum TrashEvents
{
    EVENT_TRASH_NONE,

    // Ghostly Priest
    EVENT_SHADOW_WORD_PAIN,
    EVENT_CIRCLE_OF_DESTRUCTION,
    EVENT_COWER_IN_FEAR,
    EVENT_DARK_MENDING,

    // Phantom Mage
    EVENT_FIREBALL,
    EVENT_FLAMESTRIKE,
    EVENT_FROSTBOLT,
    EVENT_CHAINS_OF_ICE,
    EVENT_HALLUCINATION,

    // Shadowy Mercenary
    EVENT_SHADOW_STEP,
    EVENT_DEADLY_POISON,
    EVENT_ENVENOMED_DAGGER_THROW,
    EVENT_KIDNEY_SHOT,

    // Spectral Footman
    EVENT_SPECTRAL_STRIKE,
    EVENT_SHIELD_BASH,
    EVENT_TORTURED_ENRAGE,

    // Tortured Rifleman
    EVENT_SHOOT,
    EVENT_CURSED_ARROW,
    EVENT_FROST_TRAP,
    EVENT_ICE_SHOT,
};

class npc_ghostly_priest : public CreatureScript
{
public:
    npc_ghostly_priest() : CreatureScript("npc_ghostly_priest") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_ghostly_priestAI(pCreature);
    }

    struct npc_ghostly_priestAI: public ScriptedAI
    {
        npc_ghostly_priestAI(Creature *c) : ScriptedAI(c)
        {
        }

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, 8000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_CIRCLE_OF_DESTRUCTION, 12000);
            events.ScheduleEvent(EVENT_COWER_IN_FEAR, 10000);
            events.ScheduleEvent(EVENT_DARK_MENDING, 20000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SHADOW_WORD_PAIN:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_SHADOW_WORD_PAIN);
                        events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, 8000);
                        return;
                    case EVENT_CIRCLE_OF_DESTRUCTION:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_CIRCLE_OF_DESTRUCTION);
                        events.ScheduleEvent(EVENT_CIRCLE_OF_DESTRUCTION, 12000);
                        return;
                    case EVENT_COWER_IN_FEAR:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_COWER_IN_FEAR);
                        events.ScheduleEvent(EVENT_COWER_IN_FEAR, 10000);
                        return;
                    case EVENT_DARK_MENDING:
                        // find an ally with missing HP
                        if (Unit *pTarget = DoSelectLowestHpFriendly(40, DUNGEON_MODE(30000,50000)))
                        {
                            DoCast(pTarget, SPELL_DARK_MENDING);
                            events.ScheduleEvent(EVENT_DARK_MENDING, 20000);
                        }
                        else
                        {
                            // no friendly unit with missing hp. re-check in just 5 sec.
                            events.ScheduleEvent(EVENT_DARK_MENDING, 5000);
                        }
                        return;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

};

class npc_phantom_mage : public CreatureScript
{
public:
    npc_phantom_mage() : CreatureScript("npc_phantom_mage") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_phantom_mageAI(pCreature);
    }

    struct npc_phantom_mageAI: public ScriptedAI
    {
        npc_phantom_mageAI(Creature *c) : ScriptedAI(c)
        {
        }

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_FIREBALL, 3000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_FLAMESTRIKE, 6000);
            events.ScheduleEvent(EVENT_FROSTBOLT, 9000);
            events.ScheduleEvent(EVENT_CHAINS_OF_ICE, 12000);
            events.ScheduleEvent(EVENT_HALLUCINATION, 40000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_FIREBALL:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_FIREBALL);
                        events.ScheduleEvent(EVENT_FIREBALL, 15000);
                        return;
                    case EVENT_FLAMESTRIKE:
                        DoCast(SPELL_FLAMESTRIKE);
                        events.ScheduleEvent(EVENT_FLAMESTRIKE, 15000);
                        return;
                    case EVENT_FROSTBOLT:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_FROSTBOLT);
                        events.ScheduleEvent(EVENT_FROSTBOLT, 15000);
                        return;
                    case EVENT_CHAINS_OF_ICE:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_CHAINS_OF_ICE);
                        events.ScheduleEvent(EVENT_CHAINS_OF_ICE, 15000);
                        return;
                    case EVENT_HALLUCINATION:
                        DoCast(SPELL_HALLUCINATION);
                        return;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

};

class npc_phantom_hallucination : public CreatureScript
{
public:
    npc_phantom_hallucination() : CreatureScript("npc_phantom_hallucination") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_phantom_hallucinationAI(pCreature);
    }

    struct npc_phantom_hallucinationAI : public npc_phantom_mage::npc_phantom_mageAI
    {
        npc_phantom_hallucinationAI(Creature *c) : npc_phantom_mage::npc_phantom_mageAI(c)
        {
        }

        void JustDied(Unit * /*pWho*/)
        {
            DoCast(SPELL_HALLUCINATION_2);
        }
    };

};

class npc_shadowy_mercenary : public CreatureScript
{
public:
    npc_shadowy_mercenary() : CreatureScript("npc_shadowy_mercenary") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_shadowy_mercenaryAI(pCreature);
    }

    struct npc_shadowy_mercenaryAI: public ScriptedAI
    {
        npc_shadowy_mercenaryAI(Creature *c) : ScriptedAI(c)
        {
        }

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHADOW_STEP, 8000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_DEADLY_POISON, 5000);
            events.ScheduleEvent(EVENT_ENVENOMED_DAGGER_THROW, 10000);
            events.ScheduleEvent(EVENT_KIDNEY_SHOT, 12000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SHADOW_STEP:
                        DoCast(SPELL_SHADOW_STEP);
                        events.ScheduleEvent(EVENT_SHADOW_STEP, 8000);
                        return;
                    case EVENT_DEADLY_POISON:
                        DoCast(me->getVictim(), SPELL_DEADLY_POISON);
                        events.ScheduleEvent(EVENT_DEADLY_POISON, 10000);
                        return;
                    case EVENT_ENVENOMED_DAGGER_THROW:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_ENVENOMED_DAGGER_THROW);
                        events.ScheduleEvent(EVENT_ENVENOMED_DAGGER_THROW, 10000);
                        return;
                    case EVENT_KIDNEY_SHOT:
                        DoCast(me->getVictim(), SPELL_KIDNEY_SHOT);
                        events.ScheduleEvent(EVENT_KIDNEY_SHOT, 10000);
                        return;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

};

class npc_spectral_footman : public CreatureScript
{
public:
    npc_spectral_footman() : CreatureScript("npc_spectral_footman") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_spectral_footmanAI(pCreature);
    }

    struct npc_spectral_footmanAI: public ScriptedAI
    {
        npc_spectral_footmanAI(Creature *c) : ScriptedAI(c)
        {
        }

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SPECTRAL_STRIKE, 5000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_SHIELD_BASH, 10000);
            events.ScheduleEvent(EVENT_TORTURED_ENRAGE, 15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SPECTRAL_STRIKE:
                        DoCast(me->getVictim(), SPELL_SPECTRAL_STRIKE);
                        events.ScheduleEvent(EVENT_SPECTRAL_STRIKE, 5000);
                        return;
                    case EVENT_SHIELD_BASH:
                        DoCast(me->getVictim(), SPELL_SHIELD_BASH);
                        events.ScheduleEvent(EVENT_SHIELD_BASH, 5000);
                        return;
                    case EVENT_TORTURED_ENRAGE:
                        DoCast(SPELL_TORTURED_ENRAGE);
                        events.ScheduleEvent(EVENT_TORTURED_ENRAGE, 15000);
                        return;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

};

class npc_tortured_rifleman : public CreatureScript
{
public:
    npc_tortured_rifleman() : CreatureScript("npc_tortured_rifleman") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_tortured_riflemanAI(pCreature);
    }

    struct npc_tortured_riflemanAI  : public ScriptedAI
    {
        npc_tortured_riflemanAI(Creature *c) : ScriptedAI(c)
        {
        }

        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHOOT, 2000); // TODO: adjust timers
            events.ScheduleEvent(EVENT_CURSED_ARROW, 10000);
            events.ScheduleEvent(EVENT_FROST_TRAP, 1000);
            events.ScheduleEvent(EVENT_ICE_SHOT, 15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SHOOT:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_SHOOT);
                        events.ScheduleEvent(EVENT_SHOOT, 2000);
                        return;
                    case EVENT_CURSED_ARROW:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_CURSED_ARROW);
                        events.ScheduleEvent(EVENT_CURSED_ARROW, 10000);
                        return;
                    case EVENT_FROST_TRAP:
                        DoCast(SPELL_FROST_TRAP);
                        events.ScheduleEvent(EVENT_FROST_TRAP, 30000);
                        return;
                    case EVENT_ICE_SHOT:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(pTarget, SPELL_ICE_SHOT);
                        events.ScheduleEvent(EVENT_ICE_SHOT, 15000);
                        return;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

};

enum EscapeEvents
{
    // Raging Ghoul
    EVENT_RAGING_GHOUL_JUMP = 1,

    // Risen Witch Doctor
    EVENT_RISEN_WITCH_DOCTOR_CURSE,
    EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT,
    EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT_VOLLEY,

    // Lumbering Abomination
    EVENT_LUMBERING_ABOMINATION_VOMIT_SPRAY,
    EVENT_LUMBERING_ABOMINATION_CLEAVE,

    // Risen Witch Doctor
    SPELL_CURSE_OF_DOOM                = 70144,
    SPELL_SHADOW_BOLT_VOLLEY           = 70145,
    SPELL_SHADOW_BOLT                  = 70080,
    SPELL_RISEN_WITCH_DOCTOR_SPAWN     = 69639,

    // Lumbering Abomination
    SPELL_CLEAVE                       = 40505,
    SPELL_VOMIT_SPRAY                  = 70176
};

class npc_risen_witch_doctor : public CreatureScript
{
    public:
        npc_risen_witch_doctor() : CreatureScript("npc_risen_witch_doctor") { }

        struct npc_risen_witch_doctorAI : public ScriptedAI
        {
            npc_risen_witch_doctorAI(Creature* creature) : ScriptedAI(creature) { }

			EventMap _events;

            void Reset()
            {
				_events.Reset();
                _events.ScheduleEvent(EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT, 6000);
                _events.ScheduleEvent(EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT_VOLLEY, 15000);
                _events.ScheduleEvent(EVENT_RISEN_WITCH_DOCTOR_CURSE, 7000);
            }

            void IsSummonedBy(Unit* summoner)
            {
                me->CastSpell(me, SPELL_RISEN_WITCH_DOCTOR_SPAWN, true);
                me->SetReactState(REACT_PASSIVE);
                me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (_events.ExecuteEvent())
                {
                    case EVENT_RISEN_WITCH_DOCTOR_CURSE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                            DoCast(target, SPELL_CURSE_OF_DOOM);
                        _events.ScheduleEvent(EVENT_RISEN_WITCH_DOCTOR_CURSE, urand(10000, 15000));
                        break;
                    case EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT:
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 20.0f, true))
                            DoCast(target, SPELL_SHADOW_BOLT);
                        _events.ScheduleEvent(EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT, urand(2000, 3000));
                        break;
                    case EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT_VOLLEY:
                        if (SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                            DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY);
                        _events.ScheduleEvent(EVENT_RISEN_WITCH_DOCTOR_SHADOW_BOLT_VOLLEY, urand(15000, 22000));
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_risen_witch_doctorAI(creature);
        }
};

class npc_lumbering_abomination : public CreatureScript
{
    public:
        npc_lumbering_abomination() : CreatureScript("npc_lumbering_abomination") { }

        struct npc_lumbering_abominationAI : public ScriptedAI
        {
            npc_lumbering_abominationAI(Creature* creature) : ScriptedAI(creature) { }

			EventMap _events;

            void Reset() override
            {
				_events.Reset();
                _events.ScheduleEvent(EVENT_LUMBERING_ABOMINATION_VOMIT_SPRAY, 15000);
                _events.ScheduleEvent(EVENT_LUMBERING_ABOMINATION_CLEAVE, 6000);
            }

            void UpdateAI(uint32 const diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (_events.ExecuteEvent())
                {
                    case EVENT_LUMBERING_ABOMINATION_VOMIT_SPRAY:
                        DoCastVictim(SPELL_VOMIT_SPRAY);
                        _events.ScheduleEvent(EVENT_LUMBERING_ABOMINATION_VOMIT_SPRAY, urand(15000, 20000));
                        break;
                    case EVENT_LUMBERING_ABOMINATION_CLEAVE:
                        DoCastVictim(SPELL_CLEAVE);
                        _events.ScheduleEvent(EVENT_LUMBERING_ABOMINATION_CLEAVE, urand(7000, 9000));
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lumbering_abominationAI(creature);
        }
};

class at_LK_combat_starter : public AreaTriggerScript
{
    public:
        at_LK_combat_starter() : AreaTriggerScript("at_LK_combat_starter") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
        {
            InstanceScript* instance = player->GetInstanceScript();
            if (player->isGameMaster() || !instance)
                return false;
			
			if (instance->GetData(TYPE_FROST_GENERAL) == DONE && instance->GetData(TYPE_PHASE) == 3)
				if (Creature* pLider = (Unit::GetCreature(*player, instance->GetData64(DATA_ESCAPE_LIDER_2))))
				{
					pLider->AI()->DoAction(ACTION_START_PREFIGHT);
					return true;
				}

            return false;
        }
};

class at_LK_talk_starter : public AreaTriggerScript
{
    public:
        at_LK_talk_starter() : AreaTriggerScript("at_LK_talk_starter") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
        {
            InstanceScript* instance = player->GetInstanceScript();
            if (player->isGameMaster() || !instance)
                return false;
			
			if (instance->GetData(TYPE_PHASE) == 4)
				if (Creature* pLider = (Unit::GetCreature(*player, instance->GetData64(DATA_ESCAPE_LIDER_2))))
				{
					pLider->AI()->DoAction(ACTION_TALK_PREFIGHT);
					return true;
				}

            return false;
        }
};

// 70017 - Gunship Cannon Fire
class spell_hor_gunship_cannon_fire : public SpellScriptLoader
{
    public:
        spell_hor_gunship_cannon_fire() : SpellScriptLoader("spell_hor_gunship_cannon_fire") { }

        class spell_hor_gunship_cannon_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hor_gunship_cannon_fire_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                if (!urand(0, 2))
                {
                    if (GetTarget()->GetEntry() == NPC_GUNSHIP_CANNON_HORDE)
                        GetTarget()->CastSpell((Unit*)NULL, SPELL_GUNSHIP_CANNON_FIRE_MISSILE_HORDE, true);
                    else
                        GetTarget()->CastSpell((Unit*)NULL, SPELL_GUNSHIP_CANNON_FIRE_MISSILE_ALLIANCE, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hor_gunship_cannon_fire_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hor_gunship_cannon_fire_AuraScript();
        }
};

void AddSC_halls_of_reflection()
{
	new npc_uther();
	new npc_quel_dellar();
    new npc_jaina_and_sylvana_HRintro();
    new npc_jaina_and_sylvana_HRextro();
    new npc_lich_king_hr();
    new npc_frostworn_general();
    new npc_ghostly_priest();
    new npc_phantom_mage();
    new npc_phantom_hallucination();
    new npc_shadowy_mercenary();
    new npc_spectral_footman();
    new npc_tortured_rifleman();
	new npc_risen_witch_doctor();
	new npc_lumbering_abomination();
	new at_LK_combat_starter();
	new at_LK_talk_starter();
	new spell_hor_gunship_cannon_fire();
}
