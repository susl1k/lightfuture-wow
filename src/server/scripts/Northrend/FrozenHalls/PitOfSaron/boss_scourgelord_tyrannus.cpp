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

#include "ScriptPCH.h"
#include "pit_of_saron.h"
#include "Vehicle.h"

enum Yells
{
    SAY_AMBUSH_1                    = -1658050,
    SAY_AMBUSH_2                    = -1658051,
    SAY_GAUNTLET_START              = -1658052,
    SAY_TYRANNUS_INTRO_1            = -1658053,
    SAY_GORKUN_INTRO_2              = -1658054,
    SAY_TYRANNUS_INTRO_3            = -1658055,

    SAY_AGGRO                       = -1658056,
    SAY_SLAY_1                      = -1658057,
    SAY_SLAY_2                      = -1658058,
    SAY_DEATH                       = -1658059,
    SAY_MARK_RIMEFANG_1             = -1658060,
    SAY_MARK_RIMEFANG_2             = -1658061,
    SAY_DARK_MIGHT_1                = -1658062,
    SAY_DARK_MIGHT_2                = -1658063,

    SAY_GORKUN_OUTRO_1              = -1658064,
    SAY_GORKUN_OUTRO_2              = -1658065,
    SAY_JAYNA_OUTRO_3               = -1658066,
    SAY_SYLVANAS_OUTRO_3            = -1658067,
    SAY_JAYNA_OUTRO_4               = -1658068,
    SAY_SYLVANAS_OUTRO_4            = -1658069,
    SAY_JAYNA_OUTRO_5               = -1658070,
};

enum Spells
{
    SPELL_OVERLORD_BRAND            = 69172,
    SPELL_OVERLORD_BRAND_HEAL       = 69190,
    SPELL_OVERLORD_BRAND_DAMAGE     = 69189,
    SPELL_FORCEFUL_SMASH            = 69155,
    SPELL_UNHOLY_POWER              = 69167,
    SPELL_MARK_OF_RIMEFANG          = 69275,
    SPELL_HOARFROST                 = 69246,

    SPELL_ICY_BLAST                 = 69232,
    SPELL_ICY_BLAST_AURA            = 69238,

    SPELL_EJECT_ALL_PASSENGERS      = 50630,
    SPELL_FULL_HEAL                 = 43979,
};

enum Events
{
    EVENT_OVERLORD_BRAND    = 1,
    EVENT_FORCEFUL_SMASH    = 2,
    EVENT_UNHOLY_POWER      = 3,
    EVENT_MARK_OF_RIMEFANG  = 4,

    // Rimefang
    EVENT_MOVE_NEXT         = 5,
    EVENT_HOARFROST         = 6,
    EVENT_ICY_BLAST         = 7,

    EVENT_INTRO_1           = 8,
    EVENT_INTRO_2           = 9,
    EVENT_INTRO_3           = 10,
    EVENT_COMBAT_START      = 11,
};

enum Phases
{
    PHASE_NONE      = 0,
    PHASE_INTRO     = 1,
    PHASE_COMBAT    = 2,
    PHASE_OUTRO     = 3,
};

enum Actions
{
    ACTION_START_INTRO      = 1,
    ACTION_START_RIMEFANG   = 2,
    ACTION_START_OUTRO      = 3,
    ACTION_END_COMBAT       = 4,
};

#define GUID_HOARFROST 1

static const Position rimefangPos[10] =
{
    {1017.299f, 168.9740f, 642.9259f, 0.000000f},
    {1047.868f, 126.4931f, 665.0453f, 0.000000f},
    {1069.828f, 138.3837f, 665.0453f, 0.000000f},
    {1063.042f, 164.5174f, 665.0453f, 0.000000f},
    {1031.158f, 195.1441f, 665.0453f, 0.000000f},
    {1019.087f, 197.8038f, 665.0453f, 0.000000f},
    {967.6233f, 168.9670f, 665.0453f, 0.000000f},
    {969.1198f, 140.4722f, 665.0453f, 0.000000f},
    {986.7153f, 141.6424f, 665.0453f, 0.000000f},
    {1012.601f, 142.4965f, 665.0453f, 0.000000f},
};

static const Position miscPos = {1018.376f, 167.2495f, 628.2811f, 0.000000f};   //tyrannus combat start position

Position LeaderSpawn = {1066.0f, 89.0f, 631.6f, 2.0f};
Position LeaderMove = {1055.0f, 112.0f, 628.2f, 2.2f};

Position SindragosaSpawn = {921.0f, 178.0f, 655.9f, 6.0f};
Position SindragosaMove1 = {925.0f, 178.0f, 661.3f, 6.0f};
Position SindragosaMove2 = {914.0f, 220.0f, 670.0f, 1.8f};
Position SindragosaTarget = {983.0f, 169.0f, 628.2f, 0.0f};

Position SlaveSpawn = {1066.0f, 89.0f, 631.6f, 2.0f};
Position SlaveMove1 = {1058.0f, 113.0f, 628.4f, 2.2f};
Position SlaveMove2 = {1007.0f, 163.0f, 628.2f, 2.5f};

Position PlayerTeleport = {1061.0f, 104.0f, 629.92f, 2.0f};

Position AddSpawn [3] = 
{
	{1059.538208f, 93.969360f ,630.644775f, 0.0f},
	{1064.258789f, 96.252075f, 631.004150f, 0.0f},
	{1070.205078f, 99.942986f, 630.942993f, 0.0f},
};

class boss_tyrannus : public CreatureScript
{
    public:
        boss_tyrannus() : CreatureScript("boss_tyrannus") { }

