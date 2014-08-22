/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: northrend_beasts
SD%Complete: 95%
SDComment: based on /dev/rsa
SDCategory:
EndScriptData */

// Known bugs:
// Gormok - Not implemented as a vehicle
//        - Snobolled (creature at back)
// Acidmaw & Dreadscale - Under ground appears as bunnys

#include "ScriptPCH.h"
#include "trial_of_the_crusader.h"

enum Yells
{
	//Gormok
	SAY_SNOBOLLED        = -1649000,
	//Acidmaw & Dreadscale
	SAY_SUBMERGE         = -1649010,
	SAY_EMERGE           = -1649011,
	SAY_BERSERK          = -1649012,
	//Icehowl
	SAY_TRAMPLE_STARE    = -1649020,
	SAY_TRAMPLE_FAIL     = -1649021,
	SAY_TRAMPLE_START    = -1649022,
};

enum Model
{
	MODEL_ACIDMAW_STATIONARY     = 29815,
	MODEL_ACIDMAW_MOBILE         = 29816,
	MODEL_DREADSCALE_STATIONARY  = 26935,
	MODEL_DREADSCALE_MOBILE      = 24564,
};

enum Summons
{
	NPC_SNOBOLD_VASSAL   = 34800,
	NPC_SLIME_POOL       = 35176,
	NPC_SNOBOLD_FIRE     = 2000030,
};

enum BossSpells
{
	//Gormok
	SPELL_IMPALE            = 66331,
	SPELL_STAGGERING_STOMP  = 67648,
	SPELL_RISING_ANGER      = 66636,
	//Snobold
	SPELL_BATTER            = 66408,
	SPELL_FIRE_BOMB         = 66313,
	SPELL_FIRE_BOMB_DOT     = 66318,
	SPELL_HEAD_CRACK        = 66407,
	
	//Acidmaw & Dreadscale
	SPELL_SWEEP           = 66794,
	SUMMON_SLIME_POOL       = 66883,
	SPELL_EMERGE          = 66947,
	SPELL_SUBMERGE        = 66948,
	SPELL_ENRAGE            = 68335,
	SPELL_SLIME_POOL_EFFECT = 66882, //In 60s it diameter grows from 10y to 40y (r=r+0.25 per second)
	//Acidmaw
	SPELL_ACID_SPIT         = 66880,
	SPELL_PARALYTIC_SPRAY   = 66901,
	SPELL_ACID_SPEW         = 66818,
	SPELL_PARALYTIC_BITE    = 66824,
	SPELL_PARALYTIC_TOXIN   = 66823,
	//Dreadscale
	SPELL_FIRE_SPIT         = 66796,
	SPELL_MOLTEN_SPEW       = 66821,
	SPELL_BURNING_BITE      = 66879,
	SPELL_BURNING_SPRAY     = 66902,
	SPELL_BURNING_BILE      = 66869,

	//Icehowl
	SPELL_FEROCIOUS_BUTT    = 66770,
	SPELL_MASSIVE_CRASH     = 66683,
	SPELL_WHIRL             = 67345,
	SPELL_ARCTIC_BREATH     = 66689,
	SPELL_TRAMPLE           = 66734,
	SPELL_FROTHING_RAGE     = 66759,
	SPELL_STAGGERED_DAZE    = 66758,
};

enum EventsGormok
{
	EVENT_GORMOK_IMPALE=1,
	EVENT_STAGGERING_STOMP=2,
	EVENT_SNOBOLD_SPAWN=3,
	EVENT_SNOBOLD_BOMB=4,
	EVENT_SNOBOLD_BATTER=5,
	EVENT_SNOBOLD_CRACK=6,
};

enum EventsJormungar
{
	EVENT_JORMUNGAR_BITE=7,
	EVENT_JORMUNGAR_SPEW=8,
	EVENT_JORMUNGAR_SLIMEPOOL=9,
	EVENT_JORMUNGAR_SPRAY=10,
	EVENT_JORMUNGAR_SWEEP=11,
	EVENT_JORMUNGAR_SUBMERGE=12,
	EVENT_JORMUNGAR_EMERGE=13,
};

enum EventsIcehowl
{
	EVENT_ICEHOWL_FEROCIOUS_BUTT=14,
	EVENT_ICEHOWL_ARCTIC_BREATH=15,
	EVENT_ICEHOWL_WHIRL=16,
	EVENT_ICEHOWL_MASSIVE_CRASH=17,
	EVENT_ICEHOWL_TRAMPLE=18,
};

