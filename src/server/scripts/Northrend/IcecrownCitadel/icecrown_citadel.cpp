/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellAuraEffects.h"
#include "SmartAI.h"
#include "icecrown_citadel.h"

// Weekly quest support
// * Deprogramming                (DONE)
// * Securing the Ramparts        (DONE)
// * Residue Rendezvous           (DONE)
// * Blood Quickening             (DONE)
// * Respite for a Tormented Soul

enum Texts
{
	// Highlord Tirion Fordring (at Light's Hammer)
	SAY_TIRION_INTRO_1              = 0,
	SAY_TIRION_INTRO_2              = 1,
	SAY_TIRION_INTRO_3              = 2,
	SAY_TIRION_INTRO_4              = 3,
	SAY_TIRION_INTRO_H_5            = 4,
	SAY_TIRION_INTRO_A_5            = 5,

	// The Lich King (at Light's Hammer)
	SAY_LK_INTRO_1                  = 0,
	SAY_LK_INTRO_2                  = 1,
	SAY_LK_INTRO_3                  = 2,
	SAY_LK_INTRO_4                  = 3,
	SAY_LK_INTRO_5                  = 4,

	// Highlord Bolvar Fordragon (at Light's Hammer)
	SAY_BOLVAR_INTRO_1              = 0,

	// High Overlord Saurfang (at Light's Hammer)
	SAY_SAURFANG_INTRO_1            = 15,
	SAY_SAURFANG_INTRO_2            = 16,
	SAY_SAURFANG_INTRO_3            = 17,
	SAY_SAURFANG_INTRO_4            = 18,

	// Muradin Bronzebeard (at Light's Hammer)
	SAY_MURADIN_INTRO_1             = 13,
	SAY_MURADIN_INTRO_2             = 14,
	SAY_MURADIN_INTRO_3             = 15,

	// Deathbound Ward
	SAY_TRAP_ACTIVATE               = 0,

	// Rotting Frost Giant
	EMOTE_DEATH_PLAGUE_WARNING      = 0,

	// Sister Svalna
	SAY_SVALNA_KILL_CAPTAIN         = 1, // happens when she kills a captain
	SAY_SVALNA_KILL                 = 4,
	SAY_SVALNA_CAPTAIN_DEATH        = 5, // happens when a captain resurrected by her dies
	SAY_SVALNA_DEATH                = 6,
	EMOTE_SVALNA_IMPALE             = 7,
	EMOTE_SVALNA_BROKEN_SHIELD      = 8,

	SAY_CROK_INTRO_1                = 0, // Ready your arms, my Argent Brothers. The Vrykul will protect the Frost Queen with their lives.
	SAY_ARNATH_INTRO_2              = 5, // Even dying here beats spending another day collecting reagents for that madman, Finklestein.
	SAY_CROK_INTRO_3                = 1, // Enough idle banter! Our champions have arrived - support them as we push our way through the hall!
	SAY_SVALNA_EVENT_START          = 0, // You may have once fought beside me, Crok, but now you are nothing more than a traitor. Come, your second death approaches!
	SAY_CROK_COMBAT_WP_0            = 2, // Draw them back to us, and we'll assist you.
	SAY_CROK_COMBAT_WP_1            = 3, // Quickly, push on!
	SAY_CROK_FINAL_WP               = 4, // Her reinforcements will arrive shortly, we must bring her down quickly!
	SAY_SVALNA_RESURRECT_CAPTAINS   = 2, // Foolish Crok. You brought my reinforcements with you. Arise, Argent Champions, and serve the Lich King in death!
	SAY_CROK_COMBAT_SVALNA          = 5, // I'll draw her attacks. Return our brothers to their graves, then help me bring her down!
	SAY_SVALNA_AGGRO                = 3, // Come, Scourgebane. I'll show the master which of us is truly worthy of the title of "Champion"!
	SAY_CAPTAIN_DEATH               = 0,
	SAY_CAPTAIN_RESURRECTED         = 1,
	SAY_CAPTAIN_KILL                = 2,
	SAY_CAPTAIN_SECOND_DEATH        = 3,
	SAY_CAPTAIN_SURVIVE_TALK        = 4,
	SAY_CROK_WEAKENING_GAUNTLET     = 6,
	SAY_CROK_WEAKENING_SVALNA       = 7,
	SAY_CROK_DEATH                  = 8,
};

enum Spells
{
	// Rotting Frost Giant
	SPELL_DEATH_PLAGUE              = 72879,
	SPELL_DEATH_PLAGUE_AURA         = 72865,
	SPELL_RECENTLY_INFECTED         = 72884,
	SPELL_DEATH_PLAGUE_KILL         = 72867,
	SPELL_STOMP                     = 64652,
	SPELL_ARCTIC_BREATH             = 72848,

	// Frost Freeze Trap
	SPELL_COLDFLAME_JETS            = 70460,

	// Alchemist Adrianna
	SPELL_HARVEST_BLIGHT_SPECIMEN   = 72155,
	SPELL_HARVEST_BLIGHT_SPECIMEN25 = 72162,

	// Crok Scourgebane
	SPELL_ICEBOUND_ARMOR            = 70714,
	SPELL_SCOURGE_STRIKE            = 71488,
	SPELL_DEATH_STRIKE              = 71489,

	// Sister Svalna
	SPELL_CARESS_OF_DEATH           = 70078,
	SPELL_IMPALING_SPEAR_KILL       = 70196,
	SPELL_REVIVE_CHAMPION           = 70053,
	SPELL_UNDEATH                   = 70089,
	SPELL_IMPALING_SPEAR            = 71443,
	SPELL_AETHER_SHIELD             = 71463,
	SPELL_HURL_SPEAR                = 71466,

	// Captain Arnath
	SPELL_DOMINATE_MIND             = 14515,
	SPELL_FLASH_HEAL_NORMAL         = 71595,
	SPELL_POWER_WORD_SHIELD_NORMAL  = 71548,
	SPELL_SMITE_NORMAL              = 71546,
	SPELL_FLASH_HEAL_UNDEAD         = 71782,
	SPELL_POWER_WORD_SHIELD_UNDEAD  = 71780,
	SPELL_SMITE_UNDEAD              = 71778,

	// Captain Brandon
	SPELL_CRUSADER_STRIKE           = 71549,
	SPELL_DIVINE_SHIELD             = 71550,
	SPELL_JUDGEMENT_OF_COMMAND      = 71551,
	SPELL_HAMMER_OF_BETRAYAL        = 71784,

	// Captain Grondel
	SPELL_CHARGE                    = 71553,
	SPELL_MORTAL_STRIKE             = 71552,
	SPELL_SUNDER_ARMOR              = 71554,
	SPELL_CONFLAGRATION             = 71785,

	// Captain Rupert
	SPELL_FEL_IRON_BOMB_NORMAL      = 71592,
	SPELL_MACHINE_GUN_NORMAL        = 71594,
	SPELL_ROCKET_LAUNCH_NORMAL      = 71590,
	SPELL_FEL_IRON_BOMB_UNDEAD      = 71787,
	SPELL_MACHINE_GUN_UNDEAD        = 71788,
	SPELL_ROCKET_LAUNCH_UNDEAD      = 71786,
	
	// Invisible Stalker (Float, Uninteractible, LargeAOI)
	SPELL_SOUL_MISSILE              = 72585,
};

// Helper defines
// Captain Arnath
#define SPELL_FLASH_HEAL        (IsUndead ? SPELL_FLASH_HEAL_UNDEAD : SPELL_FLASH_HEAL_NORMAL)
#define SPELL_POWER_WORD_SHIELD (IsUndead ? SPELL_POWER_WORD_SHIELD_UNDEAD : SPELL_POWER_WORD_SHIELD_NORMAL)
#define SPELL_SMITE             (IsUndead ? SPELL_SMITE_UNDEAD : SPELL_SMITE_NORMAL)

// Captain Rupert
#define SPELL_FEL_IRON_BOMB     (IsUndead ? SPELL_FEL_IRON_BOMB_UNDEAD : SPELL_FEL_IRON_BOMB_NORMAL)
#define SPELL_MACHINE_GUN       (IsUndead ? SPELL_MACHINE_GUN_UNDEAD : SPELL_MACHINE_GUN_NORMAL)
#define SPELL_ROCKET_LAUNCH     (IsUndead ? SPELL_ROCKET_LAUNCH_UNDEAD : SPELL_ROCKET_LAUNCH_NORMAL)

enum EventTypes
{
	// Highlord Tirion Fordring (at Light's Hammer)
	// The Lich King (at Light's Hammer)
	// Highlord Bolvar Fordragon (at Light's Hammer)
	// High Overlord Saurfang (at Light's Hammer)
	// Muradin Bronzebeard (at Light's Hammer)
	EVENT_TIRION_INTRO_2                = 1,
	EVENT_TIRION_INTRO_3                = 2,
	EVENT_TIRION_INTRO_4                = 3,
	EVENT_TIRION_INTRO_5                = 4,
	EVENT_LK_INTRO_1                    = 5,
	EVENT_TIRION_INTRO_6                = 6,
	EVENT_LK_INTRO_2                    = 7,
	EVENT_LK_INTRO_3                    = 8,
	EVENT_LK_INTRO_4                    = 9,
	EVENT_BOLVAR_INTRO_1                = 10,
	EVENT_LK_INTRO_5                    = 11,
	EVENT_SAURFANG_INTRO_1              = 12,
	EVENT_TIRION_INTRO_H_7              = 13,
	EVENT_SAURFANG_INTRO_2              = 14,
	EVENT_SAURFANG_INTRO_3              = 15,
	EVENT_SAURFANG_INTRO_4              = 16,
	EVENT_SAURFANG_RUN                  = 17,
	EVENT_MURADIN_INTRO_1               = 18,
	EVENT_MURADIN_INTRO_2               = 19,
	EVENT_MURADIN_INTRO_3               = 20,
	EVENT_TIRION_INTRO_A_7              = 21,
	EVENT_MURADIN_INTRO_4               = 22,
	EVENT_MURADIN_INTRO_5               = 23,
	EVENT_MURADIN_RUN                   = 24,

	// Rotting Frost Giant
	EVENT_DEATH_PLAGUE                  = 25,
	EVENT_STOMP                         = 26,
	EVENT_ARCTIC_BREATH                 = 27,

	// Frost Freeze Trap
	EVENT_ACTIVATE_TRAP                 = 28,

	// Crok Scourgebane
	EVENT_SCOURGE_STRIKE                = 29,
	EVENT_DEATH_STRIKE                  = 30,
	EVENT_HEALTH_CHECK                  = 31,
	EVENT_CROK_INTRO_3                  = 32,
	EVENT_START_PATHING                 = 33,

	// Sister Svalna
	EVENT_ARNATH_INTRO_2                = 34,
	EVENT_SVALNA_START                  = 35,
	EVENT_SVALNA_RESURRECT              = 36,
	EVENT_SVALNA_COMBAT                 = 37,
	EVENT_IMPALING_SPEAR                = 38,
	EVENT_AETHER_SHIELD                 = 39,

	// Captain Arnath
	EVENT_ARNATH_FLASH_HEAL             = 40,
	EVENT_ARNATH_PW_SHIELD              = 41,
	EVENT_ARNATH_SMITE                  = 42,
	EVENT_ARNATH_DOMINATE_MIND          = 43,

	// Captain Brandon
	EVENT_BRANDON_CRUSADER_STRIKE       = 44,
	EVENT_BRANDON_DIVINE_SHIELD         = 45,
	EVENT_BRANDON_JUDGEMENT_OF_COMMAND  = 46,
	EVENT_BRANDON_HAMMER_OF_BETRAYAL    = 47,

	// Captain Grondel
	EVENT_GRONDEL_CHARGE_CHECK          = 48,
	EVENT_GRONDEL_MORTAL_STRIKE         = 49,
	EVENT_GRONDEL_SUNDER_ARMOR          = 50,
	EVENT_GRONDEL_CONFLAGRATION         = 51,

	// Captain Rupert
	EVENT_RUPERT_FEL_IRON_BOMB          = 52,
	EVENT_RUPERT_MACHINE_GUN            = 53,
	EVENT_RUPERT_ROCKET_LAUNCH          = 54,
	
	// Invisible Stalker (Float, Uninteractible, LargeAOI)
	EVENT_SOUL_MISSILE                  = 55,
};

enum DataTypesICC
{
	DATA_DAMNED_KILLS       = 1,
};

enum Actions
{
	// Sister Svalna
	ACTION_KILL_CAPTAIN         = 1,
	ACTION_START_GAUNTLET       = 2,
	ACTION_RESURRECT_CAPTAINS   = 3,
	ACTION_CAPTAIN_DIES         = 4,
	ACTION_RESET_EVENT          = 5,
};

enum EventIds
{
	EVENT_AWAKEN_WARD_1 = 22900,
	EVENT_AWAKEN_WARD_2 = 22907,
	EVENT_AWAKEN_WARD_3 = 22908,
	EVENT_AWAKEN_WARD_4 = 22909,
};