        struct boss_tyrannusAI : public BossAI
        {
			boss_tyrannusAI(Creature* creature) : BossAI(creature, DATA_TYRANNUS) { spawnTimer = 0; }

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(PoSScriptName))
                    me->IsAIEnabled = false;
                else if (instance->GetBossState(DATA_TYRANNUS) != DONE)
                    Reset();
                else
                    me->DespawnOrUnsummon();
            }

            void Reset()
            {
                events.Reset();
                events.SetPhase(PHASE_NONE);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                instance->SetBossState(DATA_TYRANNUS, NOT_STARTED);
            }

            Creature* GetRimefang()
            {
                return ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_RIMEFANG));
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoScriptText(SAY_AGGRO, me);
				spawnTimer = 1;
				enrageTimer = 300000;
            }

            void AttackStart(Unit* victim)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (victim && me->Attack(victim, true) && !(events.GetPhaseMask() & (1 << PHASE_INTRO)))
                    me->GetMotionMaster()->MoveChase(victim);
            }

            void EnterEvadeMode()
            {
                instance->SetBossState(DATA_TYRANNUS, FAIL);
                if (Creature* rimefang = GetRimefang())
                    rimefang->AI()->EnterEvadeMode();
				if (Creature* slave = Unit::GetCreature(*me, instance->GetData64(DATA_SLAVE_LEADER)))
					slave->AI()->DoAction(ACTION_DESPAWN);

				summons.DespawnAll();

                me->DespawnOrUnsummon(10);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoScriptText(SAY_DEATH, me);
                instance->SetBossState(DATA_TYRANNUS, DONE);

				summons.DespawnAll();

                // Prevent corpse despawning
                if (TempSummon* summ = me->ToTempSummon())
                    summ->SetTempSummonType(TEMPSUMMON_DEAD_DESPAWN);

                // Stop combat for Rimefang
                if (Creature* rimefang = GetRimefang())
                    rimefang->AI()->DoAction(ACTION_END_COMBAT);
				
				if (Creature* slave = Unit::GetCreature(*me, instance->GetData64(DATA_SLAVE_LEADER)))
					slave->AI()->DoAction(ACTION_END_COMBAT);
				
				me->SummonCreature(NPC_SYLVANAS_PART2, LeaderSpawn)->GetMotionMaster()->MovePoint(0, LeaderMove);
            }

            void DoAction(const int32 actionId)
            {
                if (actionId == ACTION_START_INTRO)
                {
                    DoScriptText(SAY_TYRANNUS_INTRO_1, me);
                    events.SetPhase(PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_1, 14000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_2, 22000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_3, 34000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_COMBAT_START, 36000, 0, PHASE_INTRO);
                    instance->SetBossState(DATA_TYRANNUS, IN_PROGRESS);
                }
            }

			void JustSummoned(Creature* summon)
			{
				summons.Summon(summon);

				Unit* leader = Unit::GetUnit(*me, instance->GetData64(DATA_SLAVE_LEADER));
				if (leader && leader->isAlive())
				{
					summon->SetInCombatWith(leader);
					leader->SetInCombatWith(summon);
					summon->AddThreat(leader, 10.0f);
					leader->AddThreat(summon, 10.0f);
					summon->AI()->AttackStart(leader);
				}
				else
					summon->AI()->DoZoneInCombat();
			}

            void UpdateAI(const uint32 diff)
            {
				if (enrageTimer)
				{
					if (enrageTimer <= diff)
						enrageTimer = 0;
					else
						enrageTimer -= diff;
				}

				if (spawnTimer)
				{
					if (spawnTimer <= diff)
					{
						switch (urand(0,3))
						{
							case 0:
								me->SummonCreature(NPC_WRATHBONE_SKELETON, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								me->SummonCreature(NPC_WRATHBONE_SKELETON, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);

								me->SummonCreature(NPC_WRATHBONE_SORCERER, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								break;
							case 1:
								me->SummonCreature(NPC_WRATHBONE_REAVER, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								me->SummonCreature(NPC_WRATHBONE_REAVER, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);

								me->SummonCreature(NPC_WRATHBONE_SORCERER, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								break;
							case 2:
								me->SummonCreature(NPC_WRATHBONE_REAVER, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								me->SummonCreature(NPC_WRATHBONE_REAVER, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);

								me->SummonCreature(NPC_FALLEN_WARRIOR, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								break;
							case 3:
								me->SummonCreature(NPC_WRATHBONE_SORCERER, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								me->SummonCreature(NPC_FALLEN_WARRIOR, AddSpawn[urand(0,2)], TEMPSUMMON_CORPSE_DESPAWN);
								break;
						}

						if (!enrageTimer)
							spawnTimer = 2500;
						else
							spawnTimer = IsHeroic() ? 12500 : 7500;
					}
					else
						spawnTimer -= diff;
				}

                if (!UpdateVictim() && !(events.GetPhaseMask() & (1 << PHASE_INTRO)))
                    return;

				if (me->getVictim() && !me->getVictim()->ToPlayer() && !me->getVictim()->isPet())
				{
					EnterEvadeMode();
					return;
				}

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_1:
                            //DoScriptText(SAY_GORKUN_INTRO_2, pGorkunOrVictus);
                            break;
                        case EVENT_INTRO_2:
                            DoScriptText(SAY_TYRANNUS_INTRO_3, me);
                            break;
                        case EVENT_INTRO_3:
                            me->ExitVehicle();
                            me->GetMotionMaster()->MovePoint(0, miscPos);
                            break;
                        case EVENT_COMBAT_START:
                            if (Creature* rimefang = me->GetCreature(*me, instance->GetData64(DATA_RIMEFANG)))
                                rimefang->AI()->DoAction(ACTION_START_RIMEFANG);    //set rimefang also infight
                            events.SetPhase(PHASE_COMBAT);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DoCast(me, SPELL_FULL_HEAL);
                            DoZoneInCombat();
                            events.ScheduleEvent(EVENT_OVERLORD_BRAND, urand(5000, 7000));
                            events.ScheduleEvent(EVENT_FORCEFUL_SMASH, urand(14000, 16000));
                            events.ScheduleEvent(EVENT_MARK_OF_RIMEFANG, urand(25000, 27000));
                            break;
                        case EVENT_OVERLORD_BRAND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_OVERLORD_BRAND);
                            events.ScheduleEvent(EVENT_OVERLORD_BRAND, urand(11000, 12000));
                            break;
                        case EVENT_FORCEFUL_SMASH:
                            DoCastVictim(SPELL_FORCEFUL_SMASH);
                            events.ScheduleEvent(EVENT_UNHOLY_POWER, 100);
                            break;
                        case EVENT_UNHOLY_POWER:
                            DoScriptText(SAY_DARK_MIGHT_1, me);
                            DoScriptText(SAY_DARK_MIGHT_2, me);
                            DoCast(me, SPELL_UNHOLY_POWER);
                            events.ScheduleEvent(EVENT_FORCEFUL_SMASH, urand(40000, 48000));
                            break;
                        case EVENT_MARK_OF_RIMEFANG:
                            DoScriptText(SAY_MARK_RIMEFANG_1, me);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            {
                                DoScriptText(SAY_MARK_RIMEFANG_2, me, target);
                                DoCast(target, SPELL_MARK_OF_RIMEFANG);
                            }
                            events.ScheduleEvent(EVENT_MARK_OF_RIMEFANG, urand(24000, 26000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

			uint32 spawnTimer;
			uint32 enrageTimer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_tyrannusAI(creature);
        }
};

Position RimefangDespawn = {1077.0f, 136.0f, 678.0f, 0.0f};

class boss_rimefang : public CreatureScript
{
    public:
        boss_rimefang() : CreatureScript("boss_rimefang") { }

        struct boss_rimefangAI : public ScriptedAI
        {
            boss_rimefangAI(Creature* creature) : ScriptedAI(creature), _vehicle(creature->GetVehicleKit())
            {
                ASSERT(_vehicle);
            }

            void Reset()
            {
                _events.Reset();
                _events.SetPhase(PHASE_NONE);
                _currentWaypoint = 1;
                _hoarfrostTargetGUID = 0;
                me->SetCanFly(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void JustReachedHome()
            {
                _vehicle->InstallAllAccessories(false);
            }

			void MovementInform(uint32 type, uint32 id)
			{
				if (id == POINT_DESPAWN)
					me->DespawnOrUnsummon(100);
				if (id == 0)
				{
					_events.ScheduleEvent(EVENT_MOVE_NEXT, 100, 0, PHASE_COMBAT);
					_currentWaypoint++;
                    if (_currentWaypoint >= 10 || _currentWaypoint == 0)
                        _currentWaypoint = 1;
				}
			}

            void DoAction(const int32 actionId)
            {
                if (actionId == ACTION_START_RIMEFANG)
                {
                    _events.SetPhase(PHASE_COMBAT);
                    DoZoneInCombat();
                    _events.ScheduleEvent(EVENT_MOVE_NEXT, 500, 0, PHASE_COMBAT);
                    _events.ScheduleEvent(EVENT_ICY_BLAST, 15000, 0, PHASE_COMBAT);
                }
                else if (actionId == ACTION_END_COMBAT)
				{
					_events.Reset();
					me->GetMotionMaster()->MovePoint(POINT_DESPAWN, RimefangDespawn);
				}
            }

            void SetGUID(uint64 guid, int32 type)
            {
                if (type == GUID_HOARFROST)
                {
                    _hoarfrostTargetGUID = guid;
                    _events.ScheduleEvent(EVENT_HOARFROST, 1000);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim() && !(_events.GetPhaseMask() & (1 << PHASE_COMBAT)))
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_NEXT:
                            me->GetMotionMaster()->MovePoint(0, rimefangPos[_currentWaypoint]);
                            break;
                        case EVENT_ICY_BLAST:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_ICY_BLAST);
                            _events.ScheduleEvent(EVENT_ICY_BLAST, 15000, 0, PHASE_COMBAT);
                            break;
                        case EVENT_HOARFROST:
                            if (Unit* target = me->GetUnit(*me, _hoarfrostTargetGUID))
                            {
                                DoCast(target, SPELL_HOARFROST);
                                _hoarfrostTargetGUID = 0;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            Vehicle* _vehicle;
            uint64 _hoarfrostTargetGUID;
            EventMap _events;
            uint8 _currentWaypoint;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_rimefangAI(creature);
        }
};

class player_overlord_brandAI : public PlayerAI
{
    public:
        player_overlord_brandAI(Player* player) : PlayerAI(player)
        {
            tyrannus = NULL;
        }

        void SetGUID(uint64 guid, int32 /*type*/)
        {
            tyrannus = ObjectAccessor::GetCreature(*me, guid);
            me->IsAIEnabled = tyrannus != NULL;
        }

        void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType /*damageType*/)
        {
            if (tyrannus->getVictim())
                me->CastCustomSpell(SPELL_OVERLORD_BRAND_DAMAGE, SPELLVALUE_BASE_POINT0, damage, tyrannus->getVictim(), true, NULL, NULL, tyrannus->GetGUID());
        }

        void HealDone(Unit* /*target*/, uint32& addHealth)
        {
            me->CastCustomSpell(SPELL_OVERLORD_BRAND_HEAL, SPELLVALUE_BASE_POINT0, int32(addHealth*5.5f), tyrannus, true, NULL, NULL, tyrannus->GetGUID());
        }

        void UpdateAI(const uint32 /*diff*/) { }

    private:
        Creature* tyrannus;
};

class spell_tyrannus_overlord_brand : public SpellScriptLoader
{
    public:
        spell_tyrannus_overlord_brand() : SpellScriptLoader("spell_tyrannus_overlord_brand") { }

        class spell_tyrannus_overlord_brand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tyrannus_overlord_brand_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
                    return;

                oldAI = GetTarget()->GetAI();
                oldAIState = GetTarget()->IsAIEnabled;
                GetTarget()->SetAI(new player_overlord_brandAI(GetTarget()->ToPlayer()));
                GetTarget()->GetAI()->SetGUID(GetCasterGUID());
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
                    return;

                delete GetTarget()->GetAI();
                GetTarget()->SetAI(oldAI);
                GetTarget()->IsAIEnabled = oldAIState;
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tyrannus_overlord_brand_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tyrannus_overlord_brand_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

            UnitAI* oldAI;
            bool oldAIState;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_tyrannus_overlord_brand_AuraScript();
        }
};

class spell_tyrannus_mark_of_rimefang : public SpellScriptLoader
{
    public:
        spell_tyrannus_mark_of_rimefang() : SpellScriptLoader("spell_tyrannus_mark_of_rimefang") { }

        class spell_tyrannus_mark_of_rimefang_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tyrannus_mark_of_rimefang_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                if (!caster || caster->GetTypeId() != TYPEID_UNIT)
                    return;

                if (InstanceScript* instance = caster->GetInstanceScript())
                    if (Creature* rimefang = ObjectAccessor::GetCreature(*caster, instance->GetData64(DATA_RIMEFANG)))
                        rimefang->AI()->SetGUID(GetTarget()->GetGUID(), GUID_HOARFROST);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tyrannus_mark_of_rimefang_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_tyrannus_mark_of_rimefang_AuraScript();
        }
};

class npc_collapsing_icicle : public CreatureScript
{
public:
	npc_collapsing_icicle() : CreatureScript("npc_collapsing_icicle") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_collapsing_icicleAI (pCreature);
	}

	struct npc_collapsing_icicleAI : public Scripted_NoMovementAI
	{
		npc_collapsing_icicleAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
			me->SetReactState(REACT_PASSIVE);
			me->SetDisplayId(28470);
		}

		uint32 IcicleTimer;

		void Reset()
		{
			IcicleTimer = 2000;
		}
		
		void DamageDealt(Unit* who, uint32&, DamageEffectType)
		{
			if (Creature* dummy = Unit::GetCreature(*me, me->GetInstanceScript()->GetData64(DATA_GAUNTLET_DUMMY_1)))
				dummy->AI()->DoAction(ACTION_FAIL_LOOK);
		}

		void UpdateAI(const uint32 diff)
		{
			if (IcicleTimer <= diff)
			{
				DoCast(me, SPELL_ICICLE_DAMAGE);
				DoCast(me, SPELL_ICICLE_FALL);
				IcicleTimer = 10000;
				me->DespawnOrUnsummon(4000);
			}
			else IcicleTimer -= diff;
		}
	};

};

class npc_gauntlet_dummy : public CreatureScript
{
public:
	npc_gauntlet_dummy() : CreatureScript("npc_gauntlet_dummy") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_gauntlet_dummyAI (pCreature);
	}

	struct npc_gauntlet_dummyAI : public Scripted_NoMovementAI
	{
		npc_gauntlet_dummyAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
		{
			dont_look = IsHeroic();
			first = true;
			gauntletActive = false;
		}

		void DoAction(const int32 actionId)
            {
				if (actionId == ACTION_START_GAUNTLET)
                {
					if (!gauntletActive)
					{
						gauntletActive = true;
						if (first)
						{
							me->GetMap()->LoadGrid(1049.0f, -100.0f);
							me->GetMap()->LoadGrid(1053.8f, -26.2f);
							me->GetMap()->LoadGrid(1071.2f, -28.0f);
							me->GetMap()->LoadGrid(1074.8f, 9.8f);
							me->GetMap()->LoadGrid(1064.8f, 9.8f);
							me->GetCreatureListWithEntryInGrid(gauntletList, NPC_GAUNTLET_TRIGGER, me->GetEntry() == NPC_GAUNTLET_DUMMY_1 ? 75.0f : 100.0f);
							first = false;
						}
						gauntletEvents.Reset();
						gauntletEvents.ScheduleEvent(EVENT_GAUNTLET_ICICLE, 100);
					}
                }
				else if (actionId == ACTION_START_INTRO)
				{
					if (me->GetEntry() == NPC_GAUNTLET_DUMMY_1)
						if (dont_look)
							if (me->GetInstanceScript())
								me->GetInstanceScript()->DoCompleteAchievement(ACHIEVEMENT_DONT_LOOK);
					gauntletActive = false;
				}
				else if (actionId == ACTION_FAIL_LOOK)
				{
					dont_look = false;
				}
            }

		void UpdateAI(const uint32 diff)
		{
			if (gauntletActive)
			{
				gauntletEvents.Update(diff);

				if (gauntletEvents.ExecuteEvent() == EVENT_GAUNTLET_ICICLE)
				{
					for (uint8 i = 0; i < gauntletList.size()*9/110; i++)
					{
						std::list<Creature*>::iterator ite = gauntletList.begin();
						std::advance(ite, urand(0, gauntletList.size()-1));
						Creature* trigger = *ite;
						if (trigger)
							trigger->CastSpell(trigger, SPELL_ICICLE_SPAWN, true);
					}

					gauntletEvents.ScheduleEvent(EVENT_GAUNTLET_ICICLE, 3600);
				}
			}
		}

		bool dont_look;
		bool first;
		bool gauntletActive;
		EventMap gauntletEvents;
		std::list<Creature*> gauntletList;
	};

};

class at_tyrannus_event_starter : public AreaTriggerScript
{
    public:
        at_tyrannus_event_starter() : AreaTriggerScript("at_tyrannus_event_starter") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
        {
            InstanceScript* instance = player->GetInstanceScript();
            if (player->isGameMaster() || !instance)
                return false;

            if (instance->GetBossState(DATA_TYRANNUS) != IN_PROGRESS && instance->GetBossState(DATA_TYRANNUS) != DONE)
			{
                if (Creature* tyrannus = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_TYRANNUS)))
                    tyrannus->AI()->DoAction(ACTION_START_INTRO);
				if (Creature* dummy = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_GAUNTLET_DUMMY_1)))
                    dummy->AI()->DoAction(ACTION_START_INTRO);
				player->GetMap()->LoadGrid(1075.0f, 13.0f);
				if (Creature* dummy = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_GAUNTLET_DUMMY_2)))
                    dummy->AI()->DoAction(ACTION_START_INTRO);
				if (Creature* leader = player->GetMap()->SummonCreature(NPC_GORKUN_IRONSKULL_1, SlaveSpawn))
					leader->AI()->DoAction(ACTION_START_INTRO);

				return true;
			}

            return false;
        }
};

