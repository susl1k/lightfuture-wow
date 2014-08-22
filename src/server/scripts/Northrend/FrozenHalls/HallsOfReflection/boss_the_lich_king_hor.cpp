/*
 * Copyright (C) 2008 - 2010 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ScriptPCH.h"
#include "halls_of_reflection.h"
#include "ScriptedEscortAI.h"

enum
{
    SPELL_WINTER                       = 69780,
    SPELL_FURY_OF_FROSTMOURNE          = 70063,
    SPELL_SOUL_REAPER                  = 73797,
    SPELL_RAISE_DEAD                   = 69818,
    SPELL_ICE_PRISON                   = 69708,
    SPELL_DARK_ARROW                   = 70194,
    SPELL_EMERGE_VISUAL                = 50142,
    SPELL_DESTROY_ICE_WALL_02          = 70224,
    SPELL_SILENCE                      = 69413,
    SPELL_LICH_KING_CAST               = 57561,
    SPELL_GNOUL_JUMP                   = 70150,
    SPELL_ABON_STRIKE                  = 40505,
    SPELL_FROSTMOURNE_VISUAL           = 73220,

    /*SPELLS - Witch Doctor*/
    SPELL_COURSE_OF_DOOM               = 70144,
    SPELL_SHADOW_BOLT_VALLEY           = 70145,
    SPELL_SHADOW_BOLT_N                = 70080,
    SPELL_SHADOW_BOLT_H                = 70182,

    SAY_LICH_KING_WALL_01              = -1594486,
    SAY_LICH_KING_WALL_02              = -1594491,
    SAY_LICH_KING_GNOUL                = -1594482,
    SAY_LICH_KING_ABON                 = -1594483,
    SAY_LICH_KING_WINTER               = -1594481,
    SAY_LICH_KING_END_DUN              = -1594504,
    SAY_LICH_KING_WIN                  = -1594485,
};