enum MovementPoints
{
	POINT_LAND  = 1,
};

class FrostwingVrykulSearcher
{
	public:
		FrostwingVrykulSearcher(Creature const* source, float range) : _source(source), _range(range) {}

		bool operator()(Unit* unit)
		{
			if (!unit->isAlive())
				return false;

			switch (unit->GetEntry())
			{
				case NPC_YMIRJAR_BATTLE_MAIDEN:
				case NPC_YMIRJAR_DEATHBRINGER:
				case NPC_YMIRJAR_FROSTBINDER:
				case NPC_YMIRJAR_HUNTRESS:
				case NPC_YMIRJAR_WARLORD:
					break;
				default:
					return false;
			}

			if (!unit->IsWithinDist(_source, _range, false))
				return false;

			return true;
		}

	private:
		Creature const* _source;
		float _range;
};

class FrostwingGauntletRespawner
{
	public:
		void operator()(Creature* creature)
		{
			switch (creature->GetOriginalEntry())
			{
				case NPC_YMIRJAR_BATTLE_MAIDEN:
				case NPC_YMIRJAR_DEATHBRINGER:
				case NPC_YMIRJAR_FROSTBINDER:
				case NPC_YMIRJAR_HUNTRESS:
				case NPC_YMIRJAR_WARLORD:
					break;
				case NPC_CROK_SCOURGEBANE:
				case NPC_CAPTAIN_ARNATH:
				case NPC_CAPTAIN_BRANDON:
				case NPC_CAPTAIN_GRONDEL:
				case NPC_CAPTAIN_RUPERT:
					creature->AI()->DoAction(ACTION_RESET_EVENT);
					break;
				case NPC_SISTER_SVALNA:
					creature->AI()->DoAction(ACTION_RESET_EVENT);
					// return, this creature is never dead if event is reset
					return;
				default:
					return;
			}

			uint32 corpseDelay = creature->GetCorpseDelay();
			uint32 respawnDelay = creature->GetRespawnDelay();
			creature->SetCorpseDelay(1);
			creature->SetRespawnDelay(2);

			if (CreatureData const* data = creature->GetCreatureData())
				creature->SetPosition(data->posX, data->posY, data->posZ, data->orientation);
			creature->DespawnOrUnsummon();

			creature->SetCorpseDelay(corpseDelay);
			creature->SetRespawnDelay(respawnDelay);
		}
};

class CaptainSurviveTalk : public BasicEvent
{
	public:
		explicit CaptainSurviveTalk(Creature const& owner) : _owner(owner) { }

		bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
		{
			_owner.AI()->Talk(SAY_CAPTAIN_SURVIVE_TALK);
			return true;
		}

	private:
		Creature const& _owner;
};

class DeathPlagueTargetSelector
{
	public:
		explicit DeathPlagueTargetSelector(Unit* caster) : _caster(caster) {}

		bool operator()(Unit* object) const
		{
			if (object == _caster)
				return true;

			if (object->GetTypeId() != TYPEID_PLAYER)
				return true;

			if (object->HasAura(SPELL_RECENTLY_INFECTED) || object->HasAura(SPELL_DEATH_PLAGUE_AURA))
				return true;
			
			if (object->GetExactDist2d(_caster) > 8.0f)
				return true;

			return false;
		}

	private:
		Unit* _caster;
};

// at Light's Hammer
class npc_highlord_tirion_fordring_lh : public CreatureScript
{
	public:
		npc_highlord_tirion_fordring_lh() : CreatureScript("npc_highlord_tirion_fordring_lh") { }

		struct npc_highlord_tirion_fordringAI : public ScriptedAI
		{
			npc_highlord_tirion_fordringAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
			{
			}

			void Reset()
			{
				_events.Reset();
				_theLichKing = 0;
				_bolvarFordragon = 0;
				_factionNPC = 0;
				_damnedKills = 0;
			}

			// IMPORTANT NOTE: This is triggered from per-GUID scripts
			// of The Damned SAI
			void SetData(uint32 type, uint32 data)
			{
				if (type == DATA_DAMNED_KILLS && data == 1)
				{
					if (++_damnedKills == 2)
					{
						if (Creature* theLichKing = me->FindNearestCreature(NPC_THE_LICH_KING_LH, 150.0f))
						{
							if (Creature* bolvarFordragon = me->FindNearestCreature(NPC_HIGHLORD_BOLVAR_FORDRAGON_LH, 150.0f))
							{
								if (Creature* factionNPC = me->FindNearestCreature(_instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE ? NPC_SE_HIGH_OVERLORD_SAURFANG : NPC_SE_MURADIN_BRONZEBEARD, 50.0f))
								{
									me->setActive(true);
									_theLichKing = theLichKing->GetGUID();
									theLichKing->setActive(true);
									_bolvarFordragon = bolvarFordragon->GetGUID();
									bolvarFordragon->setActive(true);
									_factionNPC = factionNPC->GetGUID();
									factionNPC->setActive(true);
								}
							}
						}

						if (!_bolvarFordragon || !_theLichKing || !_factionNPC)
							return;

						Talk(SAY_TIRION_INTRO_1);
						_events.ScheduleEvent(EVENT_TIRION_INTRO_2, 4000);
						_events.ScheduleEvent(EVENT_TIRION_INTRO_3, 14000);
						_events.ScheduleEvent(EVENT_TIRION_INTRO_4, 18000);
						_events.ScheduleEvent(EVENT_TIRION_INTRO_5, 31000);
						_events.ScheduleEvent(EVENT_LK_INTRO_1, 35000);
						_events.ScheduleEvent(EVENT_TIRION_INTRO_6, 51000);
						_events.ScheduleEvent(EVENT_LK_INTRO_2, 58000);
						_events.ScheduleEvent(EVENT_LK_INTRO_3, 74000);
						_events.ScheduleEvent(EVENT_LK_INTRO_4, 86000);
						_events.ScheduleEvent(EVENT_BOLVAR_INTRO_1, 100000);
						_events.ScheduleEvent(EVENT_LK_INTRO_5, 108000);

						if (_instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
						{
							_events.ScheduleEvent(EVENT_SAURFANG_INTRO_1, 120000);
							_events.ScheduleEvent(EVENT_TIRION_INTRO_H_7, 129000);
							_events.ScheduleEvent(EVENT_SAURFANG_INTRO_2, 139000);
							_events.ScheduleEvent(EVENT_SAURFANG_INTRO_3, 150000);
							_events.ScheduleEvent(EVENT_SAURFANG_INTRO_4, 162000);
							_events.ScheduleEvent(EVENT_SAURFANG_RUN, 170000);
						}
						else
						{
							_events.ScheduleEvent(EVENT_MURADIN_INTRO_1, 120000);
							_events.ScheduleEvent(EVENT_MURADIN_INTRO_2, 124000);
							_events.ScheduleEvent(EVENT_MURADIN_INTRO_3, 127000);
							_events.ScheduleEvent(EVENT_TIRION_INTRO_A_7, 136000);
							_events.ScheduleEvent(EVENT_MURADIN_INTRO_4, 144000);
							_events.ScheduleEvent(EVENT_MURADIN_INTRO_5, 151000);
							_events.ScheduleEvent(EVENT_MURADIN_RUN, 157000);
						}
					}
				}
			}

			void UpdateAI(uint32 const diff)
			{
				if (_damnedKills != 2)
					return;

				_events.Update(diff);

				while (uint32 eventId = _events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_TIRION_INTRO_2:
							me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
							break;
						case EVENT_TIRION_INTRO_3:
							Talk(SAY_TIRION_INTRO_2);
							break;
						case EVENT_TIRION_INTRO_4:
							me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
							break;
						case EVENT_TIRION_INTRO_5:
							Talk(SAY_TIRION_INTRO_3);
							break;
						case EVENT_LK_INTRO_1:
							me->HandleEmoteCommand(EMOTE_ONESHOT_POINT_NO_SHEATHE);
							if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
								theLichKing->AI()->Talk(SAY_LK_INTRO_1);
							break;
						case EVENT_TIRION_INTRO_6:
							Talk(SAY_TIRION_INTRO_4);
							break;
						case EVENT_LK_INTRO_2:
							if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
								theLichKing->AI()->Talk(SAY_LK_INTRO_2);
							break;
						case EVENT_LK_INTRO_3:
							if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
								theLichKing->AI()->Talk(SAY_LK_INTRO_3);
							break;
						case EVENT_LK_INTRO_4:
							if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
								theLichKing->AI()->Talk(SAY_LK_INTRO_4);
							break;
						case EVENT_BOLVAR_INTRO_1:
							if (Creature* bolvarFordragon = ObjectAccessor::GetCreature(*me, _bolvarFordragon))
							{
								bolvarFordragon->AI()->Talk(SAY_BOLVAR_INTRO_1);
								bolvarFordragon->setActive(false);
							}
							break;
						case EVENT_LK_INTRO_5:
							if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
							{
								theLichKing->AI()->Talk(SAY_LK_INTRO_5);
								theLichKing->setActive(false);
							}
							break;
						case EVENT_SAURFANG_INTRO_1:
							if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
								saurfang->AI()->Talk(SAY_SAURFANG_INTRO_1);
							break;
						case EVENT_TIRION_INTRO_H_7:
							Talk(SAY_TIRION_INTRO_H_5);
							break;
						case EVENT_SAURFANG_INTRO_2:
							if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
								saurfang->AI()->Talk(SAY_SAURFANG_INTRO_2);
							break;
						case EVENT_SAURFANG_INTRO_3:
							if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
								saurfang->AI()->Talk(SAY_SAURFANG_INTRO_3);
							break;
						case EVENT_SAURFANG_INTRO_4:
							if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
								saurfang->AI()->Talk(SAY_SAURFANG_INTRO_4);
							break;
						case EVENT_MURADIN_RUN:
						case EVENT_SAURFANG_RUN:
							if (Creature* factionNPC = ObjectAccessor::GetCreature(*me, _factionNPC))
								factionNPC->GetMotionMaster()->MovePath(factionNPC->GetDBTableGUIDLow()*10, false);
							me->setActive(false);
							_damnedKills = 3;
							break;
						case EVENT_MURADIN_INTRO_1:
							if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
								muradin->AI()->Talk(SAY_MURADIN_INTRO_1);
							break;
						case EVENT_MURADIN_INTRO_2:
							if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
								muradin->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
							break;
						case EVENT_MURADIN_INTRO_3:
							if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
								muradin->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
							break;
						case EVENT_TIRION_INTRO_A_7:
							Talk(SAY_TIRION_INTRO_A_5);
							break;
						case EVENT_MURADIN_INTRO_4:
							if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
								muradin->AI()->Talk(SAY_MURADIN_INTRO_2);
							break;
						case EVENT_MURADIN_INTRO_5:
							if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
								muradin->AI()->Talk(SAY_MURADIN_INTRO_3);
							break;
						default:
							break;
					}
				}
			}

		private:
			EventMap _events;
			InstanceScript* const _instance;
			uint64 _theLichKing;
			uint64 _bolvarFordragon;
			uint64 _factionNPC;
			uint16 _damnedKills;
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_highlord_tirion_fordringAI>(creature);
		}
};

class npc_rotting_frost_giant : public CreatureScript
{
	public:
		npc_rotting_frost_giant() : CreatureScript("npc_rotting_frost_giant") { }

		struct npc_rotting_frost_giantAI : public ScriptedAI
		{
			npc_rotting_frost_giantAI(Creature* creature) : ScriptedAI(creature)
			{
			}

			void Reset()
			{
				_events.Reset();
				_events.ScheduleEvent(EVENT_DEATH_PLAGUE, 15000);
				_events.ScheduleEvent(EVENT_STOMP, urand(5000, 8000));
				_events.ScheduleEvent(EVENT_ARCTIC_BREATH, urand(10000, 15000));
			}

			void JustDied(Unit* /*killer*/)
			{
				_events.Reset();
			}
			
			void EnterCombat(Unit* /*who*/)
			{
				DoZoneInCombat();
			}

			void UpdateAI(uint32 const diff)
			{
				if (!UpdateVictim())
					return;

				_events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = _events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_DEATH_PLAGUE:
						{
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, /*DeathPlagueTargetSelector(me)*/0.0f, true))
							{
								Talk(EMOTE_DEATH_PLAGUE_WARNING, target->GetGUID());
								DoCast(target, SPELL_DEATH_PLAGUE);
							}
							_events.ScheduleEvent(EVENT_DEATH_PLAGUE, RAID_MODE(50000, 20000, 50000, 20000));
							break;
						}
						case EVENT_STOMP:
							DoCastVictim(SPELL_STOMP);
							_events.ScheduleEvent(EVENT_STOMP, urand(15000, 18000));
							break;
						case EVENT_ARCTIC_BREATH:
							DoCastVictim(SPELL_ARCTIC_BREATH);
							_events.ScheduleEvent(EVENT_ARCTIC_BREATH, urand(26000, 33000));
							break;
						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}

		private:
			EventMap _events;
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_rotting_frost_giantAI>(creature);
		}
};