class at_gauntlet_event_starter : public AreaTriggerScript
{
    public:
        at_gauntlet_event_starter() : AreaTriggerScript("at_gauntlet_event_starter") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
        {
            InstanceScript* instance = player->GetInstanceScript();
            if (player->isGameMaster() || !instance)
                return false;
			
            if (Creature* dummy = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_GAUNTLET_DUMMY_1)))
			{
				if (!CAST_AI(npc_gauntlet_dummy::npc_gauntlet_dummyAI, dummy->AI())->gauntletActive)
				{
					dummy->AI()->DoAction(ACTION_START_GAUNTLET);
					if (Creature* dummy = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_GAUNTLET_DUMMY_2)))
						dummy->AI()->DoAction(ACTION_START_GAUNTLET);
					return true;
				}
			}

            return false;
        }
};

class npc_pos_jaina_sylvanas_outro : public CreatureScript
{
    public:
        npc_pos_jaina_sylvanas_outro() : CreatureScript("npc_pos_jaina_sylvanas_outro") { }

		struct npc_pos_jaina_sylvanas_outroAI: public ScriptedAI
        {
			npc_pos_jaina_sylvanas_outroAI(Creature* creature) : ScriptedAI(creature)
			{
				portTimer = 0;
				talkTimer = 0;
				me->GetMap()->LoadGrid(933.58f, 165.9f);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
			}