enum PointsJormungar
{
	POINT_STATIONARY = 1,
	POINT_MOBILE = 2,
};

class boss_gormok : public CreatureScript
{
public:
	boss_gormok() : CreatureScript("boss_gormok") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_gormokAI(creature);
	}

	struct boss_gormokAI : public ScriptedAI
	{
		boss_gormokAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{
			m_instance = (InstanceScript*)creature->GetInstanceScript();
		}

		InstanceScript* m_instance;

		SummonList Summons;
		uint32 m_uiSummonCount;
		EventMap _events;

		void Reset()
		{
			_events.Reset();
			_events.ScheduleEvent(EVENT_GORMOK_IMPALE, 10000);
			_events.ScheduleEvent(EVENT_STAGGERING_STOMP, 20000);
			_events.ScheduleEvent(EVENT_SNOBOLD_SPAWN, 20500);

			if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ||
				GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
				m_uiSummonCount = 5;
			else
				m_uiSummonCount = 4;

			Summons.DespawnAll();
		}

		void EnterEvadeMode()
		{
			m_instance->DoUseDoorOrButton(m_instance->GetData64(GO_MAIN_GATE_DOOR));
			ScriptedAI::EnterEvadeMode();
			Summons.DespawnAll();
		}

		void MovementInform(uint32 uiType, uint32 uiId)
		{
			if (uiType != POINT_MOTION_TYPE) return;

			switch (uiId)
			{
				case 0:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
					me->SetReactState(REACT_AGGRESSIVE);
					me->SetInCombatWithZone();
					break;
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			if (m_instance)
				m_instance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_DONE);
			Creature* combat = me->SummonCreature(2000090, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ());
			combat->SetInCombatWithZone();
			me->DespawnOrUnsummon();
		}

		void JustReachedHome()
		{
			if (m_instance)
			{
				m_instance->DoUseDoorOrButton(m_instance->GetData64(GO_MAIN_GATE_DOOR));
				m_instance->SetData(TYPE_NORTHREND_BEASTS, FAIL);
			}
			me->DespawnOrUnsummon();
		}

		void EnterCombat(Unit* /*who*/)
		{
			m_instance->DoUseDoorOrButton(m_instance->GetData64(GO_MAIN_GATE_DOOR));
			me->SetInCombatWithZone();
			m_instance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_IN_PROGRESS);
		}

		
		void JustSummoned(Creature* summon)
		{
			Unit* target=SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true);
				
			summon->AI()->AttackStart(target);
			Summons.Summon(summon);
		}

		void SummonedCreatureDespawn(Creature* summon)
		{
			Summons.Despawn(summon);
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;
			
			_events.Update(uiDiff);
			
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			
			while (uint32 eventId = _events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_GORMOK_IMPALE:
						DoCastVictim(SPELL_IMPALE);
						_events.ScheduleEvent(EVENT_GORMOK_IMPALE, 10000);
					break;
					case EVENT_STAGGERING_STOMP:
						DoCastVictim(SPELL_STAGGERING_STOMP);
						_events.ScheduleEvent(EVENT_STAGGERING_STOMP, 20000);
					break;
					case EVENT_SNOBOLD_SPAWN:
						if (m_uiSummonCount > 0)
						{
							DoScriptText(SAY_SNOBOLLED, me);
							me->SummonCreature(NPC_SNOBOLD_VASSAL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN);
							me->AddAura(SPELL_RISING_ANGER, me);
							--m_uiSummonCount;
						}
						_events.ScheduleEvent(EVENT_SNOBOLD_SPAWN, 20500);
					break;
				}
			}
			
			DoMeleeAttackIfReady();
		}
	};

};