class npc_frost_freeze_trap : public CreatureScript
{
	public:
		npc_frost_freeze_trap() : CreatureScript("npc_frost_freeze_trap") { }

		struct npc_frost_freeze_trapAI: public Scripted_NoMovementAI
		{
			npc_frost_freeze_trapAI(Creature* creature) : Scripted_NoMovementAI(creature)
			{
			}

			void DoAction(int32 const action)
			{
				_events.ScheduleEvent(EVENT_ACTIVATE_TRAP, uint32(action));
			}

			void UpdateAI(uint32 const diff)
			{
				_events.Update(diff);

				if (_events.ExecuteEvent() == EVENT_ACTIVATE_TRAP)
				{
					DoCast(me, SPELL_COLDFLAME_JETS);
					if (me->GetInstanceScript()->GetData(DATA_COLDFLAME_JETS) != DONE)
						_events.ScheduleEvent(EVENT_ACTIVATE_TRAP, 22000);
				}
			}

		private:
			EventMap _events;
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_frost_freeze_trapAI>(creature);
		}
};

class npc_alchemist_adrianna : public CreatureScript
{
	public:
		npc_alchemist_adrianna() : CreatureScript("npc_alchemist_adrianna") { }

		bool OnGossipHello(Player* player, Creature* creature)
		{
			if (!creature->FindCurrentSpellBySpellId(SPELL_HARVEST_BLIGHT_SPECIMEN) && !creature->FindCurrentSpellBySpellId(SPELL_HARVEST_BLIGHT_SPECIMEN25))
				if (player->HasAura(SPELL_ORANGE_BLIGHT_RESIDUE) && player->HasAura(SPELL_GREEN_BLIGHT_RESIDUE))
					creature->CastSpell(creature, SPELL_HARVEST_BLIGHT_SPECIMEN, false);
			return false;
		}
};

class boss_sister_svalna : public CreatureScript
{
	public:
		boss_sister_svalna() : CreatureScript("boss_sister_svalna") { }

		struct boss_sister_svalnaAI : public BossAI
		{
			boss_sister_svalnaAI(Creature* creature) : BossAI(creature, DATA_SISTER_SVALNA),
				_isEventInProgress(false)
			{
			}

			void InitializeAI()
			{
				if (!me->isDead())
					Reset();

				me->SetReactState(REACT_PASSIVE);
			}

			void Reset()
			{
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
				_Reset();
				me->SetReactState(REACT_DEFENSIVE);
				_isEventInProgress = false;
			}

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();
				Talk(SAY_SVALNA_DEATH);

				uint64 delay = 1;
				for (uint32 i = 0; i < 4; ++i)
				{
					if (Creature* crusader = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CAPTAIN_ARNATH + i)))
					{
						if (crusader->isAlive() && crusader->GetEntry() == crusader->GetCreatureData()->id)
						{
							crusader->m_Events.AddEvent(new CaptainSurviveTalk(*crusader), crusader->m_Events.CalculateTime(delay));
							delay += 6000;
						}
					}
				}
			}

			void EnterCombat(Unit* /*attacker*/)
			{
				_EnterCombat();
				if (Creature* crok = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CROK_SCOURGEBANE)))
					crok->AI()->Talk(SAY_CROK_COMBAT_SVALNA);
				events.ScheduleEvent(EVENT_SVALNA_COMBAT, 9000);
				events.ScheduleEvent(EVENT_IMPALING_SPEAR, urand(20000, 30000));
				//events.ScheduleEvent(EVENT_AETHER_SHIELD, urand(20000, 30000));
			}

			void KilledUnit(Unit* victim)
			{
				switch (victim->GetTypeId())
				{
					case TYPEID_PLAYER:
						Talk(SAY_SVALNA_KILL);
						break;
					case TYPEID_UNIT:
						switch (victim->GetEntry())
						{
							case NPC_CAPTAIN_ARNATH:
							case NPC_CAPTAIN_BRANDON:
							case NPC_CAPTAIN_GRONDEL:
							case NPC_CAPTAIN_RUPERT:
								Talk(SAY_SVALNA_KILL_CAPTAIN);
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
			}

			void JustReachedHome()
			{
				_JustReachedHome();
				me->SetReactState(REACT_PASSIVE);
				me->SetCanFly(false);
			}

			void DoAction(int32 const action)
			{
				switch (action)
				{
					case ACTION_KILL_CAPTAIN:
						me->CastCustomSpell(SPELL_CARESS_OF_DEATH, SPELLVALUE_MAX_TARGETS, 1, me, true);
						break;
					case ACTION_START_GAUNTLET:
						me->setActive(true);
						_isEventInProgress = true;
						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
						events.ScheduleEvent(EVENT_SVALNA_START, 25000);
						break;
					case ACTION_RESURRECT_CAPTAINS:
						events.ScheduleEvent(EVENT_SVALNA_RESURRECT, 7000);
						break;
					case ACTION_CAPTAIN_DIES:
						Talk(SAY_SVALNA_CAPTAIN_DEATH);
						break;
					case ACTION_RESET_EVENT:
						me->setActive(false);
						Reset();
						break;
					default:
						break;
				}
			}

			void SpellHit(Unit* caster, SpellInfo const* spell)
			{
				if (spell->Id == SPELL_HURL_SPEAR && me->HasAura(SPELL_AETHER_SHIELD))
				{
					me->RemoveAurasDueToSpell(SPELL_AETHER_SHIELD);
					Talk(EMOTE_SVALNA_BROKEN_SHIELD, caster->GetGUID());
				}
			}

			void MovementInform(uint32 type, uint32 id)
			{
				if (type != POINT_MOTION_TYPE || id != POINT_LAND)
					return;

				_isEventInProgress = false;
				me->setActive(false);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
				me->SetCanFly(false);
			}

			void SpellHitTarget(Unit* target, SpellInfo const* spell)
			{
				switch (spell->Id)
				{
					case SPELL_IMPALING_SPEAR_KILL:
						me->Kill(target);
						break;
					case SPELL_IMPALING_SPEAR:
						if (TempSummon* summon = target->SummonCreature(NPC_IMPALING_SPEAR, *target))
						{
							Talk(EMOTE_SVALNA_IMPALE, target->GetGUID());
							summon->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, target, false);
							summon->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1 | UNIT_FLAG2_ALLOW_ENEMY_INTERACT);
						}
						break;
					default:
						break;
				}
			}

			void UpdateAI(uint32 const diff)
			{
				if (!UpdateVictim() && !_isEventInProgress)
					return;

				events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_SVALNA_START:
							Talk(SAY_SVALNA_EVENT_START);
							break;
						case EVENT_SVALNA_RESURRECT:
							Talk(SAY_SVALNA_RESURRECT_CAPTAINS);
							me->CastSpell(me, SPELL_REVIVE_CHAMPION, false);
							break;
						case EVENT_SVALNA_COMBAT:
							me->SetReactState(REACT_DEFENSIVE);
							Talk(SAY_SVALNA_AGGRO);
							break;
						case EVENT_IMPALING_SPEAR:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_IMPALING_SPEAR))
								DoCast(target, SPELL_IMPALING_SPEAR);
							events.ScheduleEvent(EVENT_IMPALING_SPEAR, urand(20000, 30000));
							events.ScheduleEvent(EVENT_AETHER_SHIELD, 5000);
							break;
							case EVENT_AETHER_SHIELD:
								DoCast(me, SPELL_AETHER_SHIELD);
							break;
						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}

		private:
			bool _isEventInProgress;
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<boss_sister_svalnaAI>(creature);
		}
};

#define GOSSIP_ITEM_1  "Let's go, Crok!"
class npc_crok_scourgebane : public CreatureScript
{
	public:
		npc_crok_scourgebane() : CreatureScript("npc_crok_scourgebane") { }
	/*
	bool OnGossipSelect(Player* player, Creature* creature, uint32 uiSender, uint32 action)
	{
		creature->AI()->DoAction(ACTION_START_GAUNTLET);
		player->CLOSE_GOSSIP_MENU();
		return true;
	}
	
	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PrepareGossipMenu(creature);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}
	*/
		struct npc_crok_scourgebaneAI : public npc_escortAI
		{
			npc_crok_scourgebaneAI(Creature* creature) : npc_escortAI(creature),
				_instance(creature->GetInstanceScript()), _respawnTime(creature->GetRespawnDelay()),
				_corpseDelay(creature->GetCorpseDelay())
			{
				SetDespawnAtEnd(false);
				SetDespawnAtFar(false);
				_isEventActive = false;
				_isEventDone = _instance->GetBossState(DATA_SISTER_SVALNA) == DONE;
				_didUnderTenPercentText = false;
			}

			void Reset()
			{
				if (!_isEventActive)
				{
					_events.Reset();
					_events.ScheduleEvent(EVENT_SCOURGE_STRIKE, urand(7500, 12500));
					_events.ScheduleEvent(EVENT_DEATH_STRIKE, urand(25000, 30000));
				}
				me->SetReactState(REACT_DEFENSIVE);
				_didUnderTenPercentText = false;
				_wipeCheckTimer = 1000;
			}

			void DoAction(int32 const action)
			{
				if (action == ACTION_START_GAUNTLET)
				{
					if (_isEventDone || !me->isAlive())
						return;

					_isEventActive = true;
					_isEventDone = true;
					// Load Grid with Sister Svalna
					me->GetMap()->LoadGrid(4356.71f, 2484.33f);
					if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_SISTER_SVALNA)))
						svalna->AI()->DoAction(ACTION_START_GAUNTLET);
					Talk(SAY_CROK_INTRO_1);
					_events.ScheduleEvent(EVENT_ARNATH_INTRO_2, 7000);
					_events.ScheduleEvent(EVENT_CROK_INTRO_3, 14000);
					_events.ScheduleEvent(EVENT_START_PATHING, 37000);
					me->setActive(true);
					for (uint32 i = 0; i < 4; ++i)
						if (Creature* crusader = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_CAPTAIN_ARNATH + i)))
							crusader->AI()->DoAction(ACTION_START_GAUNTLET);
				}
				else if (action == ACTION_RESET_EVENT)
				{
					_isEventActive = false;
					_isEventDone = _instance->GetBossState(DATA_SISTER_SVALNA) == DONE;
					me->setActive(false);
					_aliveTrash.clear();
					_currentWPid = 0;
				}
			}

			void SetGUID(uint64 guid, int32 type/* = 0*/)
			{
				if (type == ACTION_VRYKUL_DEATH)
				{
					_aliveTrash.erase(guid);
					if (_aliveTrash.empty())
					{
						SetEscortPaused(false);
						if (_currentWPid == 4 && _isEventActive)
						{
							_isEventActive = false;
							me->setActive(false);
							Talk(SAY_CROK_FINAL_WP);
							if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_SISTER_SVALNA)))
								svalna->AI()->DoAction(ACTION_RESURRECT_CAPTAINS);
						}
					}
				}
			}

			void WaypointReached(uint32 waypointId)
			{
				switch (waypointId)
				{
					// pause pathing until trash pack is cleared
					case 0:
						Talk(SAY_CROK_COMBAT_WP_0);
						if (!_aliveTrash.empty())
							SetEscortPaused(true);
						break;
					case 1:
						Talk(SAY_CROK_COMBAT_WP_1);
						if (!_aliveTrash.empty())
							SetEscortPaused(true);
						break;
					case 4:
						if (_aliveTrash.empty() && _isEventActive)
						{
							_isEventActive = false;
							me->setActive(false);
							Talk(SAY_CROK_FINAL_WP);
							if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_SISTER_SVALNA)))
								svalna->AI()->DoAction(ACTION_RESURRECT_CAPTAINS);
						}
						break;
					default:
						break;
				}
			}

			void WaypointStart(uint32 waypointId)
			{
				_currentWPid = waypointId;
				switch (waypointId)
				{
					case 0:
					case 1:
					case 4:
					{
						// get spawns by home position
						float minY = 2600.0f;
						float maxY = 2650.0f;
						if (waypointId == 1)
						{
							minY -= 50.0f;
							maxY -= 50.0f;
							// at waypoints 1 and 2 she kills one captain
							if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_SISTER_SVALNA)))
								svalna->AI()->DoAction(ACTION_KILL_CAPTAIN);
						}
						else if (waypointId == 4)
						{
							minY -= 100.0f;
							maxY -= 100.0f;
						}

						// get all nearby vrykul
						std::list<Creature*> temp;
						FrostwingVrykulSearcher check(me, 80.0f);
						Trinity::CreatureListSearcher<FrostwingVrykulSearcher> searcher(me, temp, check);
						me->VisitNearbyGridObject(80.0f, searcher);

						_aliveTrash.clear();
						for (std::list<Creature*>::iterator itr = temp.begin(); itr != temp.end(); ++itr)
							if ((*itr)->GetHomePosition().GetPositionY() < maxY && (*itr)->GetHomePosition().GetPositionY() > minY)
								_aliveTrash.insert((*itr)->GetGUID());
						break;
					}
					// at waypoints 1 and 2 she kills one captain
					case 2:
						if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_SISTER_SVALNA)))
							svalna->AI()->DoAction(ACTION_KILL_CAPTAIN);
						break;
					default:
						break;
				}
			}

			void DamageTaken(Unit* /*attacker*/, uint32& damage)
			{
				// check wipe
				if (!_wipeCheckTimer)
				{
					_wipeCheckTimer = 1000;
					Player* player = NULL;
					Trinity::AnyPlayerInObjectRangeCheck check(me, 60.0f);
					Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, player, check);
					me->VisitNearbyWorldObject(60.0f, searcher);
					// wipe
					if (!player)
					{
						damage *= 100;
						if (damage >= me->GetHealth())
						{
							FrostwingGauntletRespawner respawner;
							Trinity::CreatureWorker<FrostwingGauntletRespawner> worker(me, respawner);
							me->VisitNearbyGridObject(333.0f, worker);
							Talk(SAY_CROK_DEATH);
						}
						return;
					}
				}

				if (HealthBelowPct(10))
				{
					if (!_didUnderTenPercentText)
					{
						_didUnderTenPercentText = true;
						if (_isEventActive)
							Talk(SAY_CROK_WEAKENING_GAUNTLET);
						else
							Talk(SAY_CROK_WEAKENING_SVALNA);
					}

					damage = 0;
					DoCast(me, SPELL_ICEBOUND_ARMOR);
					_events.ScheduleEvent(EVENT_HEALTH_CHECK, 1000);
				}
			}

			void UpdateEscortAI(uint32 const diff)
			{
				if (_wipeCheckTimer <= diff)
					_wipeCheckTimer = 0;
				else
					_wipeCheckTimer -= diff;

				if (!UpdateVictim() && !_isEventActive)
					return;

				_events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = _events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_ARNATH_INTRO_2:
							if (Creature* arnath = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_CAPTAIN_ARNATH)))
								arnath->AI()->Talk(SAY_ARNATH_INTRO_2);
							break;
						case EVENT_CROK_INTRO_3:
							Talk(SAY_CROK_INTRO_3);
							break;
						case EVENT_START_PATHING:
							Start(true, true);
							break;
						case EVENT_SCOURGE_STRIKE:
							DoCastVictim(SPELL_SCOURGE_STRIKE);
							_events.ScheduleEvent(EVENT_SCOURGE_STRIKE, urand(10000, 14000));
							break;
						case EVENT_DEATH_STRIKE:
							if (HealthBelowPct(20))
								DoCastVictim(SPELL_DEATH_STRIKE);
							_events.ScheduleEvent(EVENT_DEATH_STRIKE, urand(5000, 10000));
							break;
						case EVENT_HEALTH_CHECK:
							if (HealthAbovePct(15))
							{
								me->RemoveAurasDueToSpell(SPELL_ICEBOUND_ARMOR);
								_didUnderTenPercentText = false;
							}
							else
							{
								me->DealHeal(me, me->CountPctFromMaxHealth(5));
								_events.ScheduleEvent(EVENT_HEALTH_CHECK, 1000);
							}
							break;
						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}

			bool CanAIAttack(Unit const* target) const
			{
				// do not see targets inside Frostwing Halls when we are not there
				return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f);
			}

		private:
			EventMap _events;
			std::set<uint64> _aliveTrash;
			InstanceScript* _instance;
			uint32 _currentWPid;
			uint32 _wipeCheckTimer;
			uint32 const _respawnTime;
			uint32 const _corpseDelay;
			bool _isEventActive;
			bool _isEventDone;
			bool _didUnderTenPercentText;
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_crok_scourgebaneAI>(creature);
		}
};

