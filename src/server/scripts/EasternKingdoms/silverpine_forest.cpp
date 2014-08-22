 /*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Silverpine_Forest
SD%Complete: 100
SDComment: Quest support: 435, 452
SDCategory: Silverpine Forest
EndScriptData */

/* ContentData
npc_deathstalker_erland
pyrewood_ambush
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npc_deathstalker_erland
######*/

enum eErland
{
    SAY_QUESTACCEPT     = -1000306,
    SAY_START           = -1000307,
    SAY_AGGRO_1         = -1000308,
    SAY_AGGRO_2         = -1000309,
    SAY_LAST            = -1000310,

    SAY_THANKS          = -1000311,
    SAY_RANE            = -1000312,
    SAY_ANSWER          = -1000313,
    SAY_MOVE_QUINN      = -1000314,

    SAY_GREETINGS       = -1000315,
    SAY_QUINN           = -1000316,
    SAY_ON_BYE          = -1000317,

    QUEST_ESCORTING     = 435,
    NPC_RANE            = 1950,
    NPC_QUINN           = 1951
};

class npc_deathstalker_erland : public CreatureScript
{
public:
    npc_deathstalker_erland() : CreatureScript("npc_deathstalker_erland") { }

    struct npc_deathstalker_erlandAI : public npc_escortAI
    {
        npc_deathstalker_erlandAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 1:
                    DoScriptText(SAY_START, me, player);
                    break;
                case 13:
                    DoScriptText(SAY_LAST, me, player);
                    player->GroupEventHappens(QUEST_ESCORTING, me);
                    break;
                case 14:
                    DoScriptText(SAY_THANKS, me, player);
                    break;
                case 15:
                    if (Unit* Rane = me->FindNearestCreature(NPC_RANE, 20))
                        DoScriptText(SAY_RANE, Rane);
                    break;
                case 16:
                    DoScriptText(SAY_ANSWER, me);
                    break;
                case 17:
                    DoScriptText(SAY_MOVE_QUINN, me);
                    break;
                case 24:
                    DoScriptText(SAY_GREETINGS, me);
                    break;
                case 25:
                    if (Unit* Quinn = me->FindNearestCreature(NPC_QUINN, 20))
                        DoScriptText(SAY_QUINN, Quinn);
                    break;
                case 26:
                    DoScriptText(SAY_ON_BYE, me, NULL);
                    break;
            }
        }

        void Reset() {}

        void EnterCombat(Unit* who)
        {
            DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2), me, who);
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCORTING)
        {
            DoScriptText(SAY_QUESTACCEPT, creature, player);

            if (npc_escortAI* pEscortAI = CAST_AI(npc_deathstalker_erland::npc_deathstalker_erlandAI, creature->AI()))
                pEscortAI->Start(true, false, player->GetGUID());
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathstalker_erlandAI(creature);
    }
};

/*######
## pyrewood_ambush
#######*/

#define QUEST_PYREWOOD_AMBUSH 452

#define NPCSAY_INIT "Get ready, they'll be arriving any minute..." //not blizzlike
#define NPCSAY_END "Thanks for your help!" //not blizzlike

static float PyrewoodSpawnPoints[3][4] =
{
    //pos_x   pos_y     pos_z    orien
    //outside
    /*
    {-400.85f, 1513.64f, 18.67f, 0},
    {-397.32f, 1514.12f, 18.67f, 0},
    {-397.44f, 1511.09f, 18.67f, 0},
    */
    //door
    {-396.17f, 1505.86f, 19.77f, 0},
    {-396.91f, 1505.77f, 19.77f, 0},
    {-397.94f, 1504.74f, 19.77f, 0},
};

#define WAIT_SECS 6000