			uint32 portTimer;
			uint32 talkTimer;

			void UpdateAI(const uint32 diff)
			{
				if (portTimer)
				{
					if (portTimer <= diff)
					{
						InstanceScript* instance = me->GetInstanceScript();

						if (!instance)
						{
							portTimer = 0;
							return;
						}

						if (instance->GetData64(DATA_END_DOOR))
							instance->DoUseDoorOrButton(instance->GetData64(DATA_END_DOOR));

						Talk(0);

						Map::PlayerList const &players = me->GetMap()->GetPlayers();
						for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
							if (Player* player = itr->getSource())
								if (player->isAlive())
								{
									player->CastSpell(player, SPELL_TELEPORT_VISUAL, true);
									player->NearTeleportTo(PlayerTeleport.GetPositionX(), PlayerTeleport.GetPositionY(), PlayerTeleport.GetPositionZ(), PlayerTeleport.GetOrientation());
								}

						portTimer = 0;
						talkTimer = 7500;
					}
					else
						portTimer -= diff;
				}

				if (talkTimer)
				{
					if (talkTimer <= diff)
					{
						Talk(1);

						talkTimer = 0;
					}
					else
						talkTimer -= diff;
				}
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_jaina_sylvanas_outroAI(creature);
        }
};

class spell_icy_blast : public SpellScriptLoader
{
    public:
        spell_icy_blast() : SpellScriptLoader("spell_icy_blast") { }

