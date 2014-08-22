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

enum eSpells
{
    SPELL_FIREBALL              = 69583, //Ymirjar Flamebearer
    SPELL_HELLFIRE              = 69586,
    SPELL_TACTICAL_BLINK        = 69584,
    SPELL_FROST_BREATH          = 69527, //Iceborn Proto-Drake
    SPELL_LEAPING_FACE_MAUL     = 69504, // Geist Ambusher
};

enum eEvents
{
    // Ymirjar Flamebearer
    EVENT_FIREBALL              = 1,
    EVENT_TACTICAL_BLINK        = 2,
};

class mob_ymirjar_flamebearer : public CreatureScript
{
    public:
        mob_ymirjar_flamebearer() : CreatureScript("mob_ymirjar_flamebearer") { }

        struct mob_ymirjar_flamebearerAI: public ScriptedAI
        {
            mob_ymirjar_flamebearerAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset()
            {
                _events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                _events.ScheduleEvent(EVENT_FIREBALL, 4000);
                _events.ScheduleEvent(EVENT_TACTICAL_BLINK, 15000);
            }

            void UpdateAI(const uint32 diff)
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
                        case EVENT_FIREBALL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FIREBALL);
                            _events.RescheduleEvent(EVENT_FIREBALL, 5000);
                            break;
                        case EVENT_TACTICAL_BLINK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_TACTICAL_BLINK);
                            DoCast(me, SPELL_HELLFIRE);
                            _events.RescheduleEvent(EVENT_TACTICAL_BLINK, 12000);
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
            return new mob_ymirjar_flamebearerAI(creature);
        }
};

class mob_iceborn_protodrake : public CreatureScript
{
    public:
        mob_iceborn_protodrake() : CreatureScript("mob_iceborn_protodrake") { }

        struct mob_iceborn_protodrakeAI: public ScriptedAI
        {
            mob_iceborn_protodrakeAI(Creature* creature) : ScriptedAI(creature), _vehicle(creature->GetVehicleKit())
            {
                ASSERT(_vehicle);
            }

            void Reset()
            {
                _frostBreathCooldown = 5000;
            }

            void EnterCombat(Unit* /*who*/)
            {
                _vehicle->RemoveAllPassengers();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (_frostBreathCooldown < diff)
                {
                    DoCastVictim(SPELL_FROST_BREATH);
                    _frostBreathCooldown = 10000;
                }
                else
                    _frostBreathCooldown -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            Vehicle* _vehicle;
            uint32 _frostBreathCooldown;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_iceborn_protodrakeAI(creature);
        }
};

class mob_geist_ambusher : public CreatureScript
{
    public:
        mob_geist_ambusher() : CreatureScript("mob_geist_ambusher") { }

        struct mob_geist_ambusherAI: public ScriptedAI
        {
            mob_geist_ambusherAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset()
            {
                _leapingFaceMaulCooldown = 9000;
            }

            void EnterCombat(Unit* who)
            {
                if (who->GetTypeId() != TYPEID_PLAYER)
                    return;

                // the max range is determined by aggro range
                if (me->GetDistance(who) > 5.0f)
                    DoCast(who, SPELL_LEAPING_FACE_MAUL);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (_leapingFaceMaulCooldown < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 5.0f, true))
                        DoCast(target, SPELL_LEAPING_FACE_MAUL);
                    _leapingFaceMaulCooldown = urand(9000, 14000);
                }
                else
                    _leapingFaceMaulCooldown -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 _leapingFaceMaulCooldown;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_geist_ambusherAI(creature);
        }
};

class spell_trash_mob_glacial_strike : public SpellScriptLoader
{
    public:
        spell_trash_mob_glacial_strike() : SpellScriptLoader("spell_trash_mob_glacial_strike") { }

        class spell_trash_mob_glacial_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_trash_mob_glacial_strike_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (GetTarget()->IsFullHealth())
                {
                    GetTarget()->RemoveAura(GetId(), 0, 0, AURA_REMOVE_BY_ENEMY_SPELL);
                    PreventDefaultAction();
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_trash_mob_glacial_strike_AuraScript::PeriodicTick, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_trash_mob_glacial_strike_AuraScript();
        }
};

Position LeadInit = {440.0f, 213.0f, 528.72f, 0.0f};
Position SpawnInit = {411.0f, 213.0f, 529.4f, 0.0f};
Position TyranLeave = {566.0f, 220.0f, 553.2f, 6.0f};
Position ChargeTarget[4] = {
	{491.0f, 205.0f, 528.7f, 3.0f},
	{485.0f, 217.0f, 528.7f, 3.0f},
	{481.0f, 226.0f, 528.7f, 3.0f},
	{475.0f, 240.0f, 528.7f, 3.0f}
};