struct npc_argent_captainAI : public ScriptedAI
{
	public:
		npc_argent_captainAI(Creature* creature) : ScriptedAI(creature), Instance(creature->GetInstanceScript()), _firstDeath(true)
		{
			FollowAngle = PET_FOLLOW_ANGLE;
			FollowDist = PET_FOLLOW_DIST;
			IsUndead = false;
		}

		void JustDied(Unit* /*killer*/)
		{
			if (_firstDeath)
			{
				_firstDeath = false;
				Talk(SAY_CAPTAIN_DEATH);
			}
			else
				Talk(SAY_CAPTAIN_SECOND_DEATH);
		}

		void KilledUnit(Unit* victim)
		{
			if (victim->GetTypeId() == TYPEID_PLAYER)
				Talk(SAY_CAPTAIN_KILL);
		}

		void DoAction(int32 const action)
		{
			if (action == ACTION_START_GAUNTLET)
			{
				if (Creature* crok = ObjectAccessor::GetCreature(*me, Instance->GetData64(DATA_CROK_SCOURGEBANE)))
				{
					me->SetReactState(REACT_DEFENSIVE);
					FollowAngle = me->GetAngle(crok) + me->GetOrientation();
					FollowDist = me->GetDistance2d(crok);
					me->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
				}

				me->setActive(true);
			}
			else if (action == ACTION_RESET_EVENT)
			{
				_firstDeath = true;
			}
		}

		void EnterCombat(Unit* /*target*/)
		{
			me->SetHomePosition(*me);
			if (IsUndead)
				DoZoneInCombat();
		}

		bool CanAIAttack(Unit const* target) const
		{
			// do not see targets inside Frostwing Halls when we are not there
			return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f);
		}

		void EnterEvadeMode()
		{
			// not yet following
			if (me->GetMotionMaster()->GetMotionSlotType(MOTION_SLOT_IDLE) != CHASE_MOTION_TYPE || IsUndead)
			{
				ScriptedAI::EnterEvadeMode();
				return;
			}

			if (!_EnterEvadeMode())
				return;

			if (!me->GetVehicle())
			{
				me->GetMotionMaster()->Clear(false);
				if (Creature* crok = ObjectAccessor::GetCreature(*me, Instance->GetData64(DATA_CROK_SCOURGEBANE)))
					me->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
			}

			Reset();
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_REVIVE_CHAMPION && !IsUndead)
			{
				IsUndead = true;
				me->setDeathState(JUST_RESPAWNED);
				uint32 newEntry = 0;
				switch (me->GetEntry())
				{
					case NPC_CAPTAIN_ARNATH:
						newEntry = NPC_CAPTAIN_ARNATH_UNDEAD;
						break;
					case NPC_CAPTAIN_BRANDON:
						newEntry = NPC_CAPTAIN_BRANDON_UNDEAD;
						break;
					case NPC_CAPTAIN_GRONDEL:
						newEntry = NPC_CAPTAIN_GRONDEL_UNDEAD;
						break;
					case NPC_CAPTAIN_RUPERT:
						newEntry = NPC_CAPTAIN_RUPERT_UNDEAD;
						break;
					default:
						return;
				}

				Talk(SAY_CAPTAIN_RESURRECTED);
				me->UpdateEntry(newEntry, Instance->GetData(DATA_TEAM_IN_INSTANCE), me->GetCreatureData());
				DoCast(me, SPELL_UNDEATH, true);
				me->ToCreature()->SetInCombatWithZone();
				AttackStart(caster->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true));
			}
		}

	protected:
		EventMap Events;
		InstanceScript* Instance;
		float FollowAngle;
		float FollowDist;
		bool IsUndead;

	private:
		bool _firstDeath;
};

class npc_captain_arnath : public CreatureScript
{
	public:
		npc_captain_arnath() : CreatureScript("npc_captain_arnath") { }

		struct npc_captain_arnathAI : public npc_argent_captainAI
		{
			npc_captain_arnathAI(Creature* creature) : npc_argent_captainAI(creature)
			{
			}

			void Reset()
			{
				Events.Reset();
				Events.ScheduleEvent(EVENT_ARNATH_FLASH_HEAL, urand(4000, 7000));
				Events.ScheduleEvent(EVENT_ARNATH_PW_SHIELD, urand(8000, 14000));
				Events.ScheduleEvent(EVENT_ARNATH_SMITE, urand(3000, 6000));
				if (Is25ManRaid() && IsUndead)
					Events.ScheduleEvent(EVENT_ARNATH_DOMINATE_MIND, urand(22000, 27000));
			}

			void UpdateAI(uint32 const diff)
			{
				if (!UpdateVictim())
					return;

				Events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = Events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_ARNATH_FLASH_HEAL:
							if (Creature* target = FindFriendlyCreature())
								DoCast(target, SPELL_FLASH_HEAL);
							Events.ScheduleEvent(EVENT_ARNATH_FLASH_HEAL, urand(6000, 9000));
							break;
						case EVENT_ARNATH_PW_SHIELD:
						{
							std::list<Creature*> targets = DoFindFriendlyMissingBuff(40.0f, SPELL_POWER_WORD_SHIELD);
							DoCast(Trinity::Containers::SelectRandomContainerElement(targets), SPELL_POWER_WORD_SHIELD);
							Events.ScheduleEvent(EVENT_ARNATH_PW_SHIELD, urand(15000, 20000));
							break;
						}
						case EVENT_ARNATH_SMITE:
							DoCastVictim(SPELL_SMITE);
							Events.ScheduleEvent(EVENT_ARNATH_SMITE, urand(4000, 7000));
							break;
						case EVENT_ARNATH_DOMINATE_MIND:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
								DoCast(target, SPELL_DOMINATE_MIND);
							Events.ScheduleEvent(EVENT_ARNATH_DOMINATE_MIND, urand(28000, 37000));
							break;
						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}

		private:
			Creature* FindFriendlyCreature() const
			{
				Creature* target = NULL;
				Trinity::MostHPMissingInRange u_check(me, 60.0f, 0);
				Trinity::CreatureLastSearcher<Trinity::MostHPMissingInRange> searcher(me, target, u_check);
				me->VisitNearbyGridObject(60.0f, searcher);
				return target;
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_captain_arnathAI>(creature);
		}
};

class npc_captain_brandon : public CreatureScript
{
	public:
		npc_captain_brandon() : CreatureScript("npc_captain_brandon") { }

		struct npc_captain_brandonAI : public npc_argent_captainAI
		{
			npc_captain_brandonAI(Creature* creature) : npc_argent_captainAI(creature)
			{
			}

			void Reset()
			{
				Events.Reset();
				Events.ScheduleEvent(EVENT_BRANDON_CRUSADER_STRIKE, urand(6000, 10000));
				Events.ScheduleEvent(EVENT_BRANDON_DIVINE_SHIELD, 500);
				Events.ScheduleEvent(EVENT_BRANDON_JUDGEMENT_OF_COMMAND, urand(8000, 13000));
				if (IsUndead)
					Events.ScheduleEvent(EVENT_BRANDON_HAMMER_OF_BETRAYAL, urand(25000, 30000));
			}

			void UpdateAI(uint32 const diff)
			{
				if (!UpdateVictim())
					return;

				Events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = Events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_BRANDON_CRUSADER_STRIKE:
							DoCastVictim(SPELL_CRUSADER_STRIKE);
							Events.ScheduleEvent(EVENT_BRANDON_CRUSADER_STRIKE, urand(6000, 12000));
							break;
						case EVENT_BRANDON_DIVINE_SHIELD:
							if (HealthBelowPct(20))
								DoCast(me, SPELL_DIVINE_SHIELD);
							Events.ScheduleEvent(EVENT_BRANDON_DIVINE_SHIELD, 500);
							break;
						case EVENT_BRANDON_JUDGEMENT_OF_COMMAND:
							DoCastVictim(SPELL_JUDGEMENT_OF_COMMAND);
							Events.ScheduleEvent(EVENT_BRANDON_JUDGEMENT_OF_COMMAND, urand(8000, 13000));
							break;
						case EVENT_BRANDON_HAMMER_OF_BETRAYAL:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
								DoCast(target, SPELL_HAMMER_OF_BETRAYAL);
							Events.ScheduleEvent(EVENT_BRANDON_HAMMER_OF_BETRAYAL, urand(45000, 60000));
							break;
						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_captain_brandonAI>(creature);
		}
};

class npc_captain_grondel : public CreatureScript
{
	public:
		npc_captain_grondel() : CreatureScript("npc_captain_grondel") { }

		struct npc_captain_grondelAI : public npc_argent_captainAI
		{
			npc_captain_grondelAI(Creature* creature) : npc_argent_captainAI(creature)
			{
			}

			void Reset()
			{
				Events.Reset();
				Events.ScheduleEvent(EVENT_GRONDEL_CHARGE_CHECK, 500);
				Events.ScheduleEvent(EVENT_GRONDEL_MORTAL_STRIKE, urand(8000, 14000));
				Events.ScheduleEvent(EVENT_GRONDEL_SUNDER_ARMOR, urand(3000, 12000));
				if (IsUndead)
					Events.ScheduleEvent(EVENT_GRONDEL_CONFLAGRATION, urand(12000, 17000));
			}