class mob_snobold_vassal : public CreatureScript
{
public:
	mob_snobold_vassal() : CreatureScript("mob_snobold_vassal") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new mob_snobold_vassalAI(creature);
	}

	struct mob_snobold_vassalAI : public ScriptedAI
	{
		mob_snobold_vassalAI(Creature* creature) : ScriptedAI(creature)
		{
			m_instance = (InstanceScript*)creature->GetInstanceScript();
			if (m_instance)
				m_instance->SetData(DATA_SNOBOLD_COUNT, INCREASE);
			creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
			creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
		}

		InstanceScript* m_instance;
		EventMap _events;
		uint64 m_uiTargetGUID;

		void Reset()
		{
			_events.Reset();
			_events.ScheduleEvent(EVENT_SNOBOLD_BOMB, 15000);
			_events.ScheduleEvent(EVENT_SNOBOLD_BATTER, 5000);
			_events.ScheduleEvent(EVENT_SNOBOLD_CRACK, 25000);
			m_uiTargetGUID=0;
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
		}
		
		void EnterCombat(Unit* who)
		{
			m_uiTargetGUID=who->GetGUID();
			me->GetMotionMaster()->MoveJump(who->GetPositionX(), who->GetPositionY(), who->GetPositionZ(), 10.0f, 20.0f);
			me->AddThreat(who, 500000.0f);
		}
		
		void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
		{
			if (pDoneBy->GetGUID()==m_uiTargetGUID)
				uiDamage = 0;
		}
		
		void EnterEvadeMode()
		{
			ScriptedAI::EnterEvadeMode();
		}
		
		void JustDied(Unit* /*killer*/)
		{
			_events.Reset();
			if (m_instance)
				m_instance->SetData(DATA_SNOBOLD_COUNT, DECREASE);
		}

		void UpdateAI(const uint32 uiDiff)
		{
		
			if (!UpdateVictim())
				return;
			
			_events.Update(uiDiff);
			
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			
			if (Unit* target=Unit::GetPlayer(*me, m_uiTargetGUID))
				if (!target->isAlive())
				{
					target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
					m_uiTargetGUID=target->GetGUID();
					me->AddThreat(target, 500000.0f);
					me->GetMotionMaster()->MoveJump(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 15.0f, 15.0f);
				}
			
			while (uint32 eventId = _events.ExecuteEvent()) {
				switch (eventId) {
					case EVENT_SNOBOLD_BOMB:
						if (Unit* target2 = SelectTarget(SELECT_TARGET_RANDOM)) 
						{
							DoCast(target2, SPELL_FIRE_BOMB);
							me->SummonCreature(NPC_SNOBOLD_FIRE, target2->GetPositionX(), target2->GetPositionY(), target2->GetPositionZ(), target2->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
						}
						_events.ScheduleEvent(EVENT_SNOBOLD_BOMB, 15000);
					break;
					case EVENT_SNOBOLD_BATTER:
						if (Unit* target=Unit::GetPlayer(*me, m_uiTargetGUID))
							DoCast(target, SPELL_BATTER);
						_events.ScheduleEvent(EVENT_SNOBOLD_BATTER, 5000);
					break;
					case EVENT_SNOBOLD_CRACK:
						if (Unit* target=Unit::GetPlayer(*me, m_uiTargetGUID))
							DoCast(target, SPELL_HEAD_CRACK);
						_events.ScheduleEvent(EVENT_SNOBOLD_CRACK, 25000);
					break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

};

class mob_snobold_fire : public CreatureScript
{
public:
	mob_snobold_fire() : CreatureScript("mob_snobold_fire") { }
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new mob_snobold_fireAI(creature);
	}
	
	struct mob_snobold_fireAI : public Scripted_NoMovementAI
	{
		mob_snobold_fireAI(Creature* creature) : Scripted_NoMovementAI(creature)
		{
			Reset();
		}
		
		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			me->SetInCombatWithZone();
			DoCast(me, SPELL_FIRE_BOMB_DOT);
		}
		
		void UpdateAI(const uint32 uiDiff)
		{
			UpdateVictim();
		}
	};
};

struct boss_jormungarAI : public ScriptedAI
{
	boss_jormungarAI(Creature* creature) : ScriptedAI(creature)
	{
		instanceScript = creature->GetInstanceScript();
	}

	void Reset()
	{
		_events.Reset();
		enraged = false;
	}
	
	void DoAction(const int32 action)
	{
		switch (stage)
		{
			case 0:
				_events.CancelEvent(EVENT_JORMUNGAR_SUBMERGE);
			break;
			case 3:
				_events.Reset();
				DoCast(me, SPELL_SUBMERGE);
				DoScriptText(SAY_SUBMERGE, me);
				stage = 4;
			break;
			case 1:
			case 2:
			case 6:
				stage = 4;
			break;
		}
	}

	void JustDied(Unit* /*killer*/)
	{
		_events.Reset();
		if (instanceScript)
			if (Creature* otherWorm = Unit::GetCreature(*me, instanceScript->GetData64(otherWormEntry)))
			{
				if (!otherWorm->isAlive())
				{
					instanceScript->SetData(TYPE_NORTHREND_BEASTS, SNAKES_DONE);

					me->DespawnOrUnsummon();
					otherWorm->DespawnOrUnsummon();
				}
				else
				{
					instanceScript->SetData(TYPE_NORTHREND_BEASTS, SNAKES_SPECIAL);
					otherWorm->GetAI()->DoAction(1);
				}
			}
	}

	void JustReachedHome()
	{
		if (instanceScript && instanceScript->GetData(TYPE_NORTHREND_BEASTS) != FAIL)
			instanceScript->SetData(TYPE_NORTHREND_BEASTS, FAIL);
		
		if (Creature* otherWorm = Unit::GetCreature(*me, instanceScript->GetData64(otherWormEntry)))
			otherWorm->DespawnOrUnsummon();
		me->DespawnOrUnsummon();
	}

	void KilledUnit(Unit* who)
	{
		if (who->GetTypeId() == TYPEID_PLAYER)
			if (instanceScript)
				instanceScript->SetData(DATA_TRIBUTE_TO_IMMORTALITY_ELEGIBLE, 0);
	}

	void EnterCombat(Unit* /*who*/)
	{
		me->SetInCombatWithZone();
		if (instanceScript)
			instanceScript->SetData(TYPE_NORTHREND_BEASTS, SNAKES_IN_PROGRESS);
	}
	
	void SpellHitTarget(Unit* target, SpellInfo const* spell)
	{
		if (spell->Id == sSpellMgr->GetSpellIdForDifficulty(spraySpell, me))
			me->CastSpell(target, specialSpell);
	}
	
	void MovementInform(uint32 uiType, uint32 uiId)
	{
		if (uiType != POINT_MOTION_TYPE)
			return;
			
		switch (uiId)
		{
			case POINT_STATIONARY:
				_events.RescheduleEvent(EVENT_JORMUNGAR_EMERGE, 1000);
				stage = 2;
			break;
			case POINT_MOBILE:
				_events.RescheduleEvent(EVENT_JORMUNGAR_EMERGE, 1000);
				stage = 5;
			break;
		}
	
			
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!UpdateVictim()) 
			return;

		_events.Update(uiDiff);
		
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;
		
		if (!me->HasAura(SPELL_ENRAGE))
			enraged = false;
		
		if (instanceScript && instanceScript->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL && !enraged)
		{
			DoCast(SPELL_ENRAGE);
			enraged = true;
			DoScriptText(SAY_BERSERK, me);
		}
		
		switch (stage)
		{
			case 0: // Mobile
				while (uint32 eventId = _events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_JORMUNGAR_BITE:
							DoCastVictim(biteSpell);
							_events.ScheduleEvent(EVENT_JORMUNGAR_BITE, biteCooldown);
						break;
						case EVENT_JORMUNGAR_SPEW:
							DoCastAOE(spewSpell);
							_events.ScheduleEvent(EVENT_JORMUNGAR_SPEW, 21000);
						break;
						case EVENT_JORMUNGAR_SLIMEPOOL:
							DoCast(me, SUMMON_SLIME_POOL);
							_events.ScheduleEvent(EVENT_JORMUNGAR_SLIMEPOOL, 12000);
						break;
						case EVENT_JORMUNGAR_SUBMERGE:
							if (!enraged)
							{
								_events.Reset();
								_events.ScheduleEvent(EVENT_JORMUNGAR_SUBMERGE, 55000);
								stage = 1;
							}
						break;
					}
				}
				DoMeleeAttackIfReady();
			break;
			case 1:
				DoCast(me, SPELL_SUBMERGE);
				DoScriptText(SAY_SUBMERGE, me);
				me->SetVisible(false);
				me->GetMotionMaster()->MovePoint(POINT_STATIONARY, ToCCommonLoc[1].GetPositionX()+urand(0, 60)-30, ToCCommonLoc[1].GetPositionY()+urand(0, 60)-30, ToCCommonLoc[1].GetPositionZ()+1.0f);
				stage = 6;//invalid-set in MovementInform
			break;
			case 2: // Wait til emerge and emerge
				if (_events.ExecuteEvent() == EVENT_JORMUNGAR_EMERGE)
				{
					me->SetVisible(true);
					me->AddAura(42716, me);//Selfroot
					DoScriptText(SAY_EMERGE, me);
					me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
					me->SetDisplayId(modelStationary);
					DoCast(me, SPELL_EMERGE);
					_events.ScheduleEvent(EVENT_JORMUNGAR_SPRAY, sprayFirstCast);
					_events.ScheduleEvent(EVENT_JORMUNGAR_SWEEP, 16000);
					stage = 3;
				}
			break;
			case 3: // Stationary
				while (uint32 eventId = _events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_JORMUNGAR_SPRAY:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
								me->CastSpell(target, spraySpell);
							_events.ScheduleEvent(EVENT_JORMUNGAR_SPRAY, 21000);
						break;
						case EVENT_JORMUNGAR_SWEEP:
							DoCastAOE(SPELL_SWEEP);
							_events.ScheduleEvent(EVENT_JORMUNGAR_SWEEP, 17000);
						break;
						case EVENT_JORMUNGAR_SUBMERGE:
							_events.Reset();
							_events.ScheduleEvent(EVENT_JORMUNGAR_SUBMERGE, 55000);
							DoCast(me, SPELL_SUBMERGE);
							DoScriptText(SAY_SUBMERGE, me);
							stage = 4;
						break;
					}
				}
				DoSpellAttackIfReady(spitSpell);
			break;
			case 4:
				me->SetVisible(false);
				me->RemoveAurasDueToSpell(42716);//Selfroot
				me->GetMotionMaster()->MovePoint(POINT_MOBILE, ToCCommonLoc[1].GetPositionX()+urand(0, 60)-30, ToCCommonLoc[1].GetPositionY()+urand(0, 60)-30, ToCCommonLoc[1].GetPositionZ());
				stage = 7;//invalid-set in MovementInform
			break;
			case 5: // Wait til emerge and emerge
				if (_events.ExecuteEvent() == EVENT_JORMUNGAR_EMERGE)
				{
					me->SetVisible(true);
					me->GetMotionMaster()->MovementExpired();
					me->GetMotionMaster()->MoveChase(me->getVictim());
					DoScriptText(SAY_EMERGE, me);
					me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
					me->SetDisplayId(modelMobile);
					DoCast(me, SPELL_EMERGE);
					stage = 0;
					_events.ScheduleEvent(EVENT_JORMUNGAR_BITE, biteCooldown);
					_events.ScheduleEvent(EVENT_JORMUNGAR_SPEW, 10000);
					_events.ScheduleEvent(EVENT_JORMUNGAR_SLIMEPOOL, 14000);
				}
			break;
		}
	}

	InstanceScript* instanceScript;

	uint32 otherWormEntry;

	uint32 modelStationary;
	uint32 modelMobile;

	uint32 biteCooldown;
	uint32 sprayFirstCast;
	
	uint32 specialSpell;
	uint32 biteSpell;
	uint32 spewSpell;
	uint32 spitSpell;
	uint32 spraySpell;

	uint8 stage;
	bool enraged;
	
	EventMap _events;
};

