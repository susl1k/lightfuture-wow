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
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_lunaclaw_spirit      80%    support for quests 6001/6002 (Body and Heart)
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests   80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 565
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_mount_vendor        100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_rogue_trainer        80%    Scripted trainers, so they are able to offer item 17126 for class quest 6681
npc_sayge               100%    Darkmoon event fortune teller, buff player based on answers given
npc_snake_trap_serpents  80%    AI for snakes that summoned by Snake Trap
npc_shadowfiend         100%    restore 5% of owner's mana when shadowfiend die from damage
npc_locksmith            75%    list of keys needs to be confirmed
npc_firework            100%    NPC's summoned by rockets and rocket clusters, for making them cast visual
a lot more...see end of AddSC_npcs_special()
EndContentData */

#include "ScriptPCH.h"
#include "LFGMgr.h"
#include "ScriptedEscortAI.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "SpellAuras.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "Vehicle.h"
#include "Group.h"

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
	SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon Creature at smaller range
	SPAWNTYPE_ALARMBOT,                                     // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
	uint32 thisCreatureEntry;
	uint32 spawnedCreatureEntry;
	SpawnType spawnType;
};

enum eEnums
{
	SPELL_GUARDS_MARK               = 38067,
	AURA_DURATION_TIME_LEFT         = 5000
};

float const RANGE_TRIPWIRE          = 15.0f;
float const RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation spawnAssociations[] =
{
	{2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
	{2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
	{21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
	{21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
	{21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
	{21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
	{21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
	{22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
	{22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
	{22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
	{22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
	{22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
	{22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
	{22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
	{22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
	{22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
	{22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
	{22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
	{22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
	{22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
	{22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
	{22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
	{22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
	{22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
	{22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
	{22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
	{22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

class npc_air_force_bots : public CreatureScript
{
public:
	npc_air_force_bots() : CreatureScript("npc_air_force_bots") { }

	struct npc_air_force_botsAI : public ScriptedAI
	{
		npc_air_force_botsAI(Creature* creature) : ScriptedAI(creature)
		{
			SpawnAssoc = NULL;
			SpawnedGUID = 0;

			// find the correct spawnhandling
			static uint32 entryCount = sizeof(spawnAssociations) / sizeof(SpawnAssociation);

			for (uint8 i = 0; i < entryCount; ++i)
			{
				if (spawnAssociations[i].thisCreatureEntry == creature->GetEntry())
				{
					SpawnAssoc = &spawnAssociations[i];
					break;
				}
			}

			if (!SpawnAssoc)
				sLog->outErrorDb("TCSR: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", creature->GetEntry());
			else
			{
				CreatureTemplate const* spawnedTemplate = sObjectMgr->GetCreatureTemplate(SpawnAssoc->spawnedCreatureEntry);

				if (!spawnedTemplate)
				{
					sLog->outErrorDb("TCSR: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", SpawnAssoc->spawnedCreatureEntry);
					SpawnAssoc = NULL;
					return;
				}
			}
		}

		SpawnAssociation* SpawnAssoc;
		uint64 SpawnedGUID;

		void Reset() {}

		Creature* SummonGuard()
		{
			Creature* summoned = me->SummonCreature(SpawnAssoc->spawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

			if (summoned)
				SpawnedGUID = summoned->GetGUID();
			else
			{
				sLog->outErrorDb("TCSR: npc_air_force_bots: wasn't able to spawn Creature %u", SpawnAssoc->spawnedCreatureEntry);
				SpawnAssoc = NULL;
			}

			return summoned;
		}

		Creature* GetSummonedGuard()
		{
			Creature* creature = Unit::GetCreature(*me, SpawnedGUID);

			if (creature && creature->isAlive())
				return creature;

			return NULL;
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (!SpawnAssoc)
				return;

			if (me->IsValidAttackTarget(who))
			{
				Player* playerTarget = who->ToPlayer();

				// airforce guards only spawn for players
				if (!playerTarget)
					return;

				Creature* lastSpawnedGuard = SpawnedGUID == 0 ? NULL : GetSummonedGuard();

				// prevent calling Unit::GetUnit at next MoveInLineOfSight call - speedup
				if (!lastSpawnedGuard)
					SpawnedGUID = 0;

				switch (SpawnAssoc->spawnType)
				{
					case SPAWNTYPE_ALARMBOT:
					{
						if (!who->IsWithinDistInMap(me, RANGE_GUARDS_MARK))
							return;

						Aura* markAura = who->GetAura(SPELL_GUARDS_MARK);
						if (markAura)
						{
							// the target wasn't able to move out of our range within 25 seconds
							if (!lastSpawnedGuard)
							{
								lastSpawnedGuard = SummonGuard();

								if (!lastSpawnedGuard)
									return;
							}

							if (markAura->GetDuration() < AURA_DURATION_TIME_LEFT)
								if (!lastSpawnedGuard->getVictim())
									lastSpawnedGuard->AI()->AttackStart(who);
						}
						else
						{
							if (!lastSpawnedGuard)
								lastSpawnedGuard = SummonGuard();

							if (!lastSpawnedGuard)
								return;

							lastSpawnedGuard->CastSpell(who, SPELL_GUARDS_MARK, true);
						}
						break;
					}
					case SPAWNTYPE_TRIPWIRE_ROOFTOP:
					{
						if (!who->IsWithinDistInMap(me, RANGE_TRIPWIRE))
							return;

						if (!lastSpawnedGuard)
							lastSpawnedGuard = SummonGuard();

						if (!lastSpawnedGuard)
							return;

						// ROOFTOP only triggers if the player is on the ground
						if (!playerTarget->IsFlying() && !lastSpawnedGuard->getVictim())
							lastSpawnedGuard->AI()->AttackStart(who);

						break;
					}
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_air_force_botsAI(creature);
	}
};

/*######
## npc_lunaclaw_spirit
######*/

enum
{
	QUEST_BODY_HEART_A      = 6001,
	QUEST_BODY_HEART_H      = 6002,

	TEXT_ID_DEFAULT         = 4714,
	TEXT_ID_PROGRESS        = 4715
};

#define GOSSIP_ITEM_GRANT   "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

class npc_lunaclaw_spirit : public CreatureScript
{
public:
	npc_lunaclaw_spirit() : CreatureScript("npc_lunaclaw_spirit") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		player->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, creature->GetGUID());
			player->AreaExploredOrEventHappens(player->GetTeam() == ALLIANCE ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
		}
		return true;
	}
};

/*########
# npc_chicken_cluck
#########*/

#define EMOTE_HELLO         -1070004
#define EMOTE_CLUCK_TEXT    -1070006

#define QUEST_CLUCK         3861
#define FACTION_FRIENDLY    35
#define FACTION_CHICKEN     31

class npc_chicken_cluck : public CreatureScript
{
public:
	npc_chicken_cluck() : CreatureScript("npc_chicken_cluck") { }

	struct npc_chicken_cluckAI : public ScriptedAI
	{
		npc_chicken_cluckAI(Creature* creature) : ScriptedAI(creature) {}

		uint32 ResetFlagTimer;

		void Reset()
		{
			ResetFlagTimer = 120000;
			me->setFaction(FACTION_CHICKEN);
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
		}

		void EnterCombat(Unit* /*who*/) {}

		void UpdateAI(uint32 const diff)
		{
			// Reset flags after a certain time has passed so that the next player has to start the 'event' again
			if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
			{
				if (ResetFlagTimer <= diff)
				{
					EnterEvadeMode();
					return;
				}
				else
					ResetFlagTimer -= diff;
			}

			if (UpdateVictim())
				DoMeleeAttackIfReady();
		}

		void ReceiveEmote(Player* player, uint32 emote)
		{
			switch (emote)
			{
				case TEXT_EMOTE_CHICKEN:
					if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE && rand() % 30 == 1)
					{
						me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
						me->setFaction(FACTION_FRIENDLY);
						DoScriptText(EMOTE_HELLO, me);
					}
					break;
				case TEXT_EMOTE_CHEER:
					if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
					{
						me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
						me->setFaction(FACTION_FRIENDLY);
						DoScriptText(EMOTE_CLUCK_TEXT, me);
					}
					break;
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_chicken_cluckAI(creature);
	}

	bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest)
	{
		if (quest->GetQuestId() == QUEST_CLUCK)
			CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

		return true;
	}

	bool OnQuestComplete(Player* /*player*/, Creature* creature, Quest const* quest)
	{
		if (quest->GetQuestId() == QUEST_CLUCK)
			CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

		return true;
	}
};

/*######
## npc_dancing_flames
######*/

#define SPELL_BRAZIER       45423
#define SPELL_SEDUCTION     47057
#define SPELL_FIERY_AURA    45427

class npc_dancing_flames : public CreatureScript
{
public:
	npc_dancing_flames() : CreatureScript("npc_dancing_flames") { }

	struct npc_dancing_flamesAI : public ScriptedAI
	{
		npc_dancing_flamesAI(Creature* creature) : ScriptedAI(creature) {}

		bool Active;
		uint32 CanIteract;

		void Reset()
		{
			Active = true;
			CanIteract = 3500;
			DoCast(me, SPELL_BRAZIER, true);
			DoCast(me, SPELL_FIERY_AURA, false);
			float x, y, z;
			me->GetPosition(x, y, z);
			me->Relocate(x, y, z + 0.94f);
			me->SetDisableGravity(true);
			me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
			WorldPacket data;                       //send update position to client
			me->BuildHeartBeatMsg(&data);
			me->SendMessageToSet(&data, true);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!Active)
			{
				if (CanIteract <= diff)
				{
					Active = true;
					CanIteract = 3500;
					me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
				}
				else
					CanIteract -= diff;
			}
		}

		void EnterCombat(Unit* /*who*/){}

		void ReceiveEmote(Player* player, uint32 emote)
		{
			if (me->IsWithinLOS(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()) && me->IsWithinDistInMap(player, 30.0f))
			{
				me->SetInFront(player);
				Active = false;

				WorldPacket data;
				me->BuildHeartBeatMsg(&data);
				me->SendMessageToSet(&data, true);
				switch (emote)
				{
					case TEXT_EMOTE_KISS:
						me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);
						break;
					case TEXT_EMOTE_WAVE:
						me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
						break;
					case TEXT_EMOTE_BOW:
						me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
						break;
					case TEXT_EMOTE_JOKE:
						me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
						break;
					case TEXT_EMOTE_DANCE:
						if (!player->HasAura(SPELL_SEDUCTION))
							DoCast(player, SPELL_SEDUCTION, true);
						break;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_dancing_flamesAI(creature);
	}
};

/*######
## Triage quest
######*/

//signed for 9623
#define SAY_DOC1    -1000201
#define SAY_DOC2    -1000202
#define SAY_DOC3    -1000203

#define DOCTOR_ALLIANCE     12939
#define DOCTOR_HORDE        12920
#define ALLIANCE_COORDS     7
#define HORDE_COORDS        6

struct Location
{
	float x, y, z, o;
};

static Location AllianceCoords[]=
{
	{-3757.38f, -4533.05f, 14.16f, 3.62f},                      // Top-far-right bunk as seen from entrance
	{-3754.36f, -4539.13f, 14.16f, 5.13f},                      // Top-far-left bunk
	{-3749.54f, -4540.25f, 14.28f, 3.34f},                      // Far-right bunk
	{-3742.10f, -4536.85f, 14.28f, 3.64f},                      // Right bunk near entrance
	{-3755.89f, -4529.07f, 14.05f, 0.57f},                      // Far-left bunk
	{-3749.51f, -4527.08f, 14.07f, 5.26f},                      // Mid-left bunk
	{-3746.37f, -4525.35f, 14.16f, 5.22f},                      // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[]=
{
	{-1013.75f, -3492.59f, 62.62f, 4.34f},                      // Left, Behind
	{-1017.72f, -3490.92f, 62.62f, 4.34f},                      // Right, Behind
	{-1015.77f, -3497.15f, 62.82f, 4.34f},                      // Left, Mid
	{-1019.51f, -3495.49f, 62.82f, 4.34f},                      // Right, Mid
	{-1017.25f, -3500.85f, 62.98f, 4.34f},                      // Left, front
	{-1020.95f, -3499.21f, 62.98f, 4.34f}                       // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

uint32 const AllianceSoldierId[3] =
{
	12938,                                                  // 12938 Injured Alliance Soldier
	12936,                                                  // 12936 Badly injured Alliance Soldier
	12937                                                   // 12937 Critically injured Alliance Soldier
};

uint32 const HordeSoldierId[3] =
{
	12923,                                                  //12923 Injured Soldier
	12924,                                                  //12924 Badly injured Soldier
	12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/
class npc_doctor : public CreatureScript
{
public:
	npc_doctor() : CreatureScript("npc_doctor") {}

	struct npc_doctorAI : public ScriptedAI
	{
		npc_doctorAI(Creature* creature) : ScriptedAI(creature) {}

		uint64 PlayerGUID;

		uint32 SummonPatientTimer;
		uint32 SummonPatientCount;
		uint32 PatientDiedCount;
		uint32 PatientSavedCount;

		bool Event;

		std::list<uint64> Patients;
		std::vector<Location*> Coordinates;

		void Reset()
		{
			PlayerGUID = 0;

			SummonPatientTimer = 10000;
			SummonPatientCount = 0;
			PatientDiedCount = 0;
			PatientSavedCount = 0;

			Patients.clear();
			Coordinates.clear();

			Event = false;

			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		}

		void BeginEvent(Player* player)
		{
			PlayerGUID = player->GetGUID();

			SummonPatientTimer = 10000;
			SummonPatientCount = 0;
			PatientDiedCount = 0;
			PatientSavedCount = 0;

			switch (me->GetEntry())
			{
				case DOCTOR_ALLIANCE:
					for (uint8 i = 0; i < ALLIANCE_COORDS; ++i)
						Coordinates.push_back(&AllianceCoords[i]);
					break;
				case DOCTOR_HORDE:
					for (uint8 i = 0; i < HORDE_COORDS; ++i)
						Coordinates.push_back(&HordeCoords[i]);
					break;
			}

			Event = true;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		}

		void PatientDied(Location* point)
		{
			Player* player = Unit::GetPlayer(*me, PlayerGUID);
			if (player && ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
			{
				++PatientDiedCount;

				if (PatientDiedCount > 5 && Event)
				{
					if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
						player->FailQuest(6624);
					else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
						player->FailQuest(6622);

					Reset();
					return;
				}

				Coordinates.push_back(point);
			}
			else
				// If no player or player abandon quest in progress
				Reset();
		}

		void PatientSaved(Creature* /*soldier*/, Player* player, Location* point)
		{
			if (player && PlayerGUID == player->GetGUID())
			{
				if ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
				{
					++PatientSavedCount;

					if (PatientSavedCount == 15)
					{
						if (!Patients.empty())
						{
							std::list<uint64>::const_iterator itr;
							for (itr = Patients.begin(); itr != Patients.end(); ++itr)
							{
								if (Creature* patient = Unit::GetCreature((*me), *itr))
									patient->setDeathState(JUST_DIED);
							}
						}

						if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
							player->AreaExploredOrEventHappens(6624);
						else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
							player->AreaExploredOrEventHappens(6622);

						Reset();
						return;
					}

					Coordinates.push_back(point);
				}
			}
		}

		void UpdateAI(uint32 const diff);

		void EnterCombat(Unit* /*who*/){}
	};

	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
	{
		if ((quest->GetQuestId() == 6624) || (quest->GetQuestId() == 6622))
			CAST_AI(npc_doctor::npc_doctorAI, creature->AI())->BeginEvent(player);

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_doctorAI(creature);
	}
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

class npc_injured_patient : public CreatureScript
{
public:
	npc_injured_patient() : CreatureScript("npc_injured_patient") { }

	struct npc_injured_patientAI : public ScriptedAI
	{
		npc_injured_patientAI(Creature* creature) : ScriptedAI(creature) {}

		uint64 DoctorGUID;
		Location* Coord;

		void Reset()
		{
			DoctorGUID = 0;
			Coord = NULL;

			//no select
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

			//no regen health
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

			//to make them lay with face down
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);

			uint32 mobId = me->GetEntry();

			switch (mobId)
			{                                                   //lower max health
				case 12923:
				case 12938:                                     //Injured Soldier
					me->SetHealth(me->CountPctFromMaxHealth(75));
					break;
				case 12924:
				case 12936:                                     //Badly injured Soldier
					me->SetHealth(me->CountPctFromMaxHealth(50));
					break;
				case 12925:
				case 12937:                                     //Critically injured Soldier
					me->SetHealth(me->CountPctFromMaxHealth(25));
					break;
			}
		}

		void EnterCombat(Unit* /*who*/){}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (caster->GetTypeId() == TYPEID_PLAYER && me->isAlive() && spell->Id == 20804)
			{
				if ((CAST_PLR(caster)->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (CAST_PLR(caster)->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
					if (DoctorGUID)
						if (Creature* doctor = Unit::GetCreature(*me, DoctorGUID))
							CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientSaved(me, CAST_PLR(caster), Coord);

				//make not selectable
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

				//regen health
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

				//stand up
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);

				DoScriptText(RAND(SAY_DOC1, SAY_DOC2, SAY_DOC3), me);

				uint32 mobId = me->GetEntry();
				me->SetWalk(false);

				switch (mobId)
				{
					case 12923:
					case 12924:
					case 12925:
						me->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
						break;
					case 12936:
					case 12937:
					case 12938:
						me->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
						break;
				}
			}
		}

		void UpdateAI(uint32 const /*diff*/)
		{
			//lower HP on every world tick makes it a useful counter, not officlone though
			if (me->isAlive() && me->GetHealth() > 6)
				me->ModifyHealth(-5);

			if (me->isAlive() && me->GetHealth() <= 6)
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->setDeathState(JUST_DIED);
				me->SetFlag(UNIT_DYNAMIC_FLAGS, 32);

				if (DoctorGUID)
					if (Creature* doctor = Unit::GetCreature((*me), DoctorGUID))
						CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientDied(Coord);
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_injured_patientAI(creature);
	}
};

void npc_doctor::npc_doctorAI::UpdateAI(uint32 const diff)
{
	if (Event && SummonPatientCount >= 20)
	{
		Reset();
		return;
	}

	if (Event)
	{
		if (SummonPatientTimer <= diff)
		{
			if (Coordinates.empty())
				return;

			std::vector<Location*>::iterator itr = Coordinates.begin() + rand() % Coordinates.size();
			uint32 patientEntry = 0;

			switch (me->GetEntry())
			{
				case DOCTOR_ALLIANCE:
					patientEntry = AllianceSoldierId[rand() % 3];
					break;
				case DOCTOR_HORDE:
					patientEntry = HordeSoldierId[rand() % 3];
					break;
				default:
					sLog->outError("TSCR: Invalid entry for Triage doctor. Please check your database");
					return;
			}

			if (Location* point = *itr)
			{
				if (Creature* Patient = me->SummonCreature(patientEntry, point->x, point->y, point->z, point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
				{
					//303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
					Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

					Patients.push_back(Patient->GetGUID());
					CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->DoctorGUID = me->GetGUID();
					CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->Coord = point;

					Coordinates.erase(itr);
				}
			}
			SummonPatientTimer = 10000;
			++SummonPatientCount;
		}
		else
			SummonPatientTimer -= diff;
	}
}

/*######
## npc_garments_of_quests
######*/

//TODO: get text for each NPC

enum eGarments
{
	SPELL_LESSER_HEAL_R2    = 2052,
	SPELL_FORTITUDE_R1      = 1243,

	QUEST_MOON              = 5621,
	QUEST_LIGHT_1           = 5624,
	QUEST_LIGHT_2           = 5625,
	QUEST_SPIRIT            = 5648,
	QUEST_DARKNESS          = 5650,

	ENTRY_SHAYA             = 12429,
	ENTRY_ROBERTS           = 12423,
	ENTRY_DOLF              = 12427,
	ENTRY_KORJA             = 12430,
	ENTRY_DG_KEL            = 12428,

	//used by 12429, 12423, 12427, 12430, 12428, but signed for 12429
	SAY_COMMON_HEALED       = -1000164,
	SAY_DG_KEL_THANKS       = -1000165,
	SAY_DG_KEL_GOODBYE      = -1000166,
	SAY_ROBERTS_THANKS      = -1000167,
	SAY_ROBERTS_GOODBYE     = -1000168,
	SAY_KORJA_THANKS        = -1000169,
	SAY_KORJA_GOODBYE       = -1000170,
	SAY_DOLF_THANKS         = -1000171,
	SAY_DOLF_GOODBYE        = -1000172,
	SAY_SHAYA_THANKS        = -1000173,
	SAY_SHAYA_GOODBYE       = -1000174, //signed for 21469
};

class npc_garments_of_quests : public CreatureScript
{
public:
	npc_garments_of_quests() : CreatureScript("npc_garments_of_quests") { }

	struct npc_garments_of_questsAI : public npc_escortAI
	{
		npc_garments_of_questsAI(Creature* creature) : npc_escortAI(creature) {Reset();}

		uint64 CasterGUID;

		bool IsHealed;
		bool CanRun;

		uint32 RunAwayTimer;

		void Reset()
		{
			CasterGUID = 0;

			IsHealed = false;
			CanRun = false;

			RunAwayTimer = 5000;

			me->SetStandState(UNIT_STAND_STATE_KNEEL);
			//expect database to have RegenHealth=0
			me->SetHealth(me->CountPctFromMaxHealth(70));
		}

		void EnterCombat(Unit* /*who*/) {}

		void SpellHit(Unit* caster, SpellInfo const* Spell)
		{
			if (Spell->Id == SPELL_LESSER_HEAL_R2 || Spell->Id == SPELL_FORTITUDE_R1)
			{
				//not while in combat
				if (me->isInCombat())
					return;

				//nothing to be done now
				if (IsHealed && CanRun)
					return;

				if (Player* player = caster->ToPlayer())
				{
					switch (me->GetEntry())
					{
						case ENTRY_SHAYA:
							if (player->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
							{
								if (IsHealed && !CanRun && Spell->Id == SPELL_FORTITUDE_R1)
								{
									DoScriptText(SAY_SHAYA_THANKS, me, caster);
									CanRun = true;
								}
								else if (!IsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
								{
									CasterGUID = caster->GetGUID();
									me->SetStandState(UNIT_STAND_STATE_STAND);
									DoScriptText(SAY_COMMON_HEALED, me, caster);
									IsHealed = true;
								}
							}
							break;
						case ENTRY_ROBERTS:
							if (player->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
							{
								if (IsHealed && !CanRun && Spell->Id == SPELL_FORTITUDE_R1)
								{
									DoScriptText(SAY_ROBERTS_THANKS, me, caster);
									CanRun = true;
								}
								else if (!IsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
								{
									CasterGUID = caster->GetGUID();
									me->SetStandState(UNIT_STAND_STATE_STAND);
									DoScriptText(SAY_COMMON_HEALED, me, caster);
									IsHealed = true;
								}
							}
							break;
						case ENTRY_DOLF:
							if (player->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
							{
								if (IsHealed && !CanRun && Spell->Id == SPELL_FORTITUDE_R1)
								{
									DoScriptText(SAY_DOLF_THANKS, me, caster);
									CanRun = true;
								}
								else if (!IsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
								{
									CasterGUID = caster->GetGUID();
									me->SetStandState(UNIT_STAND_STATE_STAND);
									DoScriptText(SAY_COMMON_HEALED, me, caster);
									IsHealed = true;
								}
							}
							break;
						case ENTRY_KORJA:
							if (player->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
							{
								if (IsHealed && !CanRun && Spell->Id == SPELL_FORTITUDE_R1)
								{
									DoScriptText(SAY_KORJA_THANKS, me, caster);
									CanRun = true;
								}
								else if (!IsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
								{
									CasterGUID = caster->GetGUID();
									me->SetStandState(UNIT_STAND_STATE_STAND);
									DoScriptText(SAY_COMMON_HEALED, me, caster);
									IsHealed = true;
								}
							}
							break;
						case ENTRY_DG_KEL:
							if (player->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
							{
								if (IsHealed && !CanRun && Spell->Id == SPELL_FORTITUDE_R1)
								{
									DoScriptText(SAY_DG_KEL_THANKS, me, caster);
									CanRun = true;
								}
								else if (!IsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
								{
									CasterGUID = caster->GetGUID();
									me->SetStandState(UNIT_STAND_STATE_STAND);
									DoScriptText(SAY_COMMON_HEALED, me, caster);
									IsHealed = true;
								}
							}
							break;
					}

					//give quest credit, not expect any special quest objectives
					if (CanRun)
						player->TalkedToCreature(me->GetEntry(), me->GetGUID());
				}
			}
		}

		void WaypointReached(uint32 /*waypointId*/)
		{

		}

		void UpdateAI(uint32 const diff)
		{
			if (CanRun && !me->isInCombat())
			{
				if (RunAwayTimer <= diff)
				{
					if (Unit* unit = Unit::GetUnit(*me, CasterGUID))
					{
						switch (me->GetEntry())
						{
							case ENTRY_SHAYA:
								DoScriptText(SAY_SHAYA_GOODBYE, me, unit);
								break;
							case ENTRY_ROBERTS:
								DoScriptText(SAY_ROBERTS_GOODBYE, me, unit);
								break;
							case ENTRY_DOLF:
								DoScriptText(SAY_DOLF_GOODBYE, me, unit);
								break;
							case ENTRY_KORJA:
								DoScriptText(SAY_KORJA_GOODBYE, me, unit);
								break;
							case ENTRY_DG_KEL:
								DoScriptText(SAY_DG_KEL_GOODBYE, me, unit);
								break;
						}

						Start(false, true, true);
					}
					else
						EnterEvadeMode();                       //something went wrong

					RunAwayTimer = 30000;
				}
				else
					RunAwayTimer -= diff;
			}

			npc_escortAI::UpdateAI(diff);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_garments_of_questsAI(creature);
	}
};

/*######
## npc_guardian
######*/

#define SPELL_DEATHTOUCH                5

class npc_guardian : public CreatureScript
{
public:
	npc_guardian() : CreatureScript("npc_guardian") { }

	struct npc_guardianAI : public ScriptedAI
	{
		npc_guardianAI(Creature* creature) : ScriptedAI(creature) {}

		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		}

		void EnterCombat(Unit* /*who*/)
		{
		}

		void UpdateAI(uint32 const /*diff*/)
		{
			if (!UpdateVictim())
				return;

			if (me->isAttackReady())
			{
				DoCast(me->getVictim(), SPELL_DEATHTOUCH, true);
				me->resetAttackTimer();
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_guardianAI(creature);
	}
};

/*######
## npc_mount_vendor
######*/

class npc_mount_vendor : public CreatureScript
{
public:
	npc_mount_vendor() : CreatureScript("npc_mount_vendor") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->isQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		bool canBuy = false;
		uint32 vendor = creature->GetEntry();
		uint8 race = player->getRace();

		switch (vendor)
		{
			case 384:                                           //Katie Hunter
			case 1460:                                          //Unger Statforth
			case 2357:                                          //Merideth Carlson
			case 4885:                                          //Gregor MacVince
				if (player->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
					player->SEND_GOSSIP_MENU(5855, creature->GetGUID());
				else canBuy = true;
				break;
			case 1261:                                          //Veron Amberstill
				if (player->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
					player->SEND_GOSSIP_MENU(5856, creature->GetGUID());
				else canBuy = true;
				break;
			case 3362:                                          //Ogunaro Wolfrunner
				if (player->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
					player->SEND_GOSSIP_MENU(5841, creature->GetGUID());
				else canBuy = true;
				break;
			case 3685:                                          //Harb Clawhoof
				if (player->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
					player->SEND_GOSSIP_MENU(5843, creature->GetGUID());
				else canBuy = true;
				break;
			case 4730:                                          //Lelanai
				if (player->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
					player->SEND_GOSSIP_MENU(5844, creature->GetGUID());
				else canBuy = true;
				break;
			case 4731:                                          //Zachariah Post
				if (player->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD_PLAYER)
					player->SEND_GOSSIP_MENU(5840, creature->GetGUID());
				else canBuy = true;
				break;
			case 7952:                                          //Zjolnir
				if (player->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
					player->SEND_GOSSIP_MENU(5842, creature->GetGUID());
				else canBuy = true;
				break;
			case 7955:                                          //Milli Featherwhistle
				if (player->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
					player->SEND_GOSSIP_MENU(5857, creature->GetGUID());
				else canBuy = true;
				break;
			case 16264:                                         //Winaestra
				if (player->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
					player->SEND_GOSSIP_MENU(10305, creature->GetGUID());
				else canBuy = true;
				break;
			case 17584:                                         //Torallius the Pack Handler
				if (player->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
					player->SEND_GOSSIP_MENU(10239, creature->GetGUID());
				else canBuy = true;
				break;
		}

		if (canBuy)
		{
			if (creature->isVendor())
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		}
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_TRADE)
			player->GetSession()->SendListInventory(creature->GetGUID());

		return true;
	}
};

/*######
## npc_rogue_trainer
######*/

#define GOSSIP_HELLO_ROGUE1 "I wish to unlearn my talents"
#define GOSSIP_HELLO_ROGUE2 "<Take the letter>"
#define GOSSIP_HELLO_ROGUE3 "Purchase a Dual Talent Specialization."

class npc_rogue_trainer : public CreatureScript
{
public:
	npc_rogue_trainer() : CreatureScript("npc_rogue_trainer") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->isQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (creature->isTrainer())
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

		if (creature->isCanTrainingAndResetTalentsOf(player))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE1, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

		if (player->GetSpecsCount() == 1 && creature->isCanTrainingAndResetTalentsOf(player) && player->getLevel() >= sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE3, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_LEARNDUALSPEC);

		if (player->getClass() == CLASS_ROGUE && player->getLevel() >= 24 && !player->HasItemCount(17126, 1) && !player->GetQuestRewardStatus(6681))
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_ROGUE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			player->SEND_GOSSIP_MENU(5996, creature->GetGUID());
		} else
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (action)
		{
			case GOSSIP_ACTION_INFO_DEF + 1:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, 21100, false);
				break;
			case GOSSIP_ACTION_TRAIN:
				player->GetSession()->SendTrainerList(creature->GetGUID());
				break;
			case GOSSIP_OPTION_UNLEARNTALENTS:
				player->CLOSE_GOSSIP_MENU();
				player->SendTalentWipeConfirm(creature->GetGUID());
				break;
			case GOSSIP_OPTION_LEARNDUALSPEC:
				if (player->GetSpecsCount() == 1 && !(player->getLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL)))
				{
					if (!player->HasEnoughMoney(10000000))
					{
						player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
						player->PlayerTalkClass->SendCloseGossip();
						break;
					}
					else
					{
						player->ModifyMoney(-10000000);

						// Cast spells that teach dual spec
						// Both are also ImplicitTarget self and must be cast by player
						player->CastSpell(player, 63680, true, NULL, NULL, player->GetGUID());
						player->CastSpell(player, 63624, true, NULL, NULL, player->GetGUID());

						// Should show another Gossip text with "Congratulations..."
						player->PlayerTalkClass->SendCloseGossip();
					}
				}
				break;
		}
		return true;
	}
};

/*######
## npc_sayge
######*/

#define SPELL_DMG       23768                               //dmg
#define SPELL_RES       23769                               //res
#define SPELL_ARM       23767                               //arm
#define SPELL_SPI       23738                               //spi
#define SPELL_INT       23766                               //int
#define SPELL_STM       23737                               //stm
#define SPELL_STR       23735                               //str
#define SPELL_AGI       23736                               //agi
#define SPELL_FORTUNE   23765                               //faire fortune

#define GOSSIP_HELLO_SAYGE  "Yes"
#define GOSSIP_SENDACTION_SAYGE1    "Slay the Man"
#define GOSSIP_SENDACTION_SAYGE2    "Turn him over to liege"
#define GOSSIP_SENDACTION_SAYGE3    "Confiscate the corn"
#define GOSSIP_SENDACTION_SAYGE4    "Let him go and have the corn"
#define GOSSIP_SENDACTION_SAYGE5    "Execute your friend painfully"
#define GOSSIP_SENDACTION_SAYGE6    "Execute your friend painlessly"
#define GOSSIP_SENDACTION_SAYGE7    "Let your friend go"
#define GOSSIP_SENDACTION_SAYGE8    "Confront the diplomat"
#define GOSSIP_SENDACTION_SAYGE9    "Show not so quiet defiance"
#define GOSSIP_SENDACTION_SAYGE10   "Remain quiet"
#define GOSSIP_SENDACTION_SAYGE11   "Speak against your brother openly"
#define GOSSIP_SENDACTION_SAYGE12   "Help your brother in"
#define GOSSIP_SENDACTION_SAYGE13   "Keep your brother out without letting him know"
#define GOSSIP_SENDACTION_SAYGE14   "Take credit, keep gold"
#define GOSSIP_SENDACTION_SAYGE15   "Take credit, share the gold"
#define GOSSIP_SENDACTION_SAYGE16   "Let the knight take credit"
#define GOSSIP_SENDACTION_SAYGE17   "Thanks"

class npc_sayge : public CreatureScript
{
public:
	npc_sayge() : CreatureScript("npc_sayge") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->isQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->HasSpellCooldown(SPELL_INT) ||
			player->HasSpellCooldown(SPELL_ARM) ||
			player->HasSpellCooldown(SPELL_DMG) ||
			player->HasSpellCooldown(SPELL_RES) ||
			player->HasSpellCooldown(SPELL_STR) ||
			player->HasSpellCooldown(SPELL_AGI) ||
			player->HasSpellCooldown(SPELL_STM) ||
			player->HasSpellCooldown(SPELL_SPI))
			player->SEND_GOSSIP_MENU(7393, creature->GetGUID());
		else
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SAYGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			player->SEND_GOSSIP_MENU(7339, creature->GetGUID());
		}

		return true;
	}

	void SendAction(Player* player, Creature* creature, uint32 action)
	{
		switch (action)
		{
			case GOSSIP_ACTION_INFO_DEF + 1:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE1,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE2,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE3,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE4,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
				player->SEND_GOSSIP_MENU(7340, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 2:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE5,            GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE6,            GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE7,            GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
				player->SEND_GOSSIP_MENU(7341, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 3:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE8,            GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE9,            GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE10,           GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
				player->SEND_GOSSIP_MENU(7361, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 4:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE11,           GOSSIP_SENDER_MAIN + 6, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE12,           GOSSIP_SENDER_MAIN + 7, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE13,           GOSSIP_SENDER_MAIN + 8, GOSSIP_ACTION_INFO_DEF);
				player->SEND_GOSSIP_MENU(7362, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 5:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE14,           GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE15,           GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE16,           GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
				player->SEND_GOSSIP_MENU(7363, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE17,           GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
				player->SEND_GOSSIP_MENU(7364, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 6:
				creature->CastSpell(player, SPELL_FORTUNE, false);
				player->SEND_GOSSIP_MENU(7365, creature->GetGUID());
				break;
		}
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (sender)
		{
			case GOSSIP_SENDER_MAIN:
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 1:
				creature->CastSpell(player, SPELL_DMG, false);
				player->AddSpellCooldown(SPELL_DMG, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 2:
				creature->CastSpell(player, SPELL_RES, false);
				player->AddSpellCooldown(SPELL_RES, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 3:
				creature->CastSpell(player, SPELL_ARM, false);
				player->AddSpellCooldown(SPELL_ARM, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 4:
				creature->CastSpell(player, SPELL_SPI, false);
				player->AddSpellCooldown(SPELL_SPI, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 5:
				creature->CastSpell(player, SPELL_INT, false);
				player->AddSpellCooldown(SPELL_INT, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 6:
				creature->CastSpell(player, SPELL_STM, false);
				player->AddSpellCooldown(SPELL_STM, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 7:
				creature->CastSpell(player, SPELL_STR, false);
				player->AddSpellCooldown(SPELL_STR, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
			case GOSSIP_SENDER_MAIN + 8:
				creature->CastSpell(player, SPELL_AGI, false);
				player->AddSpellCooldown(SPELL_AGI, 0, time(NULL) + 7200);
				SendAction(player, creature, action);
				break;
		}
		return true;
	}
};

class npc_steam_tonk : public CreatureScript
{
public:
	npc_steam_tonk() : CreatureScript("npc_steam_tonk") { }

	struct npc_steam_tonkAI : public ScriptedAI
	{
		npc_steam_tonkAI(Creature* creature) : ScriptedAI(creature) {}

		void Reset() {}
		void EnterCombat(Unit* /*who*/) {}

		void OnPossess(bool apply)
		{
			if (apply)
			{
				// Initialize the action bar without the melee attack command
				me->InitCharmInfo();
				me->GetCharmInfo()->InitEmptyActionBar(false);

				me->SetReactState(REACT_PASSIVE);
			}
			else
				me->SetReactState(REACT_AGGRESSIVE);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_steam_tonkAI(creature);
	}
};

#define SPELL_TONK_MINE_DETONATE 25099

class npc_tonk_mine : public CreatureScript
{
public:
	npc_tonk_mine() : CreatureScript("npc_tonk_mine") { }

	struct npc_tonk_mineAI : public ScriptedAI
	{
		npc_tonk_mineAI(Creature* creature) : ScriptedAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
		}

		uint32 ExplosionTimer;

		void Reset()
		{
			ExplosionTimer = 3000;
		}

		void EnterCombat(Unit* /*who*/) {}
		void AttackStart(Unit* /*who*/) {}
		void MoveInLineOfSight(Unit* /*who*/) {}

		void UpdateAI(uint32 const diff)
		{
			if (ExplosionTimer <= diff)
			{
				DoCast(me, SPELL_TONK_MINE_DETONATE, true);
				me->setDeathState(DEAD); // unsummon it
			}
			else
				ExplosionTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_tonk_mineAI(creature);
	}
};

/*####
## npc_brewfest_reveler
####*/

class npc_brewfest_reveler : public CreatureScript
{
public:
	npc_brewfest_reveler() : CreatureScript("npc_brewfest_reveler") { }

	struct npc_brewfest_revelerAI : public ScriptedAI
	{
		npc_brewfest_revelerAI(Creature* creature) : ScriptedAI(creature) {}
		void ReceiveEmote(Player* player, uint32 emote)
		{
			if (!IsHolidayActive(HOLIDAY_BREWFEST))
				return;

			if (emote == TEXT_EMOTE_DANCE)
				me->CastSpell(player, 41586, false);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_brewfest_revelerAI(creature);
	}
};

/*####
## npc_winter_reveler
####*/

enum WinterReveler
{
	SPELL_MISTLETOE_DEBUFF       = 26218,
	SPELL_CREATE_MISTLETOE       = 26206,
	SPELL_CREATE_HOLLY           = 26207,
	SPELL_CREATE_SNOWFLAKES      = 45036,
};

class npc_winter_reveler : public CreatureScript
{
	public:
		npc_winter_reveler() : CreatureScript("npc_winter_reveler") { }

		struct npc_winter_revelerAI : public ScriptedAI
		{
			npc_winter_revelerAI(Creature* creature) : ScriptedAI(creature) {}

			void ReceiveEmote(Player* player, uint32 emote)
			{
				if (player->HasAura(SPELL_MISTLETOE_DEBUFF))
					return;

				if (!IsHolidayActive(HOLIDAY_FEAST_OF_WINTER_VEIL))
					return;

				if (emote == TEXT_EMOTE_KISS)
				{
					uint32 spellId = RAND<uint32>(SPELL_CREATE_MISTLETOE, SPELL_CREATE_HOLLY, SPELL_CREATE_SNOWFLAKES);
					me->CastSpell(player, spellId, false);
					me->CastSpell(player, SPELL_MISTLETOE_DEBUFF, false);
				}
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_winter_revelerAI(creature);
		}
};

/*####
## npc_snake_trap_serpents
####*/

#define SPELL_MIND_NUMBING_POISON    25810   //Viper
#define SPELL_DEADLY_POISON          34655   //Venomous Snake
#define SPELL_CRIPPLING_POISON       30981   //Viper

#define VENOMOUS_SNAKE_TIMER 1500
#define VIPER_TIMER 3000

#define C_VIPER 19921

class npc_snake_trap : public CreatureScript
{
public:
	npc_snake_trap() : CreatureScript("npc_snake_trap_serpents") { }

	struct npc_snake_trap_serpentsAI : public ScriptedAI
	{
		npc_snake_trap_serpentsAI(Creature* creature) : ScriptedAI(creature) {}

		uint32 SpellTimer;
		bool IsViper;

		void EnterCombat(Unit* /*who*/) {}

		void Reset()
		{
			SpellTimer = 0;

			CreatureTemplate const* Info = me->GetCreatureTemplate();

			IsViper = Info->Entry == C_VIPER ? true : false;

			me->SetMaxHealth(uint32(107 * (me->getLevel() - 40) * 0.025f));
			//Add delta to make them not all hit the same time
			uint32 delta = (rand() % 7) * 100;
			me->SetStatFloatValue(UNIT_FIELD_BASEATTACKTIME, float(Info->baseattacktime + delta));
			me->SetStatFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER, float(Info->attackpower));

			// Start attacking attacker of owner on first ai update after spawn - move in line of sight may choose better target
			if (!me->getVictim() && me->isSummon())
				if (Unit* Owner = me->ToTempSummon()->GetSummoner())
					if (Owner->getAttackerForHelper())
						AttackStart(Owner->getAttackerForHelper());
		}

		//Redefined for random target selection:
		void MoveInLineOfSight(Unit* who)
		{
			if (!me->getVictim() && me->canCreatureAttack(who))
			{
				if (me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
					return;

				float attackRadius = me->GetAttackDistance(who);
				if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
				{
					if (!(rand() % 5))
					{
						me->setAttackTimer(BASE_ATTACK, (rand() % 10) * 100);
						SpellTimer = (rand() % 10) * 100;
						AttackStart(who);
					}
				}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->getVictim()->HasBreakableByDamageCrowdControlAura(me))
			{
				me->InterruptNonMeleeSpells(false);
				return;
			}

			if (SpellTimer <= diff)
			{
				if (IsViper) //Viper
				{
					if (urand(0, 2) == 0) //33% chance to cast
					{
						uint32 spell;
						if (urand(0, 1) == 0)
							spell = SPELL_MIND_NUMBING_POISON;
						else
							spell = SPELL_CRIPPLING_POISON;

						DoCast(me->getVictim(), spell);
					}

					SpellTimer = VIPER_TIMER;
				}
				else //Venomous Snake
				{
					if (urand(0, 2) == 0) //33% chance to cast
						DoCast(me->getVictim(), SPELL_DEADLY_POISON);
					SpellTimer = VENOMOUS_SNAKE_TIMER + (rand() % 5) * 100;
				}
			}
			else
				SpellTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_snake_trap_serpentsAI(creature);
	}
};

#define SAY_RANDOM_MOJO0    "Now that's what I call froggy-style!"
#define SAY_RANDOM_MOJO1    "Your lily pad or mine?"
#define SAY_RANDOM_MOJO2    "This won't take long, did it?"
#define SAY_RANDOM_MOJO3    "I thought you'd never ask!"
#define SAY_RANDOM_MOJO4    "I promise not to give you warts..."
#define SAY_RANDOM_MOJO5    "Feelin' a little froggy, are ya?"
#define SAY_RANDOM_MOJO6a   "Listen, "
#define SAY_RANDOM_MOJO6b   ", I know of a little swamp not too far from here...."
#define SAY_RANDOM_MOJO7    "There's just never enough Mojo to go around..."

class mob_mojo : public CreatureScript
{
public:
	mob_mojo() : CreatureScript("mob_mojo") { }

	struct mob_mojoAI : public ScriptedAI
	{
		mob_mojoAI(Creature* creature) : ScriptedAI(creature) {Reset();}
		uint32 hearts;
		uint64 victimGUID;
		void Reset()
		{
			victimGUID = 0;
			hearts = 15000;
			if (Unit* own = me->GetOwner())
				me->GetMotionMaster()->MoveFollow(own, 0, 0);
		}

		void EnterCombat(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (me->HasAura(20372))
			{
				if (hearts <= diff)
				{
					me->RemoveAurasDueToSpell(20372);
					hearts = 15000;
				} hearts -= diff;
			}
		}

		void ReceiveEmote(Player* player, uint32 emote)
		{
			me->HandleEmoteCommand(emote);
			Unit* own = me->GetOwner();
			if (!own || own->GetTypeId() != TYPEID_PLAYER || CAST_PLR(own)->GetTeam() != player->GetTeam())
				return;
			if (emote == TEXT_EMOTE_KISS)
			{
				std::string whisp = "";
				switch (rand() % 8)
				{
					case 0:
						whisp.append(SAY_RANDOM_MOJO0);
						break;
					case 1:
						whisp.append(SAY_RANDOM_MOJO1);
						break;
					case 2:
						whisp.append(SAY_RANDOM_MOJO2);
						break;
					case 3:
						whisp.append(SAY_RANDOM_MOJO3);
						break;
					case 4:
						whisp.append(SAY_RANDOM_MOJO4);
						break;
					case 5:
						whisp.append(SAY_RANDOM_MOJO5);
						break;
					case 6:
						whisp.append(SAY_RANDOM_MOJO6a);
						whisp.append(player->GetName());
						whisp.append(SAY_RANDOM_MOJO6b);
						break;
					case 7:
						whisp.append(SAY_RANDOM_MOJO7);
						break;
				}

				me->MonsterWhisper(whisp.c_str(), player->GetGUID());
				if (victimGUID)
					if (Player* victim = Unit::GetPlayer(*me, victimGUID))
						victim->RemoveAura(43906);//remove polymorph frog thing
				me->AddAura(43906, player);//add polymorph frog thing
				victimGUID = player->GetGUID();
				DoCast(me, 20372, true);//tag.hearts
				me->GetMotionMaster()->MoveFollow(player, 0, 0);
				hearts = 15000;
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new mob_mojoAI(creature);
	}
};

class npc_mirror_image : public CreatureScript
{
public:
	npc_mirror_image() : CreatureScript("npc_mirror_image") { }

	struct npc_mirror_imageAI : CasterAI
	{
		npc_mirror_imageAI(Creature* creature) : CasterAI(creature) {}

		void InitializeAI()
		{
			CasterAI::InitializeAI();
			Unit* owner = me->GetOwner();
			if (!owner)
				return;
			// Inherit Master's Threat List (not yet implemented)
			owner->CastSpell((Unit*)NULL, 58838, true);
			// here mirror image casts on summoner spell (not present in client dbc) 49866
			// here should be auras (not present in client dbc): 35657, 35658, 35659, 35660 selfcasted by mirror images (stats related?)
			// Clone Me!
			owner->CastSpell(me, 45204, false);
		}

		// Do not reload Creature templates on evade mode enter - prevent visual lost
		void EnterEvadeMode()
		{
			if (me->IsInEvadeMode() || !me->isAlive())
				return;

			Unit* owner = me->GetCharmerOrOwner();

			me->CombatStop(true);
			if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
			{
				me->GetMotionMaster()->Clear(false);
				me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_mirror_imageAI(creature);
	}
};

class npc_ebon_gargoyle : public CreatureScript
{
public:
	npc_ebon_gargoyle() : CreatureScript("npc_ebon_gargoyle") { }

	struct npc_ebon_gargoyleAI : CasterAI
	{
		npc_ebon_gargoyleAI(Creature* creature) : CasterAI(creature) {}

		uint32 despawnTimer;

		void InitializeAI()
		{
			CasterAI::InitializeAI();
			uint64 ownerGuid = me->GetOwnerGUID();
			if (!ownerGuid)
				return;
			// Not needed to be despawned now
			despawnTimer = 0;
			// Find victim of Summon Gargoyle spell
			std::list<Unit*> targets;
			Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 30);
			Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
			me->VisitNearbyObject(30, searcher);
			for (std::list<Unit*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
				if ((*iter)->GetAura(49206, ownerGuid))
				{
					me->Attack((*iter), false);
					break;
				}
		}

		void JustDied(Unit* /*killer*/)
		{
			// Stop Feeding Gargoyle when it dies
			if (Unit* owner = me->GetOwner())
				owner->RemoveAurasDueToSpell(50514);
		}

		// Fly away when dismissed
		void SpellHit(Unit* source, SpellInfo const* spell)
		{
			if (spell->Id != 50515 || !me->isAlive())
				return;

			Unit* owner = me->GetOwner();

			if (!owner || owner != source)
				return;

			// Stop Fighting
			me->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE, true);
			// Sanctuary
			me->CastSpell(me, 54661, true);
			me->SetReactState(REACT_PASSIVE);

			//! HACK: Creature's can't have MOVEMENTFLAG_FLYING
			// Fly Away
			me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY|MOVEMENTFLAG_ASCENDING|MOVEMENTFLAG_FLYING);
			me->SetSpeed(MOVE_FLIGHT, 0.75f, true);
			me->SetSpeed(MOVE_RUN, 0.75f, true);
			float x = me->GetPositionX() + 20 * cos(me->GetOrientation());
			float y = me->GetPositionY() + 20 * sin(me->GetOrientation());
			float z = me->GetPositionZ() + 40;
			me->GetMotionMaster()->Clear(false);
			me->GetMotionMaster()->MovePoint(0, x, y, z);

			// Despawn as soon as possible
			despawnTimer = 4 * IN_MILLISECONDS;
		}

		void UpdateAI(const uint32 diff)
		{
			if (despawnTimer > 0)
			{
				if (despawnTimer > diff)
					despawnTimer -= diff;
				else
					me->DespawnOrUnsummon();
				return;
			}
			CasterAI::UpdateAI(diff);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ebon_gargoyleAI(creature);
	}
};

class npc_lightwell : public CreatureScript
{
	public:
		npc_lightwell() : CreatureScript("npc_lightwell") { }

		struct npc_lightwellAI : public PassiveAI
		{
			npc_lightwellAI(Creature* creature) : PassiveAI(creature)
			{
				DoCast(me, 59907, false);
			}

			void EnterEvadeMode()
			{
				if (!me->isAlive())
					return;

				me->DeleteThreatList();
				me->CombatStop(true);
				me->ResetPlayerDamageReq();
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_lightwellAI(creature);
		}
};

enum eTrainingDummy
{
	NPC_ADVANCED_TARGET_DUMMY                  = 2674,
	NPC_TARGET_DUMMY                           = 2673
};

class npc_training_dummy : public CreatureScript
{
public:
	npc_training_dummy() : CreatureScript("npc_training_dummy") { }

	struct npc_training_dummyAI : Scripted_NoMovementAI
	{
		npc_training_dummyAI(Creature* creature) : Scripted_NoMovementAI(creature)
		{
			entry = creature->GetEntry();
		}

		uint32 entry;
		uint32 resetTimer;
		uint32 despawnTimer;

		void Reset()
		{
			me->SetControlled(true, UNIT_STATE_STUNNED);//disable rotate
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);//imune to knock aways like blast wave

			resetTimer = 5000;
			despawnTimer = 15000;
		}

		void EnterEvadeMode()
		{
			if (!_EnterEvadeMode())
				return;

			Reset();
		}

		void DamageTaken(Unit* /*doneBy*/, uint32& damage)
		{
			resetTimer = 5000;
			damage = 0;
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (entry != NPC_ADVANCED_TARGET_DUMMY && entry != NPC_TARGET_DUMMY)
				return;
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (!me->HasUnitState(UNIT_STATE_STUNNED))
				me->SetControlled(true, UNIT_STATE_STUNNED);//disable rotate

			if (entry != NPC_ADVANCED_TARGET_DUMMY && entry != NPC_TARGET_DUMMY)
			{
				if (resetTimer <= diff)
				{
					EnterEvadeMode();
					resetTimer = 5000;
				}
				else
					resetTimer -= diff;
				return;
			}
			else
			{
				if (despawnTimer <= diff)
					me->DespawnOrUnsummon();
				else
					despawnTimer -= diff;
			}
		}
		void MoveInLineOfSight(Unit* /*who*/){return;}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_training_dummyAI(creature);
	}
};

/*######
# npc_shadowfiend
######*/
#define GLYPH_OF_SHADOWFIEND_MANA         58227
#define GLYPH_OF_SHADOWFIEND              58228

class npc_shadowfiend : public CreatureScript
{
	public:
		npc_shadowfiend() : CreatureScript("npc_shadowfiend") { }

		struct npc_shadowfiendAI : public ScriptedAI
		{
			npc_shadowfiendAI(Creature* creature) : ScriptedAI(creature) {}

			void DamageTaken(Unit* /*killer*/, uint32& damage)
			{
				if (me->isSummon())
					if (Unit* owner = me->ToTempSummon()->GetSummoner())
						if (owner->HasAura(GLYPH_OF_SHADOWFIEND) && damage >= me->GetHealth())
							owner->CastSpell(owner, GLYPH_OF_SHADOWFIEND_MANA, true);
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_shadowfiendAI(creature);
		}
};

/*######
# npc_fire_elemental
######*/
#define SPELL_FIRENOVA        12470
#define SPELL_FIRESHIELD      13376
#define SPELL_FIREBLAST       57984

class npc_fire_elemental : public CreatureScript
{
public:
	npc_fire_elemental() : CreatureScript("npc_fire_elemental") { }

	struct npc_fire_elementalAI : public ScriptedAI
	{
		npc_fire_elementalAI(Creature* creature) : ScriptedAI(creature) {}

		uint32 FireNova_Timer;
		uint32 FireShield_Timer;
		uint32 FireBlast_Timer;

		void Reset()
		{
			FireNova_Timer = 5000 + rand() % 15000; // 5-20 sec cd
			FireBlast_Timer = 5000 + rand() % 15000; // 5-20 sec cd
			FireShield_Timer = 0;
			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireShield_Timer <= diff)
			{
				DoCast(me->getVictim(), SPELL_FIRESHIELD);
				FireShield_Timer = 2 * IN_MILLISECONDS;
			}
			else
				FireShield_Timer -= diff;

			if (FireBlast_Timer <= diff)
			{
				DoCast(me->getVictim(), SPELL_FIREBLAST);
				FireBlast_Timer = 5000 + rand() % 15000; // 5-20 sec cd
			}
			else
				FireBlast_Timer -= diff;

			if (FireNova_Timer <= diff)
			{
				DoCast(me->getVictim(), SPELL_FIRENOVA);
				FireNova_Timer = 5000 + rand() % 15000; // 5-20 sec cd
			}
			else
				FireNova_Timer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_fire_elementalAI(creature);
	}
};

/*######
# npc_earth_elemental
######*/
#define SPELL_ANGEREDEARTH        36213

class npc_earth_elemental : public CreatureScript
{
public:
	npc_earth_elemental() : CreatureScript("npc_earth_elemental") { }

	struct npc_earth_elementalAI : public ScriptedAI
	{
		npc_earth_elementalAI(Creature* creature) : ScriptedAI(creature) {}

		uint32 AngeredEarth_Timer;

		void Reset()
		{
			AngeredEarth_Timer = 0;
			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (AngeredEarth_Timer <= diff)
			{
				DoCast(me->getVictim(), SPELL_ANGEREDEARTH);
				AngeredEarth_Timer = 5000 + rand() % 15000; // 5-20 sec cd
			}
			else
				AngeredEarth_Timer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_earth_elementalAI(creature);
	}
};

/*######
# npc_wormhole
######*/

#define GOSSIP_ENGINEERING1   "Borean Tundra"
#define GOSSIP_ENGINEERING2   "Howling Fjord"
#define GOSSIP_ENGINEERING3   "Sholazar Basin"
#define GOSSIP_ENGINEERING4   "Icecrown"
#define GOSSIP_ENGINEERING5   "Storm Peaks"
#define GOSSIP_ENGINEERING6   "Underground..."

enum WormholeSpells
{
	SPELL_BOREAN_TUNDRA         = 67834,
	SPELL_SHOLAZAR_BASIN        = 67835,
	SPELL_ICECROWN              = 67836,
	SPELL_STORM_PEAKS           = 67837,
	SPELL_HOWLING_FJORD         = 67838,
	SPELL_UNDERGROUND           = 68081,

	TEXT_WORMHOLE               = 907,

	DATA_SHOW_UNDERGROUND       = 1,
};

class npc_wormhole : public CreatureScript
{
	public:
		npc_wormhole() : CreatureScript("npc_wormhole") {}

		struct npc_wormholeAI : public PassiveAI
		{
			npc_wormholeAI(Creature* creature) : PassiveAI(creature) {}

			void InitializeAI()
			{
				_showUnderground = urand(0, 100) == 0; // Guessed value, it is really rare though
			}

			uint32 GetData(uint32 type)
			{
				return (type == DATA_SHOW_UNDERGROUND && _showUnderground) ? 1 : 0;
			}

		private:
			bool _showUnderground;
		};

		bool OnGossipHello(Player* player, Creature* creature)
		{
			if (creature->isSummon())
			{
				if (player == creature->ToTempSummon()->GetSummoner())
				{
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

					if (creature->AI()->GetData(DATA_SHOW_UNDERGROUND))
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

					player->PlayerTalkClass->SendGossipMenu(TEXT_WORMHOLE, creature->GetGUID());
				}
			}

			return true;
		}

		bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
		{
			player->PlayerTalkClass->ClearMenus();

			switch (action)
			{
				case GOSSIP_ACTION_INFO_DEF + 1: // Borean Tundra
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_BOREAN_TUNDRA, false);
					break;
				case GOSSIP_ACTION_INFO_DEF + 2: // Howling Fjord
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_HOWLING_FJORD, false);
					break;
				case GOSSIP_ACTION_INFO_DEF + 3: // Sholazar Basin
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_SHOLAZAR_BASIN, false);
					break;
				case GOSSIP_ACTION_INFO_DEF + 4: // Icecrown
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_ICECROWN, false);
					break;
				case GOSSIP_ACTION_INFO_DEF + 5: // Storm peaks
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_STORM_PEAKS, false);
					break;
				case GOSSIP_ACTION_INFO_DEF + 6: // Underground
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_UNDERGROUND, false);
					break;
			}

			return true;
		}

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_wormholeAI(creature);
		}
};

/*######
## npc_pet_trainer
######*/

enum ePetTrainer
{
	TEXT_ISHUNTER               = 5838,
	TEXT_NOTHUNTER              = 5839,
	TEXT_PETINFO                = 13474,
	TEXT_CONFIRM                = 7722
};

#define GOSSIP_PET1             "How do I train my pet?"
#define GOSSIP_PET2             "I wish to untrain my pet."
#define GOSSIP_PET_CONFIRM      "Yes, please do."

class npc_pet_trainer : public CreatureScript
{
public:
	npc_pet_trainer() : CreatureScript("npc_pet_trainer") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->isQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->getClass() == CLASS_HUNTER)
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			if (player->GetPet() && player->GetPet()->getPetType() == HUNTER_PET)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

			player->PlayerTalkClass->SendGossipMenu(TEXT_ISHUNTER, creature->GetGUID());
			return true;
		}
		player->PlayerTalkClass->SendGossipMenu(TEXT_NOTHUNTER, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (action)
		{
			case GOSSIP_ACTION_INFO_DEF + 1:
				player->PlayerTalkClass->SendGossipMenu(TEXT_PETINFO, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 2:
				{
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET_CONFIRM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
					player->PlayerTalkClass->SendGossipMenu(TEXT_CONFIRM, creature->GetGUID());
				}
				break;
			case GOSSIP_ACTION_INFO_DEF + 3:
				{
					player->ResetPetTalents();
					player->CLOSE_GOSSIP_MENU();
				}
				break;
		}
		return true;
	}
};

/*######
## npc_locksmith
######*/

enum eLockSmith
{
	QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ = 10704,
	QUEST_DARK_IRON_LEGACY                = 3802,
	QUEST_THE_KEY_TO_SCHOLOMANCE_A        = 5505,
	QUEST_THE_KEY_TO_SCHOLOMANCE_H        = 5511,
	QUEST_HOTTER_THAN_HELL_A              = 10758,
	QUEST_HOTTER_THAN_HELL_H              = 10764,
	QUEST_RETURN_TO_KHAGDAR               = 9837,
	QUEST_CONTAINMENT                     = 13159,
	QUEST_ETERNAL_VIGILANCE               = 11011,
	QUEST_KEY_TO_THE_FOCUSING_IRIS        = 13372,
	QUEST_HC_KEY_TO_THE_FOCUSING_IRIS     = 13375,

	ITEM_ARCATRAZ_KEY                     = 31084,
	ITEM_SHADOWFORGE_KEY                  = 11000,
	ITEM_SKELETON_KEY                     = 13704,
	ITEM_SHATTERED_HALLS_KEY              = 28395,
	ITEM_THE_MASTERS_KEY                  = 24490,
	ITEM_VIOLET_HOLD_KEY                  = 42482,
	ITEM_ESSENCE_INFUSED_MOONSTONE        = 32449,
	ITEM_KEY_TO_THE_FOCUSING_IRIS         = 44582,
	ITEM_HC_KEY_TO_THE_FOCUSING_IRIS      = 44581,

	SPELL_ARCATRAZ_KEY                    = 54881,
	SPELL_SHADOWFORGE_KEY                 = 54882,
	SPELL_SKELETON_KEY                    = 54883,
	SPELL_SHATTERED_HALLS_KEY             = 54884,
	SPELL_THE_MASTERS_KEY                 = 54885,
	SPELL_VIOLET_HOLD_KEY                 = 67253,
	SPELL_ESSENCE_INFUSED_MOONSTONE       = 40173,
};

#define GOSSIP_LOST_ARCATRAZ_KEY                "I've lost my key to the Arcatraz."
#define GOSSIP_LOST_SHADOWFORGE_KEY             "I've lost my key to the Blackrock Depths."
#define GOSSIP_LOST_SKELETON_KEY                "I've lost my key to the Scholomance."
#define GOSSIP_LOST_SHATTERED_HALLS_KEY         "I've lost my key to the Shattered Halls."
#define GOSSIP_LOST_THE_MASTERS_KEY             "I've lost my key to the Karazhan."
#define GOSSIP_LOST_VIOLET_HOLD_KEY             "I've lost my key to the Violet Hold."
#define GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE   "I've lost my Essence-Infused Moonstone."
#define GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS    "I've lost my Key to the Focusing Iris."
#define GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS "I've lost my Heroic Key to the Focusing Iris."

class npc_locksmith : public CreatureScript
{
public:
	npc_locksmith() : CreatureScript("npc_locksmith") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		// Arcatraz Key
		if (player->GetQuestRewardStatus(QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ) && !player->HasItemCount(ITEM_ARCATRAZ_KEY, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ARCATRAZ_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		// Shadowforge Key
		if (player->GetQuestRewardStatus(QUEST_DARK_IRON_LEGACY) && !player->HasItemCount(ITEM_SHADOWFORGE_KEY, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHADOWFORGE_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

		// Skeleton Key
		if ((player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_A) || player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_H)) &&
			!player->HasItemCount(ITEM_SKELETON_KEY, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SKELETON_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

		// Shatered Halls Key
		if ((player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_A) || player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_H)) &&
			!player->HasItemCount(ITEM_SHATTERED_HALLS_KEY, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHATTERED_HALLS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

		// Master's Key
		if (player->GetQuestRewardStatus(QUEST_RETURN_TO_KHAGDAR) && !player->HasItemCount(ITEM_THE_MASTERS_KEY, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_THE_MASTERS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

		// Violet Hold Key
		if (player->GetQuestRewardStatus(QUEST_CONTAINMENT) && !player->HasItemCount(ITEM_VIOLET_HOLD_KEY, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_VIOLET_HOLD_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

		// Essence-Infused Moonstone
		if (player->GetQuestRewardStatus(QUEST_ETERNAL_VIGILANCE) && !player->HasItemCount(ITEM_ESSENCE_INFUSED_MOONSTONE, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

		// Key to the Focusing Iris
		if (player->GetQuestRewardStatus(QUEST_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_KEY_TO_THE_FOCUSING_IRIS, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

		// Heroic Key to the Focusing Iris
		if (player->GetQuestRewardStatus(QUEST_HC_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS, 1, true))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (action)
		{
			case GOSSIP_ACTION_INFO_DEF + 1:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_ARCATRAZ_KEY, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 2:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_SHADOWFORGE_KEY, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 3:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_SKELETON_KEY, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 4:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_SHATTERED_HALLS_KEY, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 5:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_THE_MASTERS_KEY, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 6:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_VIOLET_HOLD_KEY, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 7:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_ESSENCE_INFUSED_MOONSTONE, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 8:
				player->CLOSE_GOSSIP_MENU();
				player->AddItem(ITEM_KEY_TO_THE_FOCUSING_IRIS,1);
				break;
			case GOSSIP_ACTION_INFO_DEF + 9:
				player->CLOSE_GOSSIP_MENU();
				player->AddItem(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS,1);
				break;
		}
		return true;
	}
};

/*######
## npc_experience
######*/

#define EXP_COST                100000 //10 00 00 copper (10golds)
#define GOSSIP_TEXT_EXP         14736
#define GOSSIP_XP_OFF           "I no longer wish to gain experience."
#define GOSSIP_XP_ON            "I wish to start gaining experience again."

class npc_experience : public CreatureScript
{
public:
	npc_experience() : CreatureScript("npc_experience") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_OFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
		player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_EXP, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		bool noXPGain = player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
		bool doSwitch = false;

		switch (action)
		{
			case GOSSIP_ACTION_INFO_DEF + 1://xp off
				{
					if (!noXPGain)//does gain xp
						doSwitch = true;//switch to don't gain xp
				}
				break;
			case GOSSIP_ACTION_INFO_DEF + 2://xp on
				{
					if (noXPGain)//doesn't gain xp
						doSwitch = true;//switch to gain xp
				}
				break;
		}
		if (doSwitch)
		{
			if (!player->HasEnoughMoney(EXP_COST))
				player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
			else if (noXPGain)
			{
				player->ModifyMoney(-EXP_COST);
				player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
			}
			else if (!noXPGain)
			{
				player->ModifyMoney(-EXP_COST);
				player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
			}
		}
		player->PlayerTalkClass->SendCloseGossip();
		return true;
	}
};

enum Fireworks
{
	NPC_OMEN                = 15467,
	NPC_MINION_OF_OMEN      = 15466,
	NPC_FIREWORK_BLUE       = 15879,
	NPC_FIREWORK_GREEN      = 15880,
	NPC_FIREWORK_PURPLE     = 15881,
	NPC_FIREWORK_RED        = 15882,
	NPC_FIREWORK_YELLOW     = 15883,
	NPC_FIREWORK_WHITE      = 15884,
	NPC_FIREWORK_BIG_BLUE   = 15885,
	NPC_FIREWORK_BIG_GREEN  = 15886,
	NPC_FIREWORK_BIG_PURPLE = 15887,
	NPC_FIREWORK_BIG_RED    = 15888,
	NPC_FIREWORK_BIG_YELLOW = 15889,
	NPC_FIREWORK_BIG_WHITE  = 15890,

	NPC_CLUSTER_BLUE        = 15872,
	NPC_CLUSTER_RED         = 15873,
	NPC_CLUSTER_GREEN       = 15874,
	NPC_CLUSTER_PURPLE      = 15875,
	NPC_CLUSTER_WHITE       = 15876,
	NPC_CLUSTER_YELLOW      = 15877,
	NPC_CLUSTER_BIG_BLUE    = 15911,
	NPC_CLUSTER_BIG_GREEN   = 15912,
	NPC_CLUSTER_BIG_PURPLE  = 15913,
	NPC_CLUSTER_BIG_RED     = 15914,
	NPC_CLUSTER_BIG_WHITE   = 15915,
	NPC_CLUSTER_BIG_YELLOW  = 15916,
	NPC_CLUSTER_ELUNE       = 15918,

	GO_FIREWORK_LAUNCHER_1  = 180771,
	GO_FIREWORK_LAUNCHER_2  = 180868,
	GO_FIREWORK_LAUNCHER_3  = 180850,
	GO_CLUSTER_LAUNCHER_1   = 180772,
	GO_CLUSTER_LAUNCHER_2   = 180859,
	GO_CLUSTER_LAUNCHER_3   = 180869,
	GO_CLUSTER_LAUNCHER_4   = 180874,

	SPELL_ROCKET_BLUE       = 26344,
	SPELL_ROCKET_GREEN      = 26345,
	SPELL_ROCKET_PURPLE     = 26346,
	SPELL_ROCKET_RED        = 26347,
	SPELL_ROCKET_WHITE      = 26348,
	SPELL_ROCKET_YELLOW     = 26349,
	SPELL_ROCKET_BIG_BLUE   = 26351,
	SPELL_ROCKET_BIG_GREEN  = 26352,
	SPELL_ROCKET_BIG_PURPLE = 26353,
	SPELL_ROCKET_BIG_RED    = 26354,
	SPELL_ROCKET_BIG_WHITE  = 26355,
	SPELL_ROCKET_BIG_YELLOW = 26356,
	SPELL_LUNAR_FORTUNE     = 26522,

	ANIM_GO_LAUNCH_FIREWORK = 3,
	ZONE_MOONGLADE          = 493,
};

Position omenSummonPos = {7558.993f, -2839.999f, 450.0214f, 4.46f};

class npc_firework : public CreatureScript
{
public:
	npc_firework() : CreatureScript("npc_firework") { }

	struct npc_fireworkAI : public ScriptedAI
	{
		npc_fireworkAI(Creature* creature) : ScriptedAI(creature) {}

		bool isCluster()
		{
			switch (me->GetEntry())
			{
				case NPC_FIREWORK_BLUE:
				case NPC_FIREWORK_GREEN:
				case NPC_FIREWORK_PURPLE:
				case NPC_FIREWORK_RED:
				case NPC_FIREWORK_YELLOW:
				case NPC_FIREWORK_WHITE:
				case NPC_FIREWORK_BIG_BLUE:
				case NPC_FIREWORK_BIG_GREEN:
				case NPC_FIREWORK_BIG_PURPLE:
				case NPC_FIREWORK_BIG_RED:
				case NPC_FIREWORK_BIG_YELLOW:
				case NPC_FIREWORK_BIG_WHITE:
					return false;
				case NPC_CLUSTER_BLUE:
				case NPC_CLUSTER_GREEN:
				case NPC_CLUSTER_PURPLE:
				case NPC_CLUSTER_RED:
				case NPC_CLUSTER_YELLOW:
				case NPC_CLUSTER_WHITE:
				case NPC_CLUSTER_BIG_BLUE:
				case NPC_CLUSTER_BIG_GREEN:
				case NPC_CLUSTER_BIG_PURPLE:
				case NPC_CLUSTER_BIG_RED:
				case NPC_CLUSTER_BIG_YELLOW:
				case NPC_CLUSTER_BIG_WHITE:
				case NPC_CLUSTER_ELUNE:
				default:
					return true;
			}
		}

		GameObject* FindNearestLauncher()
		{
			GameObject* launcher = NULL;

			if (isCluster())
			{
				GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_1, 0.5f);
				GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_2, 0.5f);
				GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_3, 0.5f);
				GameObject* launcher4 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_4, 0.5f);

				if (launcher1)
					launcher = launcher1;
				else if (launcher2)
					launcher = launcher2;
				else if (launcher3)
					launcher = launcher3;
				else if (launcher4)
					launcher = launcher4;
			}
			else
			{
				GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_1, 0.5f);
				GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_2, 0.5f);
				GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_3, 0.5f);

				if (launcher1)
					launcher = launcher1;
				else if (launcher2)
					launcher = launcher2;
				else if (launcher3)
					launcher = launcher3;
			}

			return launcher;
		}

		uint32 GetFireworkSpell(uint32 entry)
		{
			switch (entry)
			{
				case NPC_FIREWORK_BLUE:
					return SPELL_ROCKET_BLUE;
				case NPC_FIREWORK_GREEN:
					return SPELL_ROCKET_GREEN;
				case NPC_FIREWORK_PURPLE:
					return SPELL_ROCKET_PURPLE;
				case NPC_FIREWORK_RED:
					return SPELL_ROCKET_RED;
				case NPC_FIREWORK_YELLOW:
					return SPELL_ROCKET_YELLOW;
				case NPC_FIREWORK_WHITE:
					return SPELL_ROCKET_WHITE;
				case NPC_FIREWORK_BIG_BLUE:
					return SPELL_ROCKET_BIG_BLUE;
				case NPC_FIREWORK_BIG_GREEN:
					return SPELL_ROCKET_BIG_GREEN;
				case NPC_FIREWORK_BIG_PURPLE:
					return SPELL_ROCKET_BIG_PURPLE;
				case NPC_FIREWORK_BIG_RED:
					return SPELL_ROCKET_BIG_RED;
				case NPC_FIREWORK_BIG_YELLOW:
					return SPELL_ROCKET_BIG_YELLOW;
				case NPC_FIREWORK_BIG_WHITE:
					return SPELL_ROCKET_BIG_WHITE;
				default:
					return 0;
			}
		}

		uint32 GetFireworkGameObjectId()
		{
			uint32 spellId = 0;

			switch (me->GetEntry())
			{
				case NPC_CLUSTER_BLUE:
					spellId = GetFireworkSpell(NPC_FIREWORK_BLUE);
					break;
				case NPC_CLUSTER_GREEN:
					spellId = GetFireworkSpell(NPC_FIREWORK_GREEN);
					break;
				case NPC_CLUSTER_PURPLE:
					spellId = GetFireworkSpell(NPC_FIREWORK_PURPLE);
					break;
				case NPC_CLUSTER_RED:
					spellId = GetFireworkSpell(NPC_FIREWORK_RED);
					break;
				case NPC_CLUSTER_YELLOW:
					spellId = GetFireworkSpell(NPC_FIREWORK_YELLOW);
					break;
				case NPC_CLUSTER_WHITE:
					spellId = GetFireworkSpell(NPC_FIREWORK_WHITE);
					break;
				case NPC_CLUSTER_BIG_BLUE:
					spellId = GetFireworkSpell(NPC_FIREWORK_BIG_BLUE);
					break;
				case NPC_CLUSTER_BIG_GREEN:
					spellId = GetFireworkSpell(NPC_FIREWORK_BIG_GREEN);
					break;
				case NPC_CLUSTER_BIG_PURPLE:
					spellId = GetFireworkSpell(NPC_FIREWORK_BIG_PURPLE);
					break;
				case NPC_CLUSTER_BIG_RED:
					spellId = GetFireworkSpell(NPC_FIREWORK_BIG_RED);
					break;
				case NPC_CLUSTER_BIG_YELLOW:
					spellId = GetFireworkSpell(NPC_FIREWORK_BIG_YELLOW);
					break;
				case NPC_CLUSTER_BIG_WHITE:
					spellId = GetFireworkSpell(NPC_FIREWORK_BIG_WHITE);
					break;
				case NPC_CLUSTER_ELUNE:
					spellId = GetFireworkSpell(urand(NPC_FIREWORK_BLUE, NPC_FIREWORK_WHITE));
					break;
			}

			const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);

			if (spellInfo && spellInfo->Effects[0].Effect == SPELL_EFFECT_SUMMON_OBJECT_WILD)
				return spellInfo->Effects[0].MiscValue;

			return 0;
		}

		void Reset()
		{
			if (GameObject* launcher = FindNearestLauncher())
			{
				launcher->SendCustomAnim(ANIM_GO_LAUNCH_FIREWORK);
				me->SetOrientation(launcher->GetOrientation() + M_PI/2);
			}
			else
				return;

			if (isCluster())
			{
				// Check if we are near Elune'ara lake south, if so try to summon Omen or a minion
				if (me->GetZoneId() == ZONE_MOONGLADE)
				{
					if (!me->FindNearestCreature(NPC_OMEN, 100.0f, false) && me->GetDistance2d(omenSummonPos.GetPositionX(), omenSummonPos.GetPositionY()) <= 100.0f)
					{
						switch (urand(0,9))
						{
							case 0:
							case 1:
							case 2:
							case 3:
								if (Creature* minion = me->SummonCreature(NPC_MINION_OF_OMEN, me->GetPositionX()+frand(-5.0f, 5.0f), me->GetPositionY()+frand(-5.0f, 5.0f), me->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
									minion->AI()->AttackStart(me->SelectNearestPlayer(20.0f));
								break;
							case 9:
								me->SummonCreature(NPC_OMEN, omenSummonPos);
								break;
						}
					}
				}
				if (me->GetEntry() == NPC_CLUSTER_ELUNE)
					DoCast(SPELL_LUNAR_FORTUNE);

				float displacement = 0.7f;
				for (uint8 i = 0; i < 4; i++)
					me->SummonGameObject(GetFireworkGameObjectId(), me->GetPositionX() + (i%2 == 0 ? displacement : -displacement), me->GetPositionY() + (i > 1 ? displacement : -displacement), me->GetPositionZ() + 4.0f, me->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 1);
			}
			else
				//me->CastSpell(me, GetFireworkSpell(me->GetEntry()), true);
				me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), GetFireworkSpell(me->GetEntry()), true);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_fireworkAI(creature);
	}
};

/*#####
# npc_spring_rabbit
#####*/

enum rabbitSpells
{
	SPELL_SPRING_FLING          = 61875,
	SPELL_SPRING_RABBIT_JUMP    = 61724,
	SPELL_SPRING_RABBIT_WANDER  = 61726,
	SPELL_SUMMON_BABY_BUNNY     = 61727,
	SPELL_SPRING_RABBIT_IN_LOVE = 61728,
	NPC_SPRING_RABBIT           = 32791
};

class npc_spring_rabbit : public CreatureScript
{
public:
	npc_spring_rabbit() : CreatureScript("npc_spring_rabbit") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_spring_rabbitAI(creature);
	}

	struct npc_spring_rabbitAI : public ScriptedAI
	{
		npc_spring_rabbitAI(Creature* creature) : ScriptedAI(creature) { }

		bool inLove;
		uint32 jumpTimer;
		uint32 bunnyTimer;
		uint32 searchTimer;
		uint64 rabbitGUID;

		void Reset()
		{
			inLove = false;
			rabbitGUID = 0;
			jumpTimer = urand(5000, 10000);
			bunnyTimer = urand(10000, 20000);
			searchTimer = urand(5000, 10000);
			if (Unit* owner = me->GetOwner())
				me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
		}

		void EnterCombat(Unit * /*who*/) { }

		void DoAction(const int32 /*param*/)
		{
			inLove = true;
			if (Unit* owner = me->GetOwner())
				owner->CastSpell(owner, SPELL_SPRING_FLING, true);
		}

		void UpdateAI(const uint32 diff)
		{
			if (inLove)
			{
				if (jumpTimer <= diff)
				{
					if (Unit* rabbit = Unit::GetUnit(*me, rabbitGUID))
						DoCast(rabbit, SPELL_SPRING_RABBIT_JUMP);
					jumpTimer = urand(5000, 10000);
				} else jumpTimer -= diff;

				if (bunnyTimer <= diff)
				{
					DoCast(SPELL_SUMMON_BABY_BUNNY);
					bunnyTimer = urand(20000, 40000);
				} else bunnyTimer -= diff;
			}
			else
			{
				if (searchTimer <= diff)
				{
					if (Creature* rabbit = me->FindNearestCreature(NPC_SPRING_RABBIT, 10.0f))
					{
						if (rabbit == me || rabbit->HasAura(SPELL_SPRING_RABBIT_IN_LOVE))
							return;

						me->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, me);
						DoAction(1);
						rabbit->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, rabbit);
						rabbit->AI()->DoAction(1);
						rabbit->CastSpell(rabbit, SPELL_SPRING_RABBIT_JUMP, true);
						rabbitGUID = rabbit->GetGUID();
					}
					searchTimer = urand(5000, 10000);
				} else searchTimer -= diff;
			}
		}
	};
};


/*######
## npc_kingdom_of_dalaran_quests
######*/

enum eKingdomDalaran
{
	SPELL_TELEPORT_DALARAN  = 53360,
	ITEM_KT_SIGNET          = 39740,
	QUEST_MAGICAL_KINGDOM_A = 12794,
	QUEST_MAGICAL_KINGDOM_H = 12791,
	QUEST_MAGICAL_KINGDOM_N = 12796
};

#define GOSSIP_ITEM_TELEPORT_TO "I am ready to be teleported to Dalaran."

class npc_kingdom_of_dalaran_quests : public CreatureScript
{
public:
	npc_kingdom_of_dalaran_quests() : CreatureScript("npc_kingdom_of_dalaran_quests") { }
	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->isQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->HasItemCount(ITEM_KT_SIGNET, 1) && (!player->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_A) ||
			!player->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_H) || !player->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_N)))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT_TO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			player->CLOSE_GOSSIP_MENU();
			player->CastSpell(player, SPELL_TELEPORT_DALARAN, false);
		}
		return true;
	}
};

/*######
## npc_tabard_vendor
######*/

enum
{
	QUEST_TRUE_MASTERS_OF_LIGHT = 9737,
	QUEST_THE_UNWRITTEN_PROPHECY = 9762,
	QUEST_INTO_THE_BREACH = 10259,
	QUEST_BATTLE_OF_THE_CRIMSON_WATCH = 10781,
	QUEST_SHARDS_OF_AHUNE = 11972,

	ACHIEVEMENT_EXPLORE_NORTHREND = 45,
	ACHIEVEMENT_TWENTYFIVE_TABARDS = 1021,
	ACHIEVEMENT_THE_LOREMASTER_A = 1681,
	ACHIEVEMENT_THE_LOREMASTER_H = 1682,

	ITEM_TABARD_OF_THE_HAND = 24344,
	ITEM_TABARD_OF_THE_BLOOD_KNIGHT = 25549,
	ITEM_TABARD_OF_THE_PROTECTOR = 28788,
	ITEM_OFFERING_OF_THE_SHATAR = 31408,
	ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI = 31404,
	ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI = 31405,
	ITEM_TABARD_OF_THE_SUMMER_SKIES = 35279,
	ITEM_TABARD_OF_THE_SUMMER_FLAMES = 35280,
	ITEM_TABARD_OF_THE_ACHIEVER = 40643,
	ITEM_LOREMASTERS_COLORS = 43300,
	ITEM_TABARD_OF_THE_EXPLORER = 43348,

	SPELL_TABARD_OF_THE_BLOOD_KNIGHT = 54974,
	SPELL_TABARD_OF_THE_HAND = 54976,
	SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI = 54977,
	SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI = 54982,
	SPELL_TABARD_OF_THE_ACHIEVER = 55006,
	SPELL_TABARD_OF_THE_PROTECTOR = 55008,
	SPELL_LOREMASTERS_COLORS = 58194,
	SPELL_TABARD_OF_THE_EXPLORER = 58224,
	SPELL_TABARD_OF_SUMMER_SKIES = 62768,
	SPELL_TABARD_OF_SUMMER_FLAMES = 62769
};

#define GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT "I've lost my Tabard of Blood Knight."
#define GOSSIP_LOST_TABARD_OF_THE_HAND "I've lost my Tabard of the Hand."
#define GOSSIP_LOST_TABARD_OF_THE_PROTECTOR "I've lost my Tabard of the Protector."
#define GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI "I've lost my Green Trophy Tabard of the Illidari."
#define GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI "I've lost my Purple Trophy Tabard of the Illidari."
#define GOSSIP_LOST_TABARD_OF_SUMMER_SKIES "I've lost my Tabard of Summer Skies."
#define GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES "I've lost my Tabard of Summer Flames."
#define GOSSIP_LOST_LOREMASTERS_COLORS "I've lost my Loremaster's Colors."
#define GOSSIP_LOST_TABARD_OF_THE_EXPLORER "I've lost my Tabard of the Explorer."
#define GOSSIP_LOST_TABARD_OF_THE_ACHIEVER "I've lost my Tabard of the Achiever."

class npc_tabard_vendor : public CreatureScript
{
public:
	npc_tabard_vendor() : CreatureScript("npc_tabard_vendor") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		bool lostBloodKnight = false;
		bool lostHand = false;
		bool lostProtector = false;
		bool lostIllidari = false;
		bool lostSummer = false;

		//Tabard of the Blood Knight
		if (player->GetQuestRewardStatus(QUEST_TRUE_MASTERS_OF_LIGHT) && !player->HasItemCount(ITEM_TABARD_OF_THE_BLOOD_KNIGHT, 1, true))
			lostBloodKnight = true;

		//Tabard of the Hand
		if (player->GetQuestRewardStatus(QUEST_THE_UNWRITTEN_PROPHECY) && !player->HasItemCount(ITEM_TABARD_OF_THE_HAND, 1, true))
			lostHand = true;

		//Tabard of the Protector
		if (player->GetQuestRewardStatus(QUEST_INTO_THE_BREACH) && !player->HasItemCount(ITEM_TABARD_OF_THE_PROTECTOR, 1, true))
			lostProtector = true;

		//Green Trophy Tabard of the Illidari
		//Purple Trophy Tabard of the Illidari
		if (player->GetQuestRewardStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) &&
			(!player->HasItemCount(ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
			!player->HasItemCount(ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
			!player->HasItemCount(ITEM_OFFERING_OF_THE_SHATAR, 1, true)))
			lostIllidari = true;

		//Tabard of Summer Skies
		//Tabard of Summer Flames
		if (player->GetQuestRewardStatus(QUEST_SHARDS_OF_AHUNE) &&
			!player->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_SKIES, 1, true) &&
			!player->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_FLAMES, 1, true))
			lostSummer = true;

		if (lostBloodKnight || lostHand || lostProtector || lostIllidari || lostSummer)
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

			if (lostBloodKnight)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

			if (lostHand)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_HAND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

			if (lostProtector)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_PROTECTOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

			if (lostIllidari)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
			}

			if (lostSummer)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_SKIES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
			}

			player->SEND_GOSSIP_MENU(13583, creature->GetGUID());
		}
		else
			player->GetSession()->SendListInventory(creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (action)
		{
			case GOSSIP_ACTION_TRADE:
				player->GetSession()->SendListInventory(creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 1:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_TABARD_OF_THE_BLOOD_KNIGHT, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 2:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_TABARD_OF_THE_HAND, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 3:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_TABARD_OF_THE_PROTECTOR, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 4:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 5:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 6:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_TABARD_OF_SUMMER_SKIES, false);
				break;
			case GOSSIP_ACTION_INFO_DEF + 7:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_TABARD_OF_SUMMER_FLAMES, false);
				break;
		}
		return true;
	}
};

enum eBrewfestBarkQuests
{
    BARK_FOR_THE_THUNDERBREWS       = 11294,
    BARK_FOR_TCHALIS_VOODOO_BREWERY = 11408,
    BARK_FOR_THE_BARLEYBREWS        = 11293,
    BARK_FOR_DROHNS_DISTILLERY      = 11407,

    SPELL_RAMSTEIN_SWIFT_WORK_RAM   = 43880,
    SPELL_BREWFEST_RAM              = 43883,
    SPELL_RAM_FATIGUE               = 43052,
    SPELL_SPEED_RAM_GALLOP          = 42994,
    SPELL_SPEED_RAM_CANTER          = 42993,
    SPELL_SPEED_RAM_TROT            = 42992,
    SPELL_SPEED_RAM_NORMAL          = 43310,
    SPELL_SPEED_RAM_EXHAUSED        = 43332,

    NPC_SPEED_BUNNY_GREEN           = 24263,
    NPC_SPEED_BUNNY_YELLOW          = 24264,
    NPC_SPEED_BUNNY_RED             = 24265,
    NPC_BARKER_BUNNY_1              = 24202,
    NPC_BARKER_BUNNY_2              = 24203,
    NPC_BARKER_BUNNY_3              = 24204,
    NPC_BARKER_BUNNY_4              = 24205,
};

class npc_brewfest_trigger : public CreatureScript
{
public:
    npc_brewfest_trigger() : CreatureScript("npc_brewfest_trigger") { }

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_brewfest_triggerAI(creature);
    }

    struct npc_brewfest_triggerAI : public ScriptedAI
    {
        npc_brewfest_triggerAI(Creature* c) : ScriptedAI(c) {}

        void MoveInLineOfSight(Unit *who)
        {
            if (!who)
                return;

            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if (!(CAST_PLR(who)->GetAura(SPELL_BREWFEST_RAM)))
                    return;

                if (CAST_PLR(who)->GetQuestStatus(BARK_FOR_THE_THUNDERBREWS) == QUEST_STATUS_INCOMPLETE||
                    CAST_PLR(who)->GetQuestStatus(BARK_FOR_TCHALIS_VOODOO_BREWERY) == QUEST_STATUS_INCOMPLETE||
                    CAST_PLR(who)->GetQuestStatus(BARK_FOR_THE_BARLEYBREWS) == QUEST_STATUS_INCOMPLETE||
                    CAST_PLR(who)->GetQuestStatus(BARK_FOR_DROHNS_DISTILLERY) == QUEST_STATUS_INCOMPLETE)
                {
                    uint32 creditMarkerId = me->GetEntry();
                    if ((creditMarkerId >= 24202) && (creditMarkerId <= 24205))
                    {
                        if (!CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_THE_BARLEYBREWS, creditMarkerId)||
                            !CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_THE_THUNDERBREWS, creditMarkerId)||
                            !CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_DROHNS_DISTILLERY, creditMarkerId)||
                            !CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_TCHALIS_VOODOO_BREWERY, creditMarkerId))
                            CAST_PLR(who)->KilledMonsterCredit(creditMarkerId, me->GetGUID());
                        if (CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_THE_BARLEYBREWS, NPC_BARKER_BUNNY_1)&&
                            CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_THE_BARLEYBREWS, NPC_BARKER_BUNNY_2)&&
                            CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_THE_BARLEYBREWS, NPC_BARKER_BUNNY_3)&&
                            CAST_PLR(who)->GetReqKillOrCastCurrentCount(BARK_FOR_THE_BARLEYBREWS, NPC_BARKER_BUNNY_4))
                            CAST_PLR(who)->CompleteQuest(BARK_FOR_THE_BARLEYBREWS);
                    }
                }
            }
        }
    };
};

/*####
## npc_brewfest_apple_trigger
####*/

class npc_brewfest_apple_trigger : public CreatureScript
{
public:
    npc_brewfest_apple_trigger() : CreatureScript("npc_brewfest_apple_trigger") { }

    struct npc_brewfest_apple_triggerAI : public ScriptedAI
    {
        npc_brewfest_apple_triggerAI(Creature* c) : ScriptedAI(c) {}

        void MoveInLineOfSight(Unit *who)
        {
            Player *player = who->ToPlayer();
            if (!player)
                return;
            if (player->HasAura(SPELL_RAM_FATIGUE) && me->GetDistance(player->GetPositionX(),player->GetPositionY(),player->GetPositionZ()) <= 7.5f)
                player->RemoveAura(SPELL_RAM_FATIGUE);
        }
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_brewfest_apple_triggerAI(creature);
    }
};

/*####
## spell_brewfest_speed
####*/

class spell_brewfest_speed : public SpellScriptLoader
{
public:
    spell_brewfest_speed() : SpellScriptLoader("spell_brewfest_speed") {}

    class spell_brewfest_speed_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_brewfest_speed_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_RAM_FATIGUE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_RAMSTEIN_SWIFT_WORK_RAM))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_BREWFEST_RAM))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_GALLOP))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_CANTER))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_TROT))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_NORMAL))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_GALLOP))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SPEED_RAM_EXHAUSED))
                return false;
            return true;
        }

        void HandlePeriodicTick(AuraEffect const* aurEff)
        {
            if (GetId() == SPELL_SPEED_RAM_EXHAUSED)
                return;

            Player* pCaster = GetCaster()->ToPlayer();
            if (!pCaster)
                return;
            int i;
            switch (GetId())
            {
            case SPELL_SPEED_RAM_GALLOP:
                for (i = 0; i < 5; i++)
                    pCaster->AddAura(SPELL_RAM_FATIGUE,pCaster);
                break;
            case SPELL_SPEED_RAM_CANTER:
                pCaster->AddAura(SPELL_RAM_FATIGUE,pCaster);
                break;
            case SPELL_SPEED_RAM_TROT:
                if (pCaster->HasAura(SPELL_RAM_FATIGUE))
                    if (pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() <= 2)
                        pCaster->RemoveAura(SPELL_RAM_FATIGUE);
                    else
                        pCaster->GetAura(SPELL_RAM_FATIGUE)->ModStackAmount(-2);
                break;
            case SPELL_SPEED_RAM_NORMAL:
                if (pCaster->HasAura(SPELL_RAM_FATIGUE))
                    if (pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() <= 4)
                        pCaster->RemoveAura(SPELL_RAM_FATIGUE);
                    else
                        pCaster->GetAura(SPELL_RAM_FATIGUE)->ModStackAmount(-4);
                break;
            }

            switch (aurEff->GetId())
            {
            case SPELL_SPEED_RAM_TROT:
                if (aurEff->GetTickNumber() == 4)
                    pCaster->KilledMonsterCredit(NPC_SPEED_BUNNY_GREEN, 0);
                break;
            case SPELL_SPEED_RAM_CANTER:
                if (aurEff->GetTickNumber() == 8)
                    pCaster->KilledMonsterCredit(NPC_SPEED_BUNNY_YELLOW, 0);
                break;
            case SPELL_SPEED_RAM_GALLOP:
                if (aurEff->GetTickNumber() == 8)
                    pCaster->KilledMonsterCredit(NPC_SPEED_BUNNY_RED, 0);
                break;
            }
            if (pCaster->HasAura(SPELL_RAM_FATIGUE))
                if (pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() >= 100)
                    pCaster->CastSpell(pCaster,SPELL_SPEED_RAM_EXHAUSED, false);
        }

        void OnRemove(AuraEffect const * aurEff, AuraEffectHandleModes /*mode*/)
        {
            Player* pCaster = GetCaster()->ToPlayer();
            if (!pCaster)
                return;
			
            if (!pCaster->HasAura(SPELL_BREWFEST_RAM) && !pCaster->HasAura(SPELL_RAMSTEIN_SWIFT_WORK_RAM))
                return;
			
			if (!pCaster->HasAura(SPELL_RAM_FATIGUE) || pCaster->GetAura(SPELL_RAM_FATIGUE)->GetStackAmount() < 100)
			{
                switch (GetId())
				{
					case SPELL_SPEED_RAM_GALLOP:
						if (!pCaster->HasAura(SPELL_SPEED_RAM_EXHAUSED))
							pCaster->CastSpell(pCaster, SPELL_SPEED_RAM_CANTER, false);
						break;
					case SPELL_SPEED_RAM_CANTER:
						if (!pCaster->HasAura(SPELL_SPEED_RAM_GALLOP))
							pCaster->CastSpell(pCaster, SPELL_SPEED_RAM_TROT, false);
						break;
					case SPELL_SPEED_RAM_TROT:
						if (!pCaster->HasAura(SPELL_SPEED_RAM_CANTER))
							pCaster->CastSpell(pCaster, SPELL_SPEED_RAM_NORMAL, false);
						break;
					case SPELL_SPEED_RAM_EXHAUSED:
						if (pCaster->HasAura(SPELL_RAM_FATIGUE))
							pCaster->GetAura(SPELL_RAM_FATIGUE)->SetStackAmount(85);
						pCaster->CastSpell(pCaster, SPELL_SPEED_RAM_NORMAL, false);
						break;
				}
			}
        }

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Player* pCaster = GetCaster()->ToPlayer();

            if (!pCaster)
                return;

            switch (GetId())
            {
            case SPELL_SPEED_RAM_GALLOP:
                pCaster->GetAura(SPELL_SPEED_RAM_GALLOP)->SetDuration(4000);
                break;
            case SPELL_SPEED_RAM_CANTER:
                pCaster->GetAura(SPELL_SPEED_RAM_CANTER)->SetDuration(4000);
                break;
            case SPELL_SPEED_RAM_TROT:
                pCaster->GetAura(SPELL_SPEED_RAM_TROT)->SetDuration(4000);
                break;
            }
        }

        void Register()
        {
			switch (m_scriptSpellId)
			{
				case SPELL_SPEED_RAM_NORMAL:
					OnEffectApply += AuraEffectApplyFn(spell_brewfest_speed_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
					OnEffectPeriodic += AuraEffectPeriodicFn(spell_brewfest_speed_AuraScript::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
					AfterEffectRemove += AuraEffectRemoveFn(spell_brewfest_speed_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED, AURA_EFFECT_HANDLE_REAL);
					break;
				case SPELL_SPEED_RAM_EXHAUSED:
					OnEffectApply += AuraEffectApplyFn(spell_brewfest_speed_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
					AfterEffectRemove += AuraEffectRemoveFn(spell_brewfest_speed_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED, AURA_EFFECT_HANDLE_REAL);
					break;
				default:
					OnEffectApply += AuraEffectApplyFn(spell_brewfest_speed_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED, AURA_EFFECT_HANDLE_REAL);
					OnEffectPeriodic += AuraEffectPeriodicFn(spell_brewfest_speed_AuraScript::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
					AfterEffectRemove += AuraEffectRemoveFn(spell_brewfest_speed_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED, AURA_EFFECT_HANDLE_REAL);
					break;
			}
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_brewfest_speed_AuraScript();
    }
};

/*######
## Q Пей до дна!
######*/

enum BrewfestQuestChugAndChuck
{
    QUEST_CHUG_AND_CHUCK_A      = 12022,
    QUEST_CHUG_AND_CHUCK_H      = 12191,
    
    NPC_BREWFEST_STOUT          = 24108
};

class item_brewfest_ChugAndChuck : public ItemScript
{
public:
    item_brewfest_ChugAndChuck() : ItemScript("item_brewfest_ChugAndChuck") { }

    bool OnUse(Player* player, Item* pItem, const SpellCastTargets & /*pTargets*/)
    {
        if (player->GetQuestStatus(QUEST_CHUG_AND_CHUCK_A) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_CHUG_AND_CHUCK_H) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* pStout = player->FindNearestCreature(NPC_BREWFEST_STOUT, 10.0f)) // spell range
            {
                return false;
            } else
                player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, pItem, NULL);
        } else
            player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW ,pItem, NULL);
        return true;
    }
};

class item_brewfest_ram_reins : public ItemScript
{
public:
    item_brewfest_ram_reins() : ItemScript("item_brewfest_ram_reins") { }

    bool OnUse(Player* player, Item* pItem, const SpellCastTargets & /*pTargets*/)
    {
        if ((player->HasAura(SPELL_BREWFEST_RAM) || player->HasAura(SPELL_RAMSTEIN_SWIFT_WORK_RAM)) && !player->HasAura(SPELL_SPEED_RAM_EXHAUSED))
        {
            if (player->HasAura(SPELL_SPEED_RAM_NORMAL))
                player->CastSpell(player,SPELL_SPEED_RAM_TROT,false);
            else if (player->HasAura(SPELL_SPEED_RAM_TROT))
            {
                if (player->GetAura(SPELL_SPEED_RAM_TROT)->GetDuration() < 3000)
                    player->GetAura(SPELL_SPEED_RAM_TROT)->SetDuration(4000);
                else
                    player->CastSpell(player,SPELL_SPEED_RAM_CANTER,false);
            } else if (player->HasAura(SPELL_SPEED_RAM_CANTER))
            {
                if (player->GetAura(SPELL_SPEED_RAM_CANTER)->GetDuration() < 3000)
                    player->GetAura(SPELL_SPEED_RAM_CANTER)->SetDuration(4000);
                else
                    player->CastSpell(player,SPELL_SPEED_RAM_GALLOP,false);
            } else if (player->HasAura(SPELL_SPEED_RAM_GALLOP))
                player->GetAura(SPELL_SPEED_RAM_GALLOP)->SetDuration(4000);
        }
        else
            player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW ,pItem, NULL);

        return true;
    }
};

/*####
## npc_brewfest_keg_thrower
####*/

enum BrewfestKegThrower
{
    SPELL_THROW_KEG     = 43660,
    ITEM_BREWFEST_KEG   = 33797
};

class npc_brewfest_keg_thrower : public CreatureScript
{
public:
    npc_brewfest_keg_thrower() : CreatureScript("npc_brewfest_keg_thrower") { }

    struct npc_brewfest_keg_throwerAI : public ScriptedAI
    {
        npc_brewfest_keg_throwerAI(Creature* c) : ScriptedAI(c) {}

        void MoveInLineOfSight(Unit *who)
        {
            Player *player = who->ToPlayer();
            if (!player)
                return;
            if ((player->HasAura(SPELL_BREWFEST_RAM) || player->HasAura(SPELL_RAMSTEIN_SWIFT_WORK_RAM))
                && me->GetDistance(player->GetPositionX(),player->GetPositionY(),player->GetPositionZ()) <= 25.0f
                && !player->HasItemCount(ITEM_BREWFEST_KEG,1))
            {
                me->CastSpell(player,SPELL_THROW_KEG,false);
                me->CastSpell(player,42414,false);
            }
        }
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_brewfest_keg_throwerAI(creature);
    }
};

enum BrewfestKegReceiver
{
    SPELL_CREATE_TICKETS            = 44501,
    
    QUEST_THERE_AND_BACK_AGAIN_A    = 11122,
    QUEST_THERE_AND_BACK_AGAIN_H    = 11412,
    
    NPC_BREWFEST_DELIVERY_BUNNY     = 24337
};

class npc_brewfest_keg_receiver : public CreatureScript
{
public:
    npc_brewfest_keg_receiver() : CreatureScript("npc_brewfest_keg_receiver") { }

    struct npc_brewfest_keg_receiverAI : public ScriptedAI
    {
        npc_brewfest_keg_receiverAI(Creature* c) : ScriptedAI(c) {}

        void MoveInLineOfSight(Unit *who)
        {
            Player *player = who->ToPlayer();
            if (!player)
                return;

            if ((player->HasAura(SPELL_BREWFEST_RAM) || player->HasAura(SPELL_RAMSTEIN_SWIFT_WORK_RAM))
                && me->GetDistance(player->GetPositionX(),player->GetPositionY(),player->GetPositionZ()) <= 5.0f
                && player->HasItemCount(ITEM_BREWFEST_KEG,1))
            {
                player->CastSpell(me,SPELL_THROW_KEG,true);
                player->DestroyItemCount(ITEM_BREWFEST_KEG,1,true);
                if (player->HasAura(SPELL_BREWFEST_RAM))
                    player->GetAura(SPELL_BREWFEST_RAM)->SetDuration(player->GetAura(SPELL_BREWFEST_RAM)->GetDuration() + 30000);
                if (player->HasAura(SPELL_RAMSTEIN_SWIFT_WORK_RAM))
                    player->GetAura(SPELL_RAMSTEIN_SWIFT_WORK_RAM)->SetDuration(player->GetAura(SPELL_RAMSTEIN_SWIFT_WORK_RAM)->GetDuration() + 30000);
                if (player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_A)
                    || player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_H))
                {
                    player->CastSpell(player,SPELL_CREATE_TICKETS,true);
                }
                else
                {
                    player->KilledMonsterCredit(NPC_BREWFEST_DELIVERY_BUNNY,0);
                    if (player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_A) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(QUEST_THERE_AND_BACK_AGAIN_A);
                    if (player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_H) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(QUEST_THERE_AND_BACK_AGAIN_H);
                    if (player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_A) == QUEST_STATUS_COMPLETE
                        || player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_H) == QUEST_STATUS_COMPLETE)
                        player->RemoveAura(SPELL_BREWFEST_RAM);
                }
            }
        }
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_brewfest_keg_receiverAI(creature);
    }
};

/*####
## npc_brewfest_ram_master
####*/

#define GOSSIP_ITEM_RAM "Do you have additional work?"
#define GOSSIP_ITEM_RAM_REINS "Give me another Ram Racing Reins"
#define SPELL_BREWFEST_SUMMON_RAM 43720 // Trigger Brewfest Racing Ram - Relay Race - Intro

class npc_brewfest_ram_master : public CreatureScript
{
public:
    npc_brewfest_ram_master() : CreatureScript("npc_brewfest_ram_master") { }

    bool OnGossipHello(Player *player, Creature *pCreature)
    {
        if (pCreature->isQuestGiver())
            player->PrepareQuestMenu(pCreature->GetGUID());

        if (player->HasSpellCooldown(SPELL_BREWFEST_SUMMON_RAM)
            && !player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_A)
            && !player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_H)
            && (player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_A) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_H) == QUEST_STATUS_INCOMPLETE))
            player->RemoveSpellCooldown(SPELL_BREWFEST_SUMMON_RAM);

        if (!player->HasAura(SPELL_BREWFEST_RAM) && ((player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_A) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THERE_AND_BACK_AGAIN_H) == QUEST_STATUS_INCOMPLETE
            || (!player->HasSpellCooldown(SPELL_BREWFEST_SUMMON_RAM) &&
            (player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_A)
            || player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_H))))))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RAM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if ((player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_A)
            || player->GetQuestRewardStatus(QUEST_THERE_AND_BACK_AGAIN_H))
            && !player->HasItemCount(33306,1,true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RAM_REINS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

        player->SEND_GOSSIP_MENU(384, pCreature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            if (player->HasItemCount(ITEM_BREWFEST_KEG,1))
                player->DestroyItemCount(ITEM_BREWFEST_KEG,1,true);
            player->CastSpell(player,SPELL_BREWFEST_SUMMON_RAM,true);
            player->AddSpellCooldown(SPELL_BREWFEST_SUMMON_RAM,0,time(NULL) + 18*60*60);
        }
        if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
        {
            player->CastSpell(player,44371,false);
        }
        return true;
    }
};
/*
enum DarkIronGuzzler
{
    NPC_DARK_IRON_GUZZLER       = 23709,
    NPC_DARK_IRON_HERALD        = 24536,
    NPC_DARK_IRON_SPAWN_BUNNY   = 23894,
 
    NPC_FESTIVE_KEG_1           = 23702,
    NPC_FESTIVE_KEG_2           = 23700,
    NPC_FESTIVE_KEG_3           = 23706,
    NPC_FESTIVE_KEG_4           = 24373,
    NPC_FESTIVE_KEG_5           = 24372,
 
    SPELL_GO_TO_NEW_TARGET      = 42498,
    SPELL_ATTACK_KEG            = 42393,
    SPELL_RETREAT               = 42341,
    SPELL_DRINK                 = 42436,
 
    SAY_RANDOM              = 0,
};
 
class npc_dark_iron_guzzler : public CreatureScript
{
public:
    npc_dark_iron_guzzler() : CreatureScript("npc_dark_iron_guzzler") { }
 
    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_dark_iron_guzzlerAI(creature);
    }
 
    struct npc_dark_iron_guzzlerAI : public ScriptedAI
    {
        npc_dark_iron_guzzlerAI(Creature* creature) : ScriptedAI(creature) { }
 
        bool atKeg;
        bool playersLost;
        bool barleyAlive;
        bool thunderAlive;
        bool gordokAlive;
        bool drohnAlive;
        bool tchaliAlive;
 
        uint32 AttackKegTimer;
        uint32 TalkTimer;
 
        void Reset()
        {
            AttackKegTimer = 5000;
            TalkTimer = (urand(1000, 120000));
            me->SetWalk(true);
        }
 
        void IsSummonedBy(Unit* summoner)
        {
            DoCast(me, SPELL_GO_TO_NEW_TARGET);
        }
 
        void SetData(uint32 type, uint32 data)
        {
            if (type == 10 && data == 10)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                thunderAlive = false;
            }
 
            if (type == 11 && data == 11)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                barleyAlive = false;
            }
 
            if (type == 12 && data == 12)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                gordokAlive = false;
            }
 
            if (type == 13 && data == 13)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                drohnAlive = false;
            }
 
            if (type == 14 && data == 14)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                tchaliAlive = false;
            }
        }
 
        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DRINK)
            {
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
                me->StopMoving();
 
				me->DespawnOrUnsummon(7000);
            }
 
            if (spell->Id == SPELL_RETREAT)
            {
                me->SetWalk(false);
 
                if (me->GetAreaId() == 1296)
                {
                    me->GetMotionMaster()->MovePoint(1, 1197.63f, -4293.571f, 21.243f);
                }
                else if (me->GetAreaId() == 1)
                {
                     me->GetMotionMaster()->MovePoint(2, -5152.3f, -603.529f, 398.356f);
                }
            }
 
            if (spell->Id == SPELL_GO_TO_NEW_TARGET)
            {
                if (me->GetAreaId() == 1296)
                {
                    if (drohnAlive && gordokAlive && tchaliAlive)
                    {
                        switch (urand(0, 2))
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                                break;
                            case 2:
                                me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                                break;
                        }
                    }
                    else if (!drohnAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                                break;
                        }
                    }
                    else if (!gordokAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                                break;
                        }
                    }
                    else if (!tchaliAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                                break;
                        }
                    }
                }
                else if (me->GetAreaId() == 1)
                {
                    if (barleyAlive && gordokAlive && thunderAlive)
                    {
                        switch (urand(0, 2))
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                                break;
                            case 2:
                                me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                                break;
                        }
                    }
                    else if (!barleyAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                                break;
                        }
                    }
                    else if (!gordokAlive)
                    {
                        switch (urand(0, 1))
                        {                            
                            case 0:
                                me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                                break;
                        }
                    }
                    else if (!thunderAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                                break;
                        }
                    }
                }
                atKeg = false;
            }
        }
 
        void MovementInform(uint32 Type, uint32 PointId)
        {
            if (Type != POINT_MOTION_TYPE)
                return;
 
            if (PointId == 1 || PointId == 2)
                me->DespawnOrUnsummon(1000);
 
            if (PointId == 4 || PointId == 5 || PointId == 6 || PointId == 7 || PointId == 8 || PointId == 9)
            {
                DoCast(SPELL_ATTACK_KEG);
                me->SetByteFlag(UNIT_FIELD_BYTES_1, 1, 0x01);
                atKeg = true;
            }
        }
 
        void UpdateAI(const uint32 diff)
        {
            if (!IsHolidayActive(HOLIDAY_BREWFEST))
                return;
 
            if ((!gordokAlive && !thunderAlive && !barleyAlive) || (!gordokAlive && !drohnAlive && !tchaliAlive))
            {
                DoCast(me, SPELL_RETREAT);
 
                if (Creature* herald = me->FindNearestCreature(NPC_DARK_IRON_HERALD, 100.0f))
                    herald->AI()->SetData(20, 20);
 
                if (Creature* spawnbunny = me->FindNearestCreature(NPC_DARK_IRON_SPAWN_BUNNY, 100.0f))
                    spawnbunny->DespawnOrUnsummon();
            }
 
            if (TalkTimer <= diff)
            {
                me->AI()->Talk(SAY_RANDOM);
                TalkTimer = (urand(44000, 120000));
            } else TalkTimer -= diff;
 
            if (atKeg)
            {
                if (AttackKegTimer <= diff)
                {
                    DoCast(SPELL_ATTACK_KEG);
                    AttackKegTimer = 5000;
                } else AttackKegTimer -= diff;
            }
        }
    };
};*/

// Dark Iron Guzzler in the Brewfest achievement 'Down With The Dark Iron'
enum DarkIronGuzzler
{
    NPC_DARK_IRON_GUZZLER       = 23709,
    NPC_DARK_IRON_HERALD        = 24536,
    NPC_DARK_IRON_SPAWN_BUNNY   = 23894,

    NPC_FESTIVE_KEG_1           = 23702, // Thunderbrew Festive Keg
    NPC_FESTIVE_KEG_2           = 23700, // Barleybrew Festive Keg
    NPC_FESTIVE_KEG_3           = 23706, // Gordok Festive Keg
    NPC_FESTIVE_KEG_4           = 24373, // T'chalis's Festive Keg
    NPC_FESTIVE_KEG_5           = 24372, // Drohn's Festive Keg

    SPELL_GO_TO_NEW_TARGET      = 42498,
    SPELL_ATTACK_KEG            = 42393,
    SPELL_RETREAT               = 42341,
    SPELL_DRINK                 = 42436,

    SAY_RANDOM              = 0,
};
 
class npc_dark_iron_guzzler : public CreatureScript
{
public:
    npc_dark_iron_guzzler() : CreatureScript("npc_dark_iron_guzzler") { }

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_dark_iron_guzzlerAI(creature);
    }

    struct npc_dark_iron_guzzlerAI : public ScriptedAI
    {
        npc_dark_iron_guzzlerAI(Creature* creature) : ScriptedAI(creature) { }

        bool atKeg;
        bool barleyAlive;
        bool thunderAlive;
        bool gordokAlive;
        bool drohnAlive;
        bool tchaliAlive;

        uint32 AttackKegTimer;
        uint32 TalkTimer;

        void Reset()
        {
            AttackKegTimer = 5000;
            TalkTimer = (urand(1000, 120000));
            me->SetWalk(true);
			barleyAlive = true;
			thunderAlive = true;
			gordokAlive = true;
			drohnAlive = true;
			tchaliAlive = true;
        }

        void IsSummonedBy(Unit* summoner)
        {
            // Only cast the spell on spawn
            DoCast(me, SPELL_GO_TO_NEW_TARGET);
        }

        // These values are set through SAI - when a Festive Keg dies it will set data to all Dark Iron Guzzlers within 3 yards (the killers)
        void SetData(uint32 type, uint32 data)
        {
            if (type == 10 && data == 10)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                thunderAlive = false;
            }

            if (type == 11 && data == 11)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                barleyAlive = false;
            }

            if (type == 12 && data == 12)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                gordokAlive = false;
            }

            if (type == 13 && data == 13)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                drohnAlive = false;
            }

            if (type == 14 && data == 14)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                tchaliAlive = false;
            }
        }

        // As you can see here we do not have to use a spellscript for this
        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DRINK)
            {
				me->CastSpell(caster, 42518);//give back beer

                // Fake death - it's only visual!
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
                me->StopMoving();

                // Time based on information from videos
                me->DespawnOrUnsummon(7000);
            }

            // Retreat - run back
            else if (spell->Id == SPELL_RETREAT)
            {
                // Remove walking flag so we start running
				me->SetWalk(false);

                if (me->GetAreaId() == 1296)
                {
                    me->GetMotionMaster()->MovePoint(1, 1197.63f, -4293.571f, 21.243f);
                }
                else if (me->GetAreaId() == 1)
                {
                     me->GetMotionMaster()->MovePoint(2, -5152.3f, -603.529f, 398.356f);
                }
            }

            else if (spell->Id == SPELL_GO_TO_NEW_TARGET)
            {
                // If we're at Durotar we target different kegs if we are at at Dun Morogh
                if (me->GetAreaId() == 1296)
                {
                    if (drohnAlive && gordokAlive && tchaliAlive)
                    {
                        switch (urand(0, 2))
                        {
                            case 0: // Gordok Festive Keg
                                me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                                break;
                            case 1: // Drohn's Festive Keg
                                me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                                break;
                            case 2: // Ti'chali's Festive Keg
                                me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                                break;
                        }
                    }
                    else if (!drohnAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0: // Gordok Festive Keg
                                me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                                break;
                            case 1: // Ti'chali's Festive Keg
                                me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                                break;
                        }
                    }
                    else if (!gordokAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0: // Drohn's Festive Keg
                                me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                                break;
                            case 1: // Ti'chali's Festive Keg
                                me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                                break;
                        }
                    }
                    else if (!tchaliAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0: // Gordok Festive Keg
                                me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                                break;
                            case 1: // Drohn's Festive Keg
                                me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                                break;
                        }
                    }
                }
                // If we're at Dun Morogh we target different kegs if we are at Durotar
                else if (me->GetAreaId() == 1)
                {
                    if (barleyAlive && gordokAlive && thunderAlive)
                    {
                        switch (urand(0, 2))
                        {
                            case 0: // Barleybrew Festive Keg
                                me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                                break;
                            case 1: // Thunderbrew Festive Keg
                                me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                                break;
                            case 2: // Gordok Festive Keg
                                me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                                break;
                        }
                    }
                    else if (!barleyAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0: // Thunderbrew Festive Keg
                                me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                                break;
                            case 1: // Gordok Festive Keg
                                me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                                break;
                        }
                    }
                    else if (!gordokAlive)
                    {
                        switch (urand(0, 1))
                        {                            
                            case 0: // Barleybrew Festive Keg
                                me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                                break;
                            case 1: // Thunderbrew Festive Keg
                                me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                                break;
                        }
                    }
                    else if (!thunderAlive)
                    {
                        switch (urand(0, 1))
                        {
                            case 0: // Barleybrew Festive Keg
                                me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                                break;
                            case 1: // Gordok Festive Keg
                                me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                                break;
                        }
                    }
                }
                atKeg = false;
            }
        }

        void MovementInform(uint32 Type, uint32 PointId)
        {
            if (Type != POINT_MOTION_TYPE)
                return;

			switch (PointId)
			{
				case 1:
				case 2:
					me->DespawnOrUnsummon(1000);
					break;
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
					atKeg = true;
					break;
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!IsHolidayActive(HOLIDAY_BREWFEST))
                return;

            // If all kegs are dead we should retreat because we have won
            if ((!gordokAlive && !thunderAlive && !barleyAlive) || (!gordokAlive && !drohnAlive && !tchaliAlive))
            {
                DoCast(me, SPELL_RETREAT);

                // We are doing this because we'll have to reset our scripts when we won
                if (Creature* herald = me->FindNearestCreature(NPC_DARK_IRON_HERALD, 100.0f))
                    herald->AI()->SetData(20, 20);

                // Despawn all summon bunnies so they will stop summoning guzzlers
                if (Creature* spawnbunny = me->FindNearestCreature(NPC_DARK_IRON_SPAWN_BUNNY, 100.0f))
                    spawnbunny->DespawnOrUnsummon();
            }

            if (TalkTimer <= diff)
            {
                me->AI()->Talk(SAY_RANDOM);
                TalkTimer = (urand(44000, 120000));
            } else TalkTimer -= diff;

            // Only happens if we're at keg
            if (atKeg)
            {
                if (AttackKegTimer <= diff)
                {
                    DoCast(SPELL_ATTACK_KEG);
                    AttackKegTimer = 5000;
                } else AttackKegTimer -= diff;
            }
        }
    };
};

/*######
## npc_coren direbrew
######*/

enum CorenDirebrew
{
    SPELL_DISARM                = 47310,
    SPELL_DISARM_PRECAST        = 47407,
    SPELL_MOLE_MACHINE_EMERGE   = 50313,

    NPC_ILSA_DIREBREW           = 26764,
    NPC_URSULA_DIREBREW         = 26822,
    NPC_DIREBREW_MINION         = 26776,

    EQUIP_ID_TANKARD            = 48663,
    FACTION_HOSTILE             = 736
};

#define GOSSIP_TEXT_INSULT "Insult Coren Direbrew's brew."

static Position _pos[]=
{
    {890.87f, -133.95f, -48.0f, 1.53f},
    {892.47f, -133.26f, -48.0f, 2.16f},
    {893.54f, -131.81f, -48.0f, 2.75f}
};

class npc_coren_direbrew : public CreatureScript
{
public:
    npc_coren_direbrew() : CreatureScript("npc_coren_direbrew") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_INSULT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(15858, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            creature->setFaction(FACTION_HOSTILE);
            creature->AI()->AttackStart(player);
            creature->AI()->DoZoneInCombat();
            player->CLOSE_GOSSIP_MENU();
        }

        return true;
    }

    struct npc_coren_direbrewAI : public ScriptedAI
    {
        npc_coren_direbrewAI(Creature* c) : ScriptedAI(c), _summons(me)
        {
        }

        void Reset()
        {
            me->RestoreFaction();
            me->SetCorpseDelay(90); // 1.5 minutes

            _addTimer = 20000;
            _disarmTimer = urand(10, 15) *IN_MILLISECONDS;

            _spawnedIlsa = false;
            _spawnedUrsula = false;
            _summons.DespawnAll();

            for (uint8 i = 0; i < 3; ++i)
                if (Creature* creature = me->SummonCreature(NPC_DIREBREW_MINION, _pos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                    _add[i] = creature->GetGUID();
        }

        void EnterCombat(Unit* /*who*/)
        {
            SetEquipmentSlots(false, EQUIP_ID_TANKARD, EQUIP_ID_TANKARD, EQUIP_NO_CHANGE);

            for (uint8 i = 0; i < 3; ++i)
            {
                if (_add[i])
                {
                    Creature* creature = ObjectAccessor::GetCreature((*me), _add[i]);
                    if (creature && creature->isAlive())
                    {
                        creature->setFaction(FACTION_HOSTILE);
                        creature->SetInCombatWithZone();
                    }
                    _add[i] = 0;
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (_disarmTimer <= diff)
            {
                DoCast(SPELL_DISARM_PRECAST);
                DoCastVictim(SPELL_DISARM, false);
                _disarmTimer = urand(20, 25) *IN_MILLISECONDS;
            }
            else
                _disarmTimer -= diff;

            if (_addTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                {
                    float posX, posY, posZ;
                    target->GetPosition(posX, posY, posZ);
                    target->CastSpell(target, SPELL_MOLE_MACHINE_EMERGE, true, 0, 0, me->GetGUID());
                    me->SummonCreature(NPC_DIREBREW_MINION, posX, posY, posZ, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);

                    _addTimer = 15000;
                    if (_spawnedIlsa)
                        _addTimer -= 3000;
                    if (_spawnedUrsula)
                        _addTimer -= 3000;
                }
            }
            else
                _addTimer -= diff;

            if (!_spawnedIlsa && HealthBelowPct(66))
            {
                DoSpawnCreature(NPC_ILSA_DIREBREW, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
                _spawnedIlsa = true;
            }

            if (!_spawnedUrsula && HealthBelowPct(33))
            {
                DoSpawnCreature(NPC_URSULA_DIREBREW, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
                _spawnedUrsula = true;
            }

            DoMeleeAttackIfReady();
        }

        void JustSummoned(Creature* summon)
        {
            if (me->getFaction() == FACTION_HOSTILE)
            {
                summon->setFaction(FACTION_HOSTILE);

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    summon->AI()->AttackStart(target);
            }

            _summons.Summon(summon);
        }

        void JustDied(Unit* /*killer*/)
        {
            _summons.DespawnAll();
            Map* map = me->GetMap();
            if (map && map->IsDungeon())
            {
                Map::PlayerList const& players = map->GetPlayers();
                if (!players.isEmpty())
                    for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                        if (Player* player = i->getSource())
                            if (player->GetDistance(me) < 100.0f)
                                sLFGMgr->RewardDungeonDoneFor(287, player);
            }
        }

    private:
        SummonList _summons;
        uint64 _add[3];
        uint32 _addTimer;
        uint32 _disarmTimer;
        bool _spawnedIlsa;
        bool _spawnedUrsula;
    };

    CreatureAI* GetAI(Creature* c) const
    {
        return new npc_coren_direbrewAI(c);
    }
};

/*######
## dark iron brewmaiden
######*/

enum Brewmaiden
{
    SPELL_SEND_FIRST_MUG            = 47333,
    SPELL_SEND_SECOND_MUG           = 47339,
    SPELL_HAS_BREW_BUFF             = 47376,
    SPELL_CONSUME_BREW              = 47377,
    SPELL_BARRELED                  = 47442,
    SPELL_CHUCK_MUG                 = 50276
};

class npc_brewmaiden : public CreatureScript
{
public:
    npc_brewmaiden() : CreatureScript("npc_brewmaiden") { }

    struct npc_brewmaidenAI : public ScriptedAI
    {
        npc_brewmaidenAI(Creature* c) : ScriptedAI(c)
        {
        }

        void Reset()
        {
            _brewTimer = 2000;
            _barrelTimer = 5000;
            _chuckMugTimer = 10000;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->SetInCombatWithZone();
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 1.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);

                if (me->GetEntry() == NPC_URSULA_DIREBREW)
                    me->GetMotionMaster()->MoveFollow(who, 10.0f, 0.0f);
                else
                    me->GetMotionMaster()->MoveChase(who);
            }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {

            if (spell->Id == SPELL_SEND_FIRST_MUG)
                target->CastSpell(target, SPELL_HAS_BREW_BUFF, true);

            if (spell->Id == SPELL_SEND_SECOND_MUG)
                target->CastSpell(target, SPELL_CONSUME_BREW, true);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (_brewTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);

                    if (target && me->GetDistance(target) > 5.0f)
                    {
                        DoCast(target, SPELL_SEND_FIRST_MUG);
                        _brewTimer = 12000;
                    }
                }
            }
            else
                _brewTimer -= diff;

            if (_chuckMugTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_CHUCK_MUG);

                _chuckMugTimer = 15000;
            }
            else
                _chuckMugTimer -= diff;

            if (me->GetEntry() == NPC_URSULA_DIREBREW)
            {
                if (_barrelTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCastVictim(SPELL_BARRELED);
                        _barrelTimer = urand(15, 18) *IN_MILLISECONDS;
                    }
                }
                else
                    _barrelTimer -= diff;
            }
            else
                DoMeleeAttackIfReady();
        }

    private:
        uint32 _brewTimer;
        uint32 _barrelTimer;
        uint32 _chuckMugTimer;
    };

    CreatureAI* GetAI(Creature* c) const
    {
        return new npc_brewmaidenAI(c);
    }
};

/*######
## go_mole_machine_console
######*/

enum MoleMachineConsole
{
    SPELL_TELEPORT = 49466
};

#define GOSSIP_ITEM_MOLE_CONSOLE "[PH] Please teleport me."

class go_mole_machine_console : public GameObjectScript
{
public:
    go_mole_machine_console() : GameObjectScript("go_mole_machine_console") { }

    bool OnGossipHello (Player* player, GameObject* go)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MOLE_CONSOLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(12709, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
            player->CastSpell(player, SPELL_TELEPORT, true);

        return true;
    }
};

#define START_GOSSIP "Let's fight!"
#define HOSTILE 16
#define FRIENDLY 35
#define QUEST_INTO_A_PIT 12997
#define QUEST_INTO_A_PIT_ALT 13424

class npc_hyldsmeet_warbear_rider : public CreatureScript
{
public:
	npc_hyldsmeet_warbear_rider() : CreatureScript("npc_hyldsmeet_warbear_rider") { }
	
	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->GetQuestStatus(QUEST_INTO_A_PIT) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_INTO_A_PIT_ALT) == QUEST_STATUS_INCOMPLETE)
			if (creature->GetVehicleBase())
			{
				player->PrepareGossipMenu(creature);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, START_GOSSIP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
				player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
				return true;
			}
		return false;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		if (action == GOSSIP_ACTION_INFO_DEF+1)
		{
			creature->GetVehicleBase()->GetAI()->DoAction(HOSTILE);
			player->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
};

enum BearSpells
{
	SPELL_DEMORALIZING_ROAR = 15971,
	SPELL_CHARGE = 54460,
	SPELL_SMASH = 54458,
};

class npc_hyldsmeet_warbear : public CreatureScript
{
public:
	npc_hyldsmeet_warbear() : CreatureScript("npc_hyldsmeet_warbear") { }
	
	struct npc_hyldsmeet_warbearAI : public ScriptedAI
	{
		npc_hyldsmeet_warbearAI(Creature* creature) : ScriptedAI(creature) { }
		
		void Reset()
		{
			timer = 0;
			chargeTimer = 0;
			smashTimer = 1000;
			roarTimer = 2000;
			me->setFaction(FRIENDLY);
		}
		
		void DoAction(const int32 action)
		{
			if (action == HOSTILE)
				timer = 5000;
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (timer)
			{
				if (timer <= diff)
				{
					me->setFaction(HOSTILE);
					timer = 0;
				}
				else
					timer -= diff;
			}
			
			if (!UpdateVictim())
				return;
			
			if (roarTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_ROAR);
				roarTimer = urand(20000, 30000);
			}
			else
				roarTimer -= diff;
			
			if (chargeTimer <= diff)
			{
				DoCastVictim(SPELL_CHARGE);
				chargeTimer = urand(15000, 20000);
			}
			else
				chargeTimer -= diff;
			
			if (smashTimer <= diff)
			{
				DoCastVictim(SPELL_SMASH);
				smashTimer = urand(7000, 10000);
			}
			else
				smashTimer -= diff;
			
			DoMeleeAttackIfReady();
		}
		
		uint32 timer;
		uint32 roarTimer;
		uint32 chargeTimer;
		uint32 smashTimer;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_hyldsmeet_warbearAI(creature);
	}
};

#define SPELL_BETRAYAL_PART 52590
#define SPELL_BETRAYAL_TASK 52224
#define START_TALK 1
#define GOSSIP_ITEM "Uhh, can you send me on the tour of Zul'Drak?"
#define FLY_TALK_1 "A portion of this land already be mine, mon, but it only be da beginning!"
#define FLY_TALK_2 "Here, da self-righteous crusaders be standin' in da way of our goal. Dey will fall!"
#define FLY_TALK_3 "Da Drakkari be destroyin' their lands! Dey gunna be losin' dem... soon, mon."
#define FLY_TALK_4 "Dis all soon gunna belong to da Lich King!"
#define FLY_TALK_5 "Ahh, dere be our destiny... Gundrak!"
#define FLY_TALK_6 "Study dis place well, mon. Ya gonna be helpin' me conquer it!"
#define FLY_TALK_7 "Ahh yeah, mon! Dere be great glory ta be had here!"
#define FLY_TALK_8 "Da Lich King be sharin' his power wit dem who serve him well, mon."
#define FLY_TALK_9 "Come to me now. I must be speakin' wit ya."
#define QUEST_DARK_HORIZON 12663
#define QUEST_REUNITED 12664

class npc_zul_drak_q12663_gorebag : public CreatureScript
{
public:
	npc_zul_drak_q12663_gorebag() : CreatureScript("npc_zul_drak_q12663_gorebag") { }
	
	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->GetQuestStatus(QUEST_DARK_HORIZON) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_REUNITED) == QUEST_STATUS_INCOMPLETE)
			if (!creature->AI()->GetData(START_TALK))
			{
				player->PrepareGossipMenu(creature);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
				player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
				return true;
			}
		
		return false;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		if (action == GOSSIP_ACTION_INFO_DEF+1)
		{
			creature->AI()->SetGUID(player->GetGUID(), START_TALK);
			player->CLOSE_GOSSIP_MENU();
		}
		return true;
	}

	struct npc_zul_drak_q12663_gorebagAI : public ScriptedAI
	{
		npc_zul_drak_q12663_gorebagAI(Creature* creature) : ScriptedAI(creature) { }
		
		void Reset()
		{
			talking = false;
			playerGUID = 0;
			timer = 0;
			stage = 0;
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (talking)
			{
				if (timer <= diff)
				{
					switch(stage)
					{
						case 1:
							me->MonsterSay(FLY_TALK_1, LANG_UNIVERSAL, playerGUID);
							break;
						case 2:
							me->MonsterSay(FLY_TALK_2, LANG_UNIVERSAL, playerGUID);
							break;
						case 3:
							me->MonsterSay(FLY_TALK_3, LANG_UNIVERSAL, playerGUID);
							break;
						case 4:
							me->MonsterSay(FLY_TALK_4, LANG_UNIVERSAL, playerGUID);
							break;
						case 5:
							me->MonsterSay(FLY_TALK_5, LANG_UNIVERSAL, playerGUID);
							break;
						case 6:
							me->MonsterSay(FLY_TALK_6, LANG_UNIVERSAL, playerGUID);
							break;
						case 7:
							me->MonsterSay(FLY_TALK_7, LANG_UNIVERSAL, playerGUID);
							break;
						case 8:
							me->MonsterSay(FLY_TALK_8, LANG_UNIVERSAL, playerGUID);
							break;
						case 9:
							me->MonsterSay(FLY_TALK_9, LANG_UNIVERSAL, playerGUID);
							break;
						case 10:
							if (Player* player = Unit::GetPlayer(*me, playerGUID))
								player->CastSpell(player, SPELL_BETRAYAL_PART, true);
							me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
							talking = false;
							break;
					}
					
					stage++;
					timer = 7500;
				}
				else timer -= diff;
			}
		}
		
		uint32 GetData(uint32 type)
		{
			return talking ? 1 : 0;
		}
		
		void SetGUID(uint64 guid, int32 type)
		{
			if (type = START_TALK)
			{
				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				playerGUID = guid;
				talking = true;
				timer = 1000;
				stage = 1;
			}
		}
		
		uint64 playerGUID;
		uint32 timer;
		uint32 stage;
		bool talking;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zul_drak_q12663_gorebagAI(creature);
	}
};

#define SPELL_CHARM 52244
#define SPELL_CHARM_TRIGGERED 52252

class npc_zul_drak_q12673_geist : public CreatureScript
{
public:
	npc_zul_drak_q12673_geist() : CreatureScript("npc_zul_drak_q12673_geist") { }
	
	struct npc_zul_drak_q12673_geistAI : public ScriptedAI
	{
		npc_zul_drak_q12673_geistAI(Creature* creature) : ScriptedAI(creature), timer(0) { }
		
		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_CHARM_TRIGGERED)
				if (!me->HasAura(SPELL_CHARM))
				{
					playerGUID = caster->GetGUID();
					timer = 3100;
				}
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (timer)
			{
				if (timer <= diff)
				{
					if (Unit* caster = Unit::GetUnit(*me, playerGUID))
						caster->CastSpell(caster, SPELL_BETRAYAL_PART, true);
					
					timer = 0;
				}
				else
					timer -= diff;
			}
			
			if (UpdateVictim())
				DoMeleeAttackIfReady();
		}
		
		uint64 playerGUID;
		uint32 timer;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zul_drak_q12673_geistAI(creature);
	}
};

#define SPELL_SABOTAGE_KILL_CREDIT 52346
#define NPC_SABOTAGE_KILL_BUNNY 28777

class npc_zul_drak_q12676_dynamite : public CreatureScript
{
public:
	npc_zul_drak_q12676_dynamite() : CreatureScript("npc_zul_drak_q12676_dynamite") { }
	
	struct npc_zul_drak_q12676_dynamiteAI : public ScriptedAI
	{
		npc_zul_drak_q12676_dynamiteAI(Creature* creature) : ScriptedAI(creature), timer(3000) { }
		
		void UpdateAI(uint32 const diff)
		{
			if (timer <= diff)
			{
				if (Creature* bunny = me->FindNearestCreature(NPC_SABOTAGE_KILL_BUNNY, 15.0f, true))
					if (!bunny->AI()->GetData(SPELL_SABOTAGE_KILL_CREDIT))
						if (Unit* summoner = me->ToTempSummon()->GetSummoner())
							summoner->CastSpell(summoner, SPELL_SABOTAGE_KILL_CREDIT);
				
				me->DespawnOrUnsummon();
			}
			else
				timer -= diff;
		}
		
		uint32 timer;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zul_drak_q12676_dynamiteAI(creature);
	}
};

class npc_zul_drak_q12676_bunny : public CreatureScript
{
public:
	npc_zul_drak_q12676_bunny() : CreatureScript("npc_zul_drak_q12676_bunny") { }
	
	struct npc_zul_drak_q12676_bunnyAI : public ScriptedAI
	{
		npc_zul_drak_q12676_bunnyAI(Creature* creature) : ScriptedAI(creature), cooldown(0) { }
		
		uint32 GetData(uint32 type)
		{
			if (type == SPELL_SABOTAGE_KILL_CREDIT)
			{
				if (!cooldown)
				{
					cooldown = 120000;
					me->CastSpell(me, 33802);//plamen
					return 0;
				}
				return cooldown;
			}
			
			return 0;
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (cooldown)
			{
				if (cooldown <= diff)
					cooldown = 0;
				else
					cooldown -= diff;
			}
		}
		
		uint32 cooldown;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zul_drak_q12676_bunnyAI(creature);
	}
};

#define QUEST_DISCLOSURE 12710
char* DISCLOSURE_TALK[16] = 
{
	"Ahh... there you are. The master told us you'd be arriving soon.", 
	"Please, follow me. There is much for you to see...", 
	"You should feel honored. You are the first of the master's prospects to be shown our operation. or", 
	"Ever since his arrival from Drak'Tharon, the master has spoken of the time you would be joining him here.", 
	"The things I show you now must never be spoken outside Voltarus. The world shall come to know our secrets soon enough!", 
	"Here lie our stores of blight crystal, without which our projects would be impossible.", 
	"I understand that you are to thank for the bulk of our supply.", 
	"These trolls are among those you exposed on the battlefield. Masterfully done, indeed....", 
	"We feel it best to position them here, where they might come to terms with their impending fate.", 
	"This is their destiny....", 
	"The blight slowly seeps into their bodies, gradually preparing them for their conversion.", 
	"This special preparation grants them unique powers far greater than they would otherwise know.", 
	"Soon, the master will grant them the dark gift, making them fit to serve the Lich King for eternity!", 
	"Stay for as long as you like. Glory in the fruits of your labor!", 
	"Your service has been invaluable in fulfilling the master's plan. You may forever grow in power....", 
	"Farewell."
};

class npc_zul_drak_q12710_malmortis : public CreatureScript
{
public:
	npc_zul_drak_q12710_malmortis() : CreatureScript("npc_zul_drak_q12710_malmortis") { }

	struct npc_zul_drak_q12710_malmortisAI : public npc_escortAI
	{
		npc_zul_drak_q12710_malmortisAI(Creature* creature) : npc_escortAI(creature)
		{
			AddWaypoint(0, 6256.3f, -1958.3f, 484.8f, 4000);
			AddWaypoint(1, 6256.3f, -1958.3f, 484.8f, 3000);
			AddWaypoint(2, 6231.9f, -1979.5f, 484.8f, 0);
			AddWaypoint(3, 6267.4f, -2036.0f, 484.8f, 0);
			AddWaypoint(4, 6157.6f, -2108.3f, 484.8f, 0);
			AddWaypoint(5, 6116.2f, -2045.3f, 484.8f, 7500);
			AddWaypoint(6, 6114.2f, -2053.0f, 484.8f, 0);
			AddWaypoint(17, 6083.3f, -2008.7f, 484.8f, 0);
			AddWaypoint(17, 6064.7f, -1982.9f, 484.8f, 0);
			AddWaypoint(17, 6069.1f, -1967.5f, 473.35f, 0);
			AddWaypoint(17, 6078.3f, -1959.9f, 473.35f, 0);
			AddWaypoint(17, 6084.9f, -1970.6f, 473.35f, 0);
			AddWaypoint(17, 6081.6f, -1986.4f, 461.32f, 0);
			AddWaypoint(17, 6087.7f, -2000.2f, 461.32f, 0);
			AddWaypoint(7, 6114.3f, -2016.8f, 461.32f, 7500);
			AddWaypoint(8, 6114.3f, -2016.8f, 461.32f, 7500);
			AddWaypoint(9, 6137.8f, -2037.3f, 461.32f, 5000);
			AddWaypoint(10, 6137.8f, -2037.3f, 461.32f, 7500);
			AddWaypoint(11, 6137.8f, -2037.3f, 461.32f, 7500);
			AddWaypoint(12, 6137.8f, -2037.3f, 461.32f, 7500);
			AddWaypoint(13, 6137.8f, -2037.3f, 461.32f, 3000);
			AddWaypoint(14, 6147.5f, -2034.6f, 458.3f, 0);
			AddWaypoint(17, 6160.2f, -2026.8f, 458.9f, 0);
			AddWaypoint(15, 6168.3f, -2022.3f, 455.1f, 5000);
			AddWaypoint(16, 6168.3f, -2022.3f, 455.1f, 0);
			Start(false, false, me->ToTempSummon() ? me->ToTempSummon()->GetSummonerGUID() : 0, NULL, false, false, false);
		}
		
		void WaypointReached(uint32 id)
		{
			if (id >= 17)
				return;
			
			if (id == 16)
			{
				if (Unit* summoner = me->ToTempSummon()->GetSummoner())
					summoner->CastSpell(summoner, SPELL_BETRAYAL_PART);
				me->DespawnOrUnsummon();
				return;
			}
			
			me->MonsterSay(DISCLOSURE_TALK[id], LANG_UNIVERSAL, 0);
		}
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zul_drak_q12710_malmortisAI(creature);
	}
};

const Position LichKingSpawn = {6145.2f, -2012.0f, 590.9f, 6.15f};
const Position LichKingMove = {6159.8f, -2015.0f, 590.9f, 6.08f};
const Position DrakuruCenter = {6174.8f, -2018.2f, 590.9f, 2.92f};
const Position TrollPosition[4] =
{
	{6166.2f, -2065.2f, 586.8f, 1.30f},
	{6222.9f, -2026.5f, 586.8f, 2.99f},
	{6184.3f, -1968.9f, 586.8f, 4.51f},
	{6127.5f, -2008.6f, 586.8f, 5.95f},
};

enum Drakuru_Stages
{
	STAGE_TALKING = 0,
	STAGE_FIGHTING = 1,
	STAGE_LICH_KING = 2,
	STAGE_DESPAWNING = 3,
};

enum Drakuru_Talks
{
	//prefight
	TALK_1 = 0,
	TALK_2 = 1,
	TALK_3 = 2,
	EMOTE_DISGUISE = 7,
	//fight
	TALK_4 = 3,
	//postfight
	TALK_5 = 4,
	TALK_6 = 5,
	TALK_7 = 6,
	TALK_LK_1 = 0,
	TALK_LK_2 = 1,
	TALK_LK_3 = 2,
	TALK_LK_4 = 3,
	TALK_LK_5 = 4,
	TALK_LK_6 = 5,
};

enum Drakuru_Events
{
	EVENT_TALKING_1 = 1,
	EVENT_TALKING_2 = 2,
	EVENT_TALKING_3 = 3,
	EVENT_TALKING_4 = 4,
	EVENT_TALKING_5 = 5,
	EVENT_TALKING_6 = 6,
	EVENT_SUMMON_TROLLS = 7,
	EVENT_START_FIGHT = 8,
	EVENT_FIGHT_TALK = 9,
	EVENT_SHADOW_BOLT = 10,
	EVENT_BLIGHT_CRYSTAL = 11,
	EVENT_BLIGHT_FOG = 12,
	EVENT_TALKING_LK_1 =13,
	EVENT_TALKING_LK_2 = 14,
	EVENT_TALKING_LK_3 = 15,
	EVENT_TALKING_LK_4 = 16,
	EVENT_TALKING_LK_5 = 17,
	EVENT_TALKING_LK_6 = 18,
	EVENT_TALKING_LK_7 = 19,
	EVENT_START_DESTROY = 20,
	EVENT_DESTROY_DRAKURU = 21,
	EVENT_HIDE_DRAKURU = 22,
	EVENT_DESPAWN = 23,
};

enum Drakuru_Spells
{
	SPELL_TELEPORT_VISUAL = 64446,
	SPELL_DISGUISE = 51966,
	SPELL_SHADOW_BOLT = 54113,
	SPELL_THROW_BLIGHT_CRYSTAL = 54087,
	SPELL_DESTROY_DRAKURU = 51825,
	SPELL_SPAWN_HEAD = 54253,
};

enum Drakuru_Miscs
{
	DATA_DRAKURU_SUMMONED = 471,
	GUID_BETRAYAL_PLAYER = 562,
	NPC_TRIGGER = 35651,
	NPC_DRAKURU_FIGHT = 28998,
	NPC_DRAKURU_TROLL = 28931,
	NPC_BETRAYAL_LICH_KING = 28498,
	GO_DRAKURUS_HEAD = 191458,
};

class npc_zul_drak_q12713_drakuru_start : public CreatureScript
{
public:
	npc_zul_drak_q12713_drakuru_start() : CreatureScript("npc_zul_drak_q12713_drakuru_start") { }
	
	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		if (action == 1)
		{
			if (creature->AI()->GetData(DATA_DRAKURU_SUMMONED) == DATA_DRAKURU_SUMMONED)
			{
				if (Creature* drakuru = creature->SummonCreature(NPC_DRAKURU_FIGHT, DrakuruCenter))
				{
					drakuru->AI()->SetGUID(player->GetGUID(), GUID_BETRAYAL_PLAYER);
					player->TeleportTo(drakuru->GetMapId(), DrakuruCenter.GetPositionX(), DrakuruCenter.GetPositionY(), DrakuruCenter.GetPositionZ(), DrakuruCenter.GetOrientation());
					player->CastSpell(player, SPELL_TELEPORT_VISUAL, true);
				}
			}
			else
				creature->MonsterSay("I don't have time now mon. Com back later.", LANG_UNIVERSAL, player->GetGUID());
			player->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
	
	struct npc_zul_drak_q12713_drakuru_startAI : public AggressorAI
	{
		npc_zul_drak_q12713_drakuru_startAI(Creature* creature) : AggressorAI(creature), drakuruBetreyalGUID(0) { }
		
		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_DRAKURU_FIGHT)
				drakuruBetreyalGUID = summon->GetGUID();
		}
		
		void SummonedCreatureDespawn(Creature* summon)
		{
			if (drakuruBetreyalGUID)
				if (summon->GetGUID() == drakuruBetreyalGUID)
					drakuruBetreyalGUID = 0;
		}
		
		uint32 GetData(uint32 type)
		{
			if (type == DATA_DRAKURU_SUMMONED)
				if (!drakuruBetreyalGUID)
					return DATA_DRAKURU_SUMMONED;
			
			return 0;
		}
		
		uint64 drakuruBetreyalGUID;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zul_drak_q12713_drakuru_startAI(creature);
	}
};

class npc_zul_drak_q12713_drakuru_boss : public CreatureScript
{
public:
	npc_zul_drak_q12713_drakuru_boss() : CreatureScript("npc_zul_drak_q12713_drakuru_boss") { }
	
	struct npc_zul_drak_q12713_drakuru_bossAI : public Scripted_NoMovementAI
	{
		npc_zul_drak_q12713_drakuru_bossAI(Creature* creature) : 	Scripted_NoMovementAI(creature), 
																	summons(creature), 
																	playerGUID(0), 
																	lichKingGUID(0), 
																	stage(STAGE_TALKING) { }
		
		void SetGUID(uint64 guid, int32 type)
		{
			if (type == GUID_BETRAYAL_PLAYER)
			{
				playerGUID = guid;
				events.Reset();
				events.ScheduleEvent(EVENT_TALKING_1, 4000);
			}
		}
		
		void JustSummoned(Creature* summon)
		{
			summons.Summon(summon);
			if (summon->GetEntry() == NPC_DRAKURU_TROLL)
				summon->CastSpell(summon, SPELL_TELEPORT_VISUAL, true);
		}
		
		void DamageTaken(Unit* /*doneBy*/, uint32& damage)
		{
			if (stage == STAGE_FIGHTING && me->HealthBelowPctDamaged(10, damage))
			{
				stage = STAGE_LICH_KING;
				summons.DespawnAll();
				events.Reset();
				events.ScheduleEvent(EVENT_TALKING_4, 100);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			}
			
			if (damage > me->GetHealth())
				damage = me->GetHealth() - 1;
		}
		
		void EnterEvadeMode()
		{
			if (stage == STAGE_FIGHTING)
				Evade();
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (playerGUID)
			{
				Player* player = Unit::GetPlayer(*me, playerGUID);
				if (!player || !player->isAlive())
					Evade();
				
				switch (stage)
				{
					//prefighting
					case STAGE_TALKING:
						events.Update(diff);
						
						while (uint32 eventId = events.ExecuteEvent())
						{
							switch (eventId)
							{
								case EVENT_TALKING_1:
									Talk(TALK_1);
									events.ScheduleEvent(EVENT_TALKING_2, 9000);
									events.ScheduleEvent(EVENT_SUMMON_TROLLS, 5000);
									break;
								case EVENT_TALKING_2:
									Talk(TALK_2);
									Talk(EMOTE_DISGUISE, playerGUID);
									events.ScheduleEvent(EVENT_TALKING_3, 9000);
									break;
								case EVENT_TALKING_3:
									player->RemoveAurasDueToSpell(SPELL_DISGUISE);
									Talk(TALK_3);
									events.ScheduleEvent(EVENT_START_FIGHT, 4000);
									break;
								
								case EVENT_SUMMON_TROLLS:
									for (int i = 0; i < 4; i++)
										me->SummonCreature(NPC_DRAKURU_TROLL, TrollPosition[i]);
									break;
								case EVENT_START_FIGHT:
									stage = STAGE_FIGHTING;
									AttackStart(player);
									events.Reset();
									events.ScheduleEvent(EVENT_SHADOW_BOLT, 100);
									events.ScheduleEvent(EVENT_BLIGHT_CRYSTAL, urand(5000, 20000));
									events.ScheduleEvent(EVENT_FIGHT_TALK, 15000);
									break;
							}
						}
						break;
					//fighting
					case STAGE_FIGHTING:
						if (!UpdateVictim())
							return;
						
						events.Update(diff);
						
						if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_STUNNED))
							return;
						
						while (uint32 eventId = events.ExecuteEvent())
						{
							switch (eventId)
							{
								case EVENT_SHADOW_BOLT:
									if (!SelectTarget(SELECT_TARGET_NEAREST, 0, me->GetMeleeReach()))
										DoCast(player, SPELL_SHADOW_BOLT);
									events.ScheduleEvent(EVENT_SHADOW_BOLT, 2500);
									break;
								case EVENT_BLIGHT_CRYSTAL:
									me->CastSpell(player, SPELL_THROW_BLIGHT_CRYSTAL);
									events.ScheduleEvent(EVENT_BLIGHT_CRYSTAL, urand(5000, 20000));
									break;
								case EVENT_FIGHT_TALK:
									Talk(TALK_4);
									events.ScheduleEvent(EVENT_FIGHT_TALK, 15000);
							}
						}
						
						DoMeleeAttackIfReady();
						break;
					//postfighting
					case STAGE_LICH_KING:
						events.Update(diff);
						
						while (uint32 eventId = events.ExecuteEvent())
						{
							switch (eventId)
							{
								case EVENT_TALKING_4:
									Talk(TALK_5);
									if (Creature* trigger = me->SummonCreature(NPC_TRIGGER, LichKingSpawn))
									{
										trigger->CastSpell(trigger, 51807, true);
										trigger->SetDisplayId(11686);
									}
									events.ScheduleEvent(EVENT_TALKING_5, 2000);
									break;
								case EVENT_TALKING_5:
									Talk(TALK_6);
									if (Creature* LK = me->SummonCreature(NPC_BETRAYAL_LICH_KING, LichKingSpawn))
									{
										lichKingGUID = LK->GetGUID();
										LK->GetMotionMaster()->MovePoint(0, LichKingMove);
									}
									events.ScheduleEvent(EVENT_TALKING_6, 7000);
									break;
								case EVENT_TALKING_6:
									Talk(TALK_7);
									events.ScheduleEvent(EVENT_TALKING_LK_1, 9000);
									break;
								
								case EVENT_TALKING_LK_1:
									TalkLK(TALK_LK_1);
									events.ScheduleEvent(EVENT_TALKING_LK_2, 6000);
									break;
								case EVENT_TALKING_LK_2:
									TalkLK(TALK_LK_2);
									events.ScheduleEvent(EVENT_START_DESTROY, 3000);
									events.ScheduleEvent(EVENT_TALKING_LK_3, 9000);
									break;
								case EVENT_TALKING_LK_3:
									TalkLK(TALK_LK_3);
									events.ScheduleEvent(EVENT_TALKING_LK_4, 6000);
									break;
								case EVENT_TALKING_LK_4:
									TalkLK(TALK_LK_4);
									events.ScheduleEvent(EVENT_TALKING_LK_5, 11000);
									break;
								case EVENT_TALKING_LK_5:
									TalkLK(TALK_LK_5);
									events.ScheduleEvent(EVENT_TALKING_LK_6, 4000);
									break;
								case EVENT_TALKING_LK_6:
									TalkLK(TALK_LK_6);
									events.ScheduleEvent(EVENT_TALKING_LK_7, 6000);
								case EVENT_TALKING_LK_7:
									if (Creature* LK = GetLKCreature())
										LK->GetMotionMaster()->MovePoint(0, LichKingSpawn);
									events.ScheduleEvent(EVENT_DESPAWN, 2000);
									break;
								
								case EVENT_START_DESTROY:
									if (Creature* LK = GetLKCreature())
										LK->CastSpell(LK, SPELL_DESTROY_DRAKURU);
									events.ScheduleEvent(EVENT_DESTROY_DRAKURU, 3000);
									break;
								case EVENT_DESTROY_DRAKURU:
									me->CastSpell(me, 54248, true);
									me->CastSpell(me, SPELL_SPAWN_HEAD, true);
									events.ScheduleEvent(EVENT_HIDE_DRAKURU, 500);
									break;
								case EVENT_HIDE_DRAKURU:
									me->SetVisible(false);
									break;
								case EVENT_DESPAWN:
									summons.DespawnAll();
									me->DespawnOrUnsummon();
									break;
							}
						}
						break;
				}
			}
		}
		
		void TalkLK(uint32 talk)
		{
			if (Creature* lichKing = GetLKCreature())
				lichKing->AI()->Talk(talk);
		}
		
		Creature* GetLKCreature()
		{
			return Unit::GetCreature(*me, lichKingGUID);
		}
		
		void Evade()
		{
			stage = STAGE_DESPAWNING;
			lichKingGUID = 0;
			playerGUID = 0;
			events.Reset();
			summons.DespawnAll();
			me->DespawnOrUnsummon();
		}
		
		SummonList summons;
		EventMap events;
		uint64 lichKingGUID;
		uint64 playerGUID;
		uint32 stage;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zul_drak_q12713_drakuru_bossAI(creature);
	}
};

enum QuestIds
{
	QUEST_BONES_AND_ARROWS_HORDE_ATT              = 13193,
	QUEST_JINXING_THE_WALLS_HORDE_ATT             = 13202,
	QUEST_SLAY_THEM_ALL_HORDE_ATT                 = 13180,
	QUEST_FUELING_THE_DEMOLISHERS_HORDE_ATT       = 13200,
	QUEST_HEALING_WITH_ROSES_HORDE_ATT            = 13201,
	QUEST_DEFEND_THE_SIEGE_HORDE_ATT              = 13223,

	QUEST_BONES_AND_ARROWS_HORDE_DEF              = 13199,
	QUEST_WARDING_THE_WALLS_HORDE_DEF             = 13192,
	QUEST_SLAY_THEM_ALL_HORDE_DEF                 = 13178,
	QUEST_FUELING_THE_DEMOLISHERS_HORDE_DEF       = 13191,
	QUEST_HEALING_WITH_ROSES_HORDE_DEF            = 13194,
	QUEST_TOPPLING_THE_TOWERS_HORDE_DEF           = 13539,
	QUEST_STOP_THE_SIEGE_HORDE_DEF                = 13185,

	QUEST_BONES_AND_ARROWS_ALLIANCE_ATT           = 13196,
	QUEST_WARDING_THE_WARRIORS_ALLIANCE_ATT       = 13198,
	QUEST_NO_MERCY_FOR_THE_MERCILESS_ALLIANCE_ATT = 13179,
	QUEST_DEFEND_THE_SIEGE_ALLIANCE_ATT           = 13222,
	QUEST_A_RARE_HERB_ALLIANCE_ATT                = 13195,

	QUEST_BONES_AND_ARROWS_ALLIANCE_DEF           = 13154,
	QUEST_WARDING_THE_WARRIORS_ALLIANCE_DEF       = 13153,
	QUEST_NO_MERCY_FOR_THE_MERCILESS_ALLIANCE_DEF = 13177,
	QUEST_SHOUTHERN_SABOTAGE_ALLIANCE_DEF         = 13538,
	QUEST_STOP_THE_SIEGE_ALLIANCE_DEF             = 13186,
	QUEST_A_RARE_HERB_ALLIANCE_DEF                = 13156,
};

class npc_wg_quest_giver : public CreatureScript
{
	public:
		npc_wg_quest_giver() : CreatureScript("npc_wg_quest_giver") { }

		bool OnGossipHello(Player* player, Creature* creature)
		{
			if (creature->isQuestGiver())
				player->PrepareQuestMenu(creature->GetGUID());

			if (creature->isQuestGiver())
			{
				QuestRelationBounds objectQR = sObjectMgr->GetCreatureQuestRelationBounds(creature->GetEntry());
				QuestRelationBounds objectQIR = sObjectMgr->GetCreatureQuestInvolvedRelationBounds(creature->GetEntry());

				QuestMenu& qm = player->PlayerTalkClass->GetQuestMenu();
				qm.ClearMenu();

				for (QuestRelations::const_iterator i = objectQIR.first; i != objectQIR.second; ++i)
				{
					uint32 questId = i->second;
					QuestStatus status = player->GetQuestStatus(questId);
					if (status == QUEST_STATUS_COMPLETE)
						qm.AddMenuItem(questId, 4);
					else if (status == QUEST_STATUS_INCOMPLETE)
						qm.AddMenuItem(questId, 4);
					//else if (status == QUEST_STATUS_AVAILABLE)
					//    qm.AddMenuItem(quest_id, 2);
				}

				for (QuestRelations::const_iterator i = objectQR.first; i != objectQR.second; ++i)
				{
					uint32 questId = i->second;
					Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
					if (!quest)
						continue;

					switch (questId)
					{
						// Horde attacker
						case QUEST_BONES_AND_ARROWS_HORDE_ATT:
						case QUEST_JINXING_THE_WALLS_HORDE_ATT:
						case QUEST_SLAY_THEM_ALL_HORDE_ATT:
						case QUEST_FUELING_THE_DEMOLISHERS_HORDE_ATT:
						case QUEST_HEALING_WITH_ROSES_HORDE_ATT:
						case QUEST_DEFEND_THE_SIEGE_HORDE_ATT:
							{
								QuestStatus status = player->GetQuestStatus(questId);

								if (quest->IsAutoComplete() && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 4);
								else if (status == QUEST_STATUS_NONE && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 2);
							}
							break;
						// Horde defender
						case QUEST_BONES_AND_ARROWS_HORDE_DEF:
						case QUEST_WARDING_THE_WALLS_HORDE_DEF:
						case QUEST_SLAY_THEM_ALL_HORDE_DEF:
						case QUEST_FUELING_THE_DEMOLISHERS_HORDE_DEF:
						case QUEST_HEALING_WITH_ROSES_HORDE_DEF:
						case QUEST_TOPPLING_THE_TOWERS_HORDE_DEF:
						case QUEST_STOP_THE_SIEGE_HORDE_DEF:
							{
								QuestStatus status = player->GetQuestStatus(questId);

								if (quest->IsAutoComplete() && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 4);
								else if (status == QUEST_STATUS_NONE && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 2);
							}
							break;
						// Alliance attacker
						case QUEST_BONES_AND_ARROWS_ALLIANCE_ATT:
						case QUEST_WARDING_THE_WARRIORS_ALLIANCE_ATT:
						case QUEST_NO_MERCY_FOR_THE_MERCILESS_ALLIANCE_ATT:
						case QUEST_DEFEND_THE_SIEGE_ALLIANCE_ATT:
						case QUEST_A_RARE_HERB_ALLIANCE_ATT:
							{
								QuestStatus status = player->GetQuestStatus(questId);

								if (quest->IsAutoComplete() && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 4);
								else if (status == QUEST_STATUS_NONE && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 2);
							}
							break;
						// Alliance defender
						case QUEST_BONES_AND_ARROWS_ALLIANCE_DEF:
						case QUEST_WARDING_THE_WARRIORS_ALLIANCE_DEF:
						case QUEST_NO_MERCY_FOR_THE_MERCILESS_ALLIANCE_DEF:
						case QUEST_SHOUTHERN_SABOTAGE_ALLIANCE_DEF:
						case QUEST_STOP_THE_SIEGE_ALLIANCE_DEF:
						case QUEST_A_RARE_HERB_ALLIANCE_DEF:
							{
								QuestStatus status = player->GetQuestStatus(questId);

								if (quest->IsAutoComplete() && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 4);
								else if (status == QUEST_STATUS_NONE && player->CanTakeQuest(quest, false))
									qm.AddMenuItem(questId, 2);
							}
							break;
						default:
							QuestStatus status = player->GetQuestStatus(questId);

							if (quest->IsAutoComplete() && player->CanTakeQuest(quest, false))
								qm.AddMenuItem(questId, 4);
							else if (status == QUEST_STATUS_NONE && player->CanTakeQuest(quest, false))
								qm.AddMenuItem(questId, 2);
							break;
					}
				}
			}
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
			return true;
		}
};

#define ITEM_ICEMAW_BEAR_FLANK 42733
#define QUEST_JORMUTTAR_IS_SO_FAT 13011

class spell_carve_bear_flank : public SpellScriptLoader
{
	public:
		spell_carve_bear_flank() : SpellScriptLoader("spell_carve_bear_flank") {}

		class spell_carve_bear_flank_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_carve_bear_flank_SpellScript);
			void HandleScript(SpellEffIndex effIndex)
			{
				if (Unit* target = GetCaster())
					if (target->GetTypeId() == TYPEID_PLAYER)
						if (Player* player = target->ToPlayer())
							if (player->GetQuestStatus(QUEST_JORMUTTAR_IS_SO_FAT) == QUEST_STATUS_INCOMPLETE)
							{
								ItemPosCountVec dest;
								if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_ICEMAW_BEAR_FLANK, 1) == EQUIP_ERR_OK)
								{
									Item* item = player->StoreNewItem(dest, ITEM_ICEMAW_BEAR_FLANK, true);
									player->SendNewItem(item, 1, true, false);
								}
							}
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_carve_bear_flank_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_carve_bear_flank_SpellScript();
		}
};

#define ITEM_STORMFORGED_EYE 42423
#define QUEST_FORGING_A_HEAD 12985

class spell_salvage_corpse : public SpellScriptLoader
{
	public:
		spell_salvage_corpse() : SpellScriptLoader("spell_salvage_corpse") {}

		class spell_salvage_corpse_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_salvage_corpse_SpellScript);
			void HandleScript(SpellEffIndex effIndex)
			{
				if (Unit* target = GetCaster())
					if (target->GetTypeId() == TYPEID_PLAYER)
						if (Player* player = target->ToPlayer())
							if (player->GetQuestStatus(QUEST_FORGING_A_HEAD) == QUEST_STATUS_INCOMPLETE)
							{
								ItemPosCountVec dest;
								if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_STORMFORGED_EYE, 1) == EQUIP_ERR_OK)
								{
									Item* item = player->StoreNewItem(dest, ITEM_STORMFORGED_EYE, true);
									player->SendNewItem(item, 1, true, false);
								}
							}
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_salvage_corpse_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_salvage_corpse_SpellScript();
		}
};

#define GOSSIP_KROLMIR_TEXT_YES "I am ready to be shown the fate of Krolmir."

enum
{
	FACTION_SONS_OF_HODIR = 1119,
	GOSSIP_KROLMIR_MENU_YES = 13749,
	GOSSIP_KROLMIR_MENU_NO = 13748,
	POINT_END_ESCORT = 16,
	GUID_KROLMIR_PLAYER = 325,
	SPELL_KROLMIR_COMPLETE = 56545,
	NPC_JOKKUM_VEHICLE = 30331,
	NPC_VERANUS = 30393,
	NPC_KROLMIR = 2000170,
};

class npc_king_jokkum : public CreatureScript
{
public:
	npc_king_jokkum() : CreatureScript("npc_king_jokkum") { }
	
	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		if (action == 1)
		{
			player->PlayerTalkClass->GetGossipMenu().ClearMenu();
			ReputationRank rank = player->GetReputationRank(FACTION_SONS_OF_HODIR);
			if (rank <= REP_NEUTRAL)
				player->SEND_GOSSIP_MENU(GOSSIP_KROLMIR_MENU_NO, creature->GetGUID());
			else
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_KROLMIR_TEXT_YES, GOSSIP_SENDER_MAIN, 2);
				player->SEND_GOSSIP_MENU(GOSSIP_KROLMIR_MENU_YES, creature->GetGUID());
			}
		}
		else if (action == 2)
		{
			Creature* summoned = creature->SummonCreature(NPC_JOKKUM_VEHICLE, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation());
			summoned->AI()->SetGUID(player->GetGUID(), GUID_KROLMIR_PLAYER);
			player->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
};

enum JokkumTalks
{
	TALK_JOKKUM_1 = 0,
	TALK_JOKKUM_2 = 1,
	TALK_JOKKUM_3 = 2,
	TALK_JOKKUM_4 = 3,
	TALK_JOKKUM_5 = 4,
	TALK_JOKKUM_6 = 5,
	TALK_JOKKUM_7 = 6,
	TALK_JOKKUM_8 = 7,
	TALK_JOKKUM_9 = 8,
	
	TALK_THORIM_1 = 0,
	TALK_THORIM_2 = 1,
	TALK_THORIM_3 = 2,
	TALK_THORIM_4 = 3,
};

class npc_king_jokkum_vehicle : public CreatureScript
{
public:
	npc_king_jokkum_vehicle() : CreatureScript("npc_king_jokkum_vehicle") { }
	
	struct npc_king_jokkum_vehicleAI : public npc_escortAI
	{
		npc_king_jokkum_vehicleAI(Creature* creature) : npc_escortAI(creature)
		{
			AddWaypoint(0, 7308.760742f, -2818.902344f, 797.898438f, 0);
			AddWaypoint(1, 7347.006348f, -2862.900879f, 802.718994f, 0);
			AddWaypoint(2, 7348.576660f, -2894.130127f, 819.697754f, 0);
			AddWaypoint(3, 7350.540527f, -2927.436035f, 825.549194f, 0);
			AddWaypoint(4, 7358.812012f, -2974.024658f, 846.078552f, 0);
			AddWaypoint(5, 7377.750977f, -3022.317871f, 843.369019f, 0);
			AddWaypoint(6, 7410.737305f, -3048.541748f, 837.475647f, 0);
			AddWaypoint(7, 7540.809082f, -3135.807373f, 837.626648f, 0);
			AddWaypoint(8, 7570.500488f, -3151.289063f, 846.020203f, 0);
			AddWaypoint(9, 7626.200684f, -3191.958984f, 855.159119f, 0);
			AddWaypoint(10, 7656.226563f, -3201.482666f, 863.616516f, 0);
			AddWaypoint(11, 7687.619629f, -3212.385986f, 867.538696f, 0);
			AddWaypoint(12, 7727.691895f, -3234.072510f, 861.854736f, 0);
			AddWaypoint(13, 7762.945801f, -3225.895020f, 864.295349f, 0);
			AddWaypoint(14, 7817.820801f, -3225.097900f, 857.928040f, 0);
			AddWaypoint(15, 7824.477539f, -3228.418701f, 857.472473f, 0);
			AddWaypoint(POINT_END_ESCORT, 7855.300781f, -3241.786621f, 851.605444f, 2000);
			talking = false;
			stage = 2;
			timer = 2000;
		}
		
		void SetGUID(uint64 guid, int32 type)
		{
			if (type == GUID_KROLMIR_PLAYER)
				if (Player* player = Unit::GetPlayer(*me, guid))
				{
					playerGUID = guid;
					player->EnterVehicle(me);
					SetDespawnAtEnd(false);
					Talk(TALK_JOKKUM_1, playerGUID);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
					Start(false, true);
				}
		}
		
		void WaypointReached(uint32 pointId)
		{
			if (pointId == POINT_END_ESCORT)
			{
				SetEscortPaused(true);
				if (Player* player = Player::GetPlayer(*me, playerGUID))
				{
					player->ExitVehicle();
					talking = true;
				}
			}
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (talking)
			{
				if (timer <= diff)
				{
					switch (stage)
					{
						case 2:
							Talk(TALK_JOKKUM_2, playerGUID);
							timer = 1000;
							break;
						case 3:
							if (Unit* veranus = me->SummonCreature(NPC_VERANUS, 7943.0f, -3250.8f, 900.0f, 3.14f, TEMPSUMMON_TIMED_DESPAWN, 17000))
								if (Unit* thorim = veranus->GetVehicleKit()->GetPassenger(0))
								{
									thorimGUID = thorim->GetGUID();
									veranus->GetMotionMaster()->MovePoint(0, 7898.375f, -3262.13f, 865.3f);
								}
							timer = 6000;
							break;
						case 4:
							if (Unit* thorim = Unit::GetUnit(*me, thorimGUID))
								if (Unit* veranus = thorim->GetVehicleBase())
								{
									thorim->ExitVehicle();
									thorim->GetMotionMaster()->MoveJump(7898.375f, -3262.13f, 851.3f, 10.0f, 5.0f, 0);
									veranus->GetMotionMaster()->MovePoint(0, 7867.0f, -3268.0f, 900.0f);
								}
							timer = 2000;
							break;
						case 5:
							Unit::GetCreature(*me, thorimGUID)->AI()->Talk(TALK_THORIM_1, playerGUID);
							timer = 5000;
							break;
						case 6:
							Talk(TALK_JOKKUM_3, playerGUID);
							timer = 6000;
							break;
						case 7:
							Unit::GetCreature(*me, thorimGUID)->AI()->Talk(TALK_THORIM_2, playerGUID);
							timer = 6000;
							break;
						case 8:
							Unit::GetCreature(*me, thorimGUID)->AI()->Talk(TALK_THORIM_3, playerGUID);
							timer = 5000;
							break;
						case 9:
							Talk(TALK_JOKKUM_4, playerGUID);
							timer = 6000;
							break;
						case 10:
							Talk(TALK_JOKKUM_5, playerGUID);
							timer = 6000;
							break;
						case 11:
							Talk(TALK_JOKKUM_6, playerGUID);
							timer = 5000;
							break;
						case 12:
							Talk(TALK_JOKKUM_7, playerGUID);
							me->SummonCreature(NPC_KROLMIR, 7878.0f, -3249.0f, 847.0f, 5.7f, TEMPSUMMON_TIMED_DESPAWN, 143000);
							timer = 7000;
							break;
						case 13:
							Unit::GetCreature(*me, thorimGUID)->AI()->Talk(TALK_THORIM_4, playerGUID);
							timer = 6000;
							break;
						case 14:
							Talk(TALK_JOKKUM_8, playerGUID);
							timer = 5000;
							break;
						case 15:
							Talk(TALK_JOKKUM_9, playerGUID);
							timer = 5000;
							break;
						case 16:
							if (Player* player = Player::GetPlayer(*me, playerGUID))
								player->CastSpell(player, SPELL_KROLMIR_COMPLETE, true);
							me->DespawnOrUnsummon(1000);
							Unit::GetCreature(*me, thorimGUID)->DespawnOrUnsummon(120000);
							talking = false;
							timer = 2000;
							break;
					}
					
					stage++;
				}
				else
					timer -= diff;
			}
			
			npc_escortAI::UpdateAI(diff);
		}
		
		uint64 playerGUID;
		uint64 thorimGUID;
		uint32 stage;
		uint32 timer;
		bool talking;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_king_jokkum_vehicleAI(creature);
	}
};

enum Misc_squire
{
	STATE_BANK      = 0x1,
	STATE_SHOP      = 0x2,
	STATE_MAIL      = 0x4,
	
	GOSSIP_ACTION_MAIL = 3,
	
	ACHI_PONY_UP                = 3736,
	
	SPELL_CHECK_MOUNT           = 67039,
	SPELL_CHECK_TIRED           = 67334,
	SPELL_SQUIRE_BANK_ERRAND    = 67368,
	SPELL_SQUIRE_POSTMAN        = 67376,
	SPELL_SQUIRE_SHOP           = 67377,
	SPELL_SQUIRE_TIRED          = 67401
};

enum Quests_squire
{
	QUEST_CHAMP_ORGRIMMAR       = 13726,
	QUEST_CHAMP_UNDERCITY       = 13729,
	QUEST_CHAMP_SENJIN          = 13727,
	QUEST_CHAMP_SILVERMOON      = 13731,
	QUEST_CHAMP_THUNDERBLUFF    = 13728,
	QUEST_CHAMP_STORMWIND       = 13699,
	QUEST_CHAMP_IRONFORGE       = 13713,
	QUEST_CHAMP_GNOMEREGAN      = 13723,
	QUEST_CHAMP_DARNASSUS       = 13725,
	QUEST_CHAMP_EXODAR          = 13724
};

enum Pennants_squire
{
	SPELL_DARNASSUS_PENNANT     = 63443,
	SPELL_EXODAR_PENNANT        = 63439,
	SPELL_GNOMEREGAN_PENNANT    = 63442,
	SPELL_IRONFORGE_PENNANT     = 63440,
	SPELL_ORGRIMMAR_PENNANT     = 63444,
	SPELL_SENJIN_PENNANT        = 63446,
	SPELL_SILVERMOON_PENNANT    = 63438,
	SPELL_STORMWIND_PENNANT     = 62727,
	SPELL_THUNDERBLUFF_PENNANT  = 63445,
	SPELL_UNDERCITY_PENNANT     = 63441
};

class npc_argent_squire : public CreatureScript
{
public:
	npc_argent_squire() : CreatureScript("npc_argent_squire") { }

	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
		// Argent Pony Bridle options
		if (pPlayer->GetAchievementMgr().HasAchieved(ACHI_PONY_UP))
			if (!pCreature->HasAura(SPELL_SQUIRE_TIRED))
			{
				uint8 uiBuff = (STATE_BANK | STATE_SHOP | STATE_MAIL);
				if (pCreature->HasAura(SPELL_SQUIRE_BANK_ERRAND))
					uiBuff = STATE_BANK;
				if (pCreature->HasAura(SPELL_SQUIRE_SHOP))
					uiBuff = STATE_SHOP;
				if (pCreature->HasAura(SPELL_SQUIRE_POSTMAN))
					uiBuff = STATE_MAIL;

				if (uiBuff & STATE_BANK)
					pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Visit a bank.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_BANK);
				if (uiBuff & STATE_SHOP)
					pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Visit a trader.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
				if (uiBuff & STATE_MAIL)
					pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Visit a mailbox.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_MAIL);
			}

		// Horde
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_SENJIN))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Darkspear Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_SENJIN_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_UNDERCITY))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Forsaken Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_UNDERCITY_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_ORGRIMMAR))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Orgrimmar Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_ORGRIMMAR_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_SILVERMOON))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Silvermoon Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_SILVERMOON_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_THUNDERBLUFF))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thunder Bluff Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_THUNDERBLUFF_PENNANT);

		//Alliance
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_DARNASSUS))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Darnassus Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_DARNASSUS_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_EXODAR))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Exodar Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_EXODAR_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_GNOMEREGAN))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gnomeregan Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_GNOMEREGAN_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_IRONFORGE))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ironforge Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_IRONFORGE_PENNANT);
		if (pPlayer->GetQuestRewardStatus(QUEST_CHAMP_STORMWIND))
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stormwind Champion's Pennant", GOSSIP_SENDER_MAIN, SPELL_STORMWIND_PENNANT);

		pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
	{
		pPlayer->PlayerTalkClass->ClearMenus();
		if (uiAction >= 1000) // remove old pennant aura
			pCreature->AI()->SetData(0, 0);
		switch (uiAction)
		{
			case GOSSIP_ACTION_BANK:
				pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_BANKER);
				pPlayer->GetSession()->SendShowBank(pCreature->GetGUID());
				if (!pCreature->HasAura(SPELL_SQUIRE_BANK_ERRAND))
					pCreature->AddAura(SPELL_SQUIRE_BANK_ERRAND, pCreature);
				if (!pPlayer->HasAura(SPELL_CHECK_TIRED))
					pPlayer->AddAura(SPELL_CHECK_TIRED, pPlayer);
				break;
			case GOSSIP_ACTION_TRADE:
				pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR);
				pPlayer->GetSession()->SendListInventory(pCreature->GetGUID());
				if (!pCreature->HasAura(SPELL_SQUIRE_SHOP))
					pCreature->AddAura(SPELL_SQUIRE_SHOP, pCreature);
				if (!pPlayer->HasAura(SPELL_CHECK_TIRED))
					pPlayer->AddAura(SPELL_CHECK_TIRED, pPlayer);
				break;
			case GOSSIP_ACTION_MAIL:
				pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_MAILBOX);
				pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_VENDOR);
				if (!pCreature->HasAura(SPELL_SQUIRE_POSTMAN))
					pCreature->AddAura(SPELL_SQUIRE_POSTMAN, pCreature);
				if (!pPlayer->HasAura(SPELL_CHECK_TIRED))
					pPlayer->AddAura(SPELL_CHECK_TIRED, pPlayer);
				break;

			case SPELL_SENJIN_PENNANT:
			case SPELL_UNDERCITY_PENNANT:
			case SPELL_ORGRIMMAR_PENNANT:
			case SPELL_SILVERMOON_PENNANT:
			case SPELL_THUNDERBLUFF_PENNANT:
			case SPELL_DARNASSUS_PENNANT:
			case SPELL_EXODAR_PENNANT:
			case SPELL_GNOMEREGAN_PENNANT:
			case SPELL_IRONFORGE_PENNANT:
			case SPELL_STORMWIND_PENNANT:
				pCreature->AI()->SetData(1, uiAction);
				break;
		}
		pPlayer->CLOSE_GOSSIP_MENU();
		return true;
	}

	struct npc_argent_squireAI : public ScriptedAI
	{
		npc_argent_squireAI(Creature* pCreature) : ScriptedAI(pCreature)
		{
			m_current_pennant = 0;
			check_timer = 1000;
		}

		uint32 m_current_pennant;
		uint32 check_timer;

		void UpdateAI(const uint32 uiDiff)
		{
			// have to delay the check otherwise it wont work
			if (check_timer && (check_timer <= uiDiff))
			{
				me->AddAura(SPELL_CHECK_MOUNT, me);
				if (Aura* tired = me->GetOwner()->GetAura(SPELL_CHECK_TIRED))
				{
					int32 duration = tired->GetDuration();
					tired = me->AddAura(SPELL_SQUIRE_TIRED, me);
					tired->SetDuration(duration);
				}
				check_timer = 0;
			}
			else check_timer -= uiDiff;

			if (me->HasAura(SPELL_SQUIRE_TIRED) && me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_BANKER | UNIT_NPC_FLAG_MAILBOX | UNIT_NPC_FLAG_VENDOR))
			{
				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_BANKER | UNIT_NPC_FLAG_MAILBOX | UNIT_NPC_FLAG_VENDOR);
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			}
		}
		
		void SetData(uint32 add, uint32 spell)
		{
			if (add)
			{
				me->AddAura(spell, me);
				m_current_pennant = spell;
			}
			else if (m_current_pennant)
			{
				me->RemoveAura(m_current_pennant);
				m_current_pennant = 0;
			}
		}
	};
	CreatureAI *GetAI(Creature *creature) const

	{
		return new npc_argent_squireAI(creature);
	}
};

#define GO_IRON_SCRAP 192124
#define GO_FROZEN_IRON_SCRAP 192127

class spell_throw_ice : public SpellScriptLoader
{
	public:
		spell_throw_ice() : SpellScriptLoader("spell_throw_ice") {}

		class spell_throw_ice_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_throw_ice_SpellScript);
			
			void HandleScript(SpellEffIndex effIndex)
			{
				if (GameObject* go = GetCaster()->FindNearestGameObject(GO_IRON_SCRAP, 25.0f))
				{
					GetCaster()->SummonGameObject(GO_FROZEN_IRON_SCRAP, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 0)->SetSpellId(1);
					go->SetLootState(GO_JUST_DEACTIVATED);
				}
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_throw_ice_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_throw_ice_SpellScript();
		}
};

class KillJormungarEvent : public BasicEvent
{
	public:
		KillJormungarEvent(Creature& owner, Unit* summoner) : _owner(owner), _summoner(summoner) { }

		bool Execute(uint64 /*eventTime*/, uint32 /*updateTime*/)
		{
			_owner.Kill(_summoner);
			return true;
		}

	private:
		Creature& _owner;
		Unit* _summoner;
};

class npc_q13046_jormungar : public CreatureScript
{
public:
	npc_q13046_jormungar() : CreatureScript("npc_q13046_jormungar") { }
	
	enum Feeding_Arngrim
	{
		SPELL_DISEMBODIED = 56727,
		SPELL_FEEDING_KILL_CREDIT = 56731,
		NPC_ARNGRIM = 30425,
	};
	
	struct npc_q13046_jormungarAI : public AggressorAI
	{
		npc_q13046_jormungarAI(Creature* creature) : AggressorAI(creature), spawned(false), playerGUID(0) { }
		
		void JustDied(Unit* killer)
		{
			if (killer->ToCreature())
				if (killer->GetEntry() == NPC_ARNGRIM)
					if (Player* player = Unit::GetPlayer(*me, playerGUID))
						player->CastSpell(player, SPELL_FEEDING_KILL_CREDIT, true);
			spawned = false;
			playerGUID = 0;
		}
		
		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DISEMBODIED && me->HealthBelowPct(15) && !spawned)
			{
				spawned = true;
				playerGUID = caster->GetGUID();
				Creature* arngrim = me->SummonCreature(NPC_ARNGRIM, 
								me->GetPositionX() + urand(0, 40) - 20, 
								me->GetPositionY() + urand(0, 40) - 20, 
								me->GetPositionZ(), 
								me->GetOrientation(), 
								TEMPSUMMON_TIMED_DESPAWN, 
								5000);
				arngrim->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
				arngrim->m_Events.AddEvent(new KillJormungarEvent(*arngrim, me), arngrim->m_Events.CalculateTime(3000));
			}
		}
		
		uint64 playerGUID;
		bool spawned;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_q13046_jormungarAI(creature);
	}
};

enum eQuestNPC
{
	NPC_FALLEN_WORG_DESPAWN_TIME = 300000,
	NPC_INFILTRATOR_DESPAWN_TIME = 60000,
	NPC_INFILTRATOR = 30222,
};

class npc_q12994_worg : public CreatureScript
{
	public:
		
		npc_q12994_worg() : CreatureScript("npc_q12994_worg") { }
		
		struct npc_q12994_worgAI : public npc_escortAI
		{
			npc_q12994_worgAI(Creature* creature) : npc_escortAI(creature), infiltratorGUID(0)
			{
				StartRandomEscort();
			}
			
			void Reset()
			{
				if (me->ToTempSummon())
					me->ToTempSummon()->SetTempSummonType(TEMPSUMMON_MANUAL_DESPAWN);
				me->DespawnOrUnsummon(NPC_FALLEN_WORG_DESPAWN_TIME);
			}
			
			void StartRandomEscort()
			{
				int pathId = urand(0, 2);
				
				switch (pathId)
				{
					case 0:
						AddWaypoint(0, 7120.0f, -2209.0f, 759.0f, 1000);
						AddWaypoint(1, 7134.0f, -2239.0f, 759.0f, 1000);
						AddWaypoint(2, 7150.0f, -2274.0f, 767.0f, 1000);
						break;
					case 1:
						AddWaypoint(0, 7206.0f, -2282.0f, 755.0f, 1000);
						AddWaypoint(1, 7236.0f, -2223.0f, 760.0f, 1000);
						AddWaypoint(2, 7232.0f, -2290.0f, 753.0f, 1000);
						break;
					default:
						AddWaypoint(0, 7204.0f, -2207.0f, 762.0f, 1000);
						AddWaypoint(1, 7148.0f, -2210.0f, 759.0f, 1000);
						AddWaypoint(2, 7163.0f, -2172.0f, 760.0f, 1000);
						break;
				}
				Start(false, false, me->ToTempSummon() ? me->ToTempSummon()->GetSummonerGUID() : 0);
				SetDespawnAtEnd(false);
			}
			
			void WaypointReached(uint32 pointId)
			{
				if (pointId == 2)
					infiltratorGUID = me->SummonCreature(NPC_INFILTRATOR, me->GetPositionX() + urand(0, 6) - 3, me->GetPositionY() + urand(0, 6) - 3, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, NPC_INFILTRATOR_DESPAWN_TIME)->GetGUID();
			}
			
			void UpdateAI(const uint32 uiDiff)
			{
				if (infiltratorGUID)
				{
					if (Creature* infiltrator = Unit::GetCreature(*me, infiltratorGUID))
					{
						if(infiltrator->isDead())
						{
							infiltrator->DespawnOrUnsummon(5000);
							me->DespawnOrUnsummon(1000);
						}
					}
					else
						me->DespawnOrUnsummon(1000);
				}
				
				me->setFaction(FACTION_FRIENDLY);
				
				npc_escortAI::UpdateAI(uiDiff);
			}
			
			uint64 infiltratorGUID;
		};
	
		CreatureAI* GetAI(Creature* creature) const
		{
		return new npc_q12994_worgAI(creature);
		}
};

class npc_q13003_wyrm : public CreatureScript
{
	public:
		
		npc_q13003_wyrm() : CreatureScript("npc_q13003_wyrm") { }
		
		enum Thrusting_Hodir_Spear
		{
			EVENT_REDUCE_GRIP = 1,
			EVENT_CLAWS_WARN = 2,
			EVENT_CLAWS_DAMAGE = 3,
			
			SPELL_GRIP = 56689,
			SPELL_GRAB_ON = 60533,
			SPELL_DODGE_CLAWS = 56704,
			SPELL_PERIODIC_CLAW = 60689,
			SPELL_JAWS_OF_DEATH = 56692,
			SPELL_THRUST_SPEAR = 56690,
			SPELL_MIGHTY_SPEAR_THRUST = 60586,
			SPELL_PRY_JAWS_OPEN = 56706,
			SPELL_FATAL_STRIKE = 60587,
			
			NPC_WILD_WYRM = 30275,
			
			EMOTE_CLAW_WARNING = 0,
			EMOTE_CLAW_DODGE = 1,
			EMOTE_CHANGE_PHASE = 2,
			EMOTE_FATAL_MISS = 3,
		};
		
		struct npc_q13003_wyrmAI : public NullCreatureAI
		{
			npc_q13003_wyrmAI(Creature* creature) : NullCreatureAI(creature) { }
			
			void EnterEvadeMode()
			{
				if (secondPhase)
					events.Reset();
			}
			
			void PassengerBoarded(Unit* who, int8 seatId, bool apply)
			{
				if (who->GetTypeId() == TYPEID_PLAYER)
				{
					if (apply)
					{
						me->GetMotionMaster()->MovePoint(1, 7324.0f, -2474.0f, 1000.0f);
						playerGUID = who->GetGUID();
						secondPhase = false;
						reduceGripCD = 2000;
						events.Reset();
						me->CastSpell(me, SPELL_GRIP, true);
						me->CastSpell(me, SPELL_PERIODIC_CLAW, true);
						events.ScheduleEvent(EVENT_CLAWS_WARN, 3000);
						events.ScheduleEvent(EVENT_REDUCE_GRIP, reduceGripCD);
						if (Aura* grip = me->GetAura(SPELL_GRIP))
							grip->SetStackAmount(60);
					}
					else
					{
						if (!secondPhase || seatId == 1)
						{
							who->RemoveAurasDueToSpell(SPELL_JAWS_OF_DEATH);
							EnterEvadeMode();
						}
						events.Reset();
						who->RemoveAurasDueToSpell(SPELL_JAWS_OF_DEATH);
						me->RemoveAurasDueToSpell(SPELL_GRIP);
						me->RemoveAurasDueToSpell(SPELL_PERIODIC_CLAW);
					}
				}
			}
			
			void SpellHit(Unit* caster, SpellInfo const* spell)
			{
				if (Aura* grip = me->GetAura(SPELL_GRIP))
					switch (spell->Id)
					{
						case SPELL_GRAB_ON:
							grip->ModStackAmount(10);
							return;
						case SPELL_THRUST_SPEAR:
							grip->ModStackAmount(-5);
							return;
						case SPELL_MIGHTY_SPEAR_THRUST:
							grip->ModStackAmount(-15);
							return;
					}
				
				if (Player* player = Unit::GetPlayer(*me, playerGUID))
					if (spell->Id == SPELL_FATAL_STRIKE)
						if (Aura* pryJaws = caster->GetAura(SPELL_PRY_JAWS_OPEN))
						{
							if (urand(1, 20) <= pryJaws->GetStackAmount())
								player->Kill(me);
							else
							{
								Talk(EMOTE_FATAL_MISS, playerGUID);
								pryJaws->Remove();
							}
						}
			}
			
			void MovementInform(uint32 type, uint32 point)
			{
				if (type != POINT_MOTION_TYPE)
					return;
				
				switch (point)
				{
					case 1:
						reduceGripCD = 750;
						break;
				}
			}
			
			void UpdateAI(uint32 const diff)
			{
				if (me->HealthBelowPct(30))
				{
					if (!secondPhase)
					{
						secondPhase = true;
						if (Player* player = Unit::GetPlayer(*me, playerGUID))
						{
							player->ExitVehicle();
							if (Unit* jaws = me->GetVehicleKit()->GetPassenger(1))
							{
								me->CombatStart(player);
								jaws->SetHealth(me->GetHealth());
								player->EnterVehicle(jaws);
								me->AddAura(SPELL_JAWS_OF_DEATH, player);
								Talk(EMOTE_CHANGE_PHASE, playerGUID);
							}
						}
					}
				}
				else
				{
					Aura* grip = me->GetAura(SPELL_GRIP);
					if (!grip || grip->GetStackAmount() == 1)
						if (Player* player = Unit::GetPlayer(*me, playerGUID))
							player->ExitVehicle();
					
					events.Update(diff);
					
					while (uint32 eventId = events.ExecuteEvent())
						switch (eventId)
						{
							case EVENT_REDUCE_GRIP:
								if (Aura* grip = me->GetAura(SPELL_GRIP))
									grip->ModStackAmount(-1);
								events.ScheduleEvent(EVENT_REDUCE_GRIP, reduceGripCD);
								break;
							case EVENT_CLAWS_WARN:
								Talk(EMOTE_CLAW_WARNING, playerGUID);
								events.ScheduleEvent(EVENT_CLAWS_WARN, 5000);
								events.ScheduleEvent(EVENT_CLAWS_DAMAGE, 2000);
								break;
							case EVENT_CLAWS_DAMAGE:
								if (me->HasAura(SPELL_DODGE_CLAWS))
									Talk(EMOTE_CLAW_DODGE, playerGUID);
								break;
						}
				}
			}
			
			EventMap events;
			uint64 playerGUID;
			uint32 reduceGripCD;
			bool secondPhase;
		};
		
		struct npc_q13003_jawsAI : public NullCreatureAI
		{
			npc_q13003_jawsAI(Creature* creature) : NullCreatureAI(creature) { }
			
			void SpellHit(Unit* caster, SpellInfo const* spell)
			{
				me->GetVehicleBase()->ToCreature()->AI()->SpellHit(caster, spell);
			}
			
			void PassengerBoarded(Unit* who, int8 seatId, bool apply)
			{
				if (!apply)
					me->GetVehicleBase()->ToCreature()->AI()->PassengerBoarded(who, 1, apply);
			}
		};
	
		CreatureAI* GetAI(Creature* creature) const
		{
		CreatureAI* AI;
		if (creature->GetEntry() == NPC_WILD_WYRM)
			AI = new npc_q13003_wyrmAI(creature);
		else
			AI = new npc_q13003_jawsAI(creature);
		return AI;
		}
};

/*class spell_q13003_claw_swipe : public SpellScriptLoader
{
	public:
		spell_q13003_claw_swipe() : SpellScriptLoader("spell_q13003_claw_swipe") {}

		class spell_q13003_claw_swipe_AuraScript : public AuraScript
		{
			PrepareSpellScript(spell_q13003_claw_swipe_AuraScript);
			
			void HandleTriggerSpell(AuraEffect const* aurEff)
			{
				int pct = GetTarget()->CountPctFromMaxHealth(8);
				GetTarget()->CastCustomSpell(GetTarget(), 60776, &pct, &pct, &pct, true);
			}

			void Register()
			{
				OnEffectPeriodic += AuraEffectPeriodicFn(spell_q13003_claw_swipe_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
			}
		};

		AuraScript* GetAuraScript() const
		{
			return new spell_q13003_claw_swipe_AuraScript();
		}
};*/

class npc_q11073_flare : public CreatureScript
{
public:
	npc_q11073_flare() : CreatureScript("npc_q11073_flare") { }
	
	enum Feeding_Arngrim
	{
		SPELL_FLARE_CHANNEL = 40656,
		SPELL_ANCIENT_FLAME = 40657,
	};
	
	struct npc_q11073_flareAI : public NullCreatureAI
	{
		npc_q11073_flareAI(Creature* creature) : NullCreatureAI(creature)
		{
			timer = 30000;
			casted = false;
			me->CastSpell(me, SPELL_FLARE_CHANNEL, false);
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (timer < diff)
			{
				if (!casted)
				{
					me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 287.5f, me->GetOrientation());
					casted = true;
					timer = 500;
				}
				else
				{
					me->CastSpell(me, SPELL_ANCIENT_FLAME, true);
					me->DespawnOrUnsummon(15000);
					timer = 20000;
				}
			}
			else
				timer -= diff;
		}
		
		uint32 timer;
		bool casted;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_q11073_flareAI(creature);
	}
};

enum ArgentCannonSpells
{
	SPELL_ARGENT_CANNON_SHOOT           = 57385,
	SPELL_ARGENT_CANNON_SHOOT_TRIGGER   = 57387,
	SPELL_RECONING_BOMB                 = 57412,
	SPELL_RECONING_BOMB_TRIGGER         = 57414
};

class spell_argent_cannon : public SpellScriptLoader
{
	public:
		spell_argent_cannon() : SpellScriptLoader("spell_argent_cannon") { }

		class spell_argent_cannon_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_argent_cannon_SpellScript);

			bool Validate(SpellEntry const* /*SpellEntry*/)
			{
				if (!sSpellStore.LookupEntry(SPELL_ARGENT_CANNON_SHOOT_TRIGGER))
					return false;
				if (!sSpellStore.LookupEntry(SPELL_RECONING_BOMB_TRIGGER))
					return false;
				return true;
			}

			void HandleDummy(SpellEffIndex effIndex)
			{
				const WorldLocation* loc = GetExplTargetDest();

				switch(GetSpellInfo()->Id)
				{
					case SPELL_ARGENT_CANNON_SHOOT:
						GetCaster()->CastSpell(loc->m_positionX,loc->m_positionY,loc->m_positionZ,SPELL_ARGENT_CANNON_SHOOT_TRIGGER , true);
						break;
					case SPELL_RECONING_BOMB:
						GetCaster()->CastSpell(loc->m_positionX,loc->m_positionY,loc->m_positionZ,SPELL_RECONING_BOMB_TRIGGER , true);
						break;
				}

				PreventHitDefaultEffect(effIndex);
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_argent_cannon_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_argent_cannon_SpellScript();
		}
};



enum BlessedBannerSpells {
	SPELL_BLESSING_OF_THE_CRUSADE = 58026, SPELL_CRUSADERS_SPIRE_VICTORY = 58084,
};

enum BlessedBannerSummondNPCs {
	ENTRY_BLESSED_BANNER = 30891,

	ENTRY_CRUSADER_LORD_DALFORS = 31003,
	ENTRY_ARGENT_BATTLE_PRIEST = 30919,
	ENTRY_ARGENT_MASON = 30900,

	ENTRY_REANIMATED_CAPTAIN = 30986,
	ENTRY_SCOURGE_DRUDGE = 30984,
	ENTRY_HIDEOUS_PLAGEBRINGER = 30987,
	ENTRY_HALOF_THE_DEATHBRINGER = 30989,
};

const Position CrusaderPos[8] = { { 6418.31f, 423.00f, 511.2f, 5.945f }, // Banner

		{ 6424.50f, 420.72f, 511.0f, 5.689f }, // Dalfors
		{ 6421.05f, 423.37f, 511.0f, 5.579f }, // Priest 1
		{ 6423.25f, 425.95f, 511.0f, 5.579f }, // Priest 2
		{ 6417.59f, 419.28f, 511.0f, 5.579f }, // Priest 3

		{ 6418.36f, 439.71f, 511.28f, 3.757f }, // Matron 1
		{ 6406.75f, 475.36f, 511.27f, 2.928f }, // Matron 2
		{ 6386.99f, 440.60f, 511.28f, 2.601f } // Matron 3
};

const Position ScourgePos = { 6460.16f, 403.45f, 490.07f, 2.700f //Scourge Spawn
		};

#define SAY_PRE_1               "BY THE LIGHT! Those damned monsters! Look at what they've done to our people!"
#define SAY_PRE_2               "Burn it down, boys. Burn it all down."
#define SAY_START               "Let 'em come. They'll pay for what they've done!"
#define YELL_FINISHED           "We've done it, lads! We've taken the pinnacle from the Scourge! Report to Father Gustav at once and tell him the good news! We're gonna get to buildin' and settin' up! Go!"

// Script for Battle for Crusaders' Pinnacle Controller
class npc_blessed_banner: public CreatureScript {
public:
	npc_blessed_banner() :
			CreatureScript("npc_blessed_banner") {
	}

	struct npc_blessed_bannerAI: public Scripted_NoMovementAI {
		npc_blessed_bannerAI(Creature* creature) :
				Scripted_NoMovementAI(creature), Summons(me) {
			EventStarted = false;
			EventFinished = false;
			PhaseCount = 0;
			uiSummon_Timer = 0;
			say_Timer = 0;
			says = 0;
			Summons.DespawnAll();
		}

		bool EventStarted;
		bool EventFinished;

		uint32 PhaseCount;
		uint32 uiSummon_Timer;
		uint32 say_Timer;
		uint32 says;

		SummonList Summons;

		uint64 guidHalof;

		void Reset() {
			std::list<Creature*> _banners;
			GetCreatureListWithEntryInGrid(_banners, me, ENTRY_BLESSED_BANNER,
					100.0f);
			if (!_banners.empty())
				for (std::list<Creature*>::iterator iter = _banners.begin();
						iter != _banners.end(); ++iter) {
					if ((*iter)->GetGUID() != me->GetGUID()) {
						EventFinished = true;
						me->DespawnOrUnsummon(0);
						Summons.DespawnAll();
						return;
					}
				}
		}

		void MoveInLineOfSight(Unit *attacker) {
			return;
		}

		void JustSummoned(Creature* pSummoned) {
			Summons.Summon(pSummoned);
		}

		void JustDied(Unit *killer) {
			Summons.DespawnAll();
		}

		void UpdateAI(const uint32 diff) {
			if (EventFinished)
				return;

			if (!EventStarted) {
				Creature *tempsum;

				if (tempsum = DoSummon(ENTRY_CRUSADER_LORD_DALFORS, ScourgePos,
						600000, TEMPSUMMON_TIMED_DESPAWN)) {
					tempsum->GetMotionMaster()->MovePoint(0, CrusaderPos[1]);
					tempsum->SetHomePosition(CrusaderPos[1]);
				}

				for (int i = 0; i < 3; i++) {
					if (tempsum = DoSummon(ENTRY_ARGENT_MASON, ScourgePos,
							600000, TEMPSUMMON_TIMED_DESPAWN)) {
						tempsum->GetMotionMaster()->MovePoint(0,
								CrusaderPos[5 + i]);
						tempsum->SetHomePosition(CrusaderPos[5 + i]);
					}
				}

				for (int i = 0; i < 3; i++) {
					if (tempsum = DoSummon(ENTRY_ARGENT_BATTLE_PRIEST,
							ScourgePos, 600000, TEMPSUMMON_TIMED_DESPAWN)) {
						tempsum->GetMotionMaster()->MovePoint(0,
								CrusaderPos[2 + i]);
						tempsum->SetHomePosition(CrusaderPos[2 + i]);
					}
				}

				DoCast(SPELL_BLESSING_OF_THE_CRUSADE);
				uiSummon_Timer = 30000;
				EventStarted = true;
			} else {
				if (says < 3)
					if (say_Timer <= diff) {
						switch (says) {
						case 0:
							if (Creature* dalfors = GetClosestCreatureWithEntry(me,ENTRY_CRUSADER_LORD_DALFORS,100,true))
								dalfors->MonsterSay(SAY_PRE_1, LANG_UNIVERSAL,
										NULL);
							say_Timer = 10000;
							break;
						case 1:
							if (Creature* dalfors = GetClosestCreatureWithEntry(me,ENTRY_CRUSADER_LORD_DALFORS,100,true))
								dalfors->MonsterSay(SAY_PRE_2, LANG_UNIVERSAL,
										NULL);

							say_Timer = 10000;
							break;
						case 2:
							if (Creature* dalfors = GetClosestCreatureWithEntry(me,ENTRY_CRUSADER_LORD_DALFORS,100,true))
								dalfors->MonsterSay(SAY_START, LANG_UNIVERSAL,
										NULL);

							say_Timer = 10000;
							break;
						}
						says++;
					} else
						say_Timer -= diff;

				if (uiSummon_Timer <= diff) {
					if (PhaseCount == 9) {
						Creature *tempsum;

						if (tempsum = DoSummon(ENTRY_SCOURGE_DRUDGE, ScourgePos,
								10000, TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
							tempsum->GetMotionMaster()->MovePoint(
									0,
									CrusaderPos[0].m_positionX - 10
											+ urand(0, 10),
									CrusaderPos[0].m_positionY - 10
											+ urand(0, 10),
									CrusaderPos[0].m_positionZ);
							tempsum->SetHomePosition(CrusaderPos[0]);
						}
						if (tempsum = DoSummon(ENTRY_SCOURGE_DRUDGE, ScourgePos,
								10000, TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
							tempsum->GetMotionMaster()->MovePoint(
									0,
									CrusaderPos[0].m_positionX - 10
											+ urand(0, 10),
									CrusaderPos[0].m_positionY - 10
											+ urand(0, 10),
									CrusaderPos[0].m_positionZ);
							tempsum->SetHomePosition(CrusaderPos[0]);
						}
						if (tempsum = DoSummon(ENTRY_HALOF_THE_DEATHBRINGER,
								ScourgePos, 10000,
								TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
							guidHalof = tempsum->GetGUID();
							tempsum->GetMotionMaster()->MovePoint(0,
									CrusaderPos[0]);
							tempsum->SetHomePosition(CrusaderPos[0]);
						}
					} else {
						Creature *tempsum;

						if (urand(0, 1) == 0) {
							if (tempsum = DoSummon(ENTRY_HIDEOUS_PLAGEBRINGER,
									ScourgePos, 10000,
									TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
								tempsum->GetMotionMaster()->MovePoint(
										0,
										CrusaderPos[0].m_positionX - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionY - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionZ);
								tempsum->SetHomePosition(CrusaderPos[0]);
							}
							if (tempsum = DoSummon(ENTRY_HIDEOUS_PLAGEBRINGER,
									ScourgePos, 10000,
									TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
								tempsum->GetMotionMaster()->MovePoint(
										0,
										CrusaderPos[0].m_positionX - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionY - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionZ);
								tempsum->SetHomePosition(CrusaderPos[0]);
							}
							if (tempsum = DoSummon(ENTRY_SCOURGE_DRUDGE,
									ScourgePos, 10000,
									TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
								tempsum->GetMotionMaster()->MovePoint(
										0,
										CrusaderPos[0].m_positionX - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionY - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionZ);
								tempsum->SetHomePosition(CrusaderPos[0]);
							}
						} else {
							if (tempsum = DoSummon(ENTRY_REANIMATED_CAPTAIN,
									ScourgePos, 10000,
									TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
								tempsum->GetMotionMaster()->MovePoint(
										0,
										CrusaderPos[0].m_positionX - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionY - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionZ);
								tempsum->SetHomePosition(CrusaderPos[0]);
							}
							if (tempsum = DoSummon(ENTRY_REANIMATED_CAPTAIN,
									ScourgePos, 10000,
									TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
								tempsum->GetMotionMaster()->MovePoint(
										0,
										CrusaderPos[0].m_positionX - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionY - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionZ);
								tempsum->SetHomePosition(CrusaderPos[0]);
							}
							if (tempsum = DoSummon(ENTRY_SCOURGE_DRUDGE,
									ScourgePos, 10000,
									TEMPSUMMON_CORPSE_TIMED_DESPAWN)) {
								tempsum->GetMotionMaster()->MovePoint(
										0,
										CrusaderPos[0].m_positionX - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionY - 10
												+ urand(0, 10),
										CrusaderPos[0].m_positionZ);
								tempsum->SetHomePosition(CrusaderPos[0]);
							}
						}
					}

					PhaseCount++;
					uiSummon_Timer = 20000;
				} else
					uiSummon_Timer -= diff;

				if (PhaseCount > 8) {
					if (Creature* Halof = me->GetCreature(*me,guidHalof)) {
						if (Halof->isDead()) {
							EventFinished = true;
							DoCast(me, SPELL_CRUSADERS_SPIRE_VICTORY, true);

							Summons.DespawnEntry(ENTRY_HIDEOUS_PLAGEBRINGER);
							Summons.DespawnEntry(ENTRY_REANIMATED_CAPTAIN);
							Summons.DespawnEntry(ENTRY_SCOURGE_DRUDGE);
							Summons.DespawnEntry(ENTRY_HALOF_THE_DEATHBRINGER);

							if (Creature* dalfors = GetClosestCreatureWithEntry(me,ENTRY_CRUSADER_LORD_DALFORS,100,true))
								dalfors->MonsterYell(YELL_FINISHED,
										LANG_UNIVERSAL, NULL);

							for (std::list<uint64>::iterator itr =
									Summons.begin(); itr != Summons.end();
									++itr) {
								if (Creature* temp = Creature::GetCreature(*me,*itr)) {
									temp->DespawnOrUnsummon(20000);
								}
							}

							me->DespawnOrUnsummon(20000);
						}
					}
				}
			}
		}
	};

	CreatureAI *GetAI(Creature *creature) const {
		return new npc_blessed_bannerAI(creature);
	}
};



enum CapturedCrusader {
	SPELL_NERUBIAN_WEBS = 56726,
	SPELL_GRAB_CAPTURED_CRUSADER = 56683,
	SPELL_RIDE_VEHICLE = 56687
};

// texts not in script
class npc_captured_crusader: public CreatureScript {
public:
	npc_captured_crusader() :
			CreatureScript("npc_captured_crusader") {
	}

	struct npc_captured_crusaderAI: public ScriptedAI {
		npc_captured_crusaderAI(Creature* creature) :
				ScriptedAI(creature) {
		}

		void Reset() {
			_spellHit = false;
			_vehicleTimer = 5000;
			DoCast(me, SPELL_NERUBIAN_WEBS, true);
		}

		void SpellHit(Unit* caster, SpellEntry const* spell) {
			if (spell->Id == SPELL_GRAB_CAPTURED_CRUSADER) {
				_spellHit = true;
				DoCast(caster, SPELL_RIDE_VEHICLE, true);
			}
		}

		void UpdateAI(uint32 const diff) {
			if (!_spellHit || _spellHit && me->GetVehicle())
				return;

			if (_vehicleTimer <= diff) {
				_spellHit = false;
				me->DespawnOrUnsummon();
			} else
				_vehicleTimer -= diff;
		}

	private:
		bool _spellHit;
		uint32 _vehicleTimer;
	};

	CreatureAI* GetAI(Creature* creature) const {
		return new npc_captured_crusaderAI(creature);
	}
};

/*########
 ## npc_the_ocular
 #########*/

enum TheOcularSpells {
	SPELL_THE_OCULAR_TRANSFORM = 55162,
	SPELL_DEATHLY_STARE = 55269,
	SPELL_ITS_ALL_FUN_AND_GAMES_THE_OCULAR_ON_DEATH = 55288,
	SPELL_ITS_ALL_FUN_AND_GAMES_THE_OCULAR_KILL_CREDIT = 55289
};

enum ReqCreatures {
	NPC_THE_OCULAR = 29747, NPC_THE_OCULAR_DESTROYED_KILL_CREDIT_BUNNY = 29803
};

class npc_the_ocular: public CreatureScript {
public:
	npc_the_ocular() :
			CreatureScript("npc_the_ocular") {
	}

	CreatureAI* GetAI(Creature* creature) const {
		return new npc_the_ocularAI(creature);
	}

	struct npc_the_ocularAI: public Scripted_NoMovementAI {
		npc_the_ocularAI(Creature* creature) :
				Scripted_NoMovementAI(creature) {
		}

		uint32 uiDeathlyStareTimer;

		void Reset() {
			uiDeathlyStareTimer = (urand(5000, 7000));
		}

		void DamageTaken(Unit* attacker, uint32 &damage) {
			me->LowerPlayerDamageReq(damage);
		}

		void JustDied(Unit* killer) {
			if (killer && killer->ToPlayer())
				killer->ToPlayer()->CastSpell(killer,
						SPELL_ITS_ALL_FUN_AND_GAMES_THE_OCULAR_KILL_CREDIT,
						true);
		}

		void UpdateAI(const uint32 uiDiff) {
			if (!me->HasAura(SPELL_THE_OCULAR_TRANSFORM))
				DoCast(me, SPELL_THE_OCULAR_TRANSFORM, true);

			if (!UpdateVictim())
				return;

			if (uiDeathlyStareTimer <= uiDiff) {
				DoCastVictim(SPELL_DEATHLY_STARE);
				uiDeathlyStareTimer = (urand(7000, 9000));
			} else
				uiDeathlyStareTimer -= uiDiff;
		}
	};
};

/*########
 ## npc_general_lightsbane
 #########*/

enum eGeneralLightsbaneSpells {
	SPELL_CLEAVE = 15284,
	SPELL_DEATH_AND_DECAY = 60160,
	SPELL_PLAGUE_STRIKE = 60186,
};

enum eNpcs {
	ENTRY_VILE = 29860, ENTRY_THE_LEAPER = 29859, ENTRY_LADY_NIGHTSWOOD = 29858,
};

class npc_general_lightsbane: public CreatureScript {
public:
	npc_general_lightsbane() :
			CreatureScript("npc_general_lightsbane") {
	}

	CreatureAI* GetAI(Creature* creature) const {
		return new npc_general_lightsbaneAI(creature);
	}

	struct npc_general_lightsbaneAI: public ScriptedAI {
		npc_general_lightsbaneAI(Creature* creature) :
				ScriptedAI(creature) {
		}

		uint32 uiCleave_Timer;
		uint32 uiDeathDecay_Timer;
		uint32 uiPlagueStrike_Timer;
		uint32 uiSummonSupport_Timer;
		bool supportSummoned;

		void Reset() {
			uiCleave_Timer = urand(2000, 3000);
			uiDeathDecay_Timer = urand(15000, 20000);
			uiPlagueStrike_Timer = urand(5000, 10000);

			std::list<Creature*> TargetList;
			me->GetCreatureListWithEntryInGrid(TargetList, me->GetEntry(),
					100.0f);
			if (TargetList.size() > 1) {
				me->DespawnOrUnsummon(1000);
			}

			uiSummonSupport_Timer = 5000;
			supportSummoned = false;
		}

		void UpdateAI(const uint32 uiDiff) {
			if (!UpdateVictim())
				return;

			if (!supportSummoned)
				if (uiSummonSupport_Timer <= uiDiff) {
					Creature* temp = DoSummon(ENTRY_VILE, me, 5, 20000,
							TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
					temp->AI()->AttackStart(me);

					temp = DoSummon(ENTRY_THE_LEAPER, me, 5, 20000,
							TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
					temp->AI()->AttackStart(me);

					temp = DoSummon(ENTRY_LADY_NIGHTSWOOD, me, 5, 20000,
							TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
					temp->AI()->AttackStart(me);

					uiSummonSupport_Timer = (urand(4000, 5000));
					supportSummoned = true;
				} else
					uiSummonSupport_Timer -= uiDiff;

			if (uiCleave_Timer <= uiDiff) {
				DoCastVictim(SPELL_CLEAVE);
				uiCleave_Timer = (urand(4000, 5000));
			} else
				uiCleave_Timer -= uiDiff;

			if (uiDeathDecay_Timer <= uiDiff) {
				DoCastVictim(SPELL_DEATH_AND_DECAY);
				uiDeathDecay_Timer = urand(15000, 20000);
			} else
				uiDeathDecay_Timer -= uiDiff;

			if (uiPlagueStrike_Timer <= uiDiff) {
				DoCastVictim(SPELL_PLAGUE_STRIKE);
				uiPlagueStrike_Timer = urand(5000, 10000);
			} else
				uiPlagueStrike_Timer -= uiDiff;

			DoMeleeAttackIfReady();
		}
	};
};

/*########
 ## npc_free_your_mind_vile
 #########*/

enum eFreeYourMindNPCSpells {
	SPELL_SOVEREIGN_ROD = 29070, SPELL_SOVEREIGN_ROD_TRIGGERED = 29071,
	// Vile Abilities
	SPELL_VILE_ENRAGE = 56646, // <50% HP ?
	SPELL_VILE_BACKHAND = 6253,
	// Lady Nightswood Abilities
	SPELL_BANSHEE_CURSE = 5884,
	SPELL_BANSHEE_SHRIEK = 16838,
	// The Leaper Abilities
	SPELL_LEAPER_SNISTER_STRIKE = 60195,
	SPELL_LEAPER_HUNGER_FOR_BLOOD = 60177,
};

enum eFreeYourMindNPCEntrys {
	ENTRY_FYM_VILE = 29769, ENTRY_FYM_LADY = 29770, ENTRY_FYM_LEAPER = 29840
};

#define SAY_VILE_AGGRO              "Crush... maim... DESTROY!"
#define SAY_VILE_FREED              "Vile free? Vile love %n"

#define SAY_LADY_NIGHTSWOOD_AGGRO   "Who intrudes upon my ritual?"
#define SAY_LADY_NIGHTSWOOD_FREED   " You dare? Where is Baron Sliver? I would have words with him!"

#define SAY_THE_LEAPER_AGGRO        "Mrrfrmrfrmrrrrr!"
#define SAY_THE_LEAPER_FREED        "Mrmrmmrmrmrmrm... mrmrmrmr?!"

class npc_free_your_mind: public CreatureScript {
public:
	npc_free_your_mind() :
			CreatureScript("npc_free_your_mind") {
	}

	CreatureAI* GetAI(Creature* creature) const {
		return new npc_free_your_mindAI(creature);
	}

	struct npc_free_your_mindAI: public ScriptedAI {
		npc_free_your_mindAI(Creature* creature) :
				ScriptedAI(creature) {
		}

		bool Enraged;

		uint32 uiSpell1Entry_Timer;
		uint32 uiSpell2Entry_Timer;
		uint32 uiSpell1Entry;
		uint32 uiSpell2Entry;

		void Reset() {
			switch (me->GetEntry()) {
			case ENTRY_FYM_VILE:
				uiSpell1Entry = SPELL_VILE_BACKHAND;
				uiSpell1Entry_Timer = urand(4000, 6000);
				uiSpell2Entry = SPELL_VILE_ENRAGE;
				break;
			case ENTRY_FYM_LADY:
				uiSpell1Entry = SPELL_BANSHEE_CURSE;
				uiSpell1Entry_Timer = urand(5000, 6000);
				uiSpell2Entry = SPELL_BANSHEE_SHRIEK;
				uiSpell2Entry_Timer = urand(10000, 12000);
				break;
			case ENTRY_FYM_LEAPER:
				uiSpell1Entry = SPELL_LEAPER_SNISTER_STRIKE;
				uiSpell1Entry_Timer = urand(4000, 6000);
				uiSpell2Entry = SPELL_LEAPER_HUNGER_FOR_BLOOD;
				break;
			}

			me->RestoreFaction();
		}

		void EnterCombat(Unit* who) {
			Enraged = false;
			switch (me->GetEntry()) {
			case ENTRY_FYM_VILE:
				me->MonsterSay(SAY_VILE_AGGRO, LANG_UNIVERSAL, who->GetGUID());
				break;
			case ENTRY_FYM_LEAPER:
				me->MonsterSay(SAY_THE_LEAPER_AGGRO, LANG_UNIVERSAL,
						who->GetGUID());
				break;
			case ENTRY_FYM_LADY:
				me->MonsterSay(SAY_LADY_NIGHTSWOOD_AGGRO, LANG_UNIVERSAL,
						who->GetGUID());
				break;
			}
		}

		void SpellHit(Unit* caster, SpellEntry const* spell) {
			if (spell->Id == SPELL_SOVEREIGN_ROD_TRIGGERED) {
				if (caster && caster->ToPlayer()) {
					me->setDeathState(ALIVE);
					me->setFaction(35);
					me->DespawnOrUnsummon(4000);

					switch (me->GetEntry()) {
					case ENTRY_FYM_VILE:
						me->MonsterSay(SAY_VILE_FREED, LANG_UNIVERSAL,
								caster->GetGUID());
						caster->ToPlayer()->KilledMonsterCredit(29845, 0);
						break;
					case ENTRY_FYM_LEAPER:
						me->MonsterSay(SAY_THE_LEAPER_FREED, LANG_UNIVERSAL,
								caster->GetGUID());
						caster->ToPlayer()->KilledMonsterCredit(29847, 0);
						break;
					case ENTRY_FYM_LADY:
						me->MonsterSay(SAY_LADY_NIGHTSWOOD_FREED,
								LANG_UNIVERSAL, caster->GetGUID());
						caster->ToPlayer()->KilledMonsterCredit(29846, 0);
						break;
					}
				}
			}
		}

		void UpdateAI(const uint32 uiDiff) {
			if (!UpdateVictim())
				return;

			if (uiSpell1Entry_Timer <= uiDiff) {
				DoCastVictim(uiSpell1Entry);
				switch (me->GetEntry()) {
				case ENTRY_FYM_VILE:
				case ENTRY_FYM_LEAPER:
					uiSpell1Entry_Timer = (urand(7000, 9000));
					break;
				case ENTRY_FYM_LADY:
					uiSpell1Entry_Timer = (urand(10000, 15000));
					break;
				}
			} else
				uiSpell1Entry_Timer -= uiDiff;

			if (me->GetEntry() == ENTRY_FYM_VILE) {
				if (!Enraged && HealthBelowPct(30)) {
					DoCast(me, uiSpell2Entry, true);
					Enraged = true;
				}
			} else {
				if (uiSpell2Entry_Timer <= uiDiff) {
					DoCastVictim(uiSpell2Entry);
					uiSpell2Entry_Timer = (urand(8000, 10000));
				} else
					uiSpell2Entry_Timer -= uiDiff;
			}

			DoMeleeAttackIfReady();
		}
	};
};

/*########
 ## Saronite Mine Slave
 #########*/

enum eEntrysSlaveToSaronite {
	QUEST_SLAVES_TO_SARONITE_ALLIANCE = 13300,
	QUEST_SLAVES_TO_SARONITE_HORDE = 13302,

	ENTRY_SLAVE_QUEST_CREDIT = 31866,

	SPELL_SLAVE_ENRAGE = 8599,
	SPELL_HEAD_CRACK = 3148,

	ACTION_ENRAGED = 0,
	ACTION_INSANE = 1,
	ACTION_FREED = 2,
};

const Position FreedPos[2] = { { 7030.0f, 1862.0f, 533.2f, 0.0f }, { 6947.0f,
		2027.0f, 519.7f, 0.0f } };

#define GOSSIP_OPTION_FREE  "Go on, you're free. Get out of here!"

#define SAY_SLAVE_AGGRO_1 "AHAHAHAHA... you'll join us soon enough!"
#define SAY_SLAVE_AGGRO_2 "I don't want to leave! I want to stay here!"
#define SAY_SLAVE_AGGRO_3 "I won't leave!"
#define SAY_SLAVE_AGGRO_4 "NO! You're wrong! The voices in my head are beautiful!"

#define SAY_SLAVE_INSANE_1 "I must get further underground to where he is. I must jump!"
#define SAY_SLAVE_INSANE_2 "I'll never return. The whole reason for my existence awaits below!"
#define SAY_SLAVE_INSANE_3 "I'm coming, master!"
#define SAY_SLAVE_INSANE_4 "My life for you!"

class npc_saronite_mine_slave: public CreatureScript {
public:
	npc_saronite_mine_slave() :
			CreatureScript("npc_saronite_mine_slave") {
	}

	bool OnGossipHello(Player* player, Creature* creature) {
		if ((player->GetQuestStatus(QUEST_SLAVES_TO_SARONITE_HORDE)
				== QUEST_STATUS_INCOMPLETE)
				|| (player->GetQuestStatus(QUEST_SLAVES_TO_SARONITE_ALLIANCE)
						== QUEST_STATUS_INCOMPLETE))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_FREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/,
			uint32 uiAction) {
		player->PlayerTalkClass->ClearMenus();
		player->CLOSE_GOSSIP_MENU();

		if (uiAction == (GOSSIP_ACTION_INFO_DEF + 1)) {
			creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
			if (urand(0, 1) == 0) {
				creature->AI()->DoAction(ACTION_FREED);
				player->KilledMonsterCredit(ENTRY_SLAVE_QUEST_CREDIT, 0);
			} else {
				if (urand(0, 1) == 0) {
					creature->AI()->DoAction(ACTION_ENRAGED);
					creature->setFaction(16);
					//FIXME creature->CastSpell(creature,SPELL_SLAVE_ENRAGE);
					creature->AI()->AttackStart(player);
				} else
					creature->AI()->DoAction(ACTION_INSANE);
			}
		}
		return true;
	}

	CreatureAI* GetAI(Creature* creature) const {
		return new npc_saronite_mine_slaveAI(creature);
	}

	struct npc_saronite_mine_slaveAI: public ScriptedAI {
		npc_saronite_mine_slaveAI(Creature* creature) :
				ScriptedAI(creature) {
			alreadyFreed = false;
			enraged = false;
		}

		bool enraged;
		bool alreadyFreed;

		uint32 uiHeadCrack_Timer;

		void DoAction(const int32 action) {
			switch (action) {
			case ACTION_ENRAGED:
				enraged = true;
				alreadyFreed = true;
				switch (urand(0, 3)) {
				case 0:
					me->MonsterYell(SAY_SLAVE_AGGRO_1, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				case 1:
					me->MonsterYell(SAY_SLAVE_AGGRO_2, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				case 2:
					me->MonsterYell(SAY_SLAVE_AGGRO_3, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				case 3:
					me->MonsterYell(SAY_SLAVE_AGGRO_4, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				}
				break;
			case ACTION_FREED:
				alreadyFreed = true;
				me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
				me->GetMotionMaster()->MovePoint(0, FreedPos[0]);
				me->DespawnOrUnsummon(15000);
				break;
			case ACTION_INSANE:
				alreadyFreed = true;
				switch (urand(0, 3)) {
				case 0:
					me->MonsterYell(SAY_SLAVE_INSANE_1, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				case 1:
					me->MonsterYell(SAY_SLAVE_INSANE_2, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				case 2:
					me->MonsterYell(SAY_SLAVE_INSANE_3, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				case 3:
					me->MonsterYell(SAY_SLAVE_INSANE_4, LANG_UNIVERSAL,
							me->GetGUID());
					break;
				}
				me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
				me->GetMotionMaster()->MovePoint(0, FreedPos[1]);
				me->DespawnOrUnsummon(15000);
				break;
			}
		}

		void MoveInLineOfSight(Unit* mover) {
			if (!enraged)
				return;

			ScriptedAI::MoveInLineOfSight(mover);
		}

		void AttackStart(Unit* attacker) {
			if (!enraged)
				return;

			ScriptedAI::AttackStart(attacker);
		}

		void Reset() {
			me->RestoreFaction();
			if (alreadyFreed) {
				alreadyFreed = false;
				me->DespawnOrUnsummon(10000);
			} else {
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				alreadyFreed = false;
				enraged = false;
			}
		}

		void EnterCombat(Unit* who) {
			uiHeadCrack_Timer = urand(5000, 7000);
		}

		void UpdateAI(const uint32 uiDiff) {
			if (!UpdateVictim())
				return;

			if (uiHeadCrack_Timer <= uiDiff) {
				DoCastVictim(SPELL_HEAD_CRACK);
				uiHeadCrack_Timer = (urand(7000, 9000));
			} else
				uiHeadCrack_Timer -= uiDiff;

			DoMeleeAttackIfReady();
		}
	};
};

/*######
 ## npc_vendor_tournament_fraction_champion
 ######*/

enum eAchievementsTournamentFactionChampion {
	ACHIEVEMENT_CHAMPION_OF_DARNASSUS = 2777,
	ACHIEVEMENT_CHAMPION_OF_EXODAR = 2778,
	ACHIEVEMENT_CHAMPION_OF_GNOMEREGAN = 2779,
	ACHIEVEMENT_CHAMPION_OF_IRONFORGE = 2780,
	ACHIEVEMENT_CHAMPION_OF_STORMWIND = 2781,
	ACHIEVEMENT_CHAMPION_OF_ORGRIMMAR = 2783,
	ACHIEVEMENT_CHAMPION_OF_DARKSPEARS = 2784,
	ACHIEVEMENT_CHAMPION_OF_SILVERMOON = 2785,
	ACHIEVEMENT_CHAMPION_OF_THUNDERBLUFF = 2786,
	ACHIEVEMENT_CHAMPION_OF_UNDERCITY = 2787,
};

enum eNPCVendorEntrys {
	ENTRY_EXODAR_VENDOR = 33657,
	ENTRY_GNOMEREGAN_VENDOR = 33650,
	ENTRY_DARNASSUS_VENDOR = 33653,
	ENTRY_STORMWIND_VENDOR = 33307,
	ENTRY_IRONFORGE_VENDOR = 33310,

	ENTRY_ORGRIMMAR_VENDOR = 33553,
	ENTRY_DARKSPEARS_VENDOR = 33554,
	ENTRY_SILVERMOON_VENDOR = 33557,
	ENTRY_THUNDERBLUFF_VENDOR = 33556,
	ENTRY_UNDERCITY_VENDOR = 33555,
};

class npc_vendor_tournament_fraction_champion: public CreatureScript {
public:
	npc_vendor_tournament_fraction_champion() :
			CreatureScript("npc_vendor_tournament_fraction_champion") {
	}

	bool OnGossipHello(Player* player, Creature* creature) {
		bool showVendor = false;

		switch (creature->GetEntry()) {
		case ENTRY_EXODAR_VENDOR:
			showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_EXODAR);
			break;
		case ENTRY_GNOMEREGAN_VENDOR:
			showVendor = player->HasAchieved(
					ACHIEVEMENT_CHAMPION_OF_GNOMEREGAN);
			break;
		case ENTRY_DARNASSUS_VENDOR:
			showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_DARNASSUS);
			break;
		case ENTRY_STORMWIND_VENDOR:
			showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_STORMWIND);
			break;
		case ENTRY_IRONFORGE_VENDOR:
			showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_IRONFORGE);
			break;
		case ENTRY_ORGRIMMAR_VENDOR:
			showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_ORGRIMMAR);
			break;
		case ENTRY_DARKSPEARS_VENDOR:
			showVendor = player->HasAchieved(
					ACHIEVEMENT_CHAMPION_OF_DARKSPEARS);
			break;
		case ENTRY_SILVERMOON_VENDOR:
			showVendor = player->HasAchieved(
					ACHIEVEMENT_CHAMPION_OF_SILVERMOON);
			break;
		case ENTRY_THUNDERBLUFF_VENDOR:
			showVendor = player->HasAchieved(
					ACHIEVEMENT_CHAMPION_OF_THUNDERBLUFF);
			break;
		case ENTRY_UNDERCITY_VENDOR:
			showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_UNDERCITY);
			break;
		}

		if (showVendor) {
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		}
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/,
			uint32 uiAction) {
		player->PlayerTalkClass->ClearMenus();
		if (uiAction == GOSSIP_ACTION_TRADE)
			player->GetSession()->SendListInventory(creature->GetGUID());
		return true;
	}
};

/*######
 ## npc_maiden_of_drakmar
 ######*/

#define SAY_MAIDEN_OF_FRAKMAR_1       "Are those winter hyacinths? For me?"
#define SAY_MAIDEN_OF_FRAKMAR_2       "It\'s been so long since a traveler has come here bearing flowers."
#define SAY_MAIDEN_OF_FRAKMAR_3       "The lake has been too lonely these past years. The travelers stopped coming and evil came to these waters."
#define SAY_MAIDEN_OF_FRAKMAR_4       "Your gift is a rare kindness, traveler. Please take this blade with my gratitude. Long ago, another traveler left it here, but I have little use for it."

enum eEntrys {
	GO_DRAKMAR_LILY_PAD = 194239,
	GO_BLADE_OF_DRAKMAR = 194238,
	ENTRY_MAIDEN_OF_DRAKMAR = 33273,
};

static Position miscLocations[] = {
		{ 4602.08f, -1600.22f, 156.657f, 0.128299f }, //GO_DRAKMAR_LILY_PAD
		{ 4601.53f, -1600.47f, 156.986f, 1.944937f } //GO_BLADE_OF_DRAKMAR
};

class npc_maiden_of_drakmar: public CreatureScript {
public:
	npc_maiden_of_drakmar() :
			CreatureScript("npc_maiden_of_drakmar") {
	}

	CreatureAI* GetAI(Creature* creature) const {
		return new npc_maiden_of_drakmarAI(creature);
	}

	struct npc_maiden_of_drakmarAI: public Scripted_NoMovementAI {
		npc_maiden_of_drakmarAI(Creature* creature) :
				Scripted_NoMovementAI(creature) {
			me->SetCanFly(true); // cosmetic workaround
		}

		uint32 uiSayTimer;
		uint8 uiSayStep;

		void Reset() {
			uiSayStep = 0;
			uiSayTimer = 2000;
		}

		void UpdateAI(const uint32 uiDiff) {
			if (uiSayTimer <= uiDiff) {
				switch (uiSayStep) {
				case 0:
					me->SummonGameObject(GO_DRAKMAR_LILY_PAD,
							miscLocations[0].GetPositionX(),
							miscLocations[0].GetPositionY(),
							miscLocations[0].GetPositionZ(),
							miscLocations[0].GetOrientation(), 0, 0, 0, 0,
							30000);
					me->MonsterSay(SAY_MAIDEN_OF_FRAKMAR_1, LANG_UNIVERSAL, 0);
					uiSayTimer = 3000;
					++uiSayStep;
					break;
				case 1:
					me->MonsterSay(SAY_MAIDEN_OF_FRAKMAR_2, LANG_UNIVERSAL, 0);
					uiSayTimer = 5000;
					++uiSayStep;
					break;
				case 2:
					me->MonsterSay(SAY_MAIDEN_OF_FRAKMAR_3, LANG_UNIVERSAL, 0);
					uiSayTimer = 7000;
					++uiSayStep;
					break;
				case 3:
					me->SummonGameObject(GO_BLADE_OF_DRAKMAR,
							miscLocations[1].GetPositionX(),
							miscLocations[1].GetPositionY(),
							miscLocations[1].GetPositionZ(),
							miscLocations[1].GetOrientation(), 0, 0, 0, 0,
							30000);
					me->MonsterSay(SAY_MAIDEN_OF_FRAKMAR_4, LANG_UNIVERSAL, 0);
					uiSayTimer = 99999999;
					++uiSayStep;
					break;
				}
			} else
				uiSayTimer -= uiDiff;
		}
	};
};

/*########
 ## npc_slain_tualiq_villager
 #########*/
// Maybe rewrite with SmartAI
class npc_slain_tualiq_villager: public CreatureScript {
public:
	npc_slain_tualiq_villager() :
			CreatureScript("npc_slain_tualiq_villager") {
	}

	CreatureAI* GetAI(Creature* creature) const {
		return new npc_slain_tualiq_villagerAI(creature);
	}

	struct npc_slain_tualiq_villagerAI: public Scripted_NoMovementAI {
		npc_slain_tualiq_villagerAI(Creature* creature) :
				Scripted_NoMovementAI(creature) {
		}

		bool credited;

		void Reset() {
			credited = false;
		}

		void SpellHit(Unit* caster, const SpellEntry* spellinfo) {
			if (credited)
				return;

			if (caster && caster->ToPlayer()) {
				if (spellinfo->Id == 66390) {
					credited = true;
					caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(),
							me->GetGUID());
					me->DespawnOrUnsummon(3000);
				}
			}
		}

		void AttackStart(Unit* who) {
			return;
		}

		void MoveInLineOfSight(Unit *who) {
			return;
		}

		void UpdateAI(const uint32 uiDiff) {
			return;
		}
	};
};
/*######
## npc_neltharaku
######*/

#define GOSSIP_HN "I am listening, dragon"
#define GOSSIP_SN1 "But you are dragons! How could orcs do this to you?"
#define GOSSIP_SN2 "Your mate?"
#define GOSSIP_SN3 "I have battled many beasts, dragon. I will help you."

class npc_neltharaku : public CreatureScript
{
public:
	npc_neltharaku() : CreatureScript("npc_neltharaku") { }

	bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
	{
		pPlayer->PlayerTalkClass->ClearMenus();
		switch (uiAction)
		{
			case GOSSIP_ACTION_INFO_DEF+1:
				pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_SN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
				pPlayer->SEND_GOSSIP_MENU(10614, pCreature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF+2:
				pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_SN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
				pPlayer->SEND_GOSSIP_MENU(10615, pCreature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF+3:
				pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_SN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
				pPlayer->SEND_GOSSIP_MENU(10616, pCreature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF+4:
				pPlayer->CLOSE_GOSSIP_MENU();
				pPlayer->AreaExploredOrEventHappens(10814);
				break;
		}
		return true;
	}

	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
		if (pCreature->isQuestGiver())
			pPlayer->PrepareQuestMenu(pCreature->GetGUID());

		if (pPlayer->GetQuestStatus(10814) == QUEST_STATUS_INCOMPLETE)
			pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_HN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

		pPlayer->SEND_GOSSIP_MENU(10613, pCreature->GetGUID());

		return true;
	}
};

/*######
## Quest: Defending Wyrmrest Temple ID: 12372
######*/

enum WyrmDefenderEnum
{
	// Quest data
	QUEST_DEFENDING_WYRMREST_TEMPLE          = 12372,
	GOSSIP_TEXTID_DEF1                       = 12899,

	// Gossip data
	GOSSIP_TEXTID_DEF2                       = 12900,

	// Spells data
	SPELL_CHARACTER_SCRIPT                   = 49213,
	SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE       = 52421, // ID - 52421 Wyrmrest Defender: On Low Health Boss Emote to Controller - Random /self/
	SPELL_RENEW                              = 49263, // casted to heal drakes
	SPELL_WYRMREST_DEFENDER_MOUNT            = 49256,

	// Texts data
	WHISPER_MOUNTED                        = 0,
	BOSS_EMOTE_ON_LOW_HEALTH               = 2
};

#define GOSSIP_ITEM_1      "We need to get into the fight. Are you ready?"

class npc_wyrmrest_defender : public CreatureScript
{
	public:
		npc_wyrmrest_defender() : CreatureScript("npc_wyrmrest_defender") { }

		bool OnGossipHello(Player* player, Creature* creature)
		{
			if (player->GetQuestStatus(QUEST_DEFENDING_WYRMREST_TEMPLE) == QUEST_STATUS_INCOMPLETE)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
				player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEF1, creature->GetGUID());
			}
			else
				player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

			return true;
		}

		bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
		{
			player->PlayerTalkClass->ClearMenus();
			if (action == GOSSIP_ACTION_INFO_DEF+1)
			{
				player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEF2, creature->GetGUID());
				// Makes player cast trigger spell for 49207 on self
				player->CastSpell(player, SPELL_CHARACTER_SCRIPT, true);
				// The gossip should not auto close
			}

			return true;
		}

		struct npc_wyrmrest_defenderAI : public VehicleAI
		{
			npc_wyrmrest_defenderAI(Creature* creature) : VehicleAI(creature) { }

			bool hpWarningReady;
			bool renewRecoveryCanCheck;

			uint32 RenewRecoveryChecker;

			void Reset()
			{
				hpWarningReady = true;
				renewRecoveryCanCheck = false;

				RenewRecoveryChecker = 0;
			}

			void UpdateAI(uint32 const diff)
			{
				// Check system for Health Warning should happen first time whenever get under 30%,
				// after it should be able to happen only after recovery of last renew is fully done (20 sec),
				// next one used won't interfere
				if (hpWarningReady && me->GetHealthPct() <= 30.0f)
				{
					me->CastSpell(me, SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE);
					hpWarningReady = false;
				}

				if (renewRecoveryCanCheck)
				{
					if (RenewRecoveryChecker <= diff)
					{
						renewRecoveryCanCheck = false;
						hpWarningReady = true;
					}
					else RenewRecoveryChecker -= diff;
				}
			}

			void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
			{
				switch (spell->Id)
				{
					case SPELL_WYRMREST_DEFENDER_MOUNT:
						Talk(WHISPER_MOUNTED, me->GetCharmerOrOwnerGUID());
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
						break;
					// Both below are for checking low hp warning
					case SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE:
						Talk(BOSS_EMOTE_ON_LOW_HEALTH, me->GetCharmerOrOwnerGUID());
						break;
					case SPELL_RENEW:
						if (!hpWarningReady && RenewRecoveryChecker <= 100)
						{
							RenewRecoveryChecker = 20000;
						}
						renewRecoveryCanCheck = true;
						break;
				}
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_wyrmrest_defenderAI(creature);
		}
};

/*######
## vehicle_alliance_steamtank
######*/

#define AREA_CARRION_FIELDS         4188
#define AREA_WINTERGARD_MAUSOLEUM   4246
#define AREA_THORSONS_POINT         4190

class vehicle_alliance_steamtank : public CreatureScript
{
public:
	vehicle_alliance_steamtank() : CreatureScript("vehicle_alliance_steamtank") { }

	struct vehicle_alliance_steamtankAI : public VehicleAI
	{
		vehicle_alliance_steamtankAI(Creature *c) : VehicleAI(c) { }

		uint32 check_Timer;
		bool isInUse;

		void Reset()
		{
			check_Timer = 5000;
		}

		void OnCharmed(bool apply)
		{
			isInUse = apply;

			if(!apply)
				check_Timer = 30000;
		}

		void UpdateAI(const uint32 diff)
		{
			if(!me->IsVehicle())
				return;

			if(isInUse)
			{
				if(check_Timer < diff)
				{
					uint32 area = me->GetAreaId();
					switch(area)
					{
					case AREA_CARRION_FIELDS:
					case AREA_WINTERGARD_MAUSOLEUM:
					case AREA_THORSONS_POINT:
						break;
					default:
						me->DealDamage(me,me->GetHealth());
						break;
					}

					check_Timer = 5000;
				}else check_Timer -= diff;
			}else
			{
				if(check_Timer < diff)
				{
					uint32 area = me->GetAreaId();
					if(area != AREA_THORSONS_POINT)
					{
						me->DealDamage(me,me->GetHealth());
					}
					check_Timer = 5000;
				}else check_Timer -= diff;
			}
		}
	};

	CreatureAI* GetAI(Creature *_Creature) const
	{
		return new  vehicle_alliance_steamtankAI(_Creature);
	}
};

#define NPC_GNOME 27163

class spell_drop_off_gnome : public SpellScriptLoader
{
	public:
		spell_drop_off_gnome() : SpellScriptLoader("spell_drop_off_gnome") {}

		class spell_drop_off_gnome_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_drop_off_gnome_SpellScript);
			
			void HandleScript(SpellEffIndex effIndex)
			{
				Unit* caster = GetCaster();
				caster->SummonCreature(NPC_GNOME, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000);
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_drop_off_gnome_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_drop_off_gnome_SpellScript();
		}
};

//This function is called when the player opens the gossip menubool

/*######
## npc_7th_legion_siege_engineer
######*/

#define ENTRY_ENGENEER                                  27163
#define ENTRY_SCOURGE_PLAGE_CATAPULT                    27607
#define SPELL_PLACE_SCOURGE_DISCOMBOBULATER             49114 // Summons Object for 15 secounds
#define SPELL_DESTURCTION                               49215
#define SPELL_DESTURCTION_TRIGGER                       49218
#define ENTRY_PLAGUE_PULT_CREDIT                        27625

class npc_7th_legion_siege_engineer : public CreatureScript
{
public:
	npc_7th_legion_siege_engineer() : CreatureScript("npc_7th_legion_siege_engineer") { }

	struct npc_7th_legion_siege_engineerAI : public ScriptedAI
	{
		npc_7th_legion_siege_engineerAI(Creature* creature) : ScriptedAI (creature) { }

		uint64 guid_owner;
		uint64 guid_pult;

		uint32 check_Timer;
		uint32 phase;

		void Reset()
		{
			guid_owner = 0;
			guid_pult = 0;
			phase = 0; //0 - Check for PlageCatapult, 1 - MoveToPult, 2 - Summon Object, 3 - Give Credit
			check_Timer = 2000;
		}

		void SpellHitTarget(Unit *target, const SpellEntry *spell)
		{
			if(spell->Id == SPELL_DESTURCTION_TRIGGER)
			{
				if(Player* owner = Unit::GetPlayer(*me,guid_owner))
		{
			owner->CastSpell(owner, 48813, true);
					owner->KilledMonsterCredit(ENTRY_PLAGUE_PULT_CREDIT,0);
		}
				if(target->ToCreature())
				{
					target->DealDamage(target,target->GetHealth());
					//target->ToCreature()->RemoveCorpse();
				}
			}
		}

		void MoveInLineOfSight(Unit *who)
		{
			if(guid_owner > 0)
				return;

			if(me->GetDistance2d(who) > 20)
				return;

			if(who->GetTypeId() == TYPEID_UNIT)
			{
			if (who->GetCharmer())
				sLog->outError("jmeno: %s", who->GetCharmer()->GetName());
				if(who->IsVehicle() && who->GetCharmer() && who->GetCharmer()->ToPlayer())
					guid_owner = who->GetCharmer()->ToPlayer()->GetGUID();
			}

			if(who->GetTypeId() == TYPEID_PLAYER)
			{
				guid_owner = who->ToPlayer()->GetGUID();
			}
		}

		void AttackStart(Unit *attacker){}

		void MovementInform(uint32 type, uint32 id)
		{
			if(type != POINT_MOTION_TYPE)
				return;

			if(id != 1)
				return;

			phase = 2;
			check_Timer = 2000;
		}

		void UpdateAI(const uint32 diff)
		{
			if(check_Timer <= diff)
			{
				switch(phase)
				{
				case 0:
					if(Unit::GetPlayer(*me,guid_owner))
					{
						Creature* pult = me->FindNearestCreature(ENTRY_SCOURGE_PLAGE_CATAPULT,50);
						if(pult)
						{
							guid_pult = pult->GetGUID();
							me->GetMotionMaster()->MovePoint(1,pult->GetPositionX(),pult->GetPositionY(),pult->GetPositionZ());
							phase = 1;
						}
					}
					else me->DealDamage(me,me->GetHealth());
					break;
				case 1:
					return;
				case 2:
					if(Creature* pult = Creature::GetCreature(*me,guid_pult))
					{
						me->CastSpell(pult,SPELL_DESTURCTION,true);
						me->CastSpell(me,SPELL_PLACE_SCOURGE_DISCOMBOBULATER,false);
						phase++;
				check_Timer = 10000;
					}
					break;
			case 3:
				if(Player* owner = Unit::GetPlayer(*me,guid_owner))
			{
				owner->CastSpell(owner, 48813, true);
				owner->KilledMonsterCredit(ENTRY_PLAGUE_PULT_CREDIT,0);
			}
			phase++;
			break;
				}
			}else check_Timer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_7th_legion_siege_engineerAI(creature);
	}
};
/*######
## npc_generic_harpoon_cannon
######*/

class npc_generic_harpoon_cannon : public CreatureScript
{
public:
	npc_generic_harpoon_cannon() : CreatureScript("npc_generic_harpoon_cannon") { }

	struct npc_generic_harpoon_cannonAI : public ScriptedAI
	{
		npc_generic_harpoon_cannonAI(Creature* creature) : ScriptedAI(creature) {}

		void Reset()
		{
			me->SetUnitMovementFlags(MOVEMENTFLAG_ROOT);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_generic_harpoon_cannonAI(creature);
	}
};

enum keristraszaData
{
	QUEST_SPRINGING_TRAP = 11969,

	NPC_KERISTRASZA = 26237,
	NPC_SARAGOSA    = 26299,
	NPC_MALYGOS     = 26310,
	GO_SIGNAL_FIRE  = 194151,

	SPELL_RAELORASZ_SPARK = 62272,
	SPELL_FIRE_CORPSE     = 39199,
	SPELL_FIRE_BREATH     = 31962,
	SPELL_MALYGOS_EARTHQ  = 46853,
	SPELL_TAXI_KERISTASZA = 46814,
	SPELL_ICE_BLOCK       = 56644,
	SPELL_FROSTBOLT       = 61461,

	SAY_KERISTRASZA_1     = -1002030,
	YELL_KERISTRASZA_1    = -1002031,
	YELL_KERISTRASZA_2    = -1002032,
	YELL_KERISTRASZA_3    = -1002033,
	SAY_KERISTRASZA_2     = -1002034,

	//After Fligth
	YELL_MALYGOS_1        = -1002035,
	SAY_KERISTRASZA_3     = -1002036,
	YELL_MALYGOS_2        = -1002037,
	YELL_KERISTRASZA_4    = -1002038,
};
const Position posKeristrasza[6] =
{
	{4157.00f, 7035.00f, 215.87f, 0.00f}, // Summon position
	{4063.72f, 7084.12f, 174.86f, 0.00f}, // Land position
	{4054.51f, 7084.29f, 168.12f, 0.00f}, // Burn Corpse positon
	{4048.90f, 7083.94f, 168.21f, 0.00f}, // Saragosa Corpse Spawn
	{3800.47f, 6557.50f, 170.98f, 1.55f}, // Keristrasza 2є Spawn
	{3791.76f, 6603.61f, 179.91f, 0.00f}, // Malygos Spawn
};
class npc_signal_fire : public CreatureScript
{
public:
	npc_signal_fire() : CreatureScript("npc_signal_fire") { }

	struct npc_signal_fireAI : public ScriptedAI
	{
		npc_signal_fireAI(Creature* creature) : ScriptedAI(creature) { }

		EventMap events;
		Creature* pKeristrasza;
		Creature* pSaragosa;
		Player* player;
		bool eventRunning;

		void Reset()
		{
			// Reset al variables
			events.Reset();
			pKeristrasza, pSaragosa, player = NULL;
			eventRunning = false;
			// Unlit signal
			if(GameObject* pGo = me->FindNearestGameObject(GO_SIGNAL_FIRE, 2.0f))
				pGo->SetGoState(GO_STATE_READY);
		}

		void SpellHit(Unit* caster, SpellEntry const* spell)
		{
			if (eventRunning)
				return;

			// The invisible trigger handles the event until the Fly of Keristrasza
			if(spell->Id == SPELL_RAELORASZ_SPARK)
			{
				player = caster->ToPlayer();
				// Lit the fire signal
				if(GameObject* pGo = me->FindNearestGameObject(GO_SIGNAL_FIRE, 2.0f))
					pGo->SetGoState(GO_STATE_ACTIVE);
				// Summon Keristrasza
				if(Creature* pFind = me->SummonCreature(NPC_KERISTRASZA, posKeristrasza[0]))
				{
					pKeristrasza = pFind;
					pFind->SetCanFly(true);
					pFind->GetMotionMaster()->MovePoint(1, posKeristrasza[1]);
				}
				eventRunning = true;
				events.ScheduleEvent(1, 6000);
			}
		}

		void UpdateAI(const uint32 diff)
		{
			events.Update(diff);

			switch(events.ExecuteEvent())
			{
			case 1:
				pKeristrasza->SetCanFly(false);
				DoScriptText(SAY_KERISTRASZA_1, pKeristrasza, player);
				events.ScheduleEvent(2, 3000);
				break;
			case 2:
				DoScriptText(YELL_KERISTRASZA_1, pKeristrasza);
				pKeristrasza->GetMotionMaster()->MovePoint(1, posKeristrasza[2]);
				events.ScheduleEvent(3, 5000);
				break;
			case 3:
				DoScriptText(YELL_KERISTRASZA_2, pKeristrasza);
				// Summon Saragosa and make her die
				if(Creature* pCorpse = me->SummonCreature(NPC_SARAGOSA, posKeristrasza[3]))
					pSaragosa = pCorpse;
				events.ScheduleEvent(4, 3000);
				break;
			case 4:
				DoScriptText(YELL_KERISTRASZA_3, pKeristrasza);
				pKeristrasza->CastSpell(pSaragosa, SPELL_FIRE_BREATH, true);
				events.ScheduleEvent(5, 1000);
				break;
			case 5:
				me->AddAura(SPELL_FIRE_CORPSE, pSaragosa);
				events.ScheduleEvent(6, 1000);
				break;
			case 6:
				player->CastSpell(player, SPELL_MALYGOS_EARTHQ, true);
				events.ScheduleEvent(7, 3000);
				break;
			case 7:
				DoScriptText(SAY_KERISTRASZA_2, pKeristrasza, player);
				events.ScheduleEvent(8, 3000);
				break;
			case 8:
				// Passes the control to Keristrasza
				player->CastSpell(player, SPELL_TAXI_KERISTASZA, true);
				pKeristrasza->AI()->SetGUID(player->GetGUID());
				pSaragosa->DespawnOrUnsummon(10000);
				events.ScheduleEvent(9, 20000);
				break;
			case 9:
				Reset();
				break;
			}
		}

	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_signal_fireAI(creature);
	}
};

class npc_keristrasza_coldarra : public CreatureScript
{
public:
	npc_keristrasza_coldarra() : CreatureScript("npc_keristrasza_coldarra") { }

	struct npc_keristrasza_coldarraAI : public ScriptedAI
	{
		npc_keristrasza_coldarraAI(Creature* creature) : ScriptedAI(creature) { }

		EventMap events;
		uint64 uiPlayer;
		bool waiting;
		bool finishedWay;
		Creature* pMalygos;

		void SetGUID(const uint64 &guid, int32 /*iId*/)
		{
			me->NearTeleportTo(posKeristrasza[4].GetPositionX(), posKeristrasza[4].GetPositionY(), posKeristrasza[4].GetPositionZ(), posKeristrasza[4].GetOrientation());
			me->SetVisible(false);
			uiPlayer = guid;
			waiting = true;
		}
		void Reset()
		{
			events.Reset();
			me->SetSpeed(MOVE_FLIGHT, 3.2f, true);
			pMalygos = NULL;
			uiPlayer = 0;
			waiting = false;
			finishedWay = false;
			me->SetReactState(REACT_PASSIVE);
		}


		void UpdateAI(const uint32 diff)
		{
			if(!waiting)
				return;

			if(!finishedWay)
			{
				if(Player* player = me->GetPlayer(*me, uiPlayer))
				{
					if (!player->isInFlight())
					{
						if(me->IsWithinDist(player, 10.0f))
						{
							finishedWay = true;
							me->SetVisible(true);
							player->ToPlayer()->KilledMonsterCredit(NPC_KERISTRASZA, 0);
							events.ScheduleEvent(1, 0);
						}else
							me->DespawnOrUnsummon(0);
					}
					else
						return;
				}
			}

			events.Update(diff);

			switch(events.ExecuteEvent())
			{
			case 1:
				me->SetCanFly(false);
				if(Player* player = me->GetPlayer(*me, uiPlayer))
					DoScriptText(SAY_KERISTRASZA_3, me, player);
				events.ScheduleEvent(2, 5000);
				break;
			case 2:
				if(Creature* pSumm = me->SummonCreature(NPC_MALYGOS, posKeristrasza[5]))
				{
					pMalygos = pSumm;
					pSumm->SetCanFly(true);
					pSumm->SetReactState(REACT_PASSIVE);
					pSumm->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_IMMUNE_TO_PC);
					pSumm->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.4f);
					DoScriptText(YELL_MALYGOS_2, pMalygos);
					pMalygos->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
					me->SetUInt64Value(UNIT_FIELD_TARGET, pMalygos->GetGUID());
				}
				events.ScheduleEvent(3, 6000);
				break;
			case 3:
				DoScriptText(YELL_KERISTRASZA_4, me);
				me->AddAura(SPELL_ICE_BLOCK, me);

				if(pMalygos)
					pMalygos->DespawnOrUnsummon(7000);
				me->DespawnOrUnsummon(7000);
				break;
			}

		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_keristrasza_coldarraAI(creature);
	}
};

enum AttractedReefBullData
{
	NPC_FEMALE_REEF_COW = 24797,
	SPELL_ANUNIAQS_NET = 21014,
	SPELL_TASTY_REEF_FISH = 44454,
	SPELL_LOVE_COSMETIC = 52148,
	ITEM_TASTY_REEF_FISH = 34127,
	QUEST_THE_WAY_TO_HIS_HEART = 11472
};

class npc_attracted_reef_bull : public CreatureScript
{
	public:

		npc_attracted_reef_bull() : CreatureScript("npc_attracted_reef_bull") {}

		struct npc_attracted_reef_bullAI : public ScriptedAI
		{
			npc_attracted_reef_bullAI(Creature* creature) : ScriptedAI(creature) {}

			uint64 playerGUID;
			uint8 point;

			void Reset()
			{
				playerGUID = 0;
				point = 0;
			}

			void UpdateAI(const uint32 diff) {}

			void SpellHit(Unit* caster, const SpellInfo* spell)
			{
				if (!caster->ToPlayer())
					return;

				if (spell->Id == SPELL_TASTY_REEF_FISH)
				{
					if (playerGUID == 0)
						playerGUID = caster->GetGUID();

					me->GetMotionMaster()->MovePoint(point, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
					++point;
				}

				if (Creature* female = me->FindNearestCreature(NPC_FEMALE_REEF_COW, 5.0f, true))
				{
					if (Player* player = me->GetPlayer(*me, playerGUID))
					{
						DoCast(me, SPELL_LOVE_COSMETIC);
						female->AI()->DoCast(female, SPELL_LOVE_COSMETIC);
						player->GroupEventHappens(QUEST_THE_WAY_TO_HIS_HEART, me);
						me->DespawnOrUnsummon(5000);
					}
				}
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_attracted_reef_bullAI(creature);
		}
};

class spell_anuniaqs_net : public SpellScriptLoader
{
public:
	spell_anuniaqs_net() : SpellScriptLoader("spell_anuniaqs_net") {}

	class spell_anuniaqs_net_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_anuniaqs_net_SpellScript);

		bool Validate(SpellInfo const* /*spellInfo*/)
		{
			if (!sSpellMgr->GetSpellInfo(SPELL_ANUNIAQS_NET))
				return false;
			return true;
		}

		void HandleDummy(SpellEffIndex /*effIndex*/)
		{
			if (Unit* caster = GetCaster())
				if (caster->ToPlayer())
					caster->ToPlayer()->AddItem(ITEM_TASTY_REEF_FISH, urand(1,5));
		}

		void Register()
		{
			OnEffectHit += SpellEffectFn(spell_anuniaqs_net_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_anuniaqs_net_SpellScript();
	}
};

class npc_q13047_veranus : public CreatureScript
{
public:
	npc_q13047_veranus() : CreatureScript("npc_q13047_veranus") { }
	
	struct npc_q13047_veranusAI : public npc_escortAI
	{
		npc_q13047_veranusAI(Creature* creature) : npc_escortAI(creature)
		{
			AddWaypoint(0, 8558.0f, -626.0f, 970.0f, 0);
			AddWaypoint(1, 8534.0f, -584.0f, 979.0f, 0);
			AddWaypoint(2, 8551.0f, -549.0f, 977.0f, 0);
			AddWaypoint(3, 8585.0f, -559.0f, 973.0f, 0);
			AddWaypoint(4, 8617.0f, -591.0f, 963.0f, 0);
			AddWaypoint(5, 8597.0f, -621.0f, 966.0f, 0);
		}
		
		void WaypointReached(uint32)
		{
			
		}
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_q13047_veranusAI(creature);
	}
};

class npc_q13047_thorim : public CreatureScript
{
public:
	npc_q13047_thorim() : CreatureScript("npc_q13047_thorim") { }
	
	enum The_Reckoning
	{
		ACTION_START = 578,
		
		TALK_THORIM_1 = 0,
		TALK_THORIM_2 = 1,
		TALK_THORIM_3 = 2,
		TALK_THORIM_4 = 3,
		TALK_THORIM_5 = 4,
		TALK_THORIM_6 = 5,
		
		TALK_LOKEN_1 = 0,
		TALK_LOKEN_2 = 1,
		TALK_LOKEN_3 = 2,
		TALK_LOKEN_4 = 3,
		TALK_LOKEN_5 = 4,
		TALK_LOKEN_6 = 5,
		TALK_LOKEN_7 = 6,
		TALK_LOKEN_8 = 7,
		
		NPC_RUNEFORGED_SERVANT = 30429,
		NPC_LOKEN = 30396,
		NPC_VERANUS = 30461,
		
		QUEST_RECKONING = 13047,
		
		SPELL_RECKONING_COMPLETE = 56941,
	};
	
	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
		if (pPlayer->GetQuestStatus(QUEST_RECKONING) == QUEST_STATUS_INCOMPLETE)
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'm with you Thorim.", GOSSIP_SENDER_MAIN, ACTION_START);
		
		pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
		return true;
	}
	
	bool OnGossipSelect(Player* player, Creature* creature, uint32 uiSender, uint32 uiAction)
	{
		if (uiAction == ACTION_START)
		{
			creature->AI()->SetGUID(player->GetGUID(), ACTION_START);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			player->CLOSE_GOSSIP_MENU();
			return true;
		}
		return false;
	}
	
	struct npc_q13047_thorimAI : public npc_escortAI
	{
		npc_q13047_thorimAI(Creature* creature) : npc_escortAI(creature), summons(me)
		{
			AddWaypoint(0, 8703.82f, -715.856f, 934.998f, 2000);
			AddWaypoint(1, 8681.2f, -693.1f, 931.6f, 0);
			AddWaypoint(2, 8656.2f, -668.1f, 924.0f, 0);
			AddWaypoint(3, 8638.4f, -649.7f, 923.6f, 5000);
			AddWaypoint(4, 8621.3f, -633.0f, 926.8f, 0);
			AddWaypoint(5, 8568.9f, -585.3f, 925.6f, 0);
		}
		
		void Reset()
		{
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			stage = 0;
			stageTimer = 0;
			playerGUID = 0;
		}
		
		void SetGUID(uint64 guid, int32 type)
		{
			if (type != ACTION_START)
				return;
			
			Talk(TALK_THORIM_1);
			stage = 1;
			stageTimer = 2000;
			playerGUID = guid;
		}
		
		void WaypointReached (uint32 pointId)
		{
			switch (pointId)
			{
				case 3:
					Talk(TALK_THORIM_2);
					SetRun(true);
					break;
				case 5:
					if (Creature* veranus = me->FindNearestCreature(NPC_VERANUS, 100.0f))
						CAST_AI(npc_q13047_veranus::npc_q13047_veranusAI, veranus->AI())->Start(false, false, 0, NULL, false, true);
					
					Talk(TALK_THORIM_3);
					stage = 2;
					stageTimer = 5000;
					break;
			}
		}
		
		void JustSummoned(Creature* summon)
		{
			summon->CastSpell(summon, SPELL_TELEPORT_VISUAL, true);
			summons.Summon(summon);
		}
		
		void UpdateAI(const uint32 diff)
		{
			if (stageTimer)
			{
				if (stageTimer < diff)
					switch (stage)
					{
						case 1:
							if (Creature* veranus = me->FindNearestCreature(NPC_VERANUS, 100.0f))
								veranus->GetMotionMaster()->MovePoint(625, 8597.0f, -621.0f, 956.0f);
							
							SetDespawnAtEnd(false);
							SetDespawnAtFar(false);
							Start(false);
							stageTimer = 0;
							break;
						case 2:
							//odhodit, skocit
							LokenTalk(TALK_LOKEN_1);
							stage++;
							stageTimer = 5000;
							break;
						case 3:
							Talk(TALK_THORIM_4);
							stage++;
							stageTimer = 3000;
							break;
						case 4:
							//cast spelly
							Talk(TALK_THORIM_5);
							stage++;
							stageTimer = 1000;
							break;
						case 5:
							Talk(TALK_THORIM_6);
							stage++;
							stageTimer = 1500;
							break;
						case 6:
							LokenTalk(TALK_LOKEN_2);
							stage++;
							stageTimer = 2000;
							break;
						case 7:
							LokenTalk(TALK_LOKEN_3);
							stage++;
							stageTimer = 5000;
							break;
						case 8:
							//cast chapadla
							LokenTalk(TALK_LOKEN_4);
							stage++;
							stageTimer = 5000;
							break;
						
						case 9:
							LokenTalk(TALK_LOKEN_5);
							stage++;
							stageTimer = 12000;
							break;
						case 10:
							me->SummonCreature(NPC_RUNEFORGED_SERVANT, 8582.5f, -569.0f, 925.6f, 4.6f);
							me->SummonCreature(NPC_RUNEFORGED_SERVANT, 8605.8f, -594.1f, 925.6f, 2.8f);
							LokenTalk(TALK_LOKEN_6);
							stage++;
							stageTimer = 12000;
							break;
						case 11:
							LokenTalk(TALK_LOKEN_7);
							stage++;
							stageTimer = 13000;
							break;
						case 12:
							LokenTalk(TALK_LOKEN_8);
							stage++;
							stageTimer = 13000;
							break;
						
						case 13:
							for (std::list<uint64>::iterator i = summons.begin(); i != summons.end(); i++)
								if (Creature* summon = Unit::GetCreature(*me, *i))
									summon->CastSpell(summon, SPELL_TELEPORT_VISUAL, true);
							
							if (Creature* loken = me->FindNearestCreature(NPC_LOKEN, 100.0f))
							{
								loken->CastSpell(loken, SPELL_TELEPORT_VISUAL, true);
								loken->CastSpell(me, SPELL_TELEPORT_VISUAL, true);
							}
							
							if (Creature* veranus = me->FindNearestCreature(NPC_VERANUS, 100.0f))
								veranus->CastSpell(veranus, SPELL_TELEPORT_VISUAL, true);
							
							if (Player* player = Unit::GetPlayer(*me, playerGUID))
								player->CastSpell(player, SPELL_RECKONING_COMPLETE, true);
							
							stageTimer = 2000;
							stage++;
							break;
						case 14:
							summons.DespawnAll();
							
							if (Creature* loken = me->FindNearestCreature(NPC_LOKEN, 100.0f))
							{
								loken->Kill(loken);
								loken->RemoveCorpse();
							}
							
							if (Creature* veranus = me->FindNearestCreature(NPC_VERANUS, 100.0f))
							{
								veranus->Kill(veranus);
								veranus->RemoveCorpse();
							}
							
							me->Kill(me);
							me->RemoveCorpse();
							
							stageTimer = 0;
							break;
					}
				else
					stageTimer -= diff;
			}
			
			npc_escortAI::UpdateAI(diff);
		}
		
		void LokenTalk(uint32 talk)
		{
			if (Creature* loken = me->FindNearestCreature(NPC_LOKEN, 100.0f))
				loken->AI()->Talk(talk);
		}
		
		uint8 stage;
		uint32 stageTimer;
		uint64 playerGUID;
		SummonList summons;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_q13047_thorimAI(creature);
	}
};

class npc_q12467_wyrmbait : public CreatureScript
{
public:
	npc_q12467_wyrmbait() : CreatureScript("npc_q12467_wyrmbait") { }
	
	enum Q12467
	{
		NPC_SPAWN = 26287,
	};
	
	bool OnGossipSelect(Player* player, Creature* creature, uint32 uiSender, uint32 uiAction)
	{
		if (uiAction == 1)
		{
			CAST_AI(npc_q12467_wyrmbait::npc_q12467_wyrmbaitAI, creature->AI())->timer = 5000;
			player->CLOSE_GOSSIP_MENU();
			return true;
		}
		return false;
	}
	
	struct npc_q12467_wyrmbaitAI : public AggressorAI
	{
		npc_q12467_wyrmbaitAI(Creature* creature) : AggressorAI(creature)
		{
			timer = 0;
		}
		
		void UpdateAI(const uint32 diff)
		{
			if (timer)
			{
				if (timer < diff)
				{
					me->SummonCreature(NPC_SPAWN, 4540.03f, 46.845f, 81.45f, 4.458f);
					me->MonsterSay("I'll flush her out. bastek! You just be ready for her when she comes in!", LANG_UNIVERSAL, 0);
					timer = 0;
				}
				else
					timer -= diff;
			}
			
			AggressorAI::UpdateAI(diff);
		}
		
	public:
		uint32 timer;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_q12467_wyrmbaitAI(creature);
	}
};

enum eSlim
{
	QUEST_ITEM_SLIM = 36765,
	QUEST_SLIM_PICKING = 12075,
};

class npc_slim_giant : public CreatureScript
{
public:
	npc_slim_giant() : CreatureScript("npc_slim_giant") { }

	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
		if (pPlayer->GetQuestStatus(QUEST_SLIM_PICKING) == QUEST_STATUS_INCOMPLETE)
		{
			pPlayer->PlayerTalkClass->ClearMenus();
			pPlayer->AddItem(QUEST_ITEM_SLIM, 1);
		}
		pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

		return true;
	}
};

#define GOSSIP_ROANAUK1 "Greetings, High Chief. Would you do me the honor of accepting my invitation to join the horde as an official member and leader of the Taunka?"
#define GOSSIP_ROANAUK2 "It is you who honor me, High Chief. Please read form this scroll. It is the oath of alegiance."
#define QUEST_ALL_HAIL_ROANAUK 12140
#define NPC_ROANAUK 26810

class npc_roanauk : public CreatureScript
{
public:
	npc_roanauk() : CreatureScript("npc_roanauk") { }

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (uiAction)
		{
			case GOSSIP_ACTION_INFO_DEF:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROANAUK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
				player->SEND_GOSSIP_MENU(268100, creature->GetGUID());
			break;
			case GOSSIP_ACTION_INFO_DEF+1:
				player->SEND_GOSSIP_MENU(268101, creature->GetGUID());
				player->KilledMonsterCredit(NPC_ROANAUK, creature->GetGUID());
			break;
		}
		return true;
	}

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->isQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->GetQuestStatus(QUEST_ALL_HAIL_ROANAUK) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROANAUK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(268100, creature->GetGUID());
			return true;
	}

};

/*####################
# spell_contact_brann
#####################*/

enum ContactBrannData
{
	SPELL_CONTACT_BRANN = 61122,
	NPC_BRANN = 29579,
	ZONE_STORM_PEAKS = 67
};

class spell_contact_brann : public SpellScriptLoader
{
public:
	spell_contact_brann() : SpellScriptLoader("spell_contact_brann") {}

	class spell_contact_brann_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_contact_brann_SpellScript)

		bool Validate(SpellInfo const* /*spell*/)
		{
			if (!sSpellMgr->GetSpellInfo(SPELL_CONTACT_BRANN))
				return false;
			return true;
		}

		void HandleScriptEffect(SpellEffIndex effIndex)
		{
			PreventHitDefaultEffect(effIndex);
			if (Unit* caster = GetCaster())
			   if (caster->ToPlayer())
				   if (caster->ToPlayer()->GetZoneId() == ZONE_STORM_PEAKS)
					   caster->SummonCreature(NPC_BRANN, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 120000);
		}

		void Register()
		{
			OnEffectHit += SpellEffectFn(spell_contact_brann_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_contact_brann_SpellScript();
	}
};

class spell_summon_frosthound : public SpellScriptLoader
{
	enum Q12855
	{
		NPC_FROSTHOUND = 29677,
	};
	
	public:
		spell_summon_frosthound() : SpellScriptLoader("spell_summon_frosthound") {}

		class spell_summon_frosthound_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_summon_frosthound_SpellScript);
			void HandleScript(SpellEffIndex effIndex)
			{
				PreventHitEffect(effIndex);
				if (Unit* caster = GetCaster())
					if (Creature* frosthound = caster->SummonCreature(NPC_FROSTHOUND, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), caster->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 600000))
						caster->EnterVehicle(frosthound);
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_summon_frosthound_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SUMMON);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_summon_frosthound_SpellScript();
		}
};

const Position Q12910_hound_waypoints [20] =
{
	{7160.060547f, -767.918152f, 891.917419f, 0.0f},
	{7175.936523f, -790.965271f, 902.725464f, 0.0f},
	{7197.205566f, -800.673584f, 918.384155f, 0.0f},
	{7215.651367f, -814.114746f, 923.765747f, 0.0f},
	{7232.377930f, -838.122375f, 926.780701f, 0.0f},
	{7257.223145f, -880.518677f, 924.275635f, 0.0f},
	{7267.757813f, -910.159058f, 927.182739f, 0.0f},
	{7282.507324f, -966.717529f, 918.796997f, 0.0f},
	{7336.709473f, -1045.830933f, 907.056091f, 0.0f},
	{7342.393066f, -1066.118286f, 908.231323f, 0.0f},
	{7350.359863f, -1090.668457f, 904.792297f, 0.0f},
	{7362.481934f, -1110.543335f, 909.879333f, 0.0f},
	{7369.148926f, -1130.921265f, 904.542664f, 0.0f},
	{7398.185059f, -1240.504150f, 902.795959f, 0.0f},
	{7400.799316f, -1301.207886f, 905.109375f, 0.0f},
	{7398.059570f, -1340.795288f, 910.486389f, 0.0f},
	{7353.894043f, -1410.990845f, 913.063477f, 0.0f},
	{7321.494141f, -1474.212891f, 919.503723f, 0.0f},
	{7319.132813f, -1508.736206f, 923.113525f, 0.0f},
	{7302.073242f, -1559.915161f, 939.286438f, 0.0f},
};

class npc_q12910_frosthound : public CreatureScript
{
public:
	npc_q12910_frosthound() : CreatureScript("npc_q12910_frosthound") { }
	
	enum Q12910
	{
		SPELL_COMPLETE_SNIFF = 55477,
		NPC_DWARF = 29652,
		
		MAX_WAYPOINT = 19,
		
		FROSTHOUND_EMOTE_START = 0,
		FROSTHOUND_EMOTE_DAMAGE = 1,
		FROSTHOUND_EMOTE_END = 2,
	};
	
	struct npc_q12910_frosthoundAI : public NullCreatureAI
	{
		npc_q12910_frosthoundAI(Creature* creature) : NullCreatureAI(creature)
		{
			checkPlayerTimer = 1000;
			spawnDwarfCooldown = 5000;
			spawnDwarfTimer = 0;
			waypoint = 0;
			belowHP = 0;
			walking = false;
		}
		
		void MovementInform(uint32 type, uint32 point)
		{
			if (type != POINT_MOTION_TYPE)
				return;
			
			if (waypoint == MAX_WAYPOINT)
			{
				TalkHound(FROSTHOUND_EMOTE_END);
				me->CastSpell(me, SPELL_COMPLETE_SNIFF, true);
				me->GetVehicleKit()->RemoveAllPassengers();
				me->DespawnOrUnsummon(1000);
				return;
			}
			
			waypoint++;
			walking = false;
			
		}
		
		void UpdateAI(uint32 const diff)
		{
			if (checkPlayerTimer < diff)
			{
				if (!me->GetVehicleKit()->GetPassenger(0))
					me->DespawnOrUnsummon();
				
				checkPlayerTimer = 2000;
			}
			else
				checkPlayerTimer -= diff;
			
			if (me->HealthBelowPct(75 - belowHP*25))
			{
				belowHP++;
				TalkHound(FROSTHOUND_EMOTE_DAMAGE);
			}
			
			if (!walking)
			{
				TalkHound(FROSTHOUND_EMOTE_START);
				me->GetMotionMaster()->MovePoint(waypoint, Q12910_hound_waypoints[waypoint]);
				walking = true;
			}
			
			if (spawnDwarfCooldown < diff)
			{
				spawnDwarfPosition.Relocate(me);
				spawnDwarfTimer = 300;
				spawnDwarfCooldown = urand(5000, 12000);
				
			}
			else
				spawnDwarfCooldown -= diff;
			
			if (spawnDwarfTimer)
			{
				if (spawnDwarfTimer < diff)
				{
					Creature* newDwarf = me->SummonCreature(NPC_DWARF, spawnDwarfPosition, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500);
					newDwarf->SetInCombatWith(me);
					me->SetInCombatWith(newDwarf);
					newDwarf->AddThreat(me, 1000.0f);
					me->AddThreat(newDwarf, 1000.0f);
					
					spawnDwarfTimer = 0;
				}
				else
					spawnDwarfTimer -= diff;
			}
		}
		
		void TalkHound(uint32 talk)
		{
			Unit* player = me->GetVehicleKit()->GetPassenger(0);
			Talk(talk, player->GetGUID());
		}
		
		Position spawnDwarfPosition;
		uint32 spawnDwarfCooldown;
		uint32 spawnDwarfTimer;
		uint32 checkPlayerTimer;
		uint8 waypoint;
		uint8 belowHP;
		bool walking;
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_q12910_frosthoundAI(creature);
	}
};

/*######
## npc_archivist_mechaton
######*/

enum Mechaton
{
	NPC_ARCHIVIST_MECHATON  = 29775,
	SPELL_ARCHIVISTS_SCAN   = 55224,
	SPELL_CHARGED_DISK      = 55197,
	GO_FLOOR_GLYPH          = 191762,
	GO_ENERGY_COLUMN        = 191763,
	SAY_1                   = 1,
	SAY_2                   = 2,
	SAY_3                   = 3,
	SAY_4                   = 4,
	EMOTE_1                 = 5,
	SAY_5                   = 6,
	SAY_6                   = 7,
	SAY_7                   = 8
};

class npc_archivist_mechaton : public CreatureScript
{
public:
	npc_archivist_mechaton() : CreatureScript("npc_archivist_mechaton") {}

	struct npc_archivist_mechatonAI : public ScriptedAI
	{
		npc_archivist_mechatonAI(Creature* creature) : ScriptedAI(creature)
		{
			if (me->isSummon())
			{
				me->SummonGameObject(GO_FLOOR_GLYPH, 7991.89f, -827.66f, 968.156f, -2.33874f, 0, 0, 0, 0, 27);
				me->SummonGameObject(GO_ENERGY_COLUMN, 7991.8f, -827.639f, 968.16f, 0.90757f, 0, 0, 0, 0, 27);
			}

			FirstTime = true;
		}

		uint8 saycount;
		uint32 saytimer;
		bool FirstTime;

		void Reset()
		{
			saytimer = 0;

			if (FirstTime)
				saycount = 1;
			else
			{
				saycount = 0;
				me->DespawnOrUnsummon();
			}
		}

		void DoNextText(uint32 timer)
		{
			saytimer = timer;
			++saycount;
		}

		void UpdateAI(uint32 const diff)
		{
			if (saytimer <= diff)
			{
				Unit* summoner = me->ToTempSummon();
				if (!summoner)
				{
					sLog->outError("ERROR: npc_archivist_mechaton spawnut bez summonera");
					me->DespawnOrUnsummon();
					return;
				}
				summoner = me->ToTempSummon()->GetSummoner();
				if (!summoner)
				{
					sLog->outError("ERROR: npc_archivist_mechaton spawnut bez summonera");
					me->DespawnOrUnsummon();
					return;
				}

				switch (saycount)
				{
					case 1:
						Talk(SAY_1);
						DoNextText(4000);
						break;
					case 2:
						Talk(SAY_2);
						DoNextText(3000);
						break;
					case 3:
						Talk(SAY_3);
						DoNextText(4000);
						break;
					case 4:
						Talk(SAY_4);
						me->CastSpell(summoner, SPELL_ARCHIVISTS_SCAN, true);
						DoNextText(1100);
						break;
					case 5:
						Talk(EMOTE_1);
						DoNextText(8000);
						break;
					case 6:
						Talk(SAY_5);
						me->CombatStop();
						DoNextText(4000);
						break;
					case 7:
						Talk(SAY_6);
						DoNextText(5000);
						break;
					case 8:
						Talk(SAY_7);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						me->SetReactState(REACT_AGGRESSIVE);
						AttackStart(summoner);
						DoNextText(0);
						FirstTime = false;
						break;
				}
			}
			else
				saytimer -= diff;

			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_archivist_mechatonAI(creature);
	}
};

enum Orphans
{
	ORPHAN_ORACLE                           = 33533,
	ORPHAN_WOLVAR                           = 33532,
	ORPHAN_BLOOD_ELF                        = 22817,
	ORPHAN_DRAENEI                          = 22818,
	ORPHAN_HUMAN                            = 14305,
	ORPHAN_ORCISH                           = 14444,
};

enum Texts_Orphans
{
	TEXT_ORACLE_ORPHAN_1                    = 1,
	TEXT_ORACLE_ORPHAN_2                    = 2,
	TEXT_ORACLE_ORPHAN_3                    = 3,
	TEXT_ORACLE_ORPHAN_4                    = 4,
	TEXT_ORACLE_ORPHAN_5                    = 5,
	TEXT_ORACLE_ORPHAN_6                    = 6,
	TEXT_ORACLE_ORPHAN_7                    = 7,
	TEXT_ORACLE_ORPHAN_8                    = 8,
	TEXT_ORACLE_ORPHAN_9                    = 9,
	TEXT_ORACLE_ORPHAN_10                   = 10,
	TEXT_ORACLE_ORPHAN_11                   = 11,
	TEXT_ORACLE_ORPHAN_12                   = 12,
	TEXT_ORACLE_ORPHAN_13                   = 13,
	TEXT_ORACLE_ORPHAN_14                   = 14,

	TEXT_WOLVAR_ORPHAN_1                    = 1,
	TEXT_WOLVAR_ORPHAN_2                    = 2,
	TEXT_WOLVAR_ORPHAN_3                    = 3,
	TEXT_WOLVAR_ORPHAN_4                    = 4,
	TEXT_WOLVAR_ORPHAN_5                    = 5,
	// 6 - 9 used in Nesingwary script
	TEXT_WOLVAR_ORPHAN_10                   = 10,
	TEXT_WOLVAR_ORPHAN_11                   = 11,
	TEXT_WOLVAR_ORPHAN_12                   = 12,
	TEXT_WOLVAR_ORPHAN_13                   = 13,

	TEXT_WINTERFIN_PLAYMATE_1               = 1,
	TEXT_WINTERFIN_PLAYMATE_2               = 2,

	TEXT_SNOWFALL_GLADE_PLAYMATE_1          = 1,
	TEXT_SNOWFALL_GLADE_PLAYMATE_2          = 2,

	TEXT_SOO_ROO_1                          = 1,
	TEXT_ELDER_KEKEK_1                      = 1,

	TEXT_ALEXSTRASZA_2                      = 2,
	TEXT_KRASUS_8                           = 8,
};

enum Quests_Orphans
{
	QUEST_PLAYMATE_WOLVAR                   = 13951,
	QUEST_PLAYMATE_ORACLE                   = 13950,
	QUEST_THE_BIGGEST_TREE_EVER             = 13929,
	QUEST_THE_BRONZE_DRAGONSHRINE_ORACLE    = 13933,
	QUEST_THE_BRONZE_DRAGONSHRINE_WOLVAR    = 13934,
	QUEST_MEETING_A_GREAT_ONE               = 13956,
	QUEST_THE_MIGHTY_HEMET_NESINGWARY       = 13957,
	QUEST_DOWN_AT_THE_DOCKS                 = 910,
	QUEST_GATEWAY_TO_THE_FRONTIER           = 911,
	QUEST_BOUGHT_OF_ETERNALS                = 1479,
	QUEST_SPOOKY_LIGHTHOUSE                 = 1687,
	QUEST_STONEWROUGHT_DAM                  = 1558,
	QUEST_DARK_PORTAL_H                     = 10951,
	QUEST_DARK_PORTAL_A                     = 10952,
	QUEST_LORDAERON_THRONE_ROOM             = 1800,
	QUEST_AUCHINDOUN_AND_THE_RING           = 10950,
	QUEST_TIME_TO_VISIT_THE_CAVERNS_H       = 10963,
	QUEST_TIME_TO_VISIT_THE_CAVERNS_A       = 10962,
	QUEST_THE_SEAT_OF_THE_NARUU             = 10956,
	QUEST_CALL_ON_THE_FARSEER               = 10968,
	QUEST_JHEEL_IS_AT_AERIS_LANDING         = 10954,
	QUEST_HCHUU_AND_THE_MUSHROOM_PEOPLE     = 10945,
	QUEST_VISIT_THE_THRONE_OF_ELEMENTS      = 10953,
	QUEST_NOW_WHEN_I_GROW_UP                = 11975,
	QUEST_HOME_OF_THE_BEAR_MEN              = 13930,
	QUEST_THE_DRAGON_QUEEN_ORACLE           = 13954,
	QUEST_THE_DRAGON_QUEEN_WOLVAR           = 13955,
};

enum Areatriggers_Orphans
{
	AT_DOWN_AT_THE_DOCKS                    = 3551,
	AT_GATEWAY_TO_THE_FRONTIER              = 3549,
	AT_LORDAERON_THRONE_ROOM                = 3547,
	AT_BOUGHT_OF_ETERNALS                   = 3546,
	AT_SPOOKY_LIGHTHOUSE                    = 3552,
	AT_STONEWROUGHT_DAM                     = 3548,
	AT_DARK_PORTAL                          = 4356,

	NPC_CAVERNS_OF_TIME_CW_TRIGGER          = 22872,
	NPC_EXODAR_01_CW_TRIGGER                = 22851,
	NPC_EXODAR_02_CW_TRIGGER                = 22905,
	NPC_AERIS_LANDING_CW_TRIGGER            = 22838,
	NPC_AUCHINDOUN_CW_TRIGGER               = 22831,
	NPC_SPOREGGAR_CW_TRIGGER                = 22829,
	NPC_THRONE_OF_ELEMENTS_CW_TRIGGER       = 22839,
	NPC_SILVERMOON_01_CW_TRIGGER            = 22866,
	NPC_KRASUS                              = 27990,
};

enum Misc_Orphans
{
	SPELL_SNOWBALL                          = 21343,
	SPELL_ORPHAN_OUT                        = 58818,

	DISPLAY_INVISIBLE                       = 11686,
};

uint64 getOrphanGUID(Player* player, uint32 orphan)
{
	if (Aura* orphanOut = player->GetAura(SPELL_ORPHAN_OUT))
		if (orphanOut->GetCaster() && orphanOut->GetCaster()->GetEntry() == orphan)
			return orphanOut->GetCaster()->GetGUID();

	return 0;
}

/*######
## npc_winterfin_playmate
######*/
class npc_winterfin_playmate : public CreatureScript
{
	public:
		npc_winterfin_playmate() : CreatureScript("npc_winterfin_playmate") {}

		struct npc_winterfin_playmateAI : public ScriptedAI
		{
			npc_winterfin_playmateAI(Creature* creature) : ScriptedAI(creature) {}

			void Reset()
			{
				timer = 0;
				phase = 0;
				playerGUID = 0;
				orphanGUID = 0;
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (!phase && who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
						if (player->GetQuestStatus(QUEST_PLAYMATE_ORACLE) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
							if (orphanGUID)
								phase = 1;
						}
			}

			void UpdateAI(const uint32 diff)
			{
				if (!phase)
					return;

				if (timer <= diff)
				{
					Player* player = Player::GetPlayer(*me, playerGUID);
					Creature* orphan = Creature::GetCreature(*me, orphanGUID);

					if (!orphan || !player)
					{
						Reset();
						return;
					}

					switch (phase)
					{
						case 1:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_1);
							timer = 3000;
							break;
						case 2:
							orphan->SetFacingToObject(me);
							Talk(TEXT_WINTERFIN_PLAYMATE_1);
							me->HandleEmoteCommand(EMOTE_STATE_DANCE);
							timer = 3000;
							break;
						case 3:
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_2);
							timer = 3000;
							break;
						case 4:
							Talk(TEXT_WINTERFIN_PLAYMATE_2);
							timer = 5000;
							break;
						case 5:
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_3);
							me->HandleEmoteCommand(EMOTE_STATE_NONE);
							player->GroupEventHappens(QUEST_PLAYMATE_ORACLE, me);
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							Reset();
							return;
					}
					++phase;
				}
				else
					timer -= diff;
			}

		private:
			uint32 timer;
			int8 phase;
			uint64 playerGUID;
			uint64 orphanGUID;

		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_winterfin_playmateAI(creature);
		}
};

/*######
## npc_snowfall_glade_playmate
######*/
class npc_snowfall_glade_playmate : public CreatureScript
{
	public:
		npc_snowfall_glade_playmate() : CreatureScript("npc_snowfall_glade_playmate") {}

		struct npc_snowfall_glade_playmateAI : public ScriptedAI
		{
			npc_snowfall_glade_playmateAI(Creature* creature) : ScriptedAI(creature) {}

			void Reset()
			{
				timer = 0;
				phase = 0;
				playerGUID = 0;
				orphanGUID = 0;
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (!phase && who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
						if (player->GetQuestStatus(QUEST_PLAYMATE_WOLVAR) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_WOLVAR);
							if (orphanGUID)
								phase = 1;
						}
			}

			void UpdateAI(const uint32 diff)
			{
				if (!phase)
					return;

				if (timer <= diff)
				{
					Player* player = Player::GetPlayer(*me, playerGUID);
					Creature* orphan = Creature::GetCreature(*me, orphanGUID);

					if (!orphan || !player)
					{
						Reset();
						return;
					}

					switch (phase)
					{
						case 1:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_1);
							timer = 5000;
							break;
						case 2:
							orphan->SetFacingToObject(me);
							Talk(TEXT_SNOWFALL_GLADE_PLAYMATE_1);
							DoCast(orphan, SPELL_SNOWBALL);
							timer = 5000;
							break;
						case 3:
							Talk(TEXT_SNOWFALL_GLADE_PLAYMATE_2);
							timer = 5000;
							break;
						case 4:
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_2);
							orphan->AI()->DoCast(me, SPELL_SNOWBALL);
							timer = 5000;
							break;
						case 5:
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_3);
							player->GroupEventHappens(QUEST_PLAYMATE_WOLVAR, me);
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							Reset();
							return;
					}
					++phase;
				}
				else
					timer -= diff;
			}

		private:
			uint32 timer;
			int8 phase;
			uint64 playerGUID;
			uint64 orphanGUID;
		};

		CreatureAI* GetAI(Creature* pCreature) const
		{
			return new npc_snowfall_glade_playmateAI(pCreature);
		}
};

/*######
## npc_the_biggest_tree
######*/
class npc_the_biggest_tree : public CreatureScript
{
	public:
		npc_the_biggest_tree() : CreatureScript("npc_the_biggest_tree") {}

		struct npc_the_biggest_treeAI : public ScriptedAI
		{
			npc_the_biggest_treeAI(Creature* creature) : ScriptedAI(creature)
			{
				me->SetDisplayId(DISPLAY_INVISIBLE);
			}

			void Reset()
			{
				timer = 1000;
				phase = 0;
				playerGUID = 0;
				orphanGUID = 0;
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (!phase && who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
						if (player->GetQuestStatus(QUEST_THE_BIGGEST_TREE_EVER) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
							if (orphanGUID)
								phase = 1;
						}
			}

			void UpdateAI(const uint32 diff)
			{
				if (!phase)
					return;

				if (timer <= diff)
				{
					Player* player = Player::GetPlayer(*me, playerGUID);
					Creature* orphan = Creature::GetCreature(*me, orphanGUID);

					if (!orphan || !player)
					{
						Reset();
						return;
					}

					switch (phase)
					{
						case 1:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							timer = 2000;
							break;
						case 2:
							orphan->SetFacingToObject(me);
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_4);
							timer = 5000;
							break;
						case 3:
							player->GroupEventHappens(QUEST_THE_BIGGEST_TREE_EVER, me);
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							Reset();
							return;
					}
					++phase;
				}
				else
					timer -= diff;
			}

		private:
			uint32 timer;
			uint8 phase;
			uint64 playerGUID;
			uint64 orphanGUID;

		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_the_biggest_treeAI(creature);
		}
};

/*######
## npc_high_oracle_soo_roo
######*/
class npc_high_oracle_soo_roo : public CreatureScript
{
	public:
		npc_high_oracle_soo_roo() : CreatureScript("npc_high_oracle_soo_roo") {}

		struct npc_high_oracle_soo_rooAI : public ScriptedAI
		{
			npc_high_oracle_soo_rooAI(Creature* creature) : ScriptedAI(creature) {}

			void Reset()
			{
				timer = 0;
				phase = 0;
				playerGUID = 0;
				orphanGUID = 0;
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (!phase && who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
						if (player->GetQuestStatus(QUEST_THE_BRONZE_DRAGONSHRINE_ORACLE) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
							if (orphanGUID)
								phase = 1;
						}
			}

			void UpdateAI(const uint32 diff)
			{
				if (!phase)
					return;

				if (timer <= diff)
				{
					Player* player = Player::GetPlayer(*me, playerGUID);
					Creature* orphan = Creature::GetCreature(*me, orphanGUID);

					if (!orphan || !player)
					{
						Reset();
						return;
					}

					switch (phase)
					{
						case 1:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_5);
							timer = 3000;
							break;
						case 2:
							orphan->SetFacingToObject(me);
							Talk(TEXT_SOO_ROO_1);
							timer = 6000;
							break;
						case 3:
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_6);
							player->GroupEventHappens(QUEST_THE_BRONZE_DRAGONSHRINE_ORACLE, me);
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							Reset();
							return;
					}
					++phase;
				}
				else
					timer -= diff;
			}

		private:
			uint32 timer;
			int8 phase;
			uint64 playerGUID;
			uint64 orphanGUID;

		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_high_oracle_soo_rooAI(creature);
		}
};

/*######
## npc_elder_kekek
######*/
class npc_elder_kekek : public CreatureScript
{
	public:
		npc_elder_kekek() : CreatureScript("npc_elder_kekek") {}

		struct npc_elder_kekekAI : public ScriptedAI
		{
			npc_elder_kekekAI(Creature* creature) : ScriptedAI(creature) {}

			void Reset()
			{
				timer = 0;
				phase = 0;
				playerGUID = 0;
				orphanGUID = 0;
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (!phase && who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
						if (player->GetQuestStatus(QUEST_THE_BRONZE_DRAGONSHRINE_WOLVAR) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_WOLVAR);
							if (orphanGUID)
								phase = 1;
						}
			}

			void UpdateAI(const uint32 diff)
			{
				if (!phase)
					return;

				if (timer <= diff)
				{
					Player* player = Player::GetPlayer(*me, playerGUID);
					Creature* orphan = Creature::GetCreature(*me, orphanGUID);

					if (!player || !orphan)
					{
						Reset();
						return;
					}

					switch (phase)
					{
						case 1:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_4);
							timer = 3000;
							break;
						case 2:
							Talk(TEXT_ELDER_KEKEK_1);
							timer = 6000;
							break;
						case 3:
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_5);
							player->GroupEventHappens(QUEST_THE_BRONZE_DRAGONSHRINE_WOLVAR, me);
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							Reset();
							return;
					}
					++phase;
				}
				else
					timer -= diff;
			}

		private:
			uint32 timer;
			int8 phase;
			uint64 playerGUID;
			uint64 orphanGUID;

		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_elder_kekekAI(creature);
		}
};

/*######
## npc_the_etymidian
## TODO: A red crystal as a gift for the great one should be spawned during the event.
######*/
class npc_the_etymidian : public CreatureScript
{
	public:
		npc_the_etymidian() : CreatureScript("npc_the_etymidian") {}

		struct npc_the_etymidianAI : public ScriptedAI
		{
			npc_the_etymidianAI(Creature* creature) : ScriptedAI(creature) {}

			void Reset()
			{
				timer = 0;
				phase = 0;
				playerGUID = 0;
				orphanGUID = 0;
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (!phase && who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
						if (player->GetQuestStatus(QUEST_MEETING_A_GREAT_ONE) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
							if (orphanGUID)
								phase = 1;
						}
			}

			void UpdateAI(const uint32 diff)
			{
				if (!phase)
					return;

				if (timer <= diff)
				{
					Player* player = Player::GetPlayer(*me, playerGUID);
					Creature* orphan = Creature::GetCreature(*me, orphanGUID);

					if (!orphan || !player)
					{
						Reset();
						return;
					}

					switch (phase)
					{
						case 1:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_7);
							timer = 5000;
							break;
						case 2:
							orphan->SetFacingToObject(me);
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_8);
							timer = 5000;
							break;
						case 3:
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_9);
							timer = 5000;
							break;
						case 4:
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_10);
							timer = 5000;
							break;
						case 5:
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							player->GroupEventHappens(QUEST_MEETING_A_GREAT_ONE, me);
							Reset();
							return;
					}
					++phase;
				}
				else
					timer -= diff;
			}

		private:
			uint32 timer;
			int8 phase;
			uint32 GOtimer;
			uint64 playerGUID;
			uint64 orphanGUID;

		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_the_etymidianAI(creature);
		}
};

/*######
## npc_cw_alexstrasza_trigger
######*/
class npc_alexstraza_the_lifebinder : public CreatureScript
{
	public:
		npc_alexstraza_the_lifebinder() : CreatureScript("npc_alexstraza_the_lifebinder") {}

		struct npc_alexstraza_the_lifebinderAI : public ScriptedAI
		{
			npc_alexstraza_the_lifebinderAI(Creature* creature) : ScriptedAI(creature) {}

			void Reset()
			{
				timer = 0;
				phase = 0;
				playerGUID = 0;
				orphanGUID = 0;
			}

			void SetData(uint32 type, uint32 data)
			{
				// Existing SmartAI
				if (type == 0)
				{
					switch (data)
					{
						case 1:
							me->SetOrientation(1.6049f);
							break;
						case 2:
							me->SetOrientation(me->GetHomePosition().GetOrientation());
							break;
					}
				}
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (!phase && who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
					{
						if (player->GetQuestStatus(QUEST_THE_DRAGON_QUEEN_ORACLE) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
							if (orphanGUID)
								phase = 1;
						}
						else if (player->GetQuestStatus(QUEST_THE_DRAGON_QUEEN_WOLVAR) == QUEST_STATUS_INCOMPLETE)
						{
							playerGUID = player->GetGUID();
							orphanGUID = getOrphanGUID(player, ORPHAN_WOLVAR);
							if (orphanGUID)
								phase = 7;
						}
					}
			}

			void UpdateAI(const uint32 diff)
			{
				if (!phase)
					return;

				if (timer <= diff)
				{
					Player* player = Player::GetPlayer(*me, playerGUID);
					Creature* orphan = Creature::GetCreature(*me, orphanGUID);

					if (!orphan || !player)
					{
						Reset();
						return;
					}

					switch (phase)
					{
						case 1:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_11);
							timer = 5000;
							break;
						case 2:
							orphan->SetFacingToObject(me);
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_12);
							timer = 5000;
							break;
						case 3:
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_13);
							timer = 5000;
							break;
						case 4:
							Talk(TEXT_ALEXSTRASZA_2);
							me->SetStandState(UNIT_STAND_STATE_KNEEL);
							me->SetFacingToObject(orphan);
							timer = 5000;
							break;
						case 5:
							orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_14);
							timer = 5000;
							break;
						case 6:
							me->SetStandState(UNIT_STAND_STATE_STAND);
							me->SetOrientation(me->GetHomePosition().GetOrientation());
							player->GroupEventHappens(QUEST_THE_DRAGON_QUEEN_ORACLE, me);
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							Reset();
							return;
						case 7:
							orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_11);
							timer = 5000;
							break;
						case 8:
							if (Creature* krasus = me->FindNearestCreature(NPC_KRASUS, 10.0f))
							{
								orphan->SetFacingToObject(krasus);
								krasus->AI()->Talk(TEXT_KRASUS_8);
							}
							timer = 5000;
							break;
						case 9:
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_12);
							timer = 5000;
							break;
						case 10:
							orphan->SetFacingToObject(me);
							Talk(TEXT_ALEXSTRASZA_2);
							timer = 5000;
							break;
						case 11:
							orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_13);
							timer = 5000;
							break;
						case 12:
							player->GroupEventHappens(QUEST_THE_DRAGON_QUEEN_WOLVAR, me);
							orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
							Reset();
							return;
					}
					++phase;
				}
				else
					timer -= diff;
			}

			private:
				int8 phase;
				uint32 timer;
				uint64 playerGUID;
				uint64 orphanGUID;
				uint64 alexstraszaGUID;

		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_alexstraza_the_lifebinderAI(creature);
		}
};

/*######
## npc_cw_area_trigger
######*/
class npc_cw_area_trigger : public CreatureScript
{
	public:
		npc_cw_area_trigger() : CreatureScript("npc_cw_area_trigger") {}

		struct npc_cw_area_triggerAI : public ScriptedAI
		{
			npc_cw_area_triggerAI(Creature* creature) : ScriptedAI(creature)
			{
				me->SetDisplayId(DISPLAY_INVISIBLE);
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (who && me->GetDistance2d(who) < 20.0f)
					if (Player* player = who->ToPlayer())
						if (player->HasAura(SPELL_ORPHAN_OUT))
						{
							uint32 questId = 0;
							uint32 orphanId = 0;
							switch (me->GetEntry())
							{
								case NPC_CAVERNS_OF_TIME_CW_TRIGGER:
									questId = player->GetTeam() == ALLIANCE ? QUEST_TIME_TO_VISIT_THE_CAVERNS_A : QUEST_TIME_TO_VISIT_THE_CAVERNS_H;
									orphanId = player->GetTeam() == ALLIANCE ? ORPHAN_DRAENEI : ORPHAN_BLOOD_ELF;
									break;
								case NPC_EXODAR_01_CW_TRIGGER:
									questId = QUEST_THE_SEAT_OF_THE_NARUU;
									orphanId = ORPHAN_DRAENEI;
									break;
								case NPC_EXODAR_02_CW_TRIGGER:
									questId = QUEST_CALL_ON_THE_FARSEER;
									orphanId = ORPHAN_DRAENEI;
									break;
								case NPC_AERIS_LANDING_CW_TRIGGER:
									questId = QUEST_JHEEL_IS_AT_AERIS_LANDING;
									orphanId = ORPHAN_DRAENEI;
									break;
								case NPC_AUCHINDOUN_CW_TRIGGER:
									questId = QUEST_AUCHINDOUN_AND_THE_RING;
									orphanId = ORPHAN_DRAENEI;
									break;
								case NPC_SPOREGGAR_CW_TRIGGER:
									questId = QUEST_HCHUU_AND_THE_MUSHROOM_PEOPLE;
									orphanId = ORPHAN_BLOOD_ELF;
									break;
								case NPC_THRONE_OF_ELEMENTS_CW_TRIGGER:
									questId = QUEST_VISIT_THE_THRONE_OF_ELEMENTS;
									orphanId = ORPHAN_BLOOD_ELF;
									break;
								case NPC_SILVERMOON_01_CW_TRIGGER:
									if (player->GetQuestStatus(QUEST_NOW_WHEN_I_GROW_UP) == QUEST_STATUS_INCOMPLETE && getOrphanGUID(player, ORPHAN_BLOOD_ELF))
									{
										player->AreaExploredOrEventHappens(QUEST_NOW_WHEN_I_GROW_UP);
										if (player->GetQuestStatus(QUEST_NOW_WHEN_I_GROW_UP) == QUEST_STATUS_COMPLETE)
											if (Creature* samuro = me->FindNearestCreature(25151, 20.0f))
											{
												uint32 emote = 0;
												switch (urand(1, 5))
												{
													case 1:
														emote = EMOTE_ONESHOT_WAVE;
														break;
													case 2:
														emote = EMOTE_ONESHOT_ROAR;
														break;
													case 3:
														emote = EMOTE_ONESHOT_FLEX;
														break;
													case 4:
														emote = EMOTE_ONESHOT_SALUTE;
														break;
													case 5:
														emote = EMOTE_ONESHOT_DANCE;
														break;
												}
												samuro->HandleEmoteCommand(emote);
											}
									}
									break;
							}
							if (questId && orphanId && getOrphanGUID(player, orphanId) && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
								player->AreaExploredOrEventHappens(questId);
						}
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_cw_area_triggerAI(creature);
		}
};

/*######
## npc_grizzlemaw_cw_trigger
######*/
class npc_grizzlemaw_cw_trigger : public CreatureScript
{
	public:
		npc_grizzlemaw_cw_trigger() : CreatureScript("npc_grizzlemaw_cw_trigger") {}

		struct npc_grizzlemaw_cw_triggerAI : public ScriptedAI
		{
			npc_grizzlemaw_cw_triggerAI(Creature* creature) : ScriptedAI(creature)
			{
				me->SetDisplayId(DISPLAY_INVISIBLE);
			}

			void MoveInLineOfSight(Unit* who)
			{
				if (who && who->GetDistance2d(me) < 10.0f)
					if (Player* player = who->ToPlayer())
						if (player->GetQuestStatus(QUEST_HOME_OF_THE_BEAR_MEN) == QUEST_STATUS_INCOMPLETE)
							if (Creature* orphan = Creature::GetCreature(*me, getOrphanGUID(player, ORPHAN_WOLVAR)))
							{
								player->AreaExploredOrEventHappens(QUEST_HOME_OF_THE_BEAR_MEN);
								orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_10);
							}
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_grizzlemaw_cw_triggerAI(creature);
		}
};

 /*######
## npc_anchorite_barada
######*/

#define GOSSIP_ITEM_START "I am ready, Anchorite.  Let us begin the exorcism."

#define SAY_BARADA1    -1900100
#define SAY_BARADA2    -1900101
#define SAY_BARADA3    -1900104
#define SAY_BARADA4    -1900105
#define SAY_BARADA5    -1900106
#define SAY_BARADA6    -1900107
#define SAY_BARADA7    -1900108
#define SAY_BARADA8    -1900109
#define SAY_COLONEL1   -1900110
#define SAY_COLONEL2   -1900111
#define SAY_COLONEL3   -1900112
#define SAY_COLONEL4   -1900113
#define SAY_COLONEL5   -1900114
#define SAY_COLONEL6   -1900115
#define SAY_COLONEL7   -1900116
#define SAY_COLONEL8   -1900117

enum
{
	QUEST_THE_EXORCISM =    10935,
	NPC_COLONEL_JULES =     22432,
	NPC_DARKNESS_RELEASED = 22507,
	NPC_FOUL_PURGE =        22506,
	SPELL_EXORCISM =        39277,
	SPELL_EXORCISM2 =       39278,
	SPELL_COLONEL1 =        39283,
	SPELL_COLONEL2 =        39294,
	SPELL_COLONEL3 =        39284,
	SPELL_COLONEL5 =        39295,
	SPELL_COLONEL6 =        39300,
	SPELL_COLONEL7 =        39381,
	SPELL_COLONEL8 =        39380,
	   SPELL_AURA_ME =                 39303
};

class npc_anchorite_barada : public CreatureScript
{
public:
	npc_anchorite_barada() : CreatureScript("npc_anchorite_barada") { }
	
	   CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_anchorite_baradaAI (pCreature);
	}

	   bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		player->PlayerTalkClass->ClearMenus();
		if (uiAction == GOSSIP_ACTION_INFO_DEF)
		{
			player->CLOSE_GOSSIP_MENU();
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					   CAST_AI(npc_anchorite_barada::npc_anchorite_baradaAI, creature->AI())->Exorcism = true;
			CAST_AI(npc_anchorite_barada::npc_anchorite_baradaAI, creature->AI())->uiPlayerGUID = player->GetGUID();
			   }
			   return true;
	   }

	   bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->GetQuestStatus(QUEST_THE_EXORCISM) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	   struct npc_anchorite_baradaAI : public ScriptedAI
	   {
			   npc_anchorite_baradaAI(Creature* pCreature) : ScriptedAI(pCreature) {}

			   bool Exorcism;

			   uint64 uiPlayerGUID;
			   uint32 uiTimer;
			   uint8 Phase;


			   void Reset()
			   {
					   Exorcism = false;
					   uiTimer = 0;
					   Phase = 0;
					   uiPlayerGUID = 0;
			   }

			   void AttackedBy(Unit* who) {}
			   void AttackStart(Unit* who) {}

			   void DoSpawnDarkness()
			   {
						if(Creature* pDarkness = me->SummonCreature(NPC_DARKNESS_RELEASED, -710.924f, 2754.683f, 105.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000))
						{
								pDarkness->SetReactState(REACT_PASSIVE);
								pDarkness->CastSpell(pDarkness,SPELL_AURA_ME);
								//pDarkness->SetUnitMovementFlags(MOVEMENTFLAG_LEVITATING);
								pDarkness->SetSpeed(MOVE_RUN, 0.10f);

								switch(urand(0,3))
									   {
											   case 0:
													   pDarkness->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 105.0f);
													   break;
											   case 1:
													   pDarkness->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 105.0f);
													   break;
											   case 2:
													   pDarkness->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 105.0f);
													   break;
											   case 3:
													   pDarkness->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 105.0f);
													   break;
									   }
						}
			   }

			   void DoSpawnPurge()
			   {
					   if(Creature* pColonel = me->FindNearestCreature(NPC_COLONEL_JULES, 15))
					   {

							   float X = pColonel->GetPositionX();
							   float Y = pColonel->GetPositionY();
							   float Z = me->GetPositionZ();

							   me->SummonCreature(NPC_FOUL_PURGE, X, Y, Z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
					   }
			   }


			   /*void MoveInLineOfSight(Unit *who)
			   {
					   if (who->GetTypeId() == TYPEID_PLAYER)
					   {
							   if (CAST_PLR(who)->GetQuestStatus(QUEST_THE_EXORCISM) == QUEST_STATUS_INCOMPLETE)
							   {
									   if (me->IsWithinDistInMap(((Player *)who), 5))
									   {
											   uiPlayerGUID = who->GetGUID();
									   }
							   }
					   }
			   }*/

			   uint32 NextStep(uint32 Phase)
			   {
					   Creature* pColonel = me->FindNearestCreature(NPC_COLONEL_JULES, 15);

					   switch(Phase)
					   {
							   case 1:
									   me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
									   pColonel->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
									   me->SetStandState(UNIT_STAND_STATE_STAND);
									   me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
									   DoScriptText(SAY_BARADA1, me,0);
									   return 5000;
							   case 2:
									   DoScriptText(SAY_BARADA2, me,0);
									   return 3000;                   
							   case 3:
									   me->GetMotionMaster()->MovePoint(0, -707.702f, 2749.038f, 101.590f);
									   DoScriptText(SAY_COLONEL1, pColonel, 0);
									   return 2000;
							   case 4:
									   me->GetMotionMaster()->MovePoint(0, -710.729f, 2746.039f, 101.590f);
									   return 2100;
							   case 5:
									   me->GetMotionMaster()->MovePoint(0, -710.810f, 2748.376f, 101.590f);
									   return 2000;
							   case 6:
									   me->CastSpell(me, SPELL_EXORCISM , false);
									   return 8000;
							   case 7:
									   DoScriptText(SAY_BARADA3, me,0);
									   return 10000;
							   case 8:
									   DoScriptText(SAY_COLONEL2, pColonel, 0);
									   return 8000;
							   case 9:
									   me->RemoveAllAuras();
									   me->CastSpell(me, SPELL_EXORCISM2 , false);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL1, false);
									   //pColonel->SetUnitMovementFlags(MOVEMENTFLAG_LEVITATING);
									   pColonel->SetSpeed(MOVE_RUN, 0.17f);
									   pColonel->GetMotionMaster()->MovePoint(0, -710.611f, 2753.435f, 103.774f);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL3, false);
									   return 14000;
							   case 10:
									   DoScriptText(SAY_COLONEL3, pColonel, 0);
									   DoSpawnDarkness();
									   DoSpawnDarkness();
									   return 14000;
							   case 11:
									   DoScriptText(SAY_BARADA4, me, 0);
									   DoSpawnDarkness();
									   DoSpawnDarkness();
									   return 14000;
							   case 12:
									   DoScriptText(SAY_COLONEL4, pColonel, 0);
									   DoSpawnDarkness();
									   return 14000;
							   case 13:
									   DoScriptText(SAY_BARADA5, me, 0);
									   return 14000;
							   case 14:
									   pColonel->CastSpell(pColonel, SPELL_COLONEL2, false);
									   DoSpawnDarkness();
									   return 1500;
							   case 15:
									   pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
									   DoScriptText(SAY_COLONEL5, pColonel, 0);
									   return 5000;
							   case 16:
									   pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
									   DoSpawnDarkness();
									   return 4000;
							   case 17:
									   pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL5, false);
									   pColonel->CastSpell(me,SPELL_COLONEL6, false);
									   DoScriptText(SAY_BARADA6, me, 0);
									   DoSpawnPurge();
									   return 2500;
							   case 18:
									   pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
									   DoSpawnDarkness();
									   return 3500;
							   case 19:
									   pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
									   return 4000;
							   case 20:
									   pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
									   DoScriptText(SAY_COLONEL6, pColonel, 0);
									   DoSpawnDarkness();
									   return 4000;
							   case 21:
									   pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL5, false);
									   pColonel->CastSpell(me,SPELL_COLONEL6, false);
									   DoSpawnPurge();
									   return 4000;
							   case 22:
									   pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
									   return 4000;
							   case 23:
									   pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
									   DoScriptText(SAY_BARADA7, me, 0);
									   DoSpawnDarkness();
									   return 4000;
							   case 24:
									   pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL5, false);
									   pColonel->CastSpell(me,SPELL_COLONEL6, false);
									   DoSpawnPurge();
							   case 25:
									   pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
									   DoScriptText(SAY_COLONEL7, pColonel, 0);
									   DoSpawnDarkness();
									   return 4000;
							   case 26:
									   pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
									   return 4000;
							   case 27:
									   pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
									   DoSpawnDarkness();
									   return 4000;
							   case 28:
									   pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
									   DoScriptText(SAY_BARADA6, me, 0);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL5, false);
									   pColonel->CastSpell(me,SPELL_COLONEL6, false);
									   DoSpawnPurge();
									   return 4000;
							   case 29:
									   pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
									   return 4000;
							   case 30:
									   pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
									   DoScriptText(SAY_COLONEL8, pColonel, 0);
									   return 4000;
							   case 31:
									   pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
									   return 4000;
							   case 32:
									   pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL5, false);
									   pColonel->CastSpell(me,SPELL_COLONEL6, false);
									   DoSpawnPurge();
									   return 5000;
							   case 33:
									   pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
									   DoScriptText(SAY_BARADA6, me, 0);
									   return 4000;
							   case 34:
									   pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL8, false);
									   return 4000;
							   case 35:
									   pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
									   pColonel->CastSpell(pColonel, SPELL_COLONEL7, false);
									   return 4000;
							   case 36:
									   pColonel->GetMotionMaster()->MovePoint(0, -710.792f, 2750.693f, 103.774f);
									   return 6000;
							   case 37:
									   pColonel->GetMotionMaster()->MovePoint(0, -710.111f, 2754.346f, 102.367f);
									   DoScriptText(SAY_BARADA8, me, 0);
									   return 3000;
							   case 38:
									   me->RemoveAllAuras();
									   pColonel->RemoveAllAuras();
									   me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
									   me->GetMotionMaster()->MovePoint(0, -706.726f, 2751.632f, 101.591f);
									   return 4200;
							   case 39:
									   me->GetMotionMaster()->MovePoint(0, -707.382f, 2753.994f, 101.591f);
									   return 7000;
							   case 40:
									   me->SetStandState(UNIT_STAND_STATE_KNEEL);
									   me->CombatStop();
									   return 3000;
							   case 41:
									   pColonel->SetFlag(UNIT_NPC_FLAGS, 1);
									   me->SetFlag(UNIT_NPC_FLAGS, 1);
									   if (Player* pPlayer = Unit::GetPlayer(*me, uiPlayerGUID))
									   {
										pPlayer->RewardPlayerAndGroupAtEvent(22432, pPlayer);
											   //pPlayer->AreaExploredOrEventHappens(QUEST_THE_EXORCISM);
									   }
									   return 1000;
							   case 42:
									   Reset();
							   default:
									   return 0;
					   }
			   }

			   void JustDied(Unit* who)
			   {
					   if (Creature* pColonel = me->FindNearestCreature(NPC_COLONEL_JULES, 15.0f, true))
					   {
							   pColonel->GetMotionMaster()->MovePoint(0, -710.111f, 2754.346f, 102.367f);
							   pColonel->RemoveAllAuras();
					   }
					   else
					   {
							   return;
					   }
			   }

			   void UpdateAI(const uint32 uiDiff)
			   {
					   if (uiTimer <= uiDiff)
					   {
							   if (Exorcism)
									   uiTimer = NextStep(++Phase);
					   }
					   else
					   {
							   uiTimer -= uiDiff;
					   }
			   }
	   };
};

/*######
## npc_darkness_released
######*/
enum
{
	SPELL_DARKNESS = 39307,
	NPC_BARADA =     22431
};

class npc_darkness_released : public CreatureScript
{
public:
	npc_darkness_released() : CreatureScript("npc_darkness_released") { }

	   CreatureAI* GetAI_npc_darkness_released(Creature* pCreature)
	   {
			   return new npc_darkness_releasedAI(pCreature);
	   }

	   struct npc_darkness_releasedAI : public ScriptedAI
	   {
			   npc_darkness_releasedAI(Creature* pCreature) : ScriptedAI(pCreature) {}

			   uint32 uiTimer;

			   void Reset()
			   {      
			   }

			   void MoveInLineOfSight(Unit *who)
			   {
					   if (who->GetTypeId() == TYPEID_PLAYER)
					   {
							   if (CAST_PLR(who)->GetQuestStatus(QUEST_THE_EXORCISM) == QUEST_STATUS_COMPLETE)
							   {
									   me->DespawnOrUnsummon();
							   }
					   }
			   }

			   void JustDied(Unit* who)
			   {
					   me->CastSpell(me,SPELL_DARKNESS);
			   }

			   void UpdateAI(const uint32 uiDiff)
			   {
					   if (uiTimer <= uiDiff)
					   {
							   if (Creature* pBar = me->FindNearestCreature(NPC_BARADA, 5.0f, false))
							   {
									   me->DespawnOrUnsummon();
							   }
							   uiTimer = 5000;
					   }
					   else
					   {
							   uiTimer -= uiDiff;
					   }
			   }
	   };
};

/*######
## npc_foul_purge
######*/

class npc_foul_purge : public CreatureScript
{
public:
	npc_foul_purge() : CreatureScript("npc_foul_purge") { }

	   CreatureAI* GetAI_npc_foul_purge(Creature* pCreature)
			   {
					   return new npc_foul_purgeAI(pCreature);
			   }

	   struct npc_foul_purgeAI : public ScriptedAI
	   {
			   npc_foul_purgeAI(Creature* pCreature) : ScriptedAI(pCreature) {}

			   uint32 uiTimer;

			   void Reset()
			   {
					   if (Creature* pBara = me->FindNearestCreature(NPC_BARADA, 15.0f, true))
					   {
							   AttackStart(pBara);
					   }
					   uiTimer = 4000;
			   }

			   void MoveInLineOfSight(Unit *who)
			   {
					   if (who->GetTypeId() == TYPEID_PLAYER)
					   {
							   if (CAST_PLR(who)->GetQuestStatus(QUEST_THE_EXORCISM) == QUEST_STATUS_COMPLETE)
							   {
									   me->DespawnOrUnsummon();
							   }
					   }
			   }

			   void JustDied(Unit* who)
			   {
					me->RemoveCorpse();
			   }

			   void UpdateAI(const uint32 uiDiff)
			   {
					if (uiTimer <= uiDiff)
					{
						if (Creature* pBar = me->FindNearestCreature(NPC_BARADA, 15.0f, false))
						{
							me->DespawnOrUnsummon();
						}
						uiTimer = 4000;
					}
					else
					{
						uiTimer -= uiDiff;
					}
					DoMeleeAttackIfReady();
			   }
	   };
};

/*######
## npc_demoniac_scryer
######*/

#define GOSSIP_ITEM_ATTUNE          "Yes, Scryer. You may possess me."

enum eDemoniac
{
	GOSSIP_TEXTID_PROTECT           = 10659,
	GOSSIP_TEXTID_ATTUNED           = 10643,

	QUEST_DEMONIAC                  = 10838,
	NPC_HELLFIRE_WARDLING           = 22259,
	NPC_BUTTRESS                    = 22267,                
	NPC_SPAWNER                     = 22260,                

	MAX_BUTTRESS                    = 4,
	TIME_TOTAL                      = MINUTE*10*IN_MILLISECONDS,

	SPELL_SUMMONED_DEMON            = 7741,                
	SPELL_DEMONIAC_VISITATION       = 38708,                

	SPELL_BUTTRESS_APPERANCE        = 38719,                
	SPELL_SUCKER_CHANNEL            = 38721,                
	SPELL_SUCKER_DESPAWN_MOB        = 38691
};

class npc_demoniac_scryer : public CreatureScript
{
public:
	npc_demoniac_scryer() : CreatureScript("npc_demoniac_scryer") { }

	   CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_demoniac_scryerAI (pCreature);
	}

	   bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	   {
			   if (npc_demoniac_scryerAI* pScryerAI = dynamic_cast<npc_demoniac_scryerAI*>(pCreature->AI()))
			   {
					   if (pScryerAI->IsComplete)
					   {
							   if (pPlayer->GetQuestStatus(QUEST_DEMONIAC) == QUEST_STATUS_INCOMPLETE)
									   pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ATTUNE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

							   pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ATTUNED, pCreature->GetGUID());
							   return true;
					   }
			   }

			   pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_PROTECT, pCreature->GetGUID());
			   return true;
	   }

	   bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
	   {
			   if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
			   {
					   pPlayer->CLOSE_GOSSIP_MENU();
					   pCreature->CastSpell(pPlayer, SPELL_DEMONIAC_VISITATION, false);
			   }

			   return true;
	   }

	   struct npc_demoniac_scryerAI : public ScriptedAI
	   {
			   npc_demoniac_scryerAI(Creature* pCreature) : ScriptedAI(pCreature)
			   {
					   IsComplete = false;
					   uiSpawnDemonTimer = 15000;
					   uiSpawnButtressTimer = 45000;
					   uiButtressCount = 0;
					   Reset();
			   }

			   bool IsComplete;

			   uint32 uiSpawnDemonTimer;
			   uint32 uiSpawnButtressTimer;
			   uint32 uiButtressCount;

			   void Reset()
			   {
			   }

			   void AttackedBy(Unit* pEnemy) {}
			   void AttackStart(Unit* pEnemy) {}

			   void DoSpawnButtress()
			   {
					   ++uiButtressCount;

					   float fAngle;

					   switch(uiButtressCount)
					   {
							   case 1: fAngle = 0.0f; break;
							   case 2: fAngle = M_PI+M_PI/2; break;
							   case 3: fAngle = M_PI/2; break;
							   case 4: fAngle = M_PI; break;
					   }

					   float fX, fY;
					   me->GetNearPoint2D(fX, fY, 5.0f, fAngle);

					   float fZ_Ground = me->GetMap()->GetHeight(fX, fY, MAX_HEIGHT);

					   uint32 uiTime = TIME_TOTAL - (uiSpawnButtressTimer * uiButtressCount);
					   me->SummonCreature(NPC_BUTTRESS, fX, fY, fZ_Ground, me->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, uiTime);
			   }

			   void DoSpawnDemon()
			   {
					   const Position pos = {me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation()};
					   float fX, fY, fZ;
					   me->GetRandomPoint(pos, 20.0f, fX, fY, fZ);

					   me->SummonCreature(NPC_HELLFIRE_WARDLING, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
			   }

			   void JustSummoned(Creature* pSummoned)
			   {
					   if (pSummoned->GetEntry() == NPC_HELLFIRE_WARDLING)
					   {
							   pSummoned->CastSpell(pSummoned, SPELL_SUMMONED_DEMON, false);
							   if(Unit* target = pSummoned->SelectNearestTarget(30.0f))
							   {
									   pSummoned->SetReactState(REACT_AGGRESSIVE);
									   pSummoned->AI()->AttackStart(target);
							   }
					   }
					   else
					   {
							   if (pSummoned->GetEntry() == NPC_BUTTRESS)
							   {
									   pSummoned->CastSpell(pSummoned, SPELL_BUTTRESS_APPERANCE, false);
									   pSummoned->CastSpell(me, SPELL_SUCKER_CHANNEL, true);
							   }
					   }
			   }

			   void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
			   {
					   if (pTarget->GetEntry() == NPC_HELLFIRE_WARDLING && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
							   ((Creature*)pTarget)->DespawnOrUnsummon();
			   }

			   void UpdateAI(const uint32 uiDiff)
			   {
					   if (IsComplete || !me->isAlive())
							   return;

					   if (uiSpawnButtressTimer <= uiDiff)
					   {
							   if (uiButtressCount >= MAX_BUTTRESS)
							   {
									   me->CastSpell(me, SPELL_SUCKER_DESPAWN_MOB, false);

									   if (me->isInCombat())
									   {
											   me->DeleteThreatList();
											   me->CombatStop();
									   }

									   IsComplete = true;
									   return;
							   }

							   uiSpawnButtressTimer = 45000;
							   DoSpawnButtress();
					   }
					   else
							   uiSpawnButtressTimer -= uiDiff;

					   if (uiSpawnDemonTimer <= uiDiff)
					   {
							   DoSpawnDemon();
							   uiSpawnDemonTimer = 15000;
					   }
					   else
							   uiSpawnDemonTimer -= uiDiff;
			   }
	   };
};



/*######
## npc_mana_bomb
## http://www.wowhead.com/quest=10446 The Final Code (Alliance)
## http://www.wowhead.com/quest=10447 The Final Code (Horde)
######*/

enum eManaBomb
{
	SAY_COUNT_1                 = -1000477,
	SAY_COUNT_2                 = -1000473,
	SAY_COUNT_3                 = -1000474,
	SAY_COUNT_4                 = -1000475,
	SAY_COUNT_5                 = -1000476,
	SPELL_MANA_BOMB_LIGHTNING   = 37843,
	SPELL_MANA_BOMB_EXPL        = 35513,
	NPC_MANA_BOMB_EXPL_TRIGGER  = 20767,
	NPC_MANA_BOMB_KILL_TRIGGER  = 21039
};

class npc_mana_bomb : public CreatureScript
{
public:
	npc_mana_bomb() : CreatureScript("npc_mana_bomb") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_mana_bombAI (pCreature);
	}

	struct npc_mana_bombAI : public ScriptedAI
	{
		npc_mana_bombAI(Creature* pCreature) : ScriptedAI(pCreature) 
		{ 
			Reset(); 
		}

		GameObject* pManaBomb;

		bool m_bIsActivated;
		uint32 m_uiEventTimer;
		uint32 m_uiEventCounter;

		void Reset()
		{
			pManaBomb = NULL;
			m_bIsActivated = false;
			m_uiEventTimer = 1000;
			m_uiEventCounter = 0;
		}

		void DoTrigger(Player* pPlayer, GameObject* pGo)
		{
			if (m_bIsActivated)
				return;

			m_bIsActivated = true;

			pPlayer->RewardPlayerAndGroupAtEvent(NPC_MANA_BOMB_KILL_TRIGGER, pPlayer);

			pManaBomb = pGo;
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!m_bIsActivated)
				return;

			if (m_uiEventTimer < uiDiff)
			{
				m_uiEventTimer = 1000;

				if (m_uiEventCounter < 10)
					me->CastSpell(me, SPELL_MANA_BOMB_LIGHTNING, false);

				switch(m_uiEventCounter)
				{
					case 5:
						if (pManaBomb)
							pManaBomb->SetGoState(GO_STATE_ACTIVE);

						DoScriptText(SAY_COUNT_1, me);
						break;
					case 6:
						DoScriptText(SAY_COUNT_2, me);
						break;
					case 7:
						DoScriptText(SAY_COUNT_3, me);
						break;
					case 8:
						DoScriptText(SAY_COUNT_4, me);
						break;
					case 9:
						DoScriptText(SAY_COUNT_5, me);
						break;
					case 10:
						me->CastSpell(me, SPELL_MANA_BOMB_EXPL, false);
						break;
					case 30:
						if (pManaBomb)
							pManaBomb->SetGoState(GO_STATE_READY);

						Reset();
						break;
				}

				++m_uiEventCounter;
			}
			else
				m_uiEventTimer -= uiDiff;
		}
	};
};

class go_mana_bomb : public GameObjectScript
{
public:
	go_mana_bomb() : GameObjectScript("go_mana_bomb") { }

	bool OnGossipHello(Player* pPlayer, GameObject* pGo)
	{
		if (Creature* pCreature = GetClosestCreatureWithEntry(pGo, NPC_MANA_BOMB_EXPL_TRIGGER, INTERACTION_DISTANCE))
			if (npc_mana_bomb::npc_mana_bombAI* pBombAI = CAST_AI(npc_mana_bomb::npc_mana_bombAI, pCreature->AI()))
				pBombAI->DoTrigger(pPlayer, pGo);

		return true;
	}
};

class ManaBombTargetDeselector
{
	public:
		ManaBombTargetDeselector() { }

		bool operator()(Unit* unit)
		{
			if (unit->ToPlayer())
				return true;
			
			if (unit->ToCreature())
				if (unit->ToCreature()->isTrigger())
					return true;

			return false;
		}
};

class spell_mana_bomb : public SpellScriptLoader
{
	public:
		spell_mana_bomb() : SpellScriptLoader("spell_mana_bomb") { }

		class spell_mana_bomb_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_mana_bomb_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
			{
				unitList.remove_if(ManaBombTargetDeselector());
			}

			void Register()
			{
				OnUnitTargetSelect += SpellUnitTargetFn(spell_mana_bomb_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_mana_bomb_SpellScript();
		}
};

/*######
## npc_letoll
## http://www.wowhead.com/quest=10922 "Digging Through Bones"
######*/

enum NpcLetoll
{
	SAY_LE_START                    = -1000511,
	SAY_LE_KEEP_SAFE                = -1000512,
	SAY_LE_NORTH                    = -1000513,
	SAY_LE_ARRIVE                   = -1000514,
	SAY_LE_BURIED                   = -1000515,
	SAY_LE_ALMOST                   = -1000516,
	SAY_LE_DRUM                     = -1000517,
	SAY_LE_DRUM_REPLY               = -1000518,
	SAY_LE_DISCOVERY                = -1000519,
	SAY_LE_DISCOVERY_REPLY          = -1000520,
	SAY_LE_NO_LEAVE                 = -1000521,
	SAY_LE_NO_LEAVE_REPLY1          = -1000522,
	SAY_LE_NO_LEAVE_REPLY2          = -1000523,
	SAY_LE_NO_LEAVE_REPLY3          = -1000524,
	SAY_LE_NO_LEAVE_REPLY4          = -1000525,
	SAY_LE_SHUT                     = -1000526,
	SAY_LE_REPLY_HEAR               = -1000527,
	SAY_LE_IN_YOUR_FACE             = -1000528,
	SAY_LE_HELP_HIM                 = -1000529,
	EMOTE_LE_PICK_UP                = -1000530,
	SAY_LE_THANKS                   = -1000531,
	QUEST_DIGGING_BONES             = 10922,
	NPC_RESEARCHER                  = 22464,
	NPC_BONE_SIFTER                 = 22466,
	MAX_RESEARCHER                  = 4
};

class npc_letoll : public CreatureScript
{
public:
	npc_letoll() : CreatureScript("npc_letoll") { }

	bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
	{
		if (pQuest->GetQuestId() == QUEST_DIGGING_BONES)
		{
			if (npc_letollAI* pEscortAI = dynamic_cast<npc_letollAI*>(pCreature->AI()))
			{
				DoScriptText(SAY_LE_START, pCreature);
				pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);

				pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest, true);
			}
		}        
		return true;
	}

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_letollAI(pCreature);
	}

	struct npc_letollAI : public npc_escortAI
	{
		npc_letollAI(Creature* pCreature) : npc_escortAI(pCreature)
		{
			m_uiEventTimer = 5000;
			m_uiEventCount = 0;
			Reset();
		}
		
		std::list<Creature*> m_lResearchersList;
		uint32 m_uiEventTimer;
		uint32 m_uiEventCount;

		void Reset() {}

		void SetFormation()
		{
			uint32 uiCount = 0;

			for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
			{
				float fAngle = uiCount < MAX_RESEARCHER ? M_PI/MAX_RESEARCHER - (uiCount*2*M_PI/MAX_RESEARCHER) : 0.0f;

				if ((*itr)->isAlive() && !(*itr)->isInCombat())
					(*itr)->GetMotionMaster()->MoveFollow(me, 2.5f, fAngle);

				++uiCount;
			}
		}

		Creature* GetAvailableResearcher(uint8 uiListNum)
		{
			if (!m_lResearchersList.empty())
			{
				uint8 uiNum = 1;

				for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
				{
					if (uiListNum && uiListNum != uiNum)
					{
						++uiNum;
						continue;
					}

					if ((*itr)->isAlive() && (*itr)->IsWithinDistInMap(me, 20.0f))
						return (*itr);
				}
			}
			return NULL;
		}

		void JustStartedEscort()
		{
			m_uiEventTimer = 5000;
			m_uiEventCount = 0;

			m_lResearchersList.clear();
			
			me->GetCreatureListWithEntryInGrid(m_lResearchersList, NPC_RESEARCHER, 25);

			if (!m_lResearchersList.empty())
				SetFormation();
		}

		void WaypointReached(uint32 uiPointId)
		{
			switch(uiPointId)
			{
				case 0:
					if (Player* pPlayer = GetPlayerForEscort())
						DoScriptText(SAY_LE_KEEP_SAFE, me, pPlayer);
					break;
				case 1:
					DoScriptText(SAY_LE_NORTH, me);
					break;
				case 10:
					DoScriptText(SAY_LE_ARRIVE, me);
					break;
				case 12:
					DoScriptText(SAY_LE_BURIED, me);
					SetEscortPaused(true);
					break;
				case 13:
					SetRun();
					break;
			}
		}

		void Aggro(Unit* pWho)
		{
			if (pWho->isInCombat() && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_BONE_SIFTER)
				DoScriptText(SAY_LE_HELP_HIM, me);
		}

		void JustSummoned(Creature* pSummoned)
		{
			Player* pPlayer = GetPlayerForEscort();

			if (pPlayer && pPlayer->isAlive())
				pSummoned->AI()->AttackStart(pPlayer);
			else
				pSummoned->AI()->AttackStart(me);
		}

		void UpdateEscortAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
			{
				if (HasEscortState(STATE_ESCORT_PAUSED))
				{
					if (m_uiEventTimer < uiDiff)
					{
						m_uiEventTimer = 7000;

						switch(m_uiEventCount)
						{
							case 0:
								DoScriptText(SAY_LE_ALMOST, me);
								break;
							case 1:
								DoScriptText(SAY_LE_DRUM, me);
								break;
							case 2:
								if (Creature* pResearcher = GetAvailableResearcher(0))
									DoScriptText(SAY_LE_DRUM_REPLY, pResearcher);
								break;
							case 3:
								DoScriptText(SAY_LE_DISCOVERY, me);
								break;
							case 4:
								if (Creature* pResearcher = GetAvailableResearcher(0))
									DoScriptText(SAY_LE_DISCOVERY_REPLY, pResearcher);
								break;
							case 5:
								DoScriptText(SAY_LE_NO_LEAVE, me);
								break;
							case 6:
								if (Creature* pResearcher = GetAvailableResearcher(1))
									DoScriptText(SAY_LE_NO_LEAVE_REPLY1, pResearcher);
								break;
							case 7:
								if (Creature* pResearcher = GetAvailableResearcher(2))
									DoScriptText(SAY_LE_NO_LEAVE_REPLY2, pResearcher);
								break;
							case 8:
								if (Creature* pResearcher = GetAvailableResearcher(3))
									DoScriptText(SAY_LE_NO_LEAVE_REPLY3, pResearcher);
								break;
							case 9:
								if (Creature* pResearcher = GetAvailableResearcher(4))
									DoScriptText(SAY_LE_NO_LEAVE_REPLY4, pResearcher);
								break;
							case 10:
								DoScriptText(SAY_LE_SHUT, me);
								break;
							case 11:
								if (Creature* pResearcher = GetAvailableResearcher(0))
									DoScriptText(SAY_LE_REPLY_HEAR, pResearcher);
								break;
							case 12:
								DoScriptText(SAY_LE_IN_YOUR_FACE, me);
								me->SummonCreature(NPC_BONE_SIFTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
								break;
							case 13:
								DoScriptText(EMOTE_LE_PICK_UP, me);

								if (Player* pPlayer = GetPlayerForEscort())
								{
									DoScriptText(SAY_LE_THANKS, me, pPlayer);
									pPlayer->GroupEventHappens(QUEST_DIGGING_BONES, me);
								}

								SetEscortPaused(false);
								break;
						}

						++m_uiEventCount;
					}
					else
						m_uiEventTimer -= uiDiff;
				}

				return;
			}

			DoMeleeAttackIfReady();
		}
	};
};
/*###################
# spell_fumping_39238
####################*/

enum FumpingSpellData
{
	SPELL_FUMPING = 39238,
	NPC_MATURE_BONE_SIFTER = 22482,
	NPC_SAND_GNOME = 22483
};

class spell_fumping_39238 : public SpellScriptLoader
{
public:
	spell_fumping_39238() : SpellScriptLoader("spell_fumping_39238") {}

	class spell_fumping_39238SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_fumping_39238SpellScript)

		bool Validate(SpellEntry const * /*spellEntry*/)
		{
			if (!sSpellStore.LookupEntry(SPELL_FUMPING))
				return false;         
			return true;
		}

		void HandleDummy(SpellEffIndex /*effIndex*/)
		{
			if (Unit* player = GetCaster())
			{
				switch (urand(1,2))
				{
					case 1: player->SummonCreature(NPC_MATURE_BONE_SIFTER, player->GetPositionX()+rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0); break;
					case 2: player->SummonCreature(NPC_SAND_GNOME, player->GetPositionX()+rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0); break;
				}
			}
		}
		void Register()
		{
			OnEffectHit += SpellEffectFn(spell_fumping_39238SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_fumping_39238SpellScript();
	}
};
/* Support quest: Whispers of the Raven God */

enum GO_Prophecies
{
	FIRST_PROPHECY = 184950,
	SECOND_PROPHECY = 184967,
	THIRD_PROPHECY = 184968,
	FOURTH_PROPHECY = 184969,
};

enum Prophecy_Credits
{
	FIRST_PROPHECY_CREDIT = 22798,
	SECOND_PROPHECY_CREDIT = 22799,
	THIRD_PROPHECY_CREDIT = 22800,
	FOURTH_PROPHECY_CREDIT = 22801,
};

#define QUEST_WHISPERS_RAVEN_GOD 10607

class go_prophecy : public GameObjectScript
{
	public:
		go_prophecy() : GameObjectScript("go_prophecy") { }

	bool OnGossipHello(Player *pPlayer, GameObject *pGO)
	{
		if (!pPlayer)
			return true;

		if (pPlayer->GetQuestStatus(QUEST_WHISPERS_RAVEN_GOD) == QUEST_STATUS_INCOMPLETE)
		{
			switch(pGO->GetEntry())
			{
				case FIRST_PROPHECY:
					pPlayer->KilledMonsterCredit(FIRST_PROPHECY_CREDIT, 0);
					break;
				case SECOND_PROPHECY:
					pPlayer->KilledMonsterCredit(SECOND_PROPHECY_CREDIT, 0);
					break;
				case THIRD_PROPHECY:
					pPlayer->KilledMonsterCredit(THIRD_PROPHECY_CREDIT, 0);
					break;
				case FOURTH_PROPHECY:
					pPlayer->KilledMonsterCredit(FOURTH_PROPHECY_CREDIT, 0);
					break;
			}
		}
		return true;
	};
};

enum BigBoneWormSpellData
{
	SPELL_BIGBONE_FUMPING = 39246,
	NPC_HAISHULUD = 22038
};

class spell_fumping_39246 : public SpellScriptLoader
{
public:
	spell_fumping_39246() : SpellScriptLoader("spell_fumping_39246") {}

	class spell_fumping_39246SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_fumping_39246SpellScript)

		bool Validate(SpellEntry const * /*spellEntry*/)
		{
			if (!sSpellStore.LookupEntry(SPELL_FUMPING))
				return false;
			return true;
		}

		void HandleDummy(SpellEffIndex /*effIndex*/)
		{
			if (Unit* player = GetCaster())
			{
				player->SummonCreature(NPC_HAISHULUD, player->GetPositionX()+rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+2, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
				if (urand(0,1))
					player->SummonCreature(NPC_SAND_GNOME, player->GetPositionX()+rand()%15, player->GetPositionY()+rand()%15, player->GetPositionZ()+2, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
			}
		}
		void Register()
		{
			OnEffectHit += SpellEffectFn(spell_fumping_39246SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_fumping_39246SpellScript();
	}
};

class spell_ride_the_lightning_37841 : public SpellScriptLoader
{
public:
	spell_ride_the_lightning_37841() : SpellScriptLoader("spell_ride_the_lightning_37841") {}

	class spell_ride_the_lightning_37841SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_ride_the_lightning_37841SpellScript)

		void AfterHit(SpellEffIndex /*effIndex*/)
		{
			Unit* target = GetHitUnit();
			if (target)
				if (target->HasAura(37830))
					if (target->ToPlayer())
						target->ToPlayer()->KilledMonsterCredit(21910, 0);
		}
		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_ride_the_lightning_37841SpellScript::AfterHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_ride_the_lightning_37841SpellScript();
	}
};

class go_fel_crystal_prism : public GameObjectScript
{
	public:
		go_fel_crystal_prism() : GameObjectScript("go_fel_crystal_prism") { }

	bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
	{
		go->SummonCreature(RAND(23354, 23355, 23353, 22281), go->GetPositionX()+5 + urand(0,5), go->GetPositionY() + 10 + urand(0,5), go->GetPositionZ(), go->GetOrientation());
		player->PlayerTalkClass->SendCloseGossip();
		return true;
	};
};

enum GAUGING_THE_RESONANT_FREQUENCY
{
 NPC_OSCILLATING_FREQUENCY_TRIGGER = 21760,
 SPELL_OSCILLATING_FIELD = 37418,
 QUEST_GAUGING_THE_RESONANT_FREQUENCY = 10594,
};

class spell_oscillating_frequency_scanner : public SpellScriptLoader
{
 public:
  spell_oscillating_frequency_scanner() : SpellScriptLoader("spell_oscillating_frequency_scanner") { }
  
  class spell_oscillating_frequency_scanner_SpellScript : public SpellScript
  {
   PrepareSpellScript(spell_oscillating_frequency_scanner_SpellScript);
   
   
   SpellCastResult CheckTarget()
   {
    if (GetCaster()->FindNearestCreature(NPC_OSCILLATING_FREQUENCY_TRIGGER, 25.0f))
     return SPELL_FAILED_TOO_CLOSE;
    
    return SPELL_CAST_OK;
   }
   
   void Register()
   {
    OnCheckCast += SpellCheckCastFn(spell_oscillating_frequency_scanner_SpellScript::CheckTarget);
   }
  };
  
  SpellScript* GetSpellScript() const
  {
   return new spell_oscillating_frequency_scanner_SpellScript();
  }
};

class npc_oscillating_frequency_trigger : public CreatureScript
{
public:
 npc_oscillating_frequency_trigger() : CreatureScript("npc_oscillating_frequency_trigger") { }

 struct npc_oscillating_frequency_triggerAI : public Scripted_NoMovementAI
 {
  npc_oscillating_frequency_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) { }

  void Reset()
  {
   me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
   refreshTimer = 2900;
   DoCast(SPELL_OSCILLATING_FIELD);
  }

  void UpdateAI(const uint32 diff)
  {
   if (refreshTimer < diff)
   {
    if (me->isSummon())
    {
     Unit* target = me->ToTempSummon()->GetSummoner();
     if (target->ToPlayer())
      if (target->ToPlayer()->GetQuestStatus(QUEST_GAUGING_THE_RESONANT_FREQUENCY) == QUEST_STATUS_INCOMPLETE)
      {
       std::list<Creature*> triggerList;
       target->GetCreatureListWithEntryInGrid(triggerList, NPC_OSCILLATING_FREQUENCY_TRIGGER, 40.0f);
       
       if (triggerList.size() >= 5)
        target->ToPlayer()->AreaExploredOrEventHappens(QUEST_GAUGING_THE_RESONANT_FREQUENCY);
        //target->ToPlayer()->KilledMonsterCredit(NPC_OSCILLATING_FREQUENCY_TRIGGER, 0);
      }
    }
    refreshTimer = 2900;
   }
   else
    refreshTimer -= diff;
  }
  
  uint32 refreshTimer;
 };

 CreatureAI* GetAI(Creature* pCreature) const
 {
  return new npc_oscillating_frequency_triggerAI(pCreature);
 }
};

/*###########################
# npc_father_kamaros (31279)
###########################*/

enum NotDeadYet
{
    QUEST_NOT_DEAD_YET_A = 13221,
    QUEST_NOT_DEAD_YET_H = 13229,
    NPC_NOT_DEAD_YET_TRIG = 97002,
    SPELL_PW_FORTITUDE = 23947,
    SPELL_PW_SHIELD = 20697
};

class npc_father_kamaros : public CreatureScript
{
public:
    npc_father_kamaros() : CreatureScript("npc_father_kamaros") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_NOT_DEAD_YET_A || quest->GetQuestId() == QUEST_NOT_DEAD_YET_H)
        {
            creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            creature->AI()->SetGUID(player->GetGUID());
            creature->CastSpell(player,SPELL_PW_FORTITUDE,true);
            player->CastSpell(player,SPELL_PW_SHIELD,true);
        }
        return true;
    }

	struct npc_father_kamarosAI : public npc_escortAI
    {
        npc_father_kamarosAI(Creature* creature) : npc_escortAI(creature) { }

        uint64 uiPlayer;
        
        void SetGUID(uint64 uiGuid, int32 /*iId*/)
        {
            me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_IMMUNE_TO_PC);
            uiPlayer = uiGuid;
			Start(false, false, uiPlayer, NULL, true);
        }

		void WaypointReached(uint32 point)
		{
			if (point == 53)
			{
				if (Player* player = me->GetPlayer(*me, uiPlayer))
                {
                    switch (player->GetTeam())
                    {
                        case ALLIANCE: player->GroupEventHappens(QUEST_NOT_DEAD_YET_A, me); break;
                        case HORDE: player->GroupEventHappens(QUEST_NOT_DEAD_YET_H, me); break;
                    }
                }
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_father_kamarosAI(creature);
    }
};

enum Midsummer_Blazier
{
	GO_TARGET_BLAZIER = 187708,
	NPC_TARGET_BLAZIER = 25515,
	SPELL_VISUAL_HUNTERS_MARK = 64328,
};

class npc_midsummer_blazier : public CreatureScript
{
public:
    npc_midsummer_blazier() : CreatureScript("npc_midsummer_blazier") { }

	struct npc_midsummer_blazierAI : public NullCreatureAI
    {
		npc_midsummer_blazierAI(Creature* creature) : NullCreatureAI(creature), timer(3250) { }

		uint32 timer;

        void UpdateAI(uint32 const diff)
		{
			if (timer < diff)
			{
				std::list<GameObject*> list;
				me->GetGameObjectListWithEntryInGrid(list, GO_TARGET_BLAZIER, 100.0f);
				if (list.empty())
				{
					timer = 3250;
					return;
				}
				std::list<GameObject*>::iterator itr = list.begin();
				uint32 at = urand(0, list.size()-1);
				for (uint32 k = 0; k < at; k++)
					itr++;
			
				if (Creature* summon = me->SummonCreature(NPC_TARGET_BLAZIER, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ()+1.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 3250))
					summon->CastSpell(summon, SPELL_VISUAL_HUNTERS_MARK, true);

				timer = 3250;
			}
			else
				timer -= diff;
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_midsummer_blazierAI(creature);
    }
};

enum HalloweenData
{
    NPC_STINKY_BOMB_CREDIT                  = 15415,

    GO_STINKY_BOMB_FLASK                    = 180449,
    GO_STINKY_BOMB_CLOUD                    = 180450,

    QUEST_CRASHING_WICKERMAN_FESTIVAL       = 1658,
    //costumes
    SPELL_PIRATE_COSTUME_MALE               = 24708,
    SPELL_PIRATE_COSTUME_FEMALE             = 24709,
    SPELL_NINJA_COSTUME_MALE                = 24711,
    SPELL_NINJA_COSTUME_FEMALE              = 24710,
    SPELL_LEPER_GNOME_COSTUME_MALE          = 24712,
    SPELL_LEPER_GNOME_COSTUME_FEMALE        = 24713,
    SPELL_GHOST_COSTUME_MALE                = 24735,
    SPELL_GHOST_COSTUME_FEMALE              = 24736,
    //wand spells
    SPELL_HALLOWEEN_WAND_PIRATE             = 24717,
    SPELL_HALLOWEEN_WAND_NINJA              = 24718,
    SPELL_HALLOWEEN_WAND_LEPER_GNOME        = 24719,
    SPELL_HALLOWEEN_WAND_RANDOM             = 24720,
    SPELL_HALLOWEEN_WAND_SKELETON           = 24724,
    SPELL_HALLOWEEN_WAND_WISP               = 24733,
    SPELL_HALLOWEEN_WAND_GHOST              = 24737,
    SPELL_HALLOWEEN_WAND_BAT                = 24741,

    SPELL_GRIM_VISAGE                       = 24705,

    // Headless Horseman fire event
    NPC_HEADLESS_FIRE                       = 23537,
    NPC_FIRE_DUMMY                          = 23686,
    NPC_SHADE_HORSEMAN                      = 23543,
    GO_FIRE_EFFIGY                          = 186720,
    GO_LARGE_JACK_O_LANTERN                 = 186887,


    SPELL_FIRE_CREATE_NODE                  = 42118,
    SPELL_WATER_SPOUT_VISUAL                = 42348,
    SPELL_FIRE_VISUAL_BUFF                  = 42074,
    SPELL_FIRE_SIZE_STACK                   = 42091,
    SPELL_FIRE_STARTING_SIZE                = 42096,
    SPELL_QUESTS_CREDITS                    = 42242,
    SPELL_CREATE_WATER_BUCKET               = 42349,

    SPELL_HORSEMAN_CONFLAGRATION_1          = 42380,
    SPELL_HORSEMAN_CONFLAGRATION_2          = 42869,
    SPELL_HORSEMAN_JACK_O_LANTERN           = 44185,
    SPELL_HORSEMAN_CLEAVE                   = 15496,

    SAY_HORSEMAN_SPAWN                      = 1,
    SAY_HORSEMAN_FIRES_OUT                  = 2,
    SAY_HORSEMAN_FIRES_FAIL                 = 3,
    SAY_HORSEMAN_LAUGHS                     = 4,
    SAY_DEATH                               = 5,

    QUEST_LET_THE_FIRES_COME_A              = 12135,
    QUEST_LET_THE_FIRES_COME_H              = 12139,
    QUEST_STOP_FIRES_A                      = 11131,
    QUEST_STOP_FIRES_H                      = 11219,
};

#define FIRE_NODES_PER_AREA 13

enum HalloweenFireEvents
{
    EVENT_FIRE_NONE,
    EVENT_FIRE_HIT_BY_BUCKET,
    EVENT_FIRE_VISUAL_WATER,
    EVENT_FIRE_GROW_FIRE,
    EVENT_HORSEMAN_CONFLAGRATION,
    EVENT_HORSEMAN_CLEAVE,
    EVENT_HORSEMAN_LAUGHS,
    EVENT_FIRE_FINISH,
    EVENT_FIRE_FAIL,
};

const Position FireNodesGoldShire[FIRE_NODES_PER_AREA + 1] =
{
    {-9459.41f, 43.90f, 64.23f, 0.00f},
    {-9472.57f, 41.11f, 64.17f, 0.00f},
    {-9467.22f, 85.86f, 66.20f, 0.00f},
    {-9472.94f, 93.84f, 69.20f, 0.00f},
    {-9462.50f, 103.90f, 68.51f, 0.00f},
    {-9467.84f, 100.69f, 66.12f, 0.00f},
    {-9456.91f, 112.81f, 66.12f, 0.00f},
    {-9478.22f, 41.65f, 69.85f, 0.00f},
    {-9481.30f, 24.87f, 69.08f, 0.00f},
    {-9482.69f, 14.39f, 62.94f, 0.00f},
    {-9471.16f, -6.65f, 70.76f, 0.00f},
    {-9451.26f, 38.83f, 68.02f, 0.00f},
    {-9450.13f, 89.46f, 66.22f, 0.00f},
    {-9464.28f,68.1982f,56.2331f,0.0f}, // Center of Town
};

const Position FireNodesRazorHill[FIRE_NODES_PER_AREA+1] =
{
    {372.70f, -4714.64f, 23.11f, 0.00f},
    {343.11f, -4708.87f, 29.19f, 0.00f},
    {332.06f, -4703.21f, 24.52f, 0.00f},
    {317.20f, -4694.22f, 16.78f, 0.00f},
    {326.30f, -4693.24f, 34.59f, 0.00f},
    {281.18f, -4705.37f, 22.38f, 0.00f},
    {293.32f, -4773.45f, 25.03f, 0.00f},
    {280.17f, -4831.90f, 22.25f, 0.00f},
    {319.04f, -4770.23f, 31.47f, 0.00f},
    {362.50f, -4676.11f, 28.63f, 0.00f},
    {348.71f, -4805.08f, 32.23f, 0.00f},
    {342.88f, -4837.07f, 26.29f, 0.00f},
    {361.80f, -4769.27f, 18.49f, 0.00f},
    {317.837f,-4734.06f,9.76272f,0.0f}, // Center of Town
};

/* This should be fixed ASAP, as far as I know, HHman should appear flying on villages and
start casting SPELL_FIRE_CREATE_NODE on their buildings, maybe also son zone warning, also need
to fix the quests, there are 2 aviable now, when only one should be depending if the village is
alreade setted on fire or nor.
*/
class npc_shade_horseman : public CreatureScript
{
public:
    npc_shade_horseman() : CreatureScript("npc_shade_horseman") { }


    struct npc_shade_horsemanAI : public ScriptedAI
    {
        npc_shade_horsemanAI(Creature* c) : ScriptedAI(c), fires(c) {}

        SummonList fires;
        EventMap events;
        bool moving;
        bool pointReached;
        bool allFiresSet;
        bool firesOut;
        uint32 wpCount;
        std::list<uint64> _playerList;

        void Reset()
        {
			me->SetCanFly(true);
			me->SetDisableGravity(true);
            moving = true;
            pointReached = true;
            allFiresSet = false;
            firesOut = false;
            wpCount = 0;
            _playerList.clear();
            events.Reset();

            me->Mount(25159);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_DISABLE_MOVE);
            me->AddUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT | MOVEMENTFLAG_DISABLE_GRAVITY);

            events.ScheduleEvent(EVENT_HORSEMAN_LAUGHS, urand(5000, 10000));
            events.ScheduleEvent(EVENT_HORSEMAN_CONFLAGRATION, urand(7000, 14000));
            events.ScheduleEvent(EVENT_FIRE_FAIL, 600000);
        }

        void JustSummoned(Creature* summon)
        {
            if (!summon)
                return;

            summon->SetCreatorGUID(me->GetGUID());
            fires.Summon(summon);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            if (!summon)
                return;

            fires.Despawn(summon);
        }

        void JustDied(Unit* killer)
        {
            if (killer && killer->GetAreaId() == me->GetAreaId())
                killer->SummonGameObject(GO_LARGE_JACK_O_LANTERN, me->GetPositionX(), me->GetPositionY(), killer->GetPositionZ()+1.0f, me->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 180000);

            Talk(SAY_DEATH);
        }
        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (moving && id < FIRE_NODES_PER_AREA)
            {
                if (id == 0 && !allFiresSet)
                {
                    sLog->outBasic("HEADLESS HORSEMAN BUG TRACKING Area %u.", me->GetAreaId());
                    Talk(SAY_HORSEMAN_SPAWN);
                }

                if (!allFiresSet)
                {
                    const Position pos = GetPositionsForArea()[wpCount];
                    if (pos.IsPositionValid())
                        me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_FIRE_CREATE_NODE, true);
                }

                if (id+1 == FIRE_NODES_PER_AREA)
                {
                    allFiresSet = true;
                    wpCount = 0;
                }
                else
                    ++wpCount;

                pointReached = true;
            } 
            else 
                if (id == FIRE_NODES_PER_AREA && firesOut)
                {
                    me->Dismount(); //or unmount
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_DISABLE_MOVE);
					me->RemoveUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT | MOVEMENTFLAG_DISABLE_GRAVITY);
                    events.ScheduleEvent(EVENT_HORSEMAN_CLEAVE, urand(5000, 10000));
                }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);


            if (!firesOut)
            {
                Position pos = GetPositionsForArea()[wpCount];

                if (!pos.IsPositionValid())
                    return;

                if (pointReached)
                {
                    pointReached = false;
                    me->GetMotionMaster()->MovePoint(wpCount, pos.GetPositionX(), pos.GetPositionY(), GetZForArea());
                }

                if (allFiresSet && fires.size() <= (uint32)(FIRE_NODES_PER_AREA*0.3f) && wpCount != FIRE_NODES_PER_AREA+1)
                {
                    Talk(SAY_HORSEMAN_FIRES_OUT);
                    wpCount = FIRE_NODES_PER_AREA;
                    me->GetMotionMaster()->Clear();
					pos = GetPositionsForArea()[FIRE_NODES_PER_AREA];
                    me->GetMotionMaster()->MovePoint(FIRE_NODES_PER_AREA, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
                    fires.DespawnAll();
                    firesOut = true;
                    // Credit quest to players
                    if (!_playerList.empty())
                        for (std::list<uint64>::const_iterator i = _playerList.begin();i != _playerList.end(); ++i)
                            if (Player* player = me->GetPlayer(*me, *i))
                                player->CastSpell(player, SPELL_QUESTS_CREDITS, true);
                    return;
                }

                switch(events.ExecuteEvent())
                {
                    case EVENT_FIRE_FAIL:
                        fires.DespawnAll();
                        Talk(SAY_HORSEMAN_FIRES_FAIL);
                        wpCount = FIRE_NODES_PER_AREA+1;
                        me->GetMotionMaster()->MovePoint(wpCount, pos.GetPositionX(), pos.GetPositionY(), GetZForArea());
                        if (!_playerList.empty())
                        {
                            for (std::list<uint64>::const_iterator i = _playerList.begin();i != _playerList.end(); ++i)
                            {
                                Player* player = me->GetPlayer(*me, *i);
                                if (player)
                                {
                                    uint32 questId = player->GetTeam() == ALLIANCE ? QUEST_LET_THE_FIRES_COME_A : QUEST_LET_THE_FIRES_COME_H;
                                    if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                                        player->FailQuest(questId);
                                }
                            }
                        }
                        me->DespawnOrUnsummon(10000);
                        break;
                    case EVENT_HORSEMAN_CONFLAGRATION:
                        if (!_playerList.empty())
                        {
                            for (std::list<uint64>::const_iterator i = _playerList.begin();i != _playerList.end(); ++i)
                            {
                                Player* player = me->GetPlayer(*me, *i);
                                if (player && player->GetDistance(me) <= 30.0f)
                                {
                                    me->CastSpell(player, RAND(SPELL_HORSEMAN_CONFLAGRATION_1, SPELL_HORSEMAN_CONFLAGRATION_2, SPELL_HORSEMAN_JACK_O_LANTERN), true);
                                    break;
                                }
                            }
                        }
                        events.ScheduleEvent(EVENT_HORSEMAN_CONFLAGRATION, urand(5000, 10000));
                        break;
                    case EVENT_HORSEMAN_LAUGHS:
                        Talk(SAY_HORSEMAN_LAUGHS);
                        events.ScheduleEvent(EVENT_HORSEMAN_LAUGHS, urand(10000, 25000));
                        break;
                }
            } 
            else
            {
                switch(events.ExecuteEvent())
                {
                    case EVENT_HORSEMAN_CLEAVE:
                        if (Unit* victim = me->getVictim())
                            me->CastSpell(victim, SPELL_HORSEMAN_CLEAVE, true);
                        events.ScheduleEvent(EVENT_HORSEMAN_CLEAVE, urand(5000, 10000));
                        return;
                    case EVENT_HORSEMAN_CONFLAGRATION:
                        if (!_playerList.empty())
                        {
                            for (std::list<uint64>::const_iterator i = _playerList.begin();i != _playerList.end(); ++i)
                            {
                                Player* player = me->GetPlayer(*me, *i);
								if (player && me->getVictim() != player && player->GetAreaId() == me->GetAreaId() && player->GetDistance(me) <= 30.0f)
                                {
                                    me->CastSpell(player, RAND(SPELL_HORSEMAN_CONFLAGRATION_1, SPELL_HORSEMAN_CONFLAGRATION_2, SPELL_HORSEMAN_JACK_O_LANTERN), true);
                                    break;
                                }
                            }
                        }
                        events.ScheduleEvent(EVENT_HORSEMAN_CONFLAGRATION, urand(5000, 10000));
                        break;
                }
                DoMeleeAttackIfReady();
            }
        }

        const Position* GetPositionsForArea()
        {
            switch (me->GetAreaId())
            {
                case 87: //GoldShire
                    return FireNodesGoldShire;
                case 362: // Razor Hill
                case 2337:
                case 14:
                    return FireNodesRazorHill;
            }
            return NULL;
        }

        float GetZForArea()
        {
            switch (me->GetAreaId())
            {
                case 87: //GoldShire
                    return 77.6f;
                case 362: // Razor Hill
                case 2337:
                case 14:
                    return 40.0f;
            }
            return 0.0f;
        }

        void SetGUID(uint64 guid, int32 id)
        {
            if (id == EVENT_FIRE_HIT_BY_BUCKET)
            {
                _playerList.push_back(guid);
                _playerList.unique();
            }

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shade_horsemanAI(creature);
    }
};

class npc_halloween_orphan_matron : public CreatureScript
{
public:
    npc_halloween_orphan_matron() : CreatureScript("npc_halloween_orphan_matron") { }

    uint64 _headlessHoresemanGUID;

    bool OnGossipHello(Player* player, Creature* me)
    {
        player->PrepareQuestMenu(me->GetGUID());

        if (Creature* horseman = me->GetCreature(*me, _headlessHoresemanGUID))
        {
            QuestMenu &qm = player->PlayerTalkClass->GetQuestMenu();
            QuestMenu qm2;

            uint32 quest1 = player->GetTeam() == ALLIANCE ? QUEST_LET_THE_FIRES_COME_A : QUEST_LET_THE_FIRES_COME_H;
            uint32 quest2 = player->GetTeam() == ALLIANCE ? QUEST_STOP_FIRES_A : QUEST_STOP_FIRES_H;

            // Copy current quest menu ignoring some quests
            for (uint32 i = 0; i<qm.GetMenuItemCount(); ++i)
            {
                if (qm.GetItem(i).QuestId == quest1 || qm.GetItem(i).QuestId == quest2)
                    continue;

                qm2.AddMenuItem(qm.GetItem(i).QuestId, qm.GetItem(i).QuestIcon);
            }

            if (player->GetQuestStatus(quest1) == QUEST_STATUS_NONE)
            {
                if (player->GetQuestStatus(quest2) == QUEST_STATUS_NONE)
                    qm2.AddMenuItem(quest2, 2);
                else if (player->GetQuestStatus(quest2) != QUEST_STATUS_REWARDED)
                    qm2.AddMenuItem(quest2, 4);
            }
            else
                if (player->GetQuestStatus(quest1) != QUEST_STATUS_REWARDED)
                    qm2.AddMenuItem(quest1, 4);

            qm.ClearMenu();

            for (uint32 i = 0; i<qm2.GetMenuItemCount(); ++i)
                qm.AddMenuItem(qm2.GetItem(i).QuestId, qm2.GetItem(i).QuestIcon);
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnQuestAccept(Player* player, Creature* me, Quest const* quest)
    {
        if (!(me->GetAreaId() == 87 || me->GetAreaId() == 362))
            return true;
		
        if (quest->GetQuestId() == QUEST_LET_THE_FIRES_COME_A || quest->GetQuestId() == QUEST_LET_THE_FIRES_COME_H)
        {
            Creature* horseman = me->GetCreature(*me, _headlessHoresemanGUID);

            if (!horseman)
            {
                sLog->outBasic("HEADLESS HORSEMAN BUG TRACKING. SUMMON: GUID Player: %u. Area %u.", player->GetGUID(), player->GetAreaId());
                if (Creature* newHorseman = player->SummonCreature(NPC_SHADE_HORSEMAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 20.0f, 0, TEMPSUMMON_DEAD_DESPAWN, 180000))
                    _headlessHoresemanGUID = newHorseman->GetGUID();
            }
        }
        return true;
    }
};

class at_wickerman_festival : public AreaTriggerScript
{
    public:
        at_wickerman_festival() : AreaTriggerScript("at_wickerman_festival") {}

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            player->GroupEventHappens(QUEST_CRASHING_WICKERMAN_FESTIVAL, player);
            return true;
        }
};



#define GOSSIP_WICKERMAN_EMBER "Smear the ash on my face like war paint!" //"Smear the ash on my face like war paint!"

class go_wickerman_ember : public GameObjectScript
{
public:
    go_wickerman_ember() : GameObjectScript("go_wickerman_ember") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (!player->HasAura(SPELL_GRIM_VISAGE))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WICKERMAN_EMBER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF)
            go->CastSpell(player, SPELL_GRIM_VISAGE);

        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();
        return true;
    }
};

class item_water_bucket : public ItemScript
{
    public:

        item_water_bucket() : ItemScript("item_water_bucket") { }

        bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
        {
            if (Creature* dummy = player->SummonCreature(NPC_FIRE_DUMMY, targets.GetDstPos()->GetPositionX(), targets.GetDstPos()->GetPositionY(), targets.GetDstPos()->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 500))
            {
                std::list<Creature*> firesList;
                Trinity::AllCreaturesOfEntryInRange checker(dummy, NPC_HEADLESS_FIRE, 3.0f);
                Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(dummy, firesList, checker);
                player->VisitNearbyObject(3.0f, searcher);

                if (firesList.empty())
                {
                    // Just some extra checks...
                    Creature* fire = dummy->FindNearestCreature(NPC_HEADLESS_FIRE, 3.0f, true);
                    if (fire && fire->isAlive())
                        fire->AI()->SetGUID(player->GetGUID(), EVENT_FIRE_HIT_BY_BUCKET);
                    else if (Player* friendPlr = dummy->SelectNearestPlayer(3.0f))
                    {
                        if (friendPlr->IsFriendlyTo(player) && friendPlr->isAlive())
                            player->CastSpell(friendPlr, SPELL_CREATE_WATER_BUCKET, true);
                    }
                    else
                        return false;
                }

                for (std::list<Creature*>::const_iterator i = firesList.begin(); i != firesList.end(); ++i)
                    if ((*i) && (*i)->isAlive())
                        (*i)->AI()->SetGUID(player->GetGUID(), EVENT_FIRE_HIT_BY_BUCKET);
            }
            return false;
        }
};

class npc_halloween_fire : public CreatureScript
{
public:
    npc_halloween_fire() : CreatureScript("npc_halloween_fire") { }


    struct npc_halloween_fireAI : public ScriptedAI
    {
        npc_halloween_fireAI(Creature* c) : ScriptedAI(c) {}

        bool fireEffigy;
        bool off;
        EventMap events;
        uint64 _playerGUID;

        void Reset()
        {
            off = false;
            fireEffigy = false;
            _playerGUID = 0;
            events.Reset();
            // Mark the npc if is for handling effigy instead of horseman fires
            if(GameObject* effigy = me->FindNearestGameObject(GO_FIRE_EFFIGY, 0.5f))
                fireEffigy = true;
            me->CastSpell(me, SPELL_FIRE_STARTING_SIZE, true);
            events.ScheduleEvent(EVENT_FIRE_GROW_FIRE, 1000);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            switch(events.ExecuteEvent())
            {
                case EVENT_FIRE_VISUAL_WATER:
                    me->CastSpell(me, SPELL_WATER_SPOUT_VISUAL, true);
                    if (fireEffigy)
                    {
                        if (GameObject* effigy = me->FindNearestGameObject(GO_FIRE_EFFIGY, 0.5f))
                        {
                            effigy->SetGoState(GO_STATE_READY);
                            if (Player* player = me->GetPlayer(*me, _playerGUID))
                                player->KilledMonsterCredit(me->GetEntry(),0);
                            events.ScheduleEvent(EVENT_FIRE_GROW_FIRE, 22000);
                        }
                    } else {
                        if (Aura* fireSize = me->GetAura(SPELL_FIRE_SIZE_STACK))
                        {
                            if (fireSize->GetStackAmount() < 10)
                            {
                                me->RemoveAura(fireSize);
                                me->RemoveAurasDueToSpell(SPELL_FIRE_VISUAL_BUFF);
                                me->DespawnOrUnsummon(1000);
                            } else
                                fireSize->ModStackAmount(-10);
                        }
                    }
                    break;
                case EVENT_FIRE_GROW_FIRE:
                    if (fireEffigy)
                    {
                        if (GameObject* effigy = me->FindNearestGameObject(GO_FIRE_EFFIGY, 0.5f))
                            effigy->SetGoState(GO_STATE_ACTIVE);
                    } else {
                        if (off) break; // This fire have been extinguished

                        if (Aura* fireSize = me->GetAura(SPELL_FIRE_SIZE_STACK)) // This fire have maxium size
                            if(fireSize->GetStackAmount() == 255) break;

                        if (!me->HasAura(SPELL_FIRE_STARTING_SIZE))
                            me->CastSpell(me, SPELL_FIRE_STARTING_SIZE, true);
                        if (!me->HasAura(SPELL_FIRE_VISUAL_BUFF))
                            me->CastSpell(me, SPELL_FIRE_VISUAL_BUFF, true);
                        me->CastSpell(me, SPELL_FIRE_SIZE_STACK, true);
                        events.ScheduleEvent(EVENT_FIRE_GROW_FIRE, urand(1000,2500));
                    }
                    break;
            }
        }

        void SetGUID(uint64 guid, int32 id)
        {
            if (off) return;

            if (id == EVENT_FIRE_HIT_BY_BUCKET)
            {
                _playerGUID = guid;
                if (fireEffigy)
                {
                    if (GameObject* effigy = me->FindNearestGameObject(GO_FIRE_EFFIGY, 0.5f))
                        if (effigy->GetGoState() == GO_STATE_ACTIVE)
                            events.ScheduleEvent(EVENT_FIRE_VISUAL_WATER, 1000);
                } else
                {
                    if (Creature* horseman = me->GetCreature(*me, me->GetCreatorGUID()))
                        horseman->AI()->SetGUID(_playerGUID, EVENT_FIRE_HIT_BY_BUCKET);
                    events.ScheduleEvent(EVENT_FIRE_VISUAL_WATER, 1000);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_halloween_fireAI(creature);
    }
};

enum FriendOrFowl
{
    SPELL_TURKEY_VENGEANCE  = 25285,
};

class spell_gen_turkey_marker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_marker() : SpellScriptLoader("spell_gen_turkey_marker") { }

        class spell_gen_turkey_marker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_turkey_marker_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // store stack apply times, so we can pop them while they expire
                _applyTimes.push_back(getMSTime());
                Unit* target = GetTarget();

                // on stack 15 cast the achievement crediting spell
                if (GetStackAmount() >= 15)
                    target->CastSpell(target, SPELL_TURKEY_VENGEANCE, true, NULL, aurEff, GetCasterGUID());
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (_applyTimes.empty())
                    return;

                // pop stack if it expired for us
                if (_applyTimes.front() + GetMaxDuration() < getMSTime())
                    ModStackAmount(-1, AURA_REMOVE_BY_EXPIRE);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_turkey_marker_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_turkey_marker_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

            std::list<uint32> _applyTimes;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_turkey_marker_AuraScript();
        }
};

enum TheTurkinator
{
    SPELL_KILL_COUNTER_VISUAL       = 62015,
    SPELL_KILL_COUNTER_VISUAL_MAX   = 62021,
};

#define THE_THUKINATOR_10           "Turkey Hunter!"
#define THE_THUKINATOR_20           "Turkey Domination!"
#define THE_THUKINATOR_30           "Turkey Slaughter!"
#define THE_THUKINATOR_40           "TURKEY TRIUMPH!"

class spell_gen_turkey_tracker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_tracker() : SpellScriptLoader("spell_gen_turkey_tracker") {}

        class spell_gen_turkey_tracker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_turkey_tracker_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetAura(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return;
                    
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                if (Aura const* aura = GetCaster()->ToPlayer()->GetAura(GetSpellInfo()->Id))
                {
                    switch (aura->GetStackAmount())
                    {
                        case 10:
                            target->MonsterTextEmote(THE_THUKINATOR_10, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            break;
                        case 20:
                            target->MonsterTextEmote(THE_THUKINATOR_20, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            break;
                        case 30:
                            target->MonsterTextEmote(THE_THUKINATOR_30, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            break;
                        case 40:
                            target->MonsterTextEmote(THE_THUKINATOR_40, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL_MAX); // Achievement Credit
                            break;
                        default:
                            break;
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_turkey_tracker_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_turkey_tracker_SpellScript();
        }
};

class spell_gen_feast_on : public SpellScriptLoader
{
    public:
        spell_gen_feast_on() : SpellScriptLoader("spell_gen_feast_on") { }

        class spell_gen_feast_on_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_feast_on_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetSpellInfo()->Effects[EFFECT_0].CalcValue();

                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                    if (Unit* player = caster->GetVehicleKit()->GetPassenger(0))
                        caster->CastSpell(player, bp0, true, NULL, NULL, player->ToPlayer()->GetGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_feast_on_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_feast_on_SpellScript();
        }
};

enum WellFedPilgrimsBounty
{
    // Feast On
    SPELL_A_SERVING_OF_TURKEY           = 61807,
    SPELL_A_SERVING_OF_CRANBERRIES      = 61804,
    SPELL_A_SERVING_OF_STUFFING         = 61806,
    SPELL_A_SERVING_OF_SWEET_POTATOES   = 61808,
    SPELL_A_SERVING_OF_PIE              = 61805,

    // Well Fed
    SPELL_WELL_FED_AP                   = 65414,
    SPELL_WELL_FED_ZM                   = 65412,
    SPELL_WELL_FED_HIT                  = 65416,
    SPELL_WELL_FED_HASTE                = 65410,
    SPELL_WELL_FED_SPIRIT               = 65415,

    // Pilgrim's Paunch
    SPELL_THE_SPIRIT_OF_SHARING         = 61849,
};

class spell_gen_well_fed_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_well_fed_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_well_fed_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_well_fed_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_well_fed_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                Aura const* Turkey = target->GetAura(SPELL_A_SERVING_OF_TURKEY);
                Aura const* Cranberies = target->GetAura(SPELL_A_SERVING_OF_CRANBERRIES);
                Aura const* Stuffing = target->GetAura(SPELL_A_SERVING_OF_STUFFING);
                Aura const* SweetPotatoes = target->GetAura(SPELL_A_SERVING_OF_SWEET_POTATOES);
                Aura const* Pie = target->GetAura(SPELL_A_SERVING_OF_PIE);

                if (Aura const* aura = target->GetAura(_triggeredSpellId1))
                {
                    if (aura->GetStackAmount() == 5)
                        target->CastSpell(target, _triggeredSpellId2, true);
                }

                // The Spirit of Sharing - Achievement Credit
                if (!target->GetAura(SPELL_THE_SPIRIT_OF_SHARING))
                {
                    if ((Turkey && Turkey->GetStackAmount() == 5) && (Cranberies && Cranberies->GetStackAmount() == 5) && (Stuffing && Stuffing->GetStackAmount() == 5) &&
                        (SweetPotatoes && SweetPotatoes->GetStackAmount() == 5) && (Pie && Pie->GetStackAmount() == 5))
                        target->CastSpell(target, SPELL_THE_SPIRIT_OF_SHARING, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_well_fed_pilgrims_bounty_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_well_fed_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum OnPlatePilgrimsBounty
{
    // "FOOD FIGHT!" - Achivement Credit
    SPELL_ON_PLATE_TURKEY           = 61928,
    SPELL_ON_PLATE_CRANBERRIES      = 61925,
    SPELL_ON_PLATE_STUFFING         = 61927,
    SPELL_ON_PLATE_SWEET_POTATOES   = 61929,
    SPELL_ON_PLATE_PIE              = 61926,

    // Sharing is Caring - Achivement Credit
    SPELL_PASS_THE_TURKEY           = 66373,
    SPELL_PASS_THE_CRANBERRIES      = 66372,
    SPELL_PASS_THE_STUFFING         = 66375,
    SPELL_PASS_THE_SWEET_POTATOES   = 66376,
    SPELL_PASS_THE_PIE              = 66374,
};

class spell_gen_on_plate_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_on_plate_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_on_plate_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_on_plate_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_on_plate_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                {
                    Unit* player = caster->GetVehicleKit()->GetPassenger(0);
                    if (!player)
                        return;

                    player->CastSpell(GetHitUnit(), _triggeredSpellId1, true, NULL, NULL, player->ToPlayer()->GetGUID());
                    player->CastSpell(player, _triggeredSpellId2, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_on_plate_pilgrims_bounty_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_on_plate_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum BountifulFeast
{
    // Bountiful Feast
    SPELL_BOUNTIFUL_FEAST_DRINK          = 66041,
    SPELL_BOUNTIFUL_FEAST_FOOD           = 66478,
    SPELL_BOUNTIFUL_FEAST_REFRESHMENT    = 66622,
};

class spell_gen_bountiful_feast : public SpellScriptLoader
{
    public:
        spell_gen_bountiful_feast() : SpellScriptLoader("spell_gen_bountiful_feast") { }

        class spell_gen_bountiful_feast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bountiful_feast_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_DRINK, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_FOOD, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_REFRESHMENT, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_bountiful_feast_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bountiful_feast_SpellScript();
        }
};

enum PilgrimsBountyBuffFood
{
    // Pilgrims Bounty Buff Food
    SPELL_WELL_FED_AP_TRIGGER       = 65414,
    SPELL_WELL_FED_ZM_TRIGGER       = 65412,
    SPELL_WELL_FED_HIT_TRIGGER      = 65416,
    SPELL_WELL_FED_HASTE_TRIGGER    = 65410,
    SPELL_WELL_FED_SPIRIT_TRIGGER   = 65415,
};

class spell_pilgrims_bounty_buff_food : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;
    public:
        spell_pilgrims_bounty_buff_food(const char* name, uint32 triggeredSpellId) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_pilgrims_bounty_buff_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bounty_buff_food_AuraScript)
        private:
            uint32 _triggeredSpellId;

        public:
            spell_pilgrims_bounty_buff_food_AuraScript(uint32 triggeredSpellId) : AuraScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Load()
            {
                _handled = false;
                return true;
            }

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (_handled)
                    return;

                Unit* caster = GetCaster();
                if (!caster)
                    return;

                _handled = true;
                caster->CastSpell(caster, _triggeredSpellId, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pilgrims_bounty_buff_food_AuraScript::HandleTriggerSpell, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            bool _handled;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pilgrims_bounty_buff_food_AuraScript(_triggeredSpellId);
        }
};

enum WildTurkey
{
    SPELL_TURKEY_TRACKER        = 62014,
};

class npc_wild_turkey : public CreatureScript
{
public:
    npc_wild_turkey() : CreatureScript("npc_wild_turkey") { }

    struct npc_wild_turkeyAI : public ScriptedAI
    {
        npc_wild_turkeyAI(Creature* creature) : ScriptedAI(creature) {}

        void JustDied(Unit* killer)
        {
            if (killer && killer->GetTypeId() == TYPEID_PLAYER)
                killer->CastSpell(killer, SPELL_TURKEY_TRACKER);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wild_turkeyAI(creature);
    }
};



void AddSC_npcs_special()
{
	new npc_air_force_bots();
	new npc_lunaclaw_spirit();
	new npc_chicken_cluck();
	new npc_dancing_flames();
	new npc_doctor();
	new npc_injured_patient();
	new npc_garments_of_quests();
	new npc_guardian();
	new npc_mount_vendor();
	new npc_rogue_trainer();
	new npc_sayge();
	new npc_steam_tonk();
	new npc_tonk_mine();
	new npc_winter_reveler();
	new npc_brewfest_reveler();
	new npc_snake_trap();
	new npc_mirror_image();
	new npc_ebon_gargoyle();
	new npc_lightwell();
	new mob_mojo();
	new npc_training_dummy();
	new npc_shadowfiend();
	new npc_wormhole();
	new npc_pet_trainer();
	new npc_locksmith();
	new npc_experience();
	new npc_fire_elemental();
	new npc_earth_elemental();
	new npc_firework();
	new npc_spring_rabbit();
	new npc_kingdom_of_dalaran_quests();
	new npc_tabard_vendor();
	new npc_brewfest_trigger();
    new spell_brewfest_speed();
    new npc_brewfest_apple_trigger();
    //new item_brewfest_ChugAndChuck();
    new item_brewfest_ram_reins();
    new npc_brewfest_keg_thrower();
    new npc_brewfest_keg_receiver();
    new npc_brewfest_ram_master();
    new npc_dark_iron_guzzler();
    new npc_coren_direbrew();
    new npc_brewmaiden();
    new go_mole_machine_console();
	//custom from this point------------------------------------
	new npc_q11073_flare();
	new npc_hyldsmeet_warbear_rider();
	new npc_hyldsmeet_warbear();
	new npc_zul_drak_q12663_gorebag();
	new npc_zul_drak_q12673_geist();
	new npc_zul_drak_q12676_dynamite();
	new npc_zul_drak_q12676_bunny();
	new npc_zul_drak_q12710_malmortis();
	new npc_zul_drak_q12713_drakuru_start();
	new npc_zul_drak_q12713_drakuru_boss();
	new npc_wg_quest_giver();
	new npc_argent_squire();
	new npc_king_jokkum();
	new npc_king_jokkum_vehicle();
	new npc_q13046_jormungar();
	new npc_q12994_worg();
	new npc_q13003_wyrm();
	new npc_blessed_banner();
	new npc_captured_crusader();
	//new npc_the_ocular();
	new npc_general_lightsbane();
	//new npc_free_your_mind();
	new npc_saronite_mine_slave();
	new npc_vendor_tournament_fraction_champion();
	new npc_maiden_of_drakmar();
	new npc_slain_tualiq_villager();
	new npc_neltharaku();
	new npc_wyrmrest_defender();
	new npc_7th_legion_siege_engineer();
	new npc_generic_harpoon_cannon();
	new npc_signal_fire();
	new npc_keristrasza_coldarra();
	new npc_attracted_reef_bull();
	new npc_q12467_wyrmbait();
	//new npc_q13047_veranus();
	new npc_q13047_thorim();
	new npc_slim_giant();
	new npc_roanauk();
	new npc_q12910_frosthound();
	new vehicle_alliance_steamtank();
	//new spell_q13003_claw_swipe();
	new spell_carve_bear_flank();
	new spell_salvage_corpse();
	new spell_throw_ice();
	new spell_argent_cannon();
	new spell_drop_off_gnome();
	new spell_anuniaqs_net();
	new spell_contact_brann();
	new spell_summon_frosthound();
	new npc_archivist_mechaton();
	new npc_elder_kekek();
	new npc_high_oracle_soo_roo();
	new npc_winterfin_playmate();
	new npc_snowfall_glade_playmate();
	new npc_the_etymidian();
	new npc_the_biggest_tree();
	new npc_grizzlemaw_cw_trigger();
	new npc_cw_area_trigger();
	new npc_alexstraza_the_lifebinder();
	new npc_anchorite_barada();
	new npc_darkness_released();
	new npc_foul_purge();
	new npc_demoniac_scryer();
	new go_mana_bomb();
	new npc_mana_bomb();
	new npc_letoll();
	new spell_fumping_39238();
	new go_prophecy();
	new spell_fumping_39246();
	new spell_ride_the_lightning_37841();
	new go_fel_crystal_prism();
	new spell_oscillating_frequency_scanner();
	new npc_oscillating_frequency_trigger();
    new npc_father_kamaros();
	new npc_midsummer_blazier();

	new at_wickerman_festival();
    new npc_shade_horseman();
    new npc_halloween_orphan_matron();
	new go_wickerman_ember();
    new item_water_bucket();
    new npc_halloween_fire();
    new spell_gen_turkey_marker();
    new spell_gen_turkey_tracker();
    new spell_gen_feast_on();
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_ap", SPELL_A_SERVING_OF_TURKEY, SPELL_WELL_FED_AP);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_zm", SPELL_A_SERVING_OF_CRANBERRIES, SPELL_WELL_FED_ZM);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_hit", SPELL_A_SERVING_OF_STUFFING, SPELL_WELL_FED_HIT);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_haste", SPELL_A_SERVING_OF_SWEET_POTATOES, SPELL_WELL_FED_HASTE);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_spirit", SPELL_A_SERVING_OF_PIE, SPELL_WELL_FED_SPIRIT);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_turkey", SPELL_ON_PLATE_TURKEY, SPELL_PASS_THE_TURKEY);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_cranberries", SPELL_ON_PLATE_CRANBERRIES, SPELL_PASS_THE_CRANBERRIES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_stuffing", SPELL_ON_PLATE_STUFFING, SPELL_PASS_THE_STUFFING);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_sweet_potatoes", SPELL_ON_PLATE_SWEET_POTATOES, SPELL_PASS_THE_SWEET_POTATOES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_pie", SPELL_ON_PLATE_PIE, SPELL_PASS_THE_PIE);
    new spell_gen_bountiful_feast();
    new spell_pilgrims_bounty_buff_food("spell_gen_slow_roasted_turkey", SPELL_WELL_FED_AP_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_cranberry_chutney", SPELL_WELL_FED_ZM_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_spice_bread_stuffing", SPELL_WELL_FED_HIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_pumpkin_pie", SPELL_WELL_FED_SPIRIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_candied_sweet_potato", SPELL_WELL_FED_HASTE_TRIGGER);
    new npc_wild_turkey();
}