class boss_lich_king_hor : public CreatureScript
{
public:
    boss_lich_king_hor() : CreatureScript("boss_lich_king_hor") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_lich_king_horAI(pCreature);
    }

    struct boss_lich_king_horAI : public npc_escortAI
    {
        boss_lich_king_horAI(Creature *pCreature) : npc_escortAI(pCreature), summons(me)
        {
            m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();
            Reset();
			allSummoned = false;
        }

        InstanceScript* m_pInstance;
		EventMap events;
		SummonList summons;
		
		bool allSummoned;

		void DoAction(int32 const action)
		{
			switch (action)
			{
				case ACTION_STOP_PREFIGHT:

					break;
				case ACTION_START_FIGHT:
					npc_escortAI::EnterEvadeMode();
					me->SetSpeed(MOVE_WALK, 2.2f);
					me->SetReactState(REACT_PASSIVE);
					Start(false, false);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					break;
				case ACTION_RESET_FIGHT:
					SetEscortPaused(true);
					events.ScheduleEvent(EVENT_RESET_FIGHT_1, 100);
					break;
					
				case ACTION_ICE_WALL_1:
					SetEscortPaused(true);
					me->SetSpeed(MOVE_WALK, 0.9f);
					DoCast(me, SPELL_WINTER);
					events.ScheduleEvent(EVENT_ICE_WALL_1_1, 5000);
					break;
				case ACTION_ICE_WALL_2:
					SetEscortPaused(true);
					summons.DespawnAll();
					DoCastAOE(SPELL_RAISE_DEAD);
					DoScriptText(SAY_LICH_KING_GNOUL, me);
					events.ScheduleEvent(EVENT_ICE_WALL_2_1, 3600);
					break;
				case ACTION_ICE_WALL_3:
					SetEscortPaused(true);
					summons.DespawnAll();
					DoCastAOE(SPELL_RAISE_DEAD);
					DoScriptText(SAY_LICH_KING_GNOUL, me);
					events.ScheduleEvent(EVENT_ICE_WALL_3_1, 3600);
					break;
				case ACTION_ICE_WALL_4:
					SetEscortPaused(true);
					summons.DespawnAll();
					DoCastAOE(SPELL_RAISE_DEAD);
					DoScriptText(SAY_LICH_KING_GNOUL, me);
					events.ScheduleEvent(EVENT_ICE_WALL_4_1, 3600);
					break;
				case ACTION_OUTRO:
					me->SetSpeed(MOVE_WALK, 2.5f);
					me->RemoveAurasDueToSpell(SPELL_WINTER);
					break;
			}
		}

		void AttackStart(Unit* who)
        {
            if (!m_pInstance || !who)
                return;

            if(m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS || who->GetTypeId() == TYPEID_PLAYER)
                return;

            npc_escortAI::AttackStart(who);
        }

        void WaypointReached(uint32 i)
        {
            switch(i)
            {
                case 20:
                    SetEscortPaused(true);
                    DoCast(me, SPELL_LICH_KING_CAST);
                    m_pInstance->SetData(TYPE_LICH_KING, SPECIAL);
                    m_pInstance->SetData(TYPE_PHASE, 7);
                    DoScriptText(SAY_LICH_KING_END_DUN, me);
                    if(Creature* pLider = (Unit::GetCreature((*me), m_pInstance->GetData64(DATA_ESCAPE_LIDER_2))))
					{
                        pLider->CastSpell(pLider, SPELL_SILENCE, false);
						pLider->AI()->DoAction(ACTION_OUTRO);
					}
                    break;
            }
        }

		void SummonedCreatureDies(Creature* summon, Unit* killer)
		{
			summons.Despawn(summon);

			if (allSummoned)
				if (summons.empty())
				{
					if (Creature* pLider = Unit::GetCreature((*me), m_pInstance->GetData64(DATA_ESCAPE_LIDER_2)))
					{
						if (m_pInstance->GetData(TYPE_ICE_WALL_01) == IN_PROGRESS)
						{
							m_pInstance->SetData(TYPE_ICE_WALL_01, DONE);
							pLider->AI()->DoAction(ACTION_ICE_WALL_DESTROYED);
						}
						else if (m_pInstance->GetData(TYPE_ICE_WALL_02) == IN_PROGRESS)
						{
							m_pInstance->SetData(TYPE_ICE_WALL_02, DONE);
							pLider->AI()->DoAction(ACTION_ICE_WALL_DESTROYED);
						}
						else if (m_pInstance->GetData(TYPE_ICE_WALL_03) == IN_PROGRESS)
						{
							m_pInstance->SetData(TYPE_ICE_WALL_03, DONE);
							pLider->AI()->DoAction(ACTION_ICE_WALL_DESTROYED);
						}
						else if (m_pInstance->GetData(TYPE_ICE_WALL_04) == IN_PROGRESS)
						{
							m_pInstance->SetData(TYPE_ICE_WALL_04, DONE);
							pLider->AI()->DoAction(ACTION_ICE_WALL_DESTROYED);
						}
					}

					allSummoned = false;
				}
		}

        void JustSummoned(Creature* summoned)
        {
			if (summoned->GetEntry() == me->GetEntry())
				return;

            if(!m_pInstance || !summoned)
                return;

			summons.Summon(summoned);

            summoned->SetPhaseMask(65535, true);
            summoned->SetInCombatWithZone();
            summoned->setActive(true);
			summoned->setFaction(2102);

            if (Unit* pLider = Unit::GetUnit((*me), m_pInstance->GetData64(DATA_ESCAPE_LIDER_2)))
            {
				summoned->SetInCombatWith(pLider);
				pLider->SetInCombatWith(summoned);
                summoned->AddThreat(pLider, 20.0f);
				summoned->AI()->AttackStart(pLider);
            }
        }

        void CallGuard(uint32 GuardID)
        {
            me->SummonCreature(GuardID, me->GetPositionX()+frand(-5.0f, 5.0f), me->GetPositionY()+frand(-5.0f, 5.0f), me->GetPositionZ(),4.17f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
        }

        void UpdateEscortAI(const uint32 diff)
        {
			if (m_pInstance->GetData(TYPE_LICH_KING) != IN_PROGRESS)
			{
				if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
			}

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_ICE_WALL_1_1:
						summons.DespawnAll();
						DoCastAOE(SPELL_RAISE_DEAD);
						DoScriptText(SAY_LICH_KING_WALL_01, me);
						events.ScheduleEvent(EVENT_ICE_WALL_1_2, 4000);
						break;
					case EVENT_ICE_WALL_1_2:
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						allSummoned = true;
						SetEscortPaused(false);
						break;
					case EVENT_ICE_WALL_2_1:
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_ABON);
						allSummoned = true;
						SetEscortPaused(false);
						break;
					case EVENT_ICE_WALL_3_1:
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_ABON);
						CallGuard(NPC_ABON);
						allSummoned = true;
						SetEscortPaused(false);
						break;
					case EVENT_ICE_WALL_4_1:
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_ABON);
						CallGuard(NPC_ABON);
						DoCastAOE(SPELL_RAISE_DEAD);
						SetEscortPaused(false);
						events.ScheduleEvent(EVENT_ICE_WALL_4_2, 15000);
						break;
					case EVENT_ICE_WALL_4_2:
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						CallGuard(NPC_RISEN_WITCH_DOCTOR);
						allSummoned = true;
						break;

					case EVENT_RESET_FIGHT_1:
						DoScriptText(SAY_LICH_KING_WIN, me);
						me->CastSpell(me, SPELL_FURY_OF_FROSTMOURNE, false);
						events.ScheduleEvent(EVENT_RESET_FIGHT_2, 2700);
						break;
					case EVENT_RESET_FIGHT_2:
						if (Creature* pLider = (Unit::GetCreature((*me), m_pInstance->GetData64(DATA_ESCAPE_LIDER_2))))
						{
                            pLider->SetStandState(UNIT_STAND_STATE_DEAD);
                            pLider->SetHealth(0);
						}
						if (me->GetMap())
						{
							Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
							for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
								if (Player* player = i->getSource())
									if (!player->isGameMaster())
										if (player->isAlive())
											me->Kill(player);
						}
						events.ScheduleEvent(EVENT_RESET_FIGHT_3, 5000);
						break;
					case EVENT_RESET_FIGHT_3:
						events.Reset();
						summons.DespawnAll();
						m_pInstance->SetData(TYPE_LICH_KING, FAIL);
						m_pInstance->SetData(TYPE_ICE_WALL_01, FAIL);
						m_pInstance->SetData(TYPE_ICE_WALL_02, FAIL);
						m_pInstance->SetData(TYPE_ICE_WALL_03, FAIL);
						m_pInstance->SetData(TYPE_ICE_WALL_04, FAIL);
						me->SummonCreature(me->GetEntry(), 5551.0f, 2260.0f, 733.012f, 3.85f);
						me->DespawnOrUnsummon(100);

						if(Creature* pLider = (Unit::GetCreature((*me), m_pInstance->GetData64(DATA_ESCAPE_LIDER_2))))
						{
							pLider->Respawn(true);
							pLider->NearTeleportTo(5579.1f, 2234.2f, 733.012f, 5.51f);
							pLider->AI()->DoAction(ACTION_RESET_FIGHT);
						}
						break;
				}
			}

			if (m_pInstance->GetData(TYPE_PHASE) == 6)
			{
				if (Creature* pLider = (Unit::GetCreature((*me), m_pInstance->GetData64(DATA_ESCAPE_LIDER_2))))
					if (pLider->IsWithinDistInMap(me, 5.0f))
					{
						DoAction(ACTION_RESET_FIGHT);
						m_pInstance->SetData(TYPE_PHASE, 5);
					}

				if (me->GetMap())
				{
					Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
						if (Player* player = i->getSource())
							if (!player->isGameMaster())
								if (player->isAlive())
									return;
				}
				DoAction(ACTION_RESET_FIGHT);
				m_pInstance->SetData(TYPE_PHASE, 5);
			}
        }
    };
};