class boss_acidmaw : public CreatureScript
{
	public:
	boss_acidmaw() : CreatureScript("boss_acidmaw") { }

	struct boss_acidmawAI : public boss_jormungarAI
	{
		boss_acidmawAI(Creature* creature) : boss_jormungarAI(creature) { }

		void Reset()
		{
			boss_jormungarAI::Reset();
			biteCooldown = 25000;
			sprayFirstCast = 9000;
			specialSpell = SPELL_PARALYTIC_TOXIN;
			biteSpell = SPELL_PARALYTIC_BITE;
			spewSpell = SPELL_ACID_SPEW;
			spitSpell = SPELL_ACID_SPIT;
			spraySpell = SPELL_PARALYTIC_SPRAY;
			modelStationary = MODEL_ACIDMAW_STATIONARY;
			modelMobile = MODEL_ACIDMAW_MOBILE;
			otherWormEntry = NPC_DREADSCALE;
			
			_events.ScheduleEvent(EVENT_JORMUNGAR_EMERGE, 10);
			_events.ScheduleEvent(EVENT_JORMUNGAR_SUBMERGE, 45010);
			DoCast(me, SPELL_SUBMERGE);
			stage = 2;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_acidmawAI(creature);
	}
};

class boss_dreadscale : public CreatureScript
{
public:
	boss_dreadscale() : CreatureScript("boss_dreadscale") { }