//damage 69426
//spawn a efekt 69424

class npc_pos_jaina_sylvanas : public CreatureScript
{
    public:
        npc_pos_jaina_sylvanas() : CreatureScript("npc_pos_jaina_sylvanas") { }

		struct npc_pos_jaina_sylvanasAI: public Scripted_NoMovementAI
        {
            npc_pos_jaina_sylvanasAI(Creature* creature) : Scripted_NoMovementAI(creature), summons(me) { }

			EventMap events;
			SummonList summons;

			void SummonFollowers()
			{
				Creature* left = me->SummonCreature(NPC_KILARA, *me);
				left->GetMotionMaster()->MovePoint(0, 440.0f, 215.64f, 528.72f);
				left->SetHomePosition(440.0f, 215.64f, 528.72f, 0.0f);
				Creature* right = me->SummonCreature(NPC_KORALEN, *me);
				right->GetMotionMaster()->MovePoint(0, 440.0f, 210.36f, 528.72f);
				right->SetHomePosition(440.0f, 210.36f, 528.72f, 0.0f);
				
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 16.98f, 5.12f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 16.08f, 5.31f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 12.21f, 5.35f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 10.56f, 0.06f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 13.44f, 0.29f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 11.68f, 0.57f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 15.86f, 1.13f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 19.02f, 1.23f);
				me->SummonCreature(NPC_CHAMPION_1_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 20.092f, 1.41f);
				
				me->SummonCreature(NPC_CHAMPION_2_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 7.34f, 6.15f);
				me->SummonCreature(NPC_CHAMPION_2_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 7.20f, 0.43f);
				me->SummonCreature(NPC_CHAMPION_2_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 9.05f, 0.87f);

				me->SummonCreature(NPC_CHAMPION_3_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 13.87f, 1.38f);
				me->SummonCreature(NPC_CHAMPION_3_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 10.21f, 1.29f);
				me->SummonCreature(NPC_CHAMPION_3_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 7.18f, 5.13f);
				me->SummonCreature(NPC_CHAMPION_3_HORDE, SpawnInit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000)->GetMotionMaster()->MoveFollow(me, 10.86f, 4.96f);
			}

			void FollowersAttack()
			{
				for (std::list<uint64>::iterator iterator = summons.begin(); iterator != summons.end(); iterator++)
					if (Creature* unit = Unit::GetCreature(*me, *iterator))
					{
						unit->GetMotionMaster()->Clear();
						unit->GetMotionMaster()->MovePoint(0, ChargeTarget[urand(0, 3)]);
					}
			}

			void JustSummoned(Creature* summon)
			{
				switch (summon->GetEntry())
				{
					case NPC_KILARA:
					case NPC_KORALEN:
					case NPC_ELANDRA:
					case NPC_KORLAEN:
						return;
				}
				summons.Summon(summon);
			}

			void DoAction(const int32 action)
			{
				switch (action)
				{
					case ACTION_ENTERANCE:
						SummonFollowers();
						me->GetMotionMaster()->MovePoint(0, LeadInit);
						me->SetHomePosition(LeadInit);
						events.Reset();
						events.ScheduleEvent(EVENT_LEADER_TALK_1_1, 20000);
						break;
				}
			}