        class spell_icy_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icy_blast_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetTypeId() != TYPEID_UNIT)
                    return;

				if (GetExplTargetDest())
					if (Creature* icy_blast_trigger = GetCaster()->SummonCreature(NPC_ICY_BLAST, *GetExplTargetDest(), TEMPSUMMON_TIMED_DESPAWN, 60000))
						icy_blast_trigger->CastSpell(icy_blast_trigger, SPELL_ICY_BLAST_AURA, true);
            }

            void Register()
            {
				OnEffectHitTarget += SpellEffectFn(spell_icy_blast_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icy_blast_SpellScript();
        }
};

Position MeleePositions [8] = 
{
	{1048.940918f, 103.602982f, 628.781921f, 0.0f},
	{1051.991089f, 105.077751f, 628.378113f, 0.0f},
	{1054.756592f, 106.334923f, 628.524231f, 0.0f},
	{1057.378662f, 107.335869f, 628.961426f, 0.0f},
	{1059.918335f, 108.216484f, 629.180786f, 0.0f},
	{1062.583130f, 109.096100f, 629.217102f, 0.0f},
	{1065.392578f, 109.888382f, 629.014648f, 0.0f},
	{1067.988770f, 110.583801f, 629.002502f, 0.0f}
};
Position CasterPositions [5] = 
{
	{1049.112549f, 108.983307f, 628.377747f, 0.0f},
	{1051.151489f, 109.933128f, 628.259949f, 0.0f},
	{1055.122559f, 111.905891f, 628.280823f, 0.0f},
	{1059.435303f, 113.180847f, 628.434326f, 0.0f},
	{1063.810181f, 114.234787f, 628.682068f, 0.0f}
};
Position HealerPositions [4] = 
{
	{1049.197266f, 112.429695f, 628.156189f, 0.0f},
	{1052.296631f, 114.052467f, 628.156189f, 0.0f},
	{1055.526611f, 115.396362f, 628.156189f, 0.0f},
	{1058.851318f, 116.489410f, 628.172729f, 0.0f}
};