	struct boss_dreadscaleAI : public boss_jormungarAI
	{
		boss_dreadscaleAI(Creature* creature) : boss_jormungarAI(creature)
		{
			instanceScript = creature->GetInstanceScript();
		}

		InstanceScript* instanceScript;

		void Reset()
		{
			boss_jormungarAI::Reset();
			specialSpell = SPELL_BURNING_BILE;
			biteCooldown = 15000;
			sprayFirstCast = 17000;
			biteSpell = SPELL_BURNING_BITE;
			spewSpell = SPELL_MOLTEN_SPEW;
			spitSpell = SPELL_FIRE_SPIT;
			spraySpell = SPELL_BURNING_SPRAY;
			modelStationary = MODEL_DREADSCALE_STATIONARY;
			modelMobile = MODEL_DREADSCALE_MOBILE;
			otherWormEntry = NPC_ACIDMAW;
			_events.ScheduleEvent(EVENT_JORMUNGAR_BITE, biteCooldown);
			_events.ScheduleEvent(EVENT_JORMUNGAR_SPEW, 21000);
			_events.ScheduleEvent(EVENT_JORMUNGAR_SLIMEPOOL, 12000);
			_events.ScheduleEvent(EVENT_JORMUNGAR_SUBMERGE, 45010);
			stage = 0;
		}

