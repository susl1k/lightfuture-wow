/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
SDName: Sunwell_Plateau
SD%Complete: 0
SDComment: Placeholder, Epilogue after Kil'jaeden, Captain Selana Gossips
EndScriptData */

/* ContentData
npc_prophet_velen
npc_captain_selana
EndContentData */

#include "ScriptPCH.h"
#include "sunwell_plateau.h"

/*######
## npc_prophet_velen
######*/

enum ProphetSpeeches
{
    PROPHET_SAY1 = -1580099,
    PROPHET_SAY2 = -1580100,
    PROPHET_SAY3 = -1580101,
    PROPHET_SAY4 = -1580102,
    PROPHET_SAY5 = -1580103,
    PROPHET_SAY6 = -1580104,
    PROPHET_SAY7 = -1580105,
    PROPHET_SAY8 = -1580106
};

enum LiadrinnSpeeches
{
    LIADRIN_SAY1 = -1580107,
    LIADRIN_SAY2 = -1580108,
    LIADRIN_SAY3 = -1580109
};

/*######
## npc_captain_selana
######*/

#define CS_GOSSIP1 "Give me a situation report, Captain."
#define CS_GOSSIP2 "What went wrong?"
#define CS_GOSSIP3 "Why did they stop?"
#define CS_GOSSIP4 "Your insight is appreciated."

enum 
{
	ACTION_START_BLOOD_ELF = 5,
	ACTION_START_ELSE,

	NPC_THERON = 37764,
	NPC_ROMMATH = 37763,
	NPC_AURIC = 37765,
	NPC_SUNWELL_GUARD = 37781,

	SPELL_ICE_CHAINS = 70540,

	GO_QUELDELAR = 201794,
	GO_PORTAL_TO_DALARAN = 195682,
	
	EVENT_SHARED_1 = 1,
	EVENT_SHARED_2,
	EVENT_SHARED_3,
	EVENT_BE_4,
	EVENT_BE_5,
	EVENT_BE_6,
	EVENT_BE_7,
	EVENT_BE_8,
	EVENT_BE_9,
	EVENT_BE_10,
	EVENT_BE_11,
	EVENT_ELSE_4,
	EVENT_ELSE_5,
	EVENT_ELSE_6,
	EVENT_ELSE_7,
	EVENT_ELSE_8,
	EVENT_ELSE_9,
	EVENT_ELSE_10,
	EVENT_ELSE_11,
	EVENT_ELSE_12,
	EVENT_ELSE_13,
	EVENT_ELSE_14,
	EVENT_ELSE_15,

};

static const Position quelDelarPositions [] = 
{
	{1687.22f, 627.08f, 29.98f, 0.299610f},//quel delar
	{1680.908569f, 625.846008f, 27.759514f, 0.0f},//movepos1
	{1681.079102f, 628.639465f, 27.692621f, 0.0f},//movepo2
	{1681.597534f, 623.386780f, 27.718222f, 0.0f},//movepos3
	{1731.51f, 658.63f, 28.05f, 0.0f},//portalpos
	{1684.322754f, 626.617004f, 27.440504f, 0.0f},//movepos1 alt
	{1649.654785f, 625.897339f, 28.243248f, 0.0f},//moveknockback1
};

class npc_chamberlain_galiros : public CreatureScript
{
public:
	npc_chamberlain_galiros() : CreatureScript("npc_chamberlain_galiros") { }

	struct npc_chamberlain_galirosAI : public ScriptedAI
	{
		npc_chamberlain_galirosAI(Creature* creature) : ScriptedAI(creature), talked(false) { }

		bool talked;

		void MoveInLineOfSight(Unit* who)
		{
			if (!talked)
				if (who->ToPlayer())
					if (who->GetExactDist(me) < 30.0f)
					{
						Talk(0, who->GetGUID());
						talked = true;
					}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_chamberlain_galirosAI(creature);
	}
};

class spell_cleanse_queldelar : public SpellScriptLoader
{
	public:
		spell_cleanse_queldelar() : SpellScriptLoader("spell_cleanse_queldelar") { }