class npc_pos_slave_leader : public CreatureScript
{
    public:
        npc_pos_slave_leader() : CreatureScript("npc_pos_slave_leader") { }

		struct npc_pos_slave_leaderAI: public ScriptedAI
        {
            npc_pos_slave_leaderAI(Creature* creature) : ScriptedAI(creature), summons(me) { enrageTimer = 300000; }

			EventMap combatEvents;
			EventMap talkEvents;
			SummonList summons;
			uint32 enrageTimer;

			void SummonNPCs()
			{
				//4 heal, 5 caster, 8 melee
				for (uint8 i = 0; i < 8; i++)
					me->SummonCreature(RAND(NPC_FREED_SLAVE_1_ALLIANCE, NPC_FREED_SLAVE_1_HORDE), *me)->GetMotionMaster()->MovePoint(0, MeleePositions[i]);
				for (uint8 i = 0; i < 4; i++)
					me->SummonCreature(RAND(NPC_FREED_SLAVE_2_ALLIANCE, NPC_FREED_SLAVE_2_HORDE), *me)->GetMotionMaster()->MovePoint(0, HealerPositions[i]);
				for (uint8 i = 0; i < 5; i++)
					me->SummonCreature(RAND(NPC_FREED_SLAVE_3_ALLIANCE, NPC_FREED_SLAVE_3_HORDE), *me)->GetMotionMaster()->MovePoint(0, CasterPositions[i]);
			}

			void RandomFollow()
			{
				me->SetWalk(false);
				for (std::list<uint64>::iterator ite = summons.begin(); ite != summons.end(); ite++)
					if (Creature* follower = Unit::GetCreature(*me, *ite))
						if (follower->isAlive())
							follower->GetMotionMaster()->MoveFollow(me, 5.0f*urand(1, 5), frand(0.0f, 2.0f*M_PI));
			}

			void JustSummoned(Creature* summon)
			{
				summons.Summon(summon);
			}
			
			void DamageDealt(Unit* target, uint32& damage, DamageEffectType type)
			{
				if (target->GetEntry() == NPC_TYRANNUS)
					damage = 0;
			}

			void DoAction(const int32 action)
			{
				switch (action)
				{
					case ACTION_START_INTRO:
						SummonNPCs();
						me->GetMotionMaster()->MovePoint(POINT_SLAVE_INTRO, SlaveMove1);
						talkEvents.ScheduleEvent(EVENT_TALK_SLAVE_INTRO, 14000);
						break;
					case ACTION_DESPAWN:
						summons.DespawnAll();
						me->DespawnOrUnsummon(100);
						break;
					case ACTION_END_COMBAT:
						if (me->isAlive())
						{
							RandomFollow();
							combatEvents.Reset();
							talkEvents.Reset();
							me->GetMotionMaster()->MovePoint(POINT_SLAVE_OUTRO, SlaveMove2);
						}
						else
						{
							InstanceScript* instance = me->GetInstanceScript();
							if (Creature* bla = Unit::GetCreature(*me, instance->GetData64(DATA_JAINA_SYLVANAS_2)))
								CAST_AI(npc_pos_jaina_sylvanas_outro::npc_pos_jaina_sylvanas_outroAI, bla->AI())->portTimer = 0;
							if (instance)
								if (instance->GetData64(DATA_END_DOOR))
									instance->DoUseDoorOrButton(instance->GetData64(DATA_END_DOOR));
						}
						break;
				}
			}

			void MovementInform(uint32 type, uint32 id)
			{
				switch (id)
				{
					case POINT_SLAVE_OUTRO:
						RandomFollow();
						talkEvents.ScheduleEvent(EVENT_TALK_SLAVE_OUTRO_1, 100);
						break;
				}
			}

			void EnterCombat(Unit* who)
			{
				ScriptedAI::EnterCombat(who);
				for (std::list<uint64>::iterator ite = summons.begin(); ite != summons.end(); ite++)
					if (Creature* follower = Unit::GetCreature(*me, *ite))
						follower->AI()->AttackStart(who);
			}