		void MovementInform(uint32 uiType, uint32 uiId)
		{
			boss_jormungarAI::MovementInform(uiType, uiId);
			
			if (uiType != POINT_MOTION_TYPE)
				return;
			
			switch (uiId)
			{
				case 0:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
					me->SetReactState(REACT_AGGRESSIVE);
					me->SetInCombatWithZone();
					if (Creature* otherWorm = Unit::GetCreature(*me, instanceScript->GetData64(otherWormEntry)))
					{
						otherWorm->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
						otherWorm->SetReactState(REACT_AGGRESSIVE);
						otherWorm->SetVisible(true);
						otherWorm->SetInCombatWithZone();
					}
				break;
			}
		}

		void EnterEvadeMode()
		{
			instanceScript->DoUseDoorOrButton(instanceScript->GetData64(GO_MAIN_GATE_DOOR));
			
			boss_jormungarAI::EnterEvadeMode();
		}

		void JustReachedHome()
		{
			instanceScript->DoUseDoorOrButton(instanceScript->GetData64(GO_MAIN_GATE_DOOR));

			boss_jormungarAI::JustReachedHome();
		}
		
		void EnterCombat(Unit* who)
		{
			instanceScript->DoUseDoorOrButton(instanceScript->GetData64(GO_MAIN_GATE_DOOR));
			
			boss_jormungarAI::EnterCombat(who);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_dreadscaleAI(creature);
	}
};

class mob_slime_pool : public CreatureScript
{
public:
	mob_slime_pool() : CreatureScript("mob_slime_pool") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new mob_slime_poolAI(creature);
	}

	struct mob_slime_poolAI : public ScriptedAI
	{
		mob_slime_poolAI(Creature* creature) : ScriptedAI(creature)
		{
		}

		bool casted;
		
		void Reset()
		{
			casted = false;
			me->SetReactState(REACT_PASSIVE);
		}

		void UpdateAI(const uint32 /*uiDiff*/)
		{
			if (!casted)
			{
				casted = true;
				DoCast(me, SPELL_SLIME_POOL_EFFECT);
			}
		}
	};

};

class boss_icehowl : public CreatureScript
{
public:
	boss_icehowl() : CreatureScript("boss_icehowl") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_icehowlAI(creature);
	}

	struct boss_icehowlAI : public ScriptedAI
	{
		boss_icehowlAI(Creature* creature) : ScriptedAI(creature)
		{
			m_instance = (InstanceScript*)creature->GetInstanceScript();
		}

		InstanceScript* m_instance;

		float  m_fTrampleTargetX, m_fTrampleTargetY, m_fTrampleTargetZ;
		uint64 m_uiTrampleTargetGUID;
		bool   m_bMovementStarted;
		bool   m_bMovementFinish;
		bool   m_bTrampleCasted;
		bool jumped;
		uint8  m_uiStage;
		Unit*  target;
		EventMap _events;