		class spell_cleanse_queldelar_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_cleanse_queldelar_SpellScript);

			void HandleStartScript(SpellEffIndex effIndex)
			{
				if (Creature* theron = GetCaster()->FindNearestCreature(NPC_THERON, 100.0f))
					if (theron->AI())
						theron->AI()->SetGUID(GetCaster()->GetGUID());
				
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_cleanse_queldelar_SpellScript::HandleStartScript, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_cleanse_queldelar_SpellScript();
		}
};

class npc_lorthemar_theron : public CreatureScript
{
public:
	npc_lorthemar_theron() : CreatureScript("npc_lorthemar_theron") { }

	struct npc_lorthemar_theronAI : public ScriptedAI
	{
		npc_lorthemar_theronAI(Creature* creature) : ScriptedAI(creature), quelDelar(NULL), nearGuard(NULL), rommath(NULL), auric(NULL), player(NULL) { }

		EventMap events;

		GameObject* quelDelar;
		Creature* nearGuard;
		Creature* rommath;
		Creature* auric;
		Player* player;

		void SetGUID(uint64 guid, int32 = 0)
		{
			quelDelar = me->SummonGameObject(GO_QUELDELAR, quelDelarPositions[0].GetPositionX(), quelDelarPositions[0].GetPositionY(), quelDelarPositions[0].GetPositionZ(), quelDelarPositions[0].GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 0);
			quelDelar->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

			rommath = me->FindNearestCreature(NPC_ROMMATH, 100.0f);
			auric = me->FindNearestCreature(NPC_AURIC, 100.0f);
			nearGuard = me->FindNearestCreature(NPC_SUNWELL_GUARD, 100.0f);

			player = me->GetPlayer(*me, guid);

			if (!quelDelar || !rommath || !auric || !nearGuard || !player)
				return;
			
			events.Reset();
			events.ScheduleEvent(EVENT_SHARED_1, 1000);
		}