			void UpdateAI(const uint32 diff)
			{
				talkEvents.Update(diff);

				switch (uint32 eventId = talkEvents.ExecuteEvent())
				{
					case EVENT_TALK_SLAVE_INTRO:
						Talk(0);
						me->SetHomePosition(*me);
						for (std::list<uint64>::iterator ite = summons.begin(); ite != summons.end(); ite++)
							if (Creature* follower = Unit::GetCreature(*me, *ite))
								follower->SetHomePosition(*follower);
						break;
					case EVENT_TALK_SLAVE_OUTRO_1:
						Talk(1);
						talkEvents.ScheduleEvent(EVENT_TALK_SLAVE_OUTRO_2, 22000);
						break;
					case EVENT_TALK_SLAVE_OUTRO_2:
						Talk(2);
						talkEvents.ScheduleEvent(EVENT_SINDRAGOSA_SPAWN, 3000);
						break;
					case EVENT_SINDRAGOSA_SPAWN:
						if (Creature* sindragosa = me->SummonCreature(NPC_SINDRAGOSA, SindragosaSpawn))
						{
							sindragosa->SetSpeed(MOVE_FLIGHT, 3.5f, true);
							sindragosa->GetMotionMaster()->MovePoint(POINT_SINDRAGOSA_ATTACK, SindragosaMove1);
							InstanceScript* instance = me->GetInstanceScript();
							if (Creature* bla = Unit::GetCreature(*me, instance->GetData64(DATA_JAINA_SYLVANAS_2)))
								CAST_AI(npc_pos_jaina_sylvanas_outro::npc_pos_jaina_sylvanas_outroAI, bla->AI())->portTimer = 5000;
						}
						break;
				}

				if (!UpdateVictim())
					return;

				if (enrageTimer)
				{
					if (enrageTimer <= diff)
					{
						enrageTimer = 0;
						summons.DespawnEntry(NPC_FREED_SLAVE_2_ALLIANCE);
						summons.DespawnEntry(NPC_FREED_SLAVE_2_HORDE);
						me->MonsterYell("We can't hold them much longer!", LANG_UNIVERSAL, 0);
					}
					else
						enrageTimer -= diff;
				}
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_slave_leaderAI(creature);
        }
};

class POSFrostBombExplosion : public BasicEvent
{
	public:
		POSFrostBombExplosion(Creature* owner, uint64 sindragosaGUID) : _owner(owner), _sindragosaGUID(sindragosaGUID) { }

		bool Execute(uint64 /*eventTime*/, uint32 /*updateTime*/)
		{
			_owner->CastSpell(_owner, 70521, true, NULL, NULL, _sindragosaGUID);
			_owner->CastSpell(_owner, 70521, true, NULL, NULL, _sindragosaGUID);
			_owner->DespawnOrUnsummon(5000);
			return true;
		}

	private:
		Creature* _owner;
		uint64 _sindragosaGUID;
};

class npc_pos_sindragosa : public CreatureScript
{
    public:
        npc_pos_sindragosa() : CreatureScript("npc_pos_sindragosa") { }

		struct npc_pos_sindragosaAI: public ScriptedAI
        {
			npc_pos_sindragosaAI(Creature* creature) : ScriptedAI(creature) { despawnTimer = 0; }

			uint32 despawnTimer;

			void JustSummoned(Creature* summon)
			{
				if (summon->GetEntry() == NPC_FROST_BOMB)
				{
					summon->m_Events.AddEvent(new POSFrostBombExplosion(summon, me->GetGUID()), summon->m_Events.CalculateTime(5500));
					despawnTimer = 6000;
				}
			}

			void MovementInform(uint32 type, uint32 id)
			{
				if (id == POINT_DESPAWN)
					me->DespawnOrUnsummon(100);
				else if (id == POINT_SINDRAGOSA_ATTACK)
					me->CastSpell(SindragosaTarget.GetPositionX(), SindragosaTarget.GetPositionY(), SindragosaTarget.GetPositionZ(), SPELL_FROST_BOMB_TRIGGER, false);
			}

			void UpdateAI(const uint32 diff)
			{
				if (despawnTimer)
				{
					if (despawnTimer <= diff)
					{
						me->GetMotionMaster()->MovePoint(POINT_DESPAWN, SindragosaMove2);
						despawnTimer = 0;
					}
					else
						despawnTimer -= diff;
				}
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_sindragosaAI(creature);
        }
};

enum PoSMeleeSlave
{
	EVENT_MELEE_HEROIC_STRIKE = 1,
	EVENT_MELEE_CLEAVE,
	EVENT_MELEE_SHOUT,

	SPELL_HEROIC_STRIKE = 69566,
	SPELL_CLEAVE = 59992,
	SPELL_DEMORALIZING_SHOUT = 69565,
};

class npc_pos_slave_melee : public CreatureScript
{
    public:
        npc_pos_slave_melee() : CreatureScript("npc_pos_slave_melee") { }

		struct npc_pos_slave_meleeAI: public ScriptedAI
        {
			npc_pos_slave_meleeAI(Creature* creature) : ScriptedAI(creature)
			{
				events.ScheduleEvent(EVENT_MELEE_HEROIC_STRIKE, 2500);
				events.ScheduleEvent(EVENT_MELEE_CLEAVE, 5000);
				events.ScheduleEvent(EVENT_MELEE_SHOUT, urand(10000, 20000));
			}

			EventMap events;
			
			void DamageDealt(Unit* target, uint32& damage, DamageEffectType type)
			{
				if (target->GetEntry() == NPC_TYRANNUS)
					damage = 0;
			}

			void UpdateAI(const uint32 diff)
			{
				if (!UpdateVictim())
					return;

				events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = events.ExecuteEvent())
					switch (eventId)
					{
						case EVENT_MELEE_HEROIC_STRIKE:
							DoCastVictim(SPELL_HEROIC_STRIKE);
							events.ScheduleEvent(EVENT_MELEE_HEROIC_STRIKE, 5000);
							break;
						case EVENT_MELEE_CLEAVE:
							DoCastVictim(SPELL_CLEAVE);
							events.ScheduleEvent(EVENT_MELEE_CLEAVE, 5000);
							break;
						case EVENT_MELEE_SHOUT:
							DoCastAOE(SPELL_DEMORALIZING_SHOUT);
							events.ScheduleEvent(EVENT_MELEE_SHOUT, 30000);
							break;
					}