			void UpdateAI(uint32 const diff)
			{
				if (!UpdateVictim())
					return;

				Events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = Events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_GRONDEL_CHARGE_CHECK:
							DoCastVictim(SPELL_CHARGE);
							Events.ScheduleEvent(EVENT_GRONDEL_CHARGE_CHECK, 500);
							break;
						case EVENT_GRONDEL_MORTAL_STRIKE:
							DoCastVictim(SPELL_MORTAL_STRIKE);
							Events.ScheduleEvent(EVENT_GRONDEL_MORTAL_STRIKE, urand(10000, 15000));
							break;
						case EVENT_GRONDEL_SUNDER_ARMOR:
							DoCastVictim(SPELL_SUNDER_ARMOR);
							Events.ScheduleEvent(EVENT_GRONDEL_SUNDER_ARMOR, urand(5000, 17000));
							break;
						case EVENT_GRONDEL_CONFLAGRATION:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
								DoCast(target, SPELL_CONFLAGRATION);
							Events.ScheduleEvent(EVENT_GRONDEL_CONFLAGRATION, urand(10000, 15000));
							break;
						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_captain_grondelAI>(creature);
		}
};

class npc_captain_rupert : public CreatureScript
{
	public:
		npc_captain_rupert() : CreatureScript("npc_captain_rupert") { }

		struct npc_captain_rupertAI : public npc_argent_captainAI
		{
			npc_captain_rupertAI(Creature* creature) : npc_argent_captainAI(creature)
			{
			}

			void Reset()
			{
				Events.Reset();
				Events.ScheduleEvent(EVENT_RUPERT_FEL_IRON_BOMB, urand(15000, 20000));
				Events.ScheduleEvent(EVENT_RUPERT_MACHINE_GUN, urand(25000, 30000));
				Events.ScheduleEvent(EVENT_RUPERT_ROCKET_LAUNCH, urand(10000, 15000));
			}

			void UpdateAI(uint32 const diff)
			{
				if (!UpdateVictim())
					return;

				Events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = Events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_RUPERT_FEL_IRON_BOMB:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
								DoCast(target, SPELL_FEL_IRON_BOMB);
							Events.ScheduleEvent(EVENT_RUPERT_FEL_IRON_BOMB, urand(15000, 20000));
							break;
						case EVENT_RUPERT_MACHINE_GUN:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
								DoCast(target, SPELL_MACHINE_GUN);
							Events.ScheduleEvent(EVENT_RUPERT_MACHINE_GUN, urand(25000, 30000));
							break;
						case EVENT_RUPERT_ROCKET_LAUNCH:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
								DoCast(target, SPELL_ROCKET_LAUNCH);
							Events.ScheduleEvent(EVENT_RUPERT_ROCKET_LAUNCH, urand(10000, 15000));
							break;
						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return GetIcecrownCitadelAI<npc_captain_rupertAI>(creature);
		}
};

class npc_impaling_spear : public CreatureScript
{
	public:
		npc_impaling_spear() : CreatureScript("npc_impaling_spear") { }

		struct npc_impaling_spearAI : public CreatureAI
		{
			npc_impaling_spearAI(Creature* creature) : CreatureAI(creature)
			{
			}

			void Reset()
			{
				me->SetReactState(REACT_PASSIVE);
				_vehicleCheckTimer = 500;
			}

			void UpdateAI(uint32 const diff)
			{
				if (_vehicleCheckTimer <= diff)
				{
					_vehicleCheckTimer = 500;
					if (!me->GetVehicle())
						me->DespawnOrUnsummon(100);
				}
				else
					_vehicleCheckTimer -= diff;
			}

			uint32 _vehicleCheckTimer;
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_impaling_spearAI(creature);
		}
};

class npc_arthas_teleport_visual : public CreatureScript
{
	public:
		npc_arthas_teleport_visual() : CreatureScript("npc_arthas_teleport_visual") { }

		struct npc_arthas_teleport_visualAI : public NullCreatureAI
		{
			npc_arthas_teleport_visualAI(Creature* creature) : NullCreatureAI(creature), _instance(creature->GetInstanceScript())
			{
			}

			void Reset()
			{
				_events.Reset();
				if (_instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE &&
					_instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE &&
					_instance->GetBossState(DATA_SINDRAGOSA) == DONE)
					_events.ScheduleEvent(EVENT_SOUL_MISSILE, urand(1000, 6000));
			}

			void UpdateAI(uint32 const diff)
			{
				if (_events.Empty())
					return;

				_events.Update(diff);

				if (_events.ExecuteEvent() == EVENT_SOUL_MISSILE)
				{
					DoCastAOE(SPELL_SOUL_MISSILE);
					_events.ScheduleEvent(EVENT_SOUL_MISSILE, urand(5000, 7000));
				}
			}

		private:
			InstanceScript* _instance;
			EventMap _events;
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			// Distance from the center of the spire
			if (creature->GetExactDist2d(4357.052f, 2769.421f) < 100.0f && creature->GetHomePosition().GetPositionZ() < 315.0f)
				return GetIcecrownCitadelAI<npc_arthas_teleport_visualAI>(creature);

			// Default to no script
			return NULL;
		}
};

enum TrashEvents {
	EVENT_BONE_FLURRY = 1,
	EVENT_CRYPT_SCARABS = 2,
	EVENT_DARK_MENDING = 3,
	EVENT_WEB_TRAP = 4,
	EVENT_GLACIAL_BLAST = 5,
	EVENT_SHIELD_BASH = 6,
	EVENT_DISRUPTING_SHOUT = 7,
	EVENT_SABER_LASH = 8,
	EVENT_DARK_BLESSING = 9,
	EVENT_SHADOW_BOLT = 10,
	EVENT_SHADOW_MEND = 11,
	EVENT_SHADOW_NOVA = 12,
	EVENT_CONSUMING_SHADOWS = 13,
	EVENT_CURSE_OF_AGONY = 14,
	EVENT_CHAOS_BOLT = 15,
	EVENT_SHADOW_CLEAVE = 16,
	EVENT_DARK_RECKONING = 17,
	//Upper Spire
	EVENT_SEVERED_ESSENCE = 18,
	//Plagueworks
	EVENT_TRASH_CLEAVE = 19,
	EVENT_PLAGUE_CLOUD = 20,
	EVENT_SCOURGE_HOOK = 21,
	EVENT_COMBOBULATING_SPRAY = 22,
	EVENT_PLAGUE_BLAST = 23,
	EVENT_PLAGUE_STREAM = 24,
	EVENT_LEAPING_FACE_MAUL = 25,
	EVENT_DEVOUR_HUMANOID = 26,
	EVENT_BUBBLING_PUS = 27,
	EVENT_MASSIVE_STOMP = 28,
	//Crimson Halls
	EVENT_LICH_SLAP = 29,
	EVENT_SHROUD_OF_PROTECTION = 30,
	EVENT_SHROUD_OF_SPELL_WARDING = 31,
	EVENT_AMPLIFY_MAGIC = 32,
	EVENT_BLAST_WAVE = 33,
	EVENT_FIREBALL = 34,
	EVENT_POLYMORPH = 35,
	EVENT_BLOOD_MIRROR = 36,
	EVENT_UNHOLY_STRIKE = 37,
	EVENT_BATTLE_SHOUT = 38,
	EVENT_VAMPIRE_RUSH = 39,
	EVENT_REND_FLESH = 40,
	EVENT_VAMPIRIC_CURSE = 41,
	EVENT_CHAINS_OF_SHADOW = 42,
	EVENT_BLOOD_SAP = 43,
	EVENT_SHADOWSTEP = 44,
	//Frostwing Halls
	EVENT_ADRENALINE_RUSH = 45,
	EVENT_BARBARIC_STRIKE = 46,
	EVENT_BANISH = 47,
	EVENT_DEATHS_EMBRACE = 48,
	EVENT_ICE_TRAP = 49,
	EVENT_RAPID_SHOT = 50,
	EVENT_SHOOT = 51,
	EVENT_VOLLEY = 52,
	EVENT_WHIRLWIND = 53,
	EVENT_RUSH = 54,
	EVENT_WEB = 55,
};

enum TrashSpells {
	//Lower Spire
	SPELL_BONE_FLURRY = 70960,
	SPELL_SHATTERED_BONES = 70961,
	SPELL_CRYPT_SCARABS = 70965,
	SPELL_DARK_MENDING = 71020,
	SPELL_WEB_TRAP = 70980,
	SPELL_GLACIAL_BLAST = 71029,
	SPELL_SHIELD_BASH = 70964,
	SPELL_DISRUPTING_SHOUT = 71022,
	SPELL_SABER_LASH = 71021,
	SPELL_DARK_BLESSING = 69391,
	SPELL_SHADOW_BOLT = 69387,
	SPELL_SHADOW_MEND = 69389,
	SPELL_SHADOW_NOVA = 69355,
	SPELL_CONSUMING_SHADOWS = 69405,
	SPELL_CURSE_OF_AGONY = 69404,
	SPELL_CHAOS_BOLT = 69576,
	SPELL_SHADOW_CLEAVE = 69492,
	SPELL_AURA_OF_DARKNESS = 69491,
	SPELL_DARK_RECKONING = 69483,
	//Upper Spire
	SPELL_SEVERED_ESSENCE = 71906,
	//Plagueworks
	SPELL_TRASH_CLEAVE = 40504,
	SPELL_PLAGUE_CLOUD = 71150,
	SPELL_SCOURGE_HOOK = 71140,
	SPELL_COMBOBULATING_SPRAY = 71103,
	SPELL_PLAGUE_BLAST = 73079,
	SPELL_PLAGUE_STREAM = 69871,
	SPELL_LEAPING_FACE_MAUL = 71164,
	SPELL_DEVOUR_HUMANOID = 71163,
	SPELL_BUBBLING_PUS = 71089,
	SPELL_BLIGHT_BOMB = 71088,
	SPELL_MASSIVE_STOMP = 71114,
	//Crimson Halls
	SPELL_LICH_SLAP = 72057,
	SPELL_SHROUD_OF_PROTECTION = 72065,
	SPELL_SHROUD_OF_SPELL_WARDING = 72066,
	SPELL_AMPLIFY_MAGIC = 70408,
	SPELL_BLAST_WAVE = 70407,
	SPELL_FIREBALL = 71153,
	SPELL_POLYMORPH = 70410,
	SPELL_BLOOD_MIRROR = 70451,
	SPELL_UNHOLY_STRIKE = 70437,
	SPELL_VAMPIRIC_AURA = 71736,
	SPELL_BATTLE_SHOUT = 70750,
	SPELL_VAMPIRE_RUSH = 70449,
	SPELL_REND_FLESH = 70435,
	SPELL_VAMPIRIC_CURSE = 70423,
	SPELL_CHAINS_OF_SHADOW = 70645,
	SPELL_BLOOD_SAP = 70432,
	SPELL_SHADOWSTEP = 70431,
	//Frostwing Halls
	SPELL_ADRENALINE_RUSH = 71258,
	SPELL_BARBARIC_STRIKE = 71257,
	SPELL_BANISH = 71298,
	SPELL_DEATHS_EMBRACE = 71299,
	SPELL_ARCTIC_CHILL = 71270,
	SPELL_ICE_TRAP = 71249,
	SPELL_RAPID_SHOT = 71251,
	SPELL_SHOOT = 71253,
	SPELL_VOLLEY = 71252,
	SPELL_WHIRLWIND = 41056,
	SPELL_RUSH = 71801,
	SPELL_WEB = 71327,
};

class npc_lower_spire : public CreatureScript
{
	public:
		npc_lower_spire() : CreatureScript("npc_lower_spire") { }

		struct npc_lower_spireAI : public ScriptedAI
		{
		npc_lower_spireAI(Creature* creature) : ScriptedAI(creature)
		{
			
		}
		
		EventMap _events;
		
		void EnterCombat(Unit* /*attacker*/)
		{
			_events.Reset();
			switch (me->GetEntry())
			{
				case NPC_TRASH_DAMNED:
					_events.ScheduleEvent(EVENT_BONE_FLURRY, urand(5000, 7500));
				break;
				case NPC_TRASH_NERUBAN:
					_events.ScheduleEvent(EVENT_CRYPT_SCARABS, urand(5000, 7500));
					_events.ScheduleEvent(EVENT_DARK_MENDING, urand(7500, 10000));
					_events.ScheduleEvent(EVENT_WEB_TRAP, urand(2500, 5000));
				break;
				case NPC_TRASH_SERVANT:
					_events.ScheduleEvent(EVENT_GLACIAL_BLAST, urand(5000, 7500));
				break;
				case NPC_TRASH_ANCIENT:
					_events.ScheduleEvent(EVENT_SHIELD_BASH, urand(5000, 7500));
				break;
				case NPC_TRASH_DEATHBOUND:
					_events.ScheduleEvent(EVENT_DISRUPTING_SHOUT, urand(10000, 15000));
					_events.ScheduleEvent(EVENT_SABER_LASH, urand(2500, 5000));
				break;
				case NPC_TRASH_DEATH_DISCIPLINE:
					_events.ScheduleEvent(EVENT_DARK_BLESSING, urand(7500, 10000));
					_events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2500, 5000));
					_events.ScheduleEvent(EVENT_DARK_MENDING, urand(7500, 10000));
				break;
				case NPC_TRASH_DEATH_ATTENDANT:
					_events.ScheduleEvent(EVENT_SHADOW_NOVA, urand(5000, 7500));
					_events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2500, 5000));
				break;
				case NPC_TRASH_DEATH_SERVANT:
					_events.ScheduleEvent(EVENT_CONSUMING_SHADOWS, urand(2500, 5000));
					_events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(5000, 7500));
					_events.ScheduleEvent(EVENT_CHAOS_BOLT, urand(2500, 5000));
				break;
				case NPC_TRASH_DEATH_ZEALOT:
					_events.ScheduleEvent(EVENT_SHADOW_CLEAVE, urand(2500, 5000));
				break;
				case NPC_TRASH_DEATH_HIGH_PRIEST:
					me->AddAura(SPELL_AURA_OF_DARKNESS, me);
					_events.ScheduleEvent(EVENT_DARK_RECKONING, urand(2500, 5000));
				break;
			}
		}
		