class npc_raging_gnoul : public CreatureScript
{
public:
    npc_raging_gnoul() : CreatureScript("npc_raging_gnoul") { }

    struct npc_raging_gnoulAI : public ScriptedAI
    {
        npc_raging_gnoulAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
            m_pInstance = (InstanceScript*)pCreature->GetInstanceScript();
            me->setActive(true);
        }

        InstanceScript* m_pInstance;
        uint32 EmergeTimer;
        bool Emerge;

        void Reset()
        {
            DoCast(me, SPELL_EMERGE_VISUAL);
            EmergeTimer = 4000;
            Emerge = false;
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if(!Emerge)
                return;

            ScriptedAI::AttackStart(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if(!m_pInstance)
                return;

            if(m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS)
            {
                if(!Emerge)
                {
                    if(EmergeTimer < diff)
                    {
                        Emerge = true;
						if (Unit* pLider = Unit::GetUnit((*me), m_pInstance->GetData64(DATA_ESCAPE_LIDER_2)))
						{
							DoZoneInCombat();
							me->SetInCombatWith(pLider);
							pLider->SetInCombatWith(me);
							me->AddThreat(pLider, 10.0f);
						}
						if (Unit* targetPlayer = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
							me->GetMotionMaster()->MoveJump(targetPlayer->GetPositionX(), targetPlayer->GetPositionY(), targetPlayer->GetPositionZ(), 30.0f, 30.0f);
                    }
                    else
                        EmergeTimer -= diff;
					return;
                }
            }

			if (UpdateVictim())
				DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_raging_gnoulAI(pCreature);
    }

};

void AddSC_boss_lich_king_hr()
{
    new boss_lich_king_hor();
    new npc_raging_gnoul();
}