		void Reset()
		{
			_events.Reset();
			_events.ScheduleEvent(EVENT_ICEHOWL_FEROCIOUS_BUTT, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
			_events.ScheduleEvent(EVENT_ICEHOWL_ARCTIC_BREATH, urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS));
			_events.ScheduleEvent(EVENT_ICEHOWL_WHIRL, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
			_events.ScheduleEvent(EVENT_ICEHOWL_MASSIVE_CRASH, 25*IN_MILLISECONDS);
			m_bMovementStarted = false;
			m_bMovementFinish = false;
			m_bTrampleCasted = false;
			jumped = false;
			m_uiTrampleTargetGUID = 0;
			m_fTrampleTargetX = 0;
			m_fTrampleTargetY = 0;
			m_fTrampleTargetZ = 0;
			m_uiStage = 0;
		}

		void JustDied(Unit* /*killer*/)
		{
			if (m_instance)
				m_instance->SetData(TYPE_NORTHREND_BEASTS, ICEHOWL_DONE);
		}

		void MovementInform(uint32 uiType, uint32 uiId)
		{
			if (uiType != POINT_MOTION_TYPE && uiType != EFFECT_MOTION_TYPE)
				return;

			switch (uiId)
			{
				case 0:
					if (me->GetDistance2d(ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY()) < 6.0f)
					{
						me->GetMotionMaster()->MovementExpired();
						me->GetMotionMaster()->MoveIdle();
						me->AddAura(42716, me);//Selfroot
						m_uiStage = 1;
					}
					else
					{
						me->RemoveAurasDueToSpell(42716);//Selfroot
						// Landed from Hop backwards (start trample)
						if (Unit::GetPlayer(*me, m_uiTrampleTargetGUID))
							m_uiStage = 4;
						else 
							m_uiStage = 6;
					}
					break;
				case 1: // Finish trample
					m_bMovementFinish = true;
					break;
				case 2:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
					me->SetReactState(REACT_AGGRESSIVE);
					me->SetInCombatWithZone();
					break;
			}
		}

		void EnterEvadeMode()
		{
			m_instance->DoUseDoorOrButton(m_instance->GetData64(GO_MAIN_GATE_DOOR));
			ScriptedAI::EnterEvadeMode();
		}

		void JustReachedHome()
		{
			if (m_instance)
			{
				m_instance->DoUseDoorOrButton(m_instance->GetData64(GO_MAIN_GATE_DOOR));
				m_instance->SetData(TYPE_NORTHREND_BEASTS, FAIL);
			}
			me->DespawnOrUnsummon();
		}

		void KilledUnit(Unit* who)
		{
			if (who->GetTypeId() == TYPEID_PLAYER)
				if (m_instance)
					m_instance->SetData(DATA_TRIBUTE_TO_IMMORTALITY_ELEGIBLE, 0);
		}

		void EnterCombat(Unit* /*who*/)
		{
			m_instance->SetData(TYPE_NORTHREND_BEASTS, ICEHOWL_IN_PROGRESS);
			m_instance->DoUseDoorOrButton(m_instance->GetData64(GO_MAIN_GATE_DOOR));
			
			me->SetInCombatWithZone();
		}

		void SpellHitTarget(Unit* target, const SpellInfo* spell)
		{
			if (spell->Id == SPELL_TRAMPLE && target->GetTypeId() == TYPEID_PLAYER)
				if (!m_bTrampleCasted)
				{
					DoCast(me, SPELL_FROTHING_RAGE, true);
					m_bTrampleCasted = true;
				}
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;
			
			_events.Update(uiDiff);
			
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			
			
			switch (m_uiStage)
			{
				case 0:
					while (uint32 eventId = _events.ExecuteEvent())
					{
						switch (eventId)
						{
							case EVENT_ICEHOWL_FEROCIOUS_BUTT:
								DoCastVictim(SPELL_FEROCIOUS_BUTT);
								_events.ScheduleEvent(EVENT_ICEHOWL_FEROCIOUS_BUTT, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
							break;
							case EVENT_ICEHOWL_ARCTIC_BREATH:
								if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
									DoCast(target, SPELL_ARCTIC_BREATH);
								_events.ScheduleEvent(EVENT_ICEHOWL_ARCTIC_BREATH, urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS));
							break;
							case EVENT_ICEHOWL_WHIRL:
								DoCastAOE(SPELL_WHIRL);
								_events.ScheduleEvent(EVENT_ICEHOWL_WHIRL, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
							break;
							case EVENT_ICEHOWL_MASSIVE_CRASH:
								me->GetMotionMaster()->MoveJump(ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 10.0f, 20.0f); // 1: Middle of the room
								m_uiStage = 7; //Invalid (Do nothing more than move)
								_events.Reset();
								_events.ScheduleEvent(EVENT_ICEHOWL_MASSIVE_CRASH, 55*IN_MILLISECONDS);
							break;
						}
					}
					DoMeleeAttackIfReady();
					break;
				case 1:
					DoCastAOE(SPELL_MASSIVE_CRASH);
					me->AddAura(42716, me);//Selfroot
					m_uiStage = 2;
					break;
				case 2:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
					{
						m_uiTrampleTargetGUID = target->GetGUID();
						me->SetTarget(m_uiTrampleTargetGUID);
						DoScriptText(SAY_TRAMPLE_STARE, me, target);
						m_bTrampleCasted = false;
						SetCombatMovement(false);
						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						_events.ScheduleEvent(EVENT_ICEHOWL_TRAMPLE, 5000);
						m_uiStage = 3;
					} else m_uiStage = 6;
					break;
				case 3:
					if (uint32 eventId = _events.ExecuteEvent())
						if (eventId==EVENT_ICEHOWL_TRAMPLE)
						{
							if (Unit* target = Unit::GetPlayer(*me, m_uiTrampleTargetGUID))
							{
								m_bTrampleCasted = false;
								m_bMovementStarted = true;
								m_fTrampleTargetX = target->GetPositionX();
								m_fTrampleTargetY = target->GetPositionY();
								m_fTrampleTargetZ = target->GetPositionZ();
								me->GetMotionMaster()->MoveJump(
									2*ToCCommonLoc[1].GetPositionX()-m_fTrampleTargetX,
									2*ToCCommonLoc[1].GetPositionY()-m_fTrampleTargetY,
									me->GetPositionZ(),
									14.0f, 20.0f); // 2: Hop Backwards
								m_uiStage = 7; //Invalid (Do nothing more than move)
							} else m_uiStage = 6;
							me->RemoveAurasDueToSpell(42716);//Selfroot
						}
					break;
				case 4:
					DoScriptText(SAY_TRAMPLE_START, me);
					me->GetMotionMaster()->MoveCharge(m_fTrampleTargetX, m_fTrampleTargetY, m_fTrampleTargetZ+2, 42, 1);
					me->SetTarget(me->getVictim()->GetGUID());
					m_uiStage = 5;
					_events.ScheduleEvent(EVENT_ICEHOWL_TRAMPLE, 150);
					break;
				case 5:
					if (m_bMovementFinish)
					{
						DoCastAOE(SPELL_TRAMPLE);
						m_bMovementFinish = false;
						m_uiStage = 6;
						return;
					}
					if (uint32 eventId = _events.ExecuteEvent())
						if (eventId==EVENT_ICEHOWL_TRAMPLE)
						{
							DoCastAOE(SPELL_TRAMPLE);
							_events.ScheduleEvent(EVENT_ICEHOWL_TRAMPLE, 150);
						}
					break;
				case 6:
					me->RemoveAurasDueToSpell(42716);//Selfroot
					if (!m_bTrampleCasted)
					{
						DoCast(me, SPELL_STAGGERED_DAZE);
						DoScriptText(SAY_TRAMPLE_FAIL, me);
					}
					m_bMovementStarted = false;
					me->GetMotionMaster()->MovementExpired();
					me->GetMotionMaster()->MoveChase(me->getVictim());
					SetCombatMovement(true);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					m_uiStage = 0;
					_events.ScheduleEvent(EVENT_ICEHOWL_FEROCIOUS_BUTT, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
					_events.ScheduleEvent(EVENT_ICEHOWL_ARCTIC_BREATH, urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS));
					_events.ScheduleEvent(EVENT_ICEHOWL_WHIRL, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
					break;
			}
		}
	};

};

class npc_combat_holder : public CreatureScript
{
public:
	npc_combat_holder() : CreatureScript("npc_combat_holder") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_combat_holderAI(creature);
	}

	struct npc_combat_holderAI : public ScriptedAI
	{
		npc_combat_holderAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
			timer = 0;
		}
		
		InstanceScript* instance;
		uint32 timer;

		void UpdateAI(const uint32 diff)
		{
			if (instance->GetData(TYPE_BEASTS) != IN_PROGRESS && !timer)
				timer = 1000;
			
			if (timer)
			{
				if (timer < diff)
				{
					if (instance->GetData(TYPE_BEASTS) != IN_PROGRESS)
						me->DespawnOrUnsummon();
					timer = 0;
				}
				else
					timer -= diff;
			}
		}
	};

};

void AddSC_boss_northrend_beasts()
{
	new boss_gormok();
	new mob_snobold_vassal();
	new mob_snobold_fire();
	new boss_acidmaw();
	new boss_dreadscale();
	new mob_slime_pool();
	new boss_icehowl();
	new npc_combat_holder();
}