				DoMeleeAttackIfReady();
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_slave_meleeAI(creature);
        }
};

enum PoSHealerSlave
{
	EVENT_HEALER_CHAIN_HEAL = 1,
	EVENT_HEALER_EARTH_SHIELD,
	EVENT_HEALER_TOTEM,

	SPELL_CHAIN_HEAL = 70425,
	SPELL_EARTH_SHIELD = 69569,
	SPELL_TOTEM = 35199,
};

class npc_pos_slave_healer : public CreatureScript
{
    public:
        npc_pos_slave_healer() : CreatureScript("npc_pos_slave_healer") { }

		struct npc_pos_slave_healerAI: public Scripted_NoMovementAI
        {
			npc_pos_slave_healerAI(Creature* creature) : Scripted_NoMovementAI(creature)
			{
				events.ScheduleEvent(EVENT_HEALER_CHAIN_HEAL, urand(2500, 5000));
				events.ScheduleEvent(EVENT_HEALER_EARTH_SHIELD, 5000);
				events.ScheduleEvent(EVENT_HEALER_TOTEM, urand(10000, 20000));
			}

			EventMap events;
			
			void DamageDealt(Unit* target, uint32& damage, DamageEffectType type)
			{
				if (target->GetEntry() == NPC_TYRANNUS)
					damage = 0;
			}

			void UpdateAI(const uint32 diff)
			{
				if (!UpdateVictim())
					return;

				events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = events.ExecuteEvent())
					switch (eventId)
					{
						case EVENT_HEALER_CHAIN_HEAL:
							if (Unit* target = DoSelectLowestHpFriendly(40.0f))
								DoCast(target, SPELL_CHAIN_HEAL);
							else
								DoCast(me, SPELL_CHAIN_HEAL);
							events.ScheduleEvent(EVENT_HEALER_CHAIN_HEAL, urand(2500, 5000));
							break;
						case EVENT_HEALER_EARTH_SHIELD:
							if (Unit* target = DoSelectLowestHpFriendly(40.0f, 5000))
								DoCast(target, SPELL_EARTH_SHIELD);
							events.ScheduleEvent(EVENT_HEALER_EARTH_SHIELD, 5000);
							break;
						case EVENT_HEALER_TOTEM:
							DoCastAOE(SPELL_TOTEM);
							events.ScheduleEvent(EVENT_HEALER_TOTEM, 60000);
							break;
					}

				DoMeleeAttackIfReady();
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_slave_healerAI(creature);
        }
};

enum PoSCasterSlave
{
	EVENT_CASTER_PYROBLAST = 1,
	EVENT_CASTER_BLIZZARD,

	SPELL_FIREBALL = 69570,
	SPELL_PYROBLAST = 70516,
	SPELL_BLIZZARD = 70421,
	SPELL_ICE_BLOCK = 46604,
};

class npc_pos_slave_caster : public CreatureScript
{
    public:
        npc_pos_slave_caster() : CreatureScript("npc_pos_slave_caster") { }

		struct npc_pos_slave_casterAI: public Scripted_NoMovementAI
        {
			npc_pos_slave_casterAI(Creature* creature) : Scripted_NoMovementAI(creature)
			{
				events.ScheduleEvent(EVENT_CASTER_PYROBLAST, urand(5000, 10000));
				events.ScheduleEvent(EVENT_CASTER_BLIZZARD, 11000);
				iceBlocked = false;
			}

			EventMap events;
			bool iceBlocked;
			
			void DamageDealt(Unit* target, uint32& damage, DamageEffectType type)
			{
				if (target->GetEntry() == NPC_TYRANNUS)
					damage = 0;
			}

			void UpdateAI(const uint32 diff)
			{
				if (!UpdateVictim())
					return;

				if (!iceBlocked && me->HealthBelowPct(20))
				{
					DoCast(me, SPELL_ICE_BLOCK);
					iceBlocked = true;
				}

				events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while (uint32 eventId = events.ExecuteEvent())
					switch (eventId)
					{
						case EVENT_CASTER_PYROBLAST:
							DoCastVictim(SPELL_PYROBLAST);
							events.ScheduleEvent(EVENT_CASTER_PYROBLAST, urand(5000, 10000));
							break;
						case EVENT_CASTER_BLIZZARD:
							DoCastVictim(SPELL_BLIZZARD);
							events.ScheduleEvent(EVENT_CASTER_BLIZZARD, 10000);
							break;
					}

				DoSpellAttackIfReady(SPELL_FIREBALL);
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_slave_casterAI(creature);
        }
};

void AddSC_boss_tyrannus()
{
    new boss_tyrannus();
    new boss_rimefang();
	new npc_collapsing_icicle();
	new npc_gauntlet_dummy();
	new npc_pos_slave_leader();
	new npc_pos_sindragosa();
	new npc_pos_jaina_sylvanas_outro();
	new npc_pos_slave_melee();
	new npc_pos_slave_healer();
	new npc_pos_slave_caster();
    new spell_tyrannus_overlord_brand();
    new spell_tyrannus_mark_of_rimefang();
	new spell_icy_blast();
    new at_tyrannus_event_starter();
	new at_gauntlet_event_starter();
}