			void UpdateAI(const uint32 diff)
			{
				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_LEADER_TALK_1_1:
							Talk(eventId - 1);
							FollowersAttack();
							events.ScheduleEvent(EVENT_LEADER_LIFT_FOLLOWERS, 5000);
							events.ScheduleEvent(EVENT_LEADER_TALK_1_2, 13000);
							break;
						case EVENT_LEADER_TALK_1_2:
							Talk(eventId - 1);
							events.ScheduleEvent(EVENT_LEADER_TALK_1_3, 20000);
							break;
						case EVENT_LEADER_TALK_1_3:
							Talk(eventId - 1);
							events.ScheduleEvent(EVENT_LEADER_TALK_1_4, 7000);
							break;
						case EVENT_LEADER_TALK_1_4:
							Talk(eventId - 1);
							break;

						case EVENT_LEADER_LIFT_FOLLOWERS:
							if (Creature* tyrannus = Unit::GetCreature(*me, me->GetInstanceScript()->GetData64(DATA_TYRANNUS_EVENT)))
								tyrannus->AI()->DoCastAOE(SPELL_TYRANNUS_NECROMANTIC_POWER);
							for (std::list<uint64>::iterator iterator = summons.begin(); iterator != summons.end(); iterator++)
									if (Creature* unit = Unit::GetCreature(*me, *iterator))
									{
										unit->GetMotionMaster()->Clear();
										unit->CastSpell(unit, SPELL_STRANGULATE);
										float rand = frand(5.0f, 10.0f);
										unit->GetMotionMaster()->MoveJump(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ() + rand, rand/5.0f, rand/5.0f);
									}
							events.ScheduleEvent(EVENT_LEADER_KILL_FOLLOWERS_1, 5000);
							break;
						case EVENT_LEADER_KILL_FOLLOWERS_1:
							for (std::list<uint64>::iterator iterator = summons.begin(); iterator != summons.end(); iterator++)
								if (Creature* unit = Unit::GetCreature(*me, *iterator))
								{
									unit->GetMotionMaster()->Clear();
									unit->GetMotionMaster()->MoveJump(unit->GetPositionX(), unit->GetPositionY(), 528.7f, 20.0f, 20.0f);
								}
							events.ScheduleEvent(EVENT_LEADER_KILL_FOLLOWERS_2, 500);
							break;
						case EVENT_LEADER_KILL_FOLLOWERS_2:
							for (std::list<uint64>::iterator iterator = summons.begin(); iterator != summons.end(); iterator++)
								if (Creature* unit = Unit::GetCreature(*me, *iterator))
								{
									unit->SetStandState(UNIT_STAND_STATE_DEAD);
									unit->SetHealth(0);
								}
							events.ScheduleEvent(EVENT_LEADER_RAISE_FOLLOWERS, 1500);
							break;
						case EVENT_LEADER_RAISE_FOLLOWERS:
							if (Creature* tyrannus = Unit::GetCreature(*me, me->GetInstanceScript()->GetData64(DATA_TYRANNUS_EVENT)))
								for (std::list<uint64>::iterator iterator = summons.begin(); iterator != summons.end(); iterator++)
									if (Creature* unit = Unit::GetCreature(*me, *iterator))
									{
										tyrannus->SummonCreature(NPC_INTRO_SKELETON, *unit, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
										unit->DespawnOrUnsummon();
									}
							break;
					}
				}
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_jaina_sylvanasAI(creature);
        }
};

class npc_pos_tyrannus_intro : public CreatureScript
{
    public:
        npc_pos_tyrannus_intro() : CreatureScript("npc_pos_tyrannus_intro") { }

        struct npc_pos_tyrannus_introAI: public ScriptedAI
        {
			npc_pos_tyrannus_introAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

			EventMap events;
			SummonList summons;

			void DoAction(const int32 action)
			{
				switch (action)
				{
					case ACTION_ENTERANCE:
						events.Reset();
						events.ScheduleEvent(EVENT_TYR_TALK_1_1, 2000);
						break;
				}
			}

			void MovementInform(uint32 type, uint32 id)
			{
				if (id == POINT_DESPAWN)
					me->DespawnOrUnsummon(100);
			}

			void JustSummoned(Creature* summon)
			{
				summon->CastSpell(summon, SPELL_TYRANNUS_RAISE_DEAD_VISUAL, true);
				summons.Summon(summon);
			}

			void UpdateAI(const uint32 diff)
			{
				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
						case EVENT_TYR_TALK_1_1:
							Talk(eventId - 1);
							events.ScheduleEvent(EVENT_TYR_TALK_1_2, 6000);
							break;
						case EVENT_TYR_TALK_1_2:
							Talk(eventId - 1);
							events.ScheduleEvent(EVENT_TYR_TALK_1_3, 17000);
							break;
						case EVENT_TYR_TALK_1_3:
							Talk(eventId - 1);
							events.ScheduleEvent(EVENT_TYR_TALK_1_4, 15000);
							break;
						case EVENT_TYR_TALK_1_4:
							Talk(eventId - 1);
							events.ScheduleEvent(EVENT_TYR_LEAVE, 6000);
							break;
						case EVENT_TYR_LEAVE:
							if (Creature* leader = Unit::GetCreature(*me, me->GetInstanceScript()->GetData64(DATA_JAINA_SYLVANAS_1)))
								for (std::list<uint64>::iterator iterator = summons.begin(); iterator != summons.end(); iterator++)
									if (Creature* unit = Unit::GetCreature(*me, *iterator))
										DoZoneInCombat(unit, 100.0f);
							me->GetMotionMaster()->MovePoint(POINT_DESPAWN, TyranLeave);
							break;
					}
				}
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pos_tyrannus_introAI(creature);
        }
};

void AddSC_pit_of_saron()
{
    new mob_ymirjar_flamebearer();
    new mob_iceborn_protodrake();
    new mob_geist_ambusher();
    new spell_trash_mob_glacial_strike();
	new npc_pos_jaina_sylvanas();
	new npc_pos_tyrannus_intro();
}