		void DamageTaken(Unit* attacker, uint32& damage)
		{
			if (me->GetEntry() == NPC_TRASH_DAMNED)
				if (damage > me->GetHealth())
					if (attacker != me)
						DoCastAOE(SPELL_SHATTERED_BONES);
		}
	
		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;
			
			_events.Update(uiDiff);
			
			if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAuraType(SPELL_AURA_MOD_STUN))
				return;
			
			while (uint32 eventId = _events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_BONE_FLURRY:
						me->AddAura(SPELL_BONE_FLURRY, me);
						_events.ScheduleEvent(EVENT_BONE_FLURRY, 20000);
					break;
					case EVENT_CRYPT_SCARABS:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_CRYPT_SCARABS);
						_events.ScheduleEvent(EVENT_CRYPT_SCARABS, 15000);
					break;
					case EVENT_DARK_MENDING:
						if (Unit* target = DoSelectLowestHpFriendly(40.0f, 50000)) {
							DoCast(target, SPELL_DARK_MENDING);
							_events.ScheduleEvent(EVENT_DARK_MENDING, 20000);
							break;
						}
						//no target-check in 2 seconds again
						_events.ScheduleEvent(EVENT_DARK_MENDING, 2000);
					break;
					case EVENT_WEB_TRAP:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_WEB_TRAP);
						_events.ScheduleEvent(EVENT_WEB_TRAP, 20000);
					break;
					case EVENT_GLACIAL_BLAST:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_GLACIAL_BLAST);
						_events.ScheduleEvent(EVENT_GLACIAL_BLAST, 15000);
					break;
					case EVENT_SHIELD_BASH:
						DoCastVictim(SPELL_SHIELD_BASH);
						_events.ScheduleEvent(EVENT_SHIELD_BASH, 8000);
					break;
					case EVENT_DISRUPTING_SHOUT:
						DoCastAOE(SPELL_DISRUPTING_SHOUT);
						_events.ScheduleEvent(EVENT_DISRUPTING_SHOUT, 30000);
					break;
					case EVENT_SABER_LASH:
						DoCastVictim(SPELL_SABER_LASH);
						_events.ScheduleEvent(EVENT_SABER_LASH, 5000);
					break;
					case EVENT_DARK_BLESSING:
					{
						std::list<Creature*> list = DoFindFriendlyMissingBuff(30.0f, SPELL_DARK_BLESSING);
						if (!list.empty())
						{
							std::list<Creature*>::const_iterator itr = list.begin();
							advance(itr, rand()%list.size());
							DoCast(*itr, SPELL_DARK_BLESSING);
						}
						_events.ScheduleEvent(EVENT_DARK_BLESSING, 7500);
						break;
					}
					case EVENT_SHADOW_BOLT:
						DoCastVictim(SPELL_SHADOW_BOLT);
						_events.ScheduleEvent(EVENT_SHADOW_BOLT, 5000);
					break;
					case EVENT_SHADOW_MEND:
						if (Unit* target = DoSelectLowestHpFriendly(40.0f, 75000)) {
							DoCast(target, SPELL_SHADOW_MEND);
							_events.ScheduleEvent(EVENT_SHADOW_MEND, 20000);
							break;
						}
						//no target-check in 2 seconds again
						_events.ScheduleEvent(EVENT_SHADOW_MEND, 2000);
					break;
					case EVENT_SHADOW_NOVA:
						DoCast(me, SPELL_SHADOW_NOVA);
						_events.ScheduleEvent(EVENT_SHADOW_NOVA, 7500);
					break;
					case EVENT_CONSUMING_SHADOWS:
						DoCastVictim(SPELL_CONSUMING_SHADOWS);
						_events.ScheduleEvent(EVENT_CONSUMING_SHADOWS, 25000);
					break;
					case EVENT_CURSE_OF_AGONY:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
							DoCast(target, SPELL_CURSE_OF_AGONY);
						_events.ScheduleEvent(EVENT_CURSE_OF_AGONY, 15000);
					break;
					case EVENT_CHAOS_BOLT:
						DoCastVictim(SPELL_CHAOS_BOLT);
						_events.ScheduleEvent(EVENT_CHAOS_BOLT, 5000);
					break;
					case EVENT_SHADOW_CLEAVE:
						DoCastVictim(SPELL_SHADOW_CLEAVE);
						_events.ScheduleEvent(EVENT_SHADOW_CLEAVE, 5000);
					break;
					case EVENT_DARK_RECKONING:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f))
							DoCast(target, SPELL_DARK_RECKONING);
						_events.ScheduleEvent(EVENT_DARK_RECKONING, 15000);
					break;
				}
			}
			
			DoMeleeAttackIfReady();
		}
		};
	
		CreatureAI* GetAI(Creature* creature) const
		{
		return new npc_lower_spireAI(creature);
		}
};

class npc_plagueworks : public CreatureScript
{
	public:
		npc_plagueworks() : CreatureScript("npc_plagueworks") { }

		struct npc_plagueworksAI : public ScriptedAI
		{
		npc_plagueworksAI(Creature* creature) : ScriptedAI(creature)
		{
			
		}
		
		EventMap _events;
		uint64 leapGUID;
		
		void EnterCombat(Unit* /*attacker*/)
		{
			_events.Reset();
			leapGUID=0;
			switch (me->GetEntry())
			{
				case NPC_TRASH_VALKYR_HERALD:
					//_events.ScheduleEvent(EVENT_SEVERED_ESSENCE, urand(10000, 15000));
				break;
				case NPC_TRASH_BLIGHTED_ABOM:
					_events.ScheduleEvent(EVENT_TRASH_CLEAVE, urand(2500, 5000));
					_events.ScheduleEvent(EVENT_PLAGUE_CLOUD, urand(10000, 15000));
					_events.ScheduleEvent(EVENT_SCOURGE_HOOK, urand(10000, 15000));
				break;
				case NPC_TRASH_PLAGUE_SCIENTIST:
					_events.ScheduleEvent(EVENT_COMBOBULATING_SPRAY, urand(5000, 7500));
					_events.ScheduleEvent(EVENT_PLAGUE_BLAST, urand(2500, 5000));
					_events.ScheduleEvent(EVENT_PLAGUE_STREAM, urand(5000, 7500));
				break;
				case NPC_TRASH_VENGEFUL_FLESH:
					_events.ScheduleEvent(EVENT_LEAPING_FACE_MAUL, urand(12500, 25000));
				break;
				case NPC_TRASH_PUSTULATING_HORROR:
					_events.ScheduleEvent(EVENT_BUBBLING_PUS, urand(7500, 10000));
				break;
				case NPC_TRASH_DECAYING_COLOSSUS:
					_events.ScheduleEvent(EVENT_MASSIVE_STOMP, urand(10000, 15000));
				break;
			}
		}
		
		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->GetEntry() == NPC_TRASH_PUSTULATING_HORROR)
				if (me->HealthBelowPctDamaged(25, damage))
					if (!me->HasUnitState(UNIT_STATE_CASTING))
						DoCastAOE(SPELL_BLIGHT_BOMB);
		}
		
		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;
			
			_events.Update(uiDiff);
			
			if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAuraType(SPELL_AURA_MOD_STUN))
				return;
			
			while (uint32 eventId = _events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_SEVERED_ESSENCE:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_SEVERED_ESSENCE);
						_events.ScheduleEvent(EVENT_SEVERED_ESSENCE, 15000);
					break;
					case EVENT_TRASH_CLEAVE:
						DoCastVictim(SPELL_TRASH_CLEAVE);
						_events.ScheduleEvent(EVENT_TRASH_CLEAVE, 5000);
					break;
					case EVENT_PLAGUE_CLOUD:
						DoCastAOE(SPELL_PLAGUE_CLOUD);
						_events.ScheduleEvent(EVENT_PLAGUE_CLOUD, 30000);
					break;
					case EVENT_SCOURGE_HOOK:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_SCOURGE_HOOK);
						_events.ScheduleEvent(EVENT_SCOURGE_HOOK, 15000);
					break;
					case EVENT_COMBOBULATING_SPRAY:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, SPELL_COMBOBULATING_SPRAY);
						_events.ScheduleEvent(EVENT_COMBOBULATING_SPRAY, 15000);
					break;
					case EVENT_PLAGUE_BLAST:
						DoCastVictim(SPELL_PLAGUE_BLAST);
						_events.ScheduleEvent(EVENT_PLAGUE_BLAST, 5000);
					break;
					case EVENT_PLAGUE_STREAM:
					{
						std::list<Creature*> list = DoFindFriendlyMissingBuff(30.0f, SPELL_PLAGUE_STREAM);
						if (!list.empty())
						{
							std::list<Creature*>::const_iterator itr = list.begin();
							advance(itr, rand()%list.size());
							me->AddAura(SPELL_PLAGUE_STREAM, *itr);
						}
						_events.ScheduleEvent(EVENT_PLAGUE_STREAM, 15000);
						break;
					}
					case EVENT_LEAPING_FACE_MAUL:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
						{
							leapGUID=target->GetGUID();
							DoCast(target, SPELL_LEAPING_FACE_MAUL);
							_events.ScheduleEvent(EVENT_DEVOUR_HUMANOID, 1000);
						}
						_events.ScheduleEvent(EVENT_LEAPING_FACE_MAUL, 20000);
					break;
					case EVENT_DEVOUR_HUMANOID:
						if (Unit* target = Unit::GetUnit((*me), leapGUID))
							DoCast(target, SPELL_DEVOUR_HUMANOID);
						leapGUID=0;
					break;
					case EVENT_BUBBLING_PUS:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_BUBBLING_PUS);
						_events.ScheduleEvent(EVENT_BUBBLING_PUS, 15000);
					break;
					case EVENT_MASSIVE_STOMP:
						DoCastAOE(SPELL_MASSIVE_STOMP);
						_events.ScheduleEvent(EVENT_MASSIVE_STOMP, 15000);
					break;
				}
			}
			
			DoMeleeAttackIfReady();
		}
		};
	
		CreatureAI* GetAI(Creature* creature) const
		{
		return new npc_plagueworksAI(creature);
		}
};

class npc_crimson_halls : public CreatureScript
{
	public:
		npc_crimson_halls() : CreatureScript("npc_crimson_halls") { }

		struct npc_crimson_hallsAI : public ScriptedAI
		{
			npc_crimson_hallsAI(Creature* creature) : ScriptedAI(creature)
			{
				
			}
			
			EventMap _events;
			uint64 mirrorSourceGUID;
			uint64 mirrorTargetGUID;
		