		void TalkQuel(Creature* who, uint32 talkID, uint64 guid = 0)
		{
			who->AI()->Talk(talkID, guid);
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 e = events.ExecuteEvent())
			{
				switch (e)
				{
					case EVENT_SHARED_1:
						TalkQuel(nearGuard, 0);
						events.ScheduleEvent(EVENT_SHARED_2, 5000);
						break;
					case EVENT_SHARED_2:
						TalkQuel(me, 0);
						me->MonsterMoveWithSpeed(quelDelarPositions[1].GetPositionX(), quelDelarPositions[1].GetPositionY(), quelDelarPositions[1].GetPositionZ(), 2.5f);
						rommath->MonsterMoveWithSpeed(quelDelarPositions[2].GetPositionX(), quelDelarPositions[2].GetPositionY(), quelDelarPositions[2].GetPositionZ(), 2.5f);
						auric->MonsterMoveWithSpeed(quelDelarPositions[3].GetPositionX(), quelDelarPositions[3].GetPositionY(), quelDelarPositions[3].GetPositionZ(), 2.5f);
						events.ScheduleEvent(EVENT_SHARED_3, 10000);
						break;
					case EVENT_SHARED_3:
						TalkQuel(rommath, 0);
						if (player)
						{
							if (player->getRace() == RACE_BLOODELF)
								events.ScheduleEvent(EVENT_BE_4, 5000);
							else
								events.ScheduleEvent(EVENT_ELSE_4, 5000);
						}
						break;
					case EVENT_BE_4:
						TalkQuel(me, 1);
						events.ScheduleEvent(EVENT_BE_5, 7000);
						break;
					case EVENT_BE_5:
						TalkQuel(me, 2, player->GetGUID());
						events.ScheduleEvent(EVENT_BE_6, 10000);
						break;
					case EVENT_BE_6:
						TalkQuel(rommath, 1, player->GetGUID());
						events.ScheduleEvent(EVENT_BE_7, 7000);
						break;
					case EVENT_BE_7:
						TalkQuel(rommath, 2);
						events.ScheduleEvent(EVENT_BE_8, 8000);
						break;
					case EVENT_BE_8:
						TalkQuel(rommath, 3);
						events.ScheduleEvent(EVENT_BE_9, 7600);
						break;
					case EVENT_BE_9:
						TalkQuel(auric, 0);
						events.ScheduleEvent(EVENT_BE_10, 6000);
						break;
					case EVENT_BE_10:
						TalkQuel(auric, 1);
						events.ScheduleEvent(EVENT_BE_11, 9000);
						break;
					case EVENT_BE_11:
						TalkQuel(rommath, 4, player->GetGUID());
						quelDelar->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
						me->SummonGameObject(GO_PORTAL_TO_DALARAN, quelDelarPositions[4].GetPositionX(), quelDelarPositions[4].GetPositionY(), quelDelarPositions[4].GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
						break;
						
					case EVENT_ELSE_4:
						TalkQuel(me, 3, player->GetGUID());
						events.ScheduleEvent(EVENT_ELSE_5, 6000);
						break;
					case EVENT_ELSE_5:
						me->MonsterMoveWithSpeed(quelDelarPositions[5].GetPositionX(), quelDelarPositions[5].GetPositionY(), quelDelarPositions[5].GetPositionZ(), 1.5f);
						events.ScheduleEvent(EVENT_ELSE_6, 4000);
						break;
					case EVENT_ELSE_6:
						me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
						events.ScheduleEvent(EVENT_ELSE_7, 1000);
						break;
					case EVENT_ELSE_7:
						me->GetMotionMaster()->MoveJump(quelDelarPositions[6].GetPositionX(), quelDelarPositions[6].GetPositionY(), quelDelarPositions[6].GetPositionZ(), 25.0f, 15.0f);
						events.ScheduleEvent(EVENT_ELSE_8, 2000);
						break;
					case EVENT_ELSE_8:
						TalkQuel(nearGuard, 1);
						nearGuard->GetMotionMaster()->MoveFollow(player, 1.0f, 0.0f);
						player->AddAura(SPELL_ICE_CHAINS, player);
						events.ScheduleEvent(EVENT_ELSE_9, 1500);
						break;
					case EVENT_ELSE_9:
						TalkQuel(rommath, 5);
						events.ScheduleEvent(EVENT_ELSE_10, 500);
						break;
					case EVENT_ELSE_10:
						TalkQuel(nearGuard, 2);
						nearGuard->HandleEmoteCommand(EMOTE_STATE_READY1H);
						events.ScheduleEvent(EVENT_ELSE_11, 5000);
						break;
					case EVENT_ELSE_11:
						TalkQuel(auric, 2);
						events.ScheduleEvent(EVENT_ELSE_12, 6000);
						break;
					case EVENT_ELSE_12:
						TalkQuel(auric, 3);
						events.ScheduleEvent(EVENT_ELSE_13, 10000);
						break;
					case EVENT_ELSE_13:
						TalkQuel(rommath, 6);
						nearGuard->HandleEmoteCommand(EMOTE_STATE_STAND);
						nearGuard->GetMotionMaster()->MoveTargetedHome();
						player->RemoveAurasDueToSpell(SPELL_ICE_CHAINS);
						events.ScheduleEvent(EVENT_ELSE_14, 5000);
						break;
					case EVENT_ELSE_14:
						TalkQuel(rommath, 7, player->GetGUID());
						quelDelar->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
						events.ScheduleEvent(EVENT_ELSE_15, 12000);
						break;
					case EVENT_ELSE_15:
						TalkQuel(auric, 4, player->GetGUID());
						me->SummonGameObject(GO_PORTAL_TO_DALARAN, quelDelarPositions[4].GetPositionX(), quelDelarPositions[4].GetPositionY(), quelDelarPositions[4].GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
						break;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_lorthemar_theronAI(creature);
	}
};

void AddSC_sunwell_plateau()
{
	new npc_chamberlain_galiros();
	new spell_cleanse_queldelar();
	new npc_lorthemar_theron();
}