class pyrewood_ambush : public CreatureScript
{
public:
    pyrewood_ambush() : CreatureScript("pyrewood_ambush") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest *quest)
    {
        if (quest->GetQuestId() == QUEST_PYREWOOD_AMBUSH && !CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->QuestInProgress)
        {
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->QuestInProgress = true;
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->Phase = 0;
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->KillCount = 0;
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->PlayerGUID = player->GetGUID();
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new pyrewood_ambushAI (creature);
    }

    struct pyrewood_ambushAI : public ScriptedAI
    {
        pyrewood_ambushAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
           QuestInProgress = false;
        }

        uint32 Phase;
        int8 KillCount;
        uint32 WaitTimer;
        uint64 PlayerGUID;
        SummonList Summons;

        bool QuestInProgress;

        void Reset()
        {
            WaitTimer = WAIT_SECS;

            if (!QuestInProgress) //fix reset values (see UpdateVictim)
            {
                Phase = 0;
                KillCount = 0;
                PlayerGUID = 0;
                Summons.DespawnAll();
            }
        }

        void EnterCombat(Unit* /*who*/){}

        void JustSummoned(Creature* summoned)
        {
            Summons.Summon(summoned);
            ++KillCount;
        }

        void SummonedCreatureDespawn(Creature* summoned)
        {
            Summons.Despawn(summoned);
            --KillCount;
        }

        void SummonCreatureWithRandomTarget(uint32 creatureId, int position)
        {
            if (Creature* summoned = me->SummonCreature(creatureId, PyrewoodSpawnPoints[position][0], PyrewoodSpawnPoints[position][1], PyrewoodSpawnPoints[position][2], PyrewoodSpawnPoints[position][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
            {
                Player* player = NULL;
                Unit* target = NULL;
                if (PlayerGUID)
                {
                    player = Unit::GetPlayer(*me, PlayerGUID);
                    if (player)
                        target = RAND((Unit*)me, (Unit*)player);
                } else
                    target = me;

                if (target)
                {
                    summoned->setFaction(168);
                    summoned->AddThreat(target, 32.0f);
                    summoned->AI()->AttackStart(target);
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (PlayerGUID)
                if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                    if (player->GetQuestStatus(QUEST_PYREWOOD_AMBUSH) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(QUEST_PYREWOOD_AMBUSH);
        }

        void UpdateAI(const uint32 diff)
        {
            //sLog->outString("DEBUG: p(%i) k(%i) d(%u) W(%i)", Phase, KillCount, diff, WaitTimer);

            if (!QuestInProgress)
                return;

            if (KillCount && Phase < 6)
            {
                if (!UpdateVictim()) //reset() on target Despawn...
                    return;

                DoMeleeAttackIfReady();
                return;
            }

            switch (Phase)
            {
                case 0:
                    if (WaitTimer == WAIT_SECS)
                        me->MonsterSay(NPCSAY_INIT, LANG_UNIVERSAL, 0); //no blizzlike

                    if (WaitTimer <= diff)
                    {
                        WaitTimer -= diff;
                        return;
                    }
                    break;
                case 1:
                    SummonCreatureWithRandomTarget(2060, 1);
                    break;
                case 2:
                    SummonCreatureWithRandomTarget(2061, 2);
                    SummonCreatureWithRandomTarget(2062, 0);
                    break;
                case 3:
                    SummonCreatureWithRandomTarget(2063, 1);
                    SummonCreatureWithRandomTarget(2064, 2);
                    SummonCreatureWithRandomTarget(2065, 0);
                    break;
                case 4:
                    SummonCreatureWithRandomTarget(2066, 1);
                    SummonCreatureWithRandomTarget(2067, 0);
                    SummonCreatureWithRandomTarget(2068, 2);
                    break;
                case 5: //end
                    if (PlayerGUID)
                    {
                        if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                        {
                            me->MonsterSay(NPCSAY_END, LANG_UNIVERSAL, 0); //not blizzlike
                            player->GroupEventHappens(QUEST_PYREWOOD_AMBUSH, me);
                        }
                    }
                    QuestInProgress = false;
                    Reset();
                    break;
            }
            ++Phase; //prepare next phase
        }
    };
};

enum
{
	EVENT_START_1 = 1,
	EVENT_START_2,
	EVENT_START_3,
	EVENT_START_4,
	EVENT_START_5,
	EVENT_SATULE,
	EVENT_MIDDLE_1,
	EVENT_MIDDLE_2,
	EVENT_MIDDLE_3,
	EVENT_END_1,
	EVENT_END_2,
	EVENT_END_3,
	EVENT_END_4,
	EVENT_END_5,
	EVENT_SUMMON_1,
	EVENT_SUMMON_2,
	EVENT_SUMMON_3,
	EVENT_SUMMON_4,
	EVENT_SUMMON_5,
	EVENT_SUMMON_6,
	EVENT_SUMMON_7,
	EVENT_SUMMON_8,
	EVENT_SUMMON_9,

	GROUP_SUMMONS = 1,
	GROUP_ZOMBIES = 1,
	GROUP_GHOULS,
	GROUP_CRYPT,

	ACTION_SALUTE,
	ACTION_MOVE,
	ACTION_DESPAWN,
	ACTION_CONTINUE,

	POINT_PHASE_2,

	NPC_THALOREIN_DAWNSEEKER = 37205,
	NPC_DEATH_KNIGHT = 37542,
	NPC_KILL_CREDIT = 37601,
};

static const Position quelDelarPositions [] = 
{
	{11797.857422f, -7069.258301f, 25.796497f, 5.8f},
	{11788.438477f, -7064.461426f, 25.917301f, 0.0f},
	{11824.782227f, -7078.496582f, 27.280348f, 0.0f},
	{11771.444336f, -7048.375977f, 25.008316f, 5.5f},
};

class npc_thalorein_dawnseeker_dead : public CreatureScript
{
public:
	npc_thalorein_dawnseeker_dead() : CreatureScript("npc_thalorein_dawnseeker_dead") { }

	bool OnGossipSelect(Player* player, Creature* me, uint32, uint32)
	{
		player->CLOSE_GOSSIP_MENU();

		me->SummonCreature(NPC_THALOREIN_DAWNSEEKER, quelDelarPositions[0])->AI()->SetGUID(player->GetGUID());

		return true;
	}
};

class npc_thalorein_dawnseeker : public CreatureScript
{
public:
    npc_thalorein_dawnseeker() : CreatureScript("npc_thalorein_dawnseeker") { }

	struct npc_thalorein_dawnseekerAI : public ScriptedAI
    {
        npc_thalorein_dawnseekerAI(Creature *pCreature) : ScriptedAI(pCreature)
		{
			if (!me->ToTempSummon())
			{
				me->DespawnOrUnsummon(1000);
				return;
			}

			events.Reset();
			events.ScheduleEvent(EVENT_START_1, 1000);
			me->SummonCreatureGroup(GROUP_SUMMONS, &list);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
			player = NULL;
			DK = NULL;
		}

		std::list<TempSummon*> list;
		EventMap events;
		Player* player;
		Creature* DK;

		void DoAction(int32 const action)
		{
			if (action != ACTION_CONTINUE)
				return;

			events.ScheduleEvent(EVENT_END_1, 2000);
		}

		void SetGUID(uint64 guid, int32 type)
		{
			player = Unit::GetPlayer(*me, guid);
		}

		void JustDied(Unit* killer)
		{
			SummonAction(ACTION_DESPAWN);
			DK->DespawnOrUnsummon();
			ScriptedAI::JustDied(killer);
		}

		void SummonAction(uint32 action)
		{
			for (std::list<TempSummon*>::iterator i = list.begin(); i != list.end(); i++)
				if (*i)
				{
					switch (action)
					{
						case ACTION_SALUTE:
							(*i)->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
							break;
						case ACTION_MOVE:
							(*i)->GetMotionMaster()->MovePoint(0, quelDelarPositions[2]);
							break;
						case ACTION_DESPAWN:
							(*i)->DespawnOrUnsummon();
							break;
					}
				}
		}

		void MovementInform(uint32 type, uint32 id)
		{
			if (id == POINT_PHASE_2)
			{
				events.ScheduleEvent(EVENT_MIDDLE_1, 1000);
				SummonAction(ACTION_DESPAWN);
				list.clear();
			}
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 e = events.ExecuteEvent())
			{
				switch (e)
				{
					case EVENT_START_1:
						Talk(0);
						events.ScheduleEvent(EVENT_START_2, 5000);
						break;
					case EVENT_START_2:
						Talk(1);
						events.ScheduleEvent(EVENT_START_3, 6000);
						break;
					case EVENT_START_3:
						Talk(2);
						events.ScheduleEvent(EVENT_START_4, 7300);
						break;
					case EVENT_START_4:
						Talk(3);
						events.ScheduleEvent(EVENT_START_5, 7300); 
						events.ScheduleEvent(EVENT_SATULE, 1000);
						break;
					case EVENT_SATULE:
						SummonAction(ACTION_SALUTE);
						me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
						break;
					case EVENT_START_5:
						me->GetMotionMaster()->MovePoint(POINT_PHASE_2, quelDelarPositions[1]);
						SummonAction(ACTION_MOVE);
						break;

					case EVENT_MIDDLE_1:
						me->SetHomePosition(*me);
						Talk(4);
						events.ScheduleEvent(EVENT_MIDDLE_2, 6600);
						break;
					case EVENT_MIDDLE_2:
						Talk(5);
						DK = me->SummonCreature(NPC_DEATH_KNIGHT, quelDelarPositions[3]);
						events.ScheduleEvent(EVENT_MIDDLE_3, 7100);
						break;
					case EVENT_MIDDLE_3:
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
						break;
						
					case EVENT_END_1:
						Talk(6);
						events.ScheduleEvent(EVENT_END_2, 4000);
						break;
					case EVENT_END_2:
						Talk(7);
						events.ScheduleEvent(EVENT_END_3, 4000);
						break;
					case EVENT_END_3:
						Talk(8);
						events.ScheduleEvent(EVENT_END_4, 4000);
						break;
					case EVENT_END_4:
						Talk(9);
						player->KilledMonsterCredit(NPC_KILL_CREDIT, 0);
						events.ScheduleEvent(EVENT_END_5, 10000);
						break;
					case EVENT_END_5:
						me->DespawnOrUnsummon(1000);
						break;
				}
			}

			if (UpdateVictim())
				DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_thalorein_dawnseekerAI(pCreature);
    }
};

class npc_queldelar_deathknight : public CreatureScript
{
public:
    npc_queldelar_deathknight() : CreatureScript("npc_queldelar_deathknight") { }

	struct npc_queldelar_deathknightAI : public ScriptedAI
    {
        npc_queldelar_deathknightAI(Creature *pCreature) : ScriptedAI(pCreature), summons(me)
		{
			if (!me->ToTempSummon() || !me->ToTempSummon()->GetSummoner())
			{
				me->DespawnOrUnsummon(1000);
				return;
			}
			else
				summoner = me->ToTempSummon()->GetSummoner();

			events.Reset();
			events.ScheduleEvent(EVENT_SUMMON_1, 7100);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
		}

		SummonList summons;
		EventMap events;
		Unit* summoner;
		uint8 summoned;

		void SummonGroup(uint32 group)
		{
			switch (group)
			{
				case GROUP_ZOMBIES:
				case GROUP_GHOULS:
				case GROUP_CRYPT:
					me->SummonCreatureGroup(group);
					break;
			}
			summoned++;
		}
		
		void JustSummoned(Creature* summon)
		{
			summons.Summon(summon);
			summon->AI()->AttackStart(summoner);
		}

		void SummonedCreatureDies(Creature* summon, Unit* killer)
		{
			summons.Despawn(summon);
			if (summons.empty())
				switch (summoned)
				{
					case 1:
						events.ScheduleEvent(EVENT_SUMMON_4, 1000);
						break;
					case 2:
						events.ScheduleEvent(EVENT_SUMMON_6, 1000);
						break;
					case 3:
						events.ScheduleEvent(EVENT_SUMMON_8, 1000);
						break;
				}
		}

		void JustDied(Unit* killer)
		{
			summoner->GetAI()->DoAction(ACTION_CONTINUE);
			ScriptedAI::JustDied(killer);
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 e = events.ExecuteEvent())
			{
				switch (e)
				{
					case EVENT_SUMMON_1:
						Talk(0);
						events.ScheduleEvent(EVENT_SUMMON_2, 6600);
						break;
					case EVENT_SUMMON_2:
						summoned = 0;
						Talk(1);
						events.ScheduleEvent(EVENT_SUMMON_3, 5000);
						break;
					case EVENT_SUMMON_3:
						SummonGroup(GROUP_ZOMBIES);
						break;
					case EVENT_SUMMON_4:
						Talk(2);
						events.ScheduleEvent(EVENT_SUMMON_5, 5000);
						break;
					case EVENT_SUMMON_5:
						SummonGroup(GROUP_GHOULS);
						break;
					case EVENT_SUMMON_6:
						Talk(3);
						events.ScheduleEvent(EVENT_SUMMON_7, 5000);
						break;
					case EVENT_SUMMON_7:
						SummonGroup(GROUP_CRYPT);
						break;
					case EVENT_SUMMON_8:
						Talk(4);
						events.ScheduleEvent(EVENT_SUMMON_9, 7000);
					case EVENT_SUMMON_9:
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
						AttackStart(summoner);
						break;
				}
			}

			if (UpdateVictim())
				DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_queldelar_deathknightAI(pCreature);
    }
};

#define MAP_SUNWELL 580

class npc_warden_of_sunwell : public CreatureScript
{
public:
	npc_warden_of_sunwell() : CreatureScript("npc_warden_of_sunwell") { }

	bool OnGossipSelect(Player* player, Creature* me, uint32, uint32)
	{
		player->CLOSE_GOSSIP_MENU();

		player->TeleportTo(MAP_SUNWELL, 1815.87f, 625.77f, 69.61f, 2.7f, 0);

		return true;
	}
};

/*######
## AddSC
######*/

void AddSC_silverpine_forest()
{
    new npc_deathstalker_erland();
    new pyrewood_ambush();
	new npc_thalorein_dawnseeker_dead();
	new npc_thalorein_dawnseeker();
	new npc_queldelar_deathknight();
	new npc_warden_of_sunwell();
}