			void EnterCombat(Unit* attacker)
			{
				_events.Reset();
				mirrorSourceGUID = 0;
				mirrorTargetGUID = 0;
				switch (me->GetEntry())
				{
					case NPC_TRASH_ADVISOR:
						_events.ScheduleEvent(EVENT_LICH_SLAP, urand(5000, 7500));
						_events.ScheduleEvent(EVENT_SHROUD_OF_PROTECTION, 1000);
					break;
					case NPC_TRASH_ARCHMAGE:
						_events.ScheduleEvent(EVENT_AMPLIFY_MAGIC, urand(1000, 2500));
						_events.ScheduleEvent(EVENT_BLAST_WAVE, urand(5000, 10000));
						_events.ScheduleEvent(EVENT_FIREBALL, urand(2500, 5000));
						_events.ScheduleEvent(EVENT_POLYMORPH, urand(7500, 10000));
					break;
					case NPC_TRASH_BLOOD_KNIGHT:
						//_events.ScheduleEvent(EVENT_BLOOD_MIRROR, urand(7500, 10000));
						_events.ScheduleEvent(EVENT_UNHOLY_STRIKE, urand(5000, 75000));
						me->AddAura(SPELL_VAMPIRIC_AURA, me);
					break;
					case NPC_TRASH_COMMANDER:
						DoCastAOE(SPELL_BATTLE_SHOUT);
						_events.ScheduleEvent(EVENT_BATTLE_SHOUT, 20000);
						_events.ScheduleEvent(EVENT_VAMPIRE_RUSH, 500);
					break;
					case NPC_TRASH_LIEUTENANT:
						_events.ScheduleEvent(EVENT_REND_FLESH, urand(5000, 7500));
						_events.ScheduleEvent(EVENT_VAMPIRIC_CURSE, urand(5000, 7500));
					break;
					case NPC_TRASH_NOBLE:
						_events.ScheduleEvent(EVENT_CHAINS_OF_SHADOW, urand(7500, 10000));
						_events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2500, 5000));
					break;
					case NPC_TRASH_TACTICIAN:
						_events.ScheduleEvent(EVENT_BLOOD_SAP, urand(7500, 10000));
						_events.ScheduleEvent(EVENT_UNHOLY_STRIKE, urand(5000, 75000));
						_events.ScheduleEvent(EVENT_SHADOWSTEP, 500);
					break;
				}
			}
			
			void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/)
			{
				/*if (target->HasAura(SPELL_BLOOD_MIRROR))
					if (uint64 guid = (target->GetGUID() == mirrorSourceGUID ? mirrorTargetGUID : 0))
					{
						Unit* target2 = Unit::GetUnit(*me, guid);
						if (target2->isAlive())
							me->DealDamage(target2, damage/2);
					}*/
			}
			
			void UpdateAI(const uint32 uiDiff)
			{
				if (!UpdateVictim())
					return;
				
				_events.Update(uiDiff);
				
				if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAuraType(SPELL_AURA_MOD_STUN))
					return;
				
				while (uint32 eventId = _events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_SHADOW_BOLT:
							DoCastVictim(SPELL_SHADOW_BOLT);
							_events.ScheduleEvent(EVENT_SHADOW_BOLT, 5000);
						break;
						case EVENT_LICH_SLAP:
							DoCastVictim(SPELL_LICH_SLAP);
							_events.ScheduleEvent(EVENT_LICH_SLAP, 15000);
						break;
						case EVENT_SHROUD_OF_PROTECTION:
							if (Unit* target = DoSelectLowestHpFriendly(40.0f, 100000))
							{
								DoCast(target, SPELL_SHROUD_OF_PROTECTION);
								_events.ScheduleEvent(EVENT_SHROUD_OF_SPELL_WARDING, 15000);
							}
							else 
							_events.ScheduleEvent(EVENT_SHROUD_OF_PROTECTION, 2000);
						break;
						case EVENT_SHROUD_OF_SPELL_WARDING:
							if (Unit* target = DoSelectLowestHpFriendly(40.0f, 100000))
							{
								DoCast(target, SPELL_SHROUD_OF_SPELL_WARDING);
								_events.ScheduleEvent(EVENT_SHROUD_OF_PROTECTION, 15000);
							}
							else 
								_events.ScheduleEvent(EVENT_SHROUD_OF_SPELL_WARDING, 2000);
						break;
						case EVENT_AMPLIFY_MAGIC:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f))
								DoCast(target, SPELL_AMPLIFY_MAGIC);
							_events.ScheduleEvent(EVENT_AMPLIFY_MAGIC, 3000);
						break;
						case EVENT_BLAST_WAVE:
							DoCastAOE(SPELL_BLAST_WAVE);
							_events.ScheduleEvent(EVENT_BLAST_WAVE, 10000);
						break;
						case EVENT_FIREBALL:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
								DoCast(target, SPELL_FIREBALL);
							_events.ScheduleEvent(EVENT_FIREBALL, 5000);
						break;
						case EVENT_POLYMORPH:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
								DoCast(target, SPELL_POLYMORPH);
							_events.ScheduleEvent(EVENT_POLYMORPH, 10000);
						break;
						case EVENT_BLOOD_MIRROR:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							{
								Unit* target2;
								do 
								{
									target2 = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f);
								}
								while (target != target2);
								DoCast(target, SPELL_BLOOD_MIRROR);
								DoCast(target2, SPELL_BLOOD_MIRROR);
								mirrorSourceGUID = target->GetGUID();
								mirrorTargetGUID = target2->GetGUID();
							}
							_events.ScheduleEvent(EVENT_BLOOD_MIRROR, 40000);
						break;
						case EVENT_UNHOLY_STRIKE:
							DoCastVictim(SPELL_UNHOLY_STRIKE);
							_events.ScheduleEvent(EVENT_UNHOLY_STRIKE, 5000);
						break;
						case EVENT_BATTLE_SHOUT:
							DoCastAOE(SPELL_BATTLE_SHOUT);
							_events.ScheduleEvent(EVENT_BATTLE_SHOUT, 20000);
						break;
						case EVENT_VAMPIRE_RUSH:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 45.0f))
								DoCast(target, SPELL_VAMPIRE_RUSH);
							_events.ScheduleEvent(EVENT_VAMPIRE_RUSH, 10000);
						break;
						case EVENT_REND_FLESH:
							DoCastVictim(SPELL_REND_FLESH);
							_events.ScheduleEvent(EVENT_REND_FLESH, 10000);
						break;
						case EVENT_VAMPIRIC_CURSE:
							DoCastVictim(SPELL_VAMPIRIC_CURSE);
							_events.ScheduleEvent(EVENT_VAMPIRIC_CURSE, 15000);
						break;
						case EVENT_CHAINS_OF_SHADOW:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
								DoCast(target, SPELL_CHAINS_OF_SHADOW);
							_events.ScheduleEvent(EVENT_CHAINS_OF_SHADOW, 15000);
						break;
						case EVENT_BLOOD_SAP:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 15.0f))
								DoCast(target, SPELL_BLOOD_SAP);
							_events.ScheduleEvent(EVENT_BLOOD_SAP, 15000);
						break;
						case EVENT_SHADOWSTEP:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
								DoCast(target, SPELL_SHADOWSTEP);
							_events.ScheduleEvent(EVENT_SHADOWSTEP, 10000);
						break;
					}
				}
				
				DoMeleeAttackIfReady();
			}
		};
	
		CreatureAI* GetAI(Creature* creature) const
		{
		return new npc_crimson_hallsAI(creature);
		}
};

class npc_frostwing_halls : public CreatureScript
{
	public:
		npc_frostwing_halls() : CreatureScript("npc_frostwing_halls") { }

		struct npc_frostwing_hallsAI : public ScriptedAI
		{
		npc_frostwing_hallsAI(Creature* creature) : ScriptedAI(creature)
		{
			
		}
		
		EventMap _events;
		
		void EnterCombat(Unit* attacker)
		{
			_events.Reset();
			switch (me->GetEntry())
			{
				case NPC_YMIRJAR_BATTLE_MAIDEN:
					_events.ScheduleEvent(EVENT_ADRENALINE_RUSH, urand(1000, 2500));
					_events.ScheduleEvent(EVENT_BARBARIC_STRIKE, urand(2500, 5000));
				break;
				case NPC_YMIRJAR_DEATHBRINGER:
					_events.ScheduleEvent(EVENT_BANISH, urand(5000, 7500));
					_events.ScheduleEvent(EVENT_DEATHS_EMBRACE, urand(5000, 7500));
					_events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2500, 5000));
				break;
				case NPC_YMIRJAR_FROSTBINDER:
					me->AddAura(SPELL_ARCTIC_CHILL, me);
				break;
				case NPC_YMIRJAR_HUNTRESS:
					_events.ScheduleEvent(EVENT_ICE_TRAP, urand(5000, 7500));
					_events.ScheduleEvent(EVENT_RAPID_SHOT, urand(7500, 10000));
					_events.ScheduleEvent(EVENT_SHOOT, urand(1000, 2500));
					_events.ScheduleEvent(EVENT_VOLLEY, urand(5000, 7500));
				break;
				case NPC_YMIRJAR_WARLORD:
					_events.ScheduleEvent(EVENT_WHIRLWIND, urand(5000, 7500));
				break;
				case NPC_TRASH_NERUB_CHAMPION:
					_events.ScheduleEvent(EVENT_RUSH, urand(5000, 7500));
				break;
				case NPC_TRASH_NERUB_WEBWEAVER:
					_events.ScheduleEvent(EVENT_WEB, urand(7500, 1000));
					_events.ScheduleEvent(EVENT_CRYPT_SCARABS, urand(5000, 7500));
				break;
			}
		}
		
		bool CanAIAttack(Unit const* target) const
		{
			// do not see targets inside Frostwing Halls when we are not there
			return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f);
		}
		
		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;
			
			_events.Update(uiDiff);
			
			if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAuraType(SPELL_AURA_MOD_STUN))
				return;
			
			while (uint32 eventId = _events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_SHADOW_BOLT:
						DoCastVictim(SPELL_SHADOW_BOLT);
						_events.ScheduleEvent(EVENT_SHADOW_BOLT, 5000);
					break;
					case EVENT_CRYPT_SCARABS:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_CRYPT_SCARABS);
						_events.ScheduleEvent(EVENT_CRYPT_SCARABS, 15000);
					break;
					case EVENT_ADRENALINE_RUSH:
						me->AddAura(SPELL_ADRENALINE_RUSH, me);
						_events.ScheduleEvent(EVENT_ADRENALINE_RUSH, 15000);
					break;
					case EVENT_BARBARIC_STRIKE:
						DoCastVictim(SPELL_BARBARIC_STRIKE);
						_events.ScheduleEvent(EVENT_BARBARIC_STRIKE, 2500);
					break;
					case EVENT_BANISH:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f))
							DoCast(target, SPELL_BANISH);
						_events.ScheduleEvent(EVENT_BANISH, 15000);
					break;
					case EVENT_DEATHS_EMBRACE:
						DoCast(me, SPELL_DEATHS_EMBRACE);
						_events.ScheduleEvent(EVENT_DEATHS_EMBRACE, 20000);
					break;
					case EVENT_ICE_TRAP:
						DoCastAOE(SPELL_ICE_TRAP);
						_events.ScheduleEvent(EVENT_ICE_TRAP, 40000);
					break;
					case EVENT_RAPID_SHOT:
						DoCastAOE(SPELL_RAPID_SHOT);
						_events.ScheduleEvent(EVENT_RAPID_SHOT, 20000);
					break;
					case EVENT_SHOOT:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
							DoCast(target, SPELL_SHOOT);
						_events.ScheduleEvent(EVENT_SHOOT, 2700);
					break;
					case EVENT_VOLLEY:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
							DoCast(target, SPELL_VOLLEY);
						_events.ScheduleEvent(EVENT_VOLLEY, 10000);
					break;
					case EVENT_WHIRLWIND:
						DoCastAOE(SPELL_WHIRLWIND);
						_events.ScheduleEvent(EVENT_WHIRLWIND, 7500);
					break;
					case EVENT_RUSH:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 25.0f))
							DoCast(target, SPELL_RUSH);
						_events.ScheduleEvent(EVENT_RUSH, 7500);
					break;
					case EVENT_WEB:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
							DoCast(target, SPELL_WEB);
						_events.ScheduleEvent(EVENT_WEB, 10000);
					break;
				}
			}
			
			DoMeleeAttackIfReady();
		}
		};
	
		CreatureAI* GetAI(Creature* creature) const
		{
		return new npc_frostwing_hallsAI(creature);
		}
};

class spell_icc_stoneform : public SpellScriptLoader
{
	public:
		spell_icc_stoneform() : SpellScriptLoader("spell_icc_stoneform") { }

		class spell_icc_stoneform_AuraScript : public AuraScript
		{
			PrepareAuraScript(spell_icc_stoneform_AuraScript);

			void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
			{
				if (Creature* target = GetTarget()->ToCreature())
				{
					target->SetReactState(REACT_PASSIVE);
					target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
					target->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_CUSTOM_SPELL_02);
				}
			}

			void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
			{
				if (Creature* target = GetTarget()->ToCreature())
				{
					target->SetReactState(REACT_AGGRESSIVE);
					target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
					target->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
				}
			}

			void Register()
			{
				OnEffectApply += AuraEffectApplyFn(spell_icc_stoneform_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
				OnEffectRemove += AuraEffectRemoveFn(spell_icc_stoneform_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
			}
		};

		AuraScript* GetAuraScript() const
		{
			return new spell_icc_stoneform_AuraScript();
		}
};

const static Position deathwardPoints[] = 
{
	{-214.051651f, 2225.724854f, 35.233479f, 0.0f},
	{-214.137344f, 2192.201416f, 35.233479f, 0.0f},
	{-290.764526f, 2203.863037f, 42.014103f, 0.0f},
	{-288.462433f, 2223.533203f, 42.010818f, 0.0f}
};

class spell_icc_sprit_alarm : public SpellScriptLoader
{
	public:
		spell_icc_sprit_alarm() : SpellScriptLoader("spell_icc_sprit_alarm") { }

		class spell_icc_sprit_alarm_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_icc_sprit_alarm_SpellScript);

			void HandleEvent(SpellEffIndex effIndex)
			{
				PreventHitDefaultEffect(effIndex);
				uint32 trapId = 0;
				switch (GetSpellInfo()->Effects[effIndex].MiscValue)
				{
					case EVENT_AWAKEN_WARD_1:
						trapId = GO_SPIRIT_ALARM_1;
						break;
					case EVENT_AWAKEN_WARD_2:
						trapId = GO_SPIRIT_ALARM_2;
						break;
					case EVENT_AWAKEN_WARD_3:
						trapId = GO_SPIRIT_ALARM_3;
						break;
					case EVENT_AWAKEN_WARD_4:
						trapId = GO_SPIRIT_ALARM_4;
						break;
					default:
						return;
				}

				if (GameObject* trap = GetCaster()->FindNearestGameObject(trapId, 5.0f))
					trap->SetRespawnTime(trap->GetGOInfo()->trap.autoCloseTime);

				std::list<Creature*> wards;
				GetCaster()->GetCreatureListWithEntryInGrid(wards, NPC_DEATHBOUND_WARD, 150.0f);
				wards.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
				for (std::list<Creature*>::iterator itr = wards.begin(); itr != wards.end(); ++itr)
				{
					if ((*itr)->isAlive() && (*itr)->HasAura(SPELL_STONEFORM))
					{
						(*itr)->AI()->Talk(SAY_TRAP_ACTIVATE);
						(*itr)->RemoveAurasDueToSpell(SPELL_STONEFORM);

						int min = 0;
						float dist = 1000000.0f;
						for (int j = 0; j < 4; j++)
						{
							if (dist > (*itr)->GetExactDist2d(deathwardPoints[j].GetPositionX(), deathwardPoints[j].GetPositionY()))
							{
								dist = (*itr)->GetExactDist2d(deathwardPoints[j].GetPositionX(), deathwardPoints[j].GetPositionY());
								min = j;
							}
						}
						(*itr)->GetMotionMaster()->MovePoint(0, deathwardPoints[min]);

						/*
						if (Unit* target = (*itr)->SelectNearestTarget(150.0f))
							(*itr)->AI()->AttackStart(target);
						*/
						break;
					}
				}
			}

			void Register()
			{
				OnEffectHit += SpellEffectFn(spell_icc_sprit_alarm_SpellScript::HandleEvent, EFFECT_2, SPELL_EFFECT_SEND_EVENT);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_icc_sprit_alarm_SpellScript();
		}
};

class spell_frost_giant_death_plague_periodic : public SpellScriptLoader
{
	public:
		spell_frost_giant_death_plague_periodic() : SpellScriptLoader("spell_frost_giant_death_plague_periodic") { }

		class spell_frost_giant_death_plague_periodic_AuraScript : public AuraScript
		{
			PrepareAuraScript(spell_frost_giant_death_plague_periodic_AuraScript);

			void HandleTriggerSpell(AuraEffect const* aurEff)
			{
				PreventDefaultAction();
				GetCaster()->Kill(GetCaster());
				
				//unitList.remove_if(DeathPlagueTargetSelector(GetCaster()));
			}

			void Register()
			{
				OnEffectPeriodic += AuraEffectPeriodicFn(spell_frost_giant_death_plague_periodic_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
			}
		};

		AuraScript* GetAuraScript() const
		{
			return new spell_frost_giant_death_plague_periodic_AuraScript();
		}
};

class spell_frost_giant_death_plague : public SpellScriptLoader
{
	public:
		spell_frost_giant_death_plague() : SpellScriptLoader("spell_frost_giant_death_plague") { }

		class spell_frost_giant_death_plague_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_frost_giant_death_plague_SpellScript);

			bool Load()
			{
				_failed = false;
				return true;
			}

			// First effect
			void CountTargets(std::list<Unit*>& unitList)
			{
				//unitList.remove(GetCaster());
				unitList.remove_if (DeathPlagueTargetSelector(GetCaster()));
				_failed = true;//unitList.empty();//unitList.size() > 1;
			}

			// Second effect
			void FilterTargets(std::list<Unit*>& unitList)
			{
				// Select valid targets for jump
				unitList.remove_if (DeathPlagueTargetSelector(GetCaster()));
				/*if (!unitList.empty())
				{
					Unit* target = Trinity::Containers::SelectRandomContainerElement(unitList);
					unitList.clear();
					unitList.push_back(target);
				}*/

				unitList.push_back(GetCaster());
			}

			void HandleScript(SpellEffIndex effIndex)
			{
				GetCaster()->Kill(GetCaster());
				PreventHitDefaultEffect(effIndex);
				/*if (_failed)
				{
					GetCaster()->CastSpell(GetCaster(), SPELL_DEATH_PLAGUE_KILL, true);
					if (GetCaster()->isAlive())
						GetCaster()->Kill(GetCaster());
				}
				else
				{
					if (GetHitUnit() != GetCaster())
					{
						GetCaster()->CastSpell(GetHitUnit(), SPELL_DEATH_PLAGUE_AURA, true);
						if (!GetHitUnit()->HasAura(SPELL_DEATH_PLAGUE_AURA))
							GetCaster()->AddAura(SPELL_DEATH_PLAGUE_AURA, GetHitUnit());
					}
					else
					{
						GetCaster()->CastSpell(GetCaster(), SPELL_RECENTLY_INFECTED, true);
						if (!GetCaster()->HasAura(SPELL_RECENTLY_INFECTED))
							GetCaster()->AddAura(SPELL_RECENTLY_INFECTED, GetCaster());
					}
				}*/
			}

			void Register()
			{
				OnUnitTargetSelect += SpellUnitTargetFn(spell_frost_giant_death_plague_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
				OnUnitTargetSelect += SpellUnitTargetFn(spell_frost_giant_death_plague_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
				OnEffectHitTarget += SpellEffectFn(spell_frost_giant_death_plague_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
			}

			bool _failed;
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_frost_giant_death_plague_SpellScript();
		}
};

class spell_icc_harvest_blight_specimen : public SpellScriptLoader
{
	public:
		spell_icc_harvest_blight_specimen() : SpellScriptLoader("spell_icc_harvest_blight_specimen") { }

		class spell_icc_harvest_blight_specimen_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_icc_harvest_blight_specimen_SpellScript);

			void HandleScript(SpellEffIndex effIndex)
			{
				PreventHitDefaultEffect(effIndex);
				GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
			}

			void HandleQuestComplete(SpellEffIndex /*effIndex*/)
			{
				GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
			}

			void Register()
			{
				OnEffectHitTarget += SpellEffectFn(spell_icc_harvest_blight_specimen_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
				OnEffectHitTarget += SpellEffectFn(spell_icc_harvest_blight_specimen_SpellScript::HandleQuestComplete, EFFECT_1, SPELL_EFFECT_QUEST_COMPLETE);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_icc_harvest_blight_specimen_SpellScript();
		}
};

class AliveCheck
{
	public:
		bool operator()(WorldObject* object) const
		{
			if (Unit* unit = object->ToUnit())
				return unit->isAlive();
			return true;
		}
};

class spell_svalna_revive_champion : public SpellScriptLoader
{
	public:
		spell_svalna_revive_champion() : SpellScriptLoader("spell_svalna_revive_champion") { }

		class spell_svalna_revive_champion_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_svalna_revive_champion_SpellScript);

			void RemoveAliveTarget(std::list<Unit*>& unitList)
			{
				unitList.remove_if(AliveCheck());
				Trinity::Containers::RandomResizeList(unitList, 2);
			}

			void Land(SpellEffIndex /*effIndex*/)
			{
				Creature* caster = GetCaster()->ToCreature();
				if (!caster)
					return;

				Position pos;
				caster->GetPosition(&pos);
				caster->GetNearPosition(pos, 5.0f, 0.0f);
				pos.m_positionZ = caster->GetBaseMap()->GetHeight(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), true, 20.0f);
				pos.m_positionZ += 0.05f;
				caster->SetHomePosition(pos);
				caster->GetMotionMaster()->MovePoint(POINT_LAND, pos);
			}

			void Register()
			{
				OnUnitTargetSelect += SpellUnitTargetFn(spell_svalna_revive_champion_SpellScript::RemoveAliveTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
				OnEffectHit += SpellEffectFn(spell_svalna_revive_champion_SpellScript::Land, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_svalna_revive_champion_SpellScript();
		}
};

class spell_svalna_remove_spear : public SpellScriptLoader
{
	public:
		spell_svalna_remove_spear() : SpellScriptLoader("spell_svalna_remove_spear") { }

		class spell_svalna_remove_spear_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_svalna_remove_spear_SpellScript);

			void HandleScript(SpellEffIndex effIndex)
			{
				PreventHitDefaultEffect(effIndex);
				if (Creature* target = GetHitCreature())
				{
					if (Unit* vehicle = target->GetVehicleBase())
						vehicle->RemoveAurasDueToSpell(SPELL_IMPALING_SPEAR);
					target->DespawnOrUnsummon(1);
				}
			}

			void Register()
			{
				OnEffectHitTarget += SpellEffectFn(spell_svalna_remove_spear_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_svalna_remove_spear_SpellScript();
		}
};

class spell_icc_soul_missile : public SpellScriptLoader
{
	public:
		spell_icc_soul_missile() : SpellScriptLoader("spell_icc_soul_missile") { }

		class spell_icc_soul_missile_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_icc_soul_missile_SpellScript);

			void RelocateDest()
			{
				static Position const offset = {0.0f, 0.0f, 200.0f, 0.0f};
				const_cast<WorldLocation*>(GetExplTargetDest())->RelocateOffset(offset);
			}

			void Register()
			{
				OnCast += SpellCastFn(spell_icc_soul_missile_SpellScript::RelocateDest);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_icc_soul_missile_SpellScript();
		}
};

class at_icc_saurfang_portal : public AreaTriggerScript
{
	public:
		at_icc_saurfang_portal() : AreaTriggerScript("at_icc_saurfang_portal") { }

		bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
		{
			InstanceScript* instance = player->GetInstanceScript();
			if (!instance || instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
				return true;

			player->TeleportTo(631, 4126.35f, 2769.23f, 350.963f, 0.0f);

			if (instance->GetData(DATA_COLDFLAME_JETS) == NOT_STARTED)
			{
				// Process relocation now, to preload the grid and initialize traps
				player->GetMap()->PlayerRelocation(player, 4126.35f, 2769.23f, 350.963f, 0.0f);

				instance->SetData(DATA_COLDFLAME_JETS, IN_PROGRESS);
				std::list<Creature*> traps;
				GetCreatureListWithEntryInGrid(traps, player, NPC_FROST_FREEZE_TRAP, 120.0f);
				//traps.sort(Trinity::ObjectDistanceOrderPred(player));
				//bool instant = false;
				for (std::list<Creature*>::iterator itr = traps.begin(); itr != traps.end(); ++itr)
				{
					(*itr)->AI()->DoAction(urand(1000, 11000));
				}
			}

			return true;
		}
};

class at_icc_shutdown_traps : public AreaTriggerScript
{
	public:
		at_icc_shutdown_traps() : AreaTriggerScript("at_icc_shutdown_traps") { }

		bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
		{
			if (!player->isGameMaster())
				if (InstanceScript* instance = player->GetInstanceScript())
					instance->SetData(DATA_COLDFLAME_JETS, DONE);
			return true;
		}
};

class at_icc_start_blood_quickening : public AreaTriggerScript
{
	public:
		at_icc_start_blood_quickening() : AreaTriggerScript("at_icc_start_blood_quickening") { }

		bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
		{
			if (!player->isGameMaster())
				if (InstanceScript* instance = player->GetInstanceScript())
					if (instance->GetData(DATA_BLOOD_QUICKENING_STATE) == NOT_STARTED)
						instance->SetData(DATA_BLOOD_QUICKENING_STATE, IN_PROGRESS);
			return true;
		}
};

class at_icc_start_frostwing_gauntlet : public AreaTriggerScript
{
	public:
		at_icc_start_frostwing_gauntlet() : AreaTriggerScript("at_icc_start_frostwing_gauntlet") { }

		bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
		{
			if (!player->isGameMaster())
				if (InstanceScript* instance = player->GetInstanceScript())
					if (Creature* crok = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_CROK_SCOURGEBANE)))
						//crok->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
						crok->AI()->DoAction(ACTION_START_GAUNTLET);
			return true;
		}
};

void AddSC_icecrown_citadel()
{
	new npc_highlord_tirion_fordring_lh();
	new npc_rotting_frost_giant();
	new npc_frost_freeze_trap();
	new npc_alchemist_adrianna();
	new boss_sister_svalna();
	new npc_crok_scourgebane();
	new npc_captain_arnath();
	new npc_captain_brandon();
	new npc_captain_grondel();
	new npc_captain_rupert();
	new npc_impaling_spear();
	new npc_arthas_teleport_visual();
	new npc_lower_spire();
	new npc_plagueworks();
	new npc_crimson_halls();
	new npc_frostwing_halls();
	new spell_icc_stoneform();
	new spell_icc_sprit_alarm();
	new spell_frost_giant_death_plague();
	new spell_icc_harvest_blight_specimen();
	new spell_trigger_spell_from_caster("spell_svalna_caress_of_death", SPELL_IMPALING_SPEAR_KILL);
	new spell_svalna_revive_champion();
	new spell_svalna_remove_spear();
	new spell_icc_soul_missile();
	new at_icc_saurfang_portal();
	new at_icc_shutdown_traps();
	new at_icc_start_blood_quickening();
	new at_icc_start_frostwing_gauntlet();
}
