/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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
SDName: Boss_Midnight
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"

#define OVERRIDE 

enum Midnight
{
    SAY_MIDNIGHT_KILL           = 0,
    SAY_APPEAR                  = 1,
    SAY_MOUNT                   = 2,

    SAY_KILL                    = 0,
    SAY_DISARMED                = 1,
    SAY_DEATH                   = 2,
    SAY_RANDOM                  = 3,

    SPELL_SHADOWCLEAVE          = 29832,
    SPELL_INTANGIBLE_PRESENCE   = 29833,
    SPELL_BERSERKER_CHARGE      = 26561,                   //Only when mounted

    MOUNTED_DISPLAYID           = 16040,

    //Attumen (@todo Use the summoning spell instead of Creature id. It works, but is not convenient for us)
    SUMMON_ATTUMEN              = 15550,
};

class boss_attumen : public CreatureScript
{
public:
    boss_attumen() : CreatureScript("boss_attumen") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_attumenAI(creature);
    }

    struct boss_attumenAI : public ScriptedAI
    {
        boss_attumenAI(Creature* creature) : ScriptedAI(creature)
        {
            Phase = 1;

            CleaveTimer = urand(10000, 15000);
            CurseTimer = 30000;
            RandomYellTimer = urand(30000, 60000);              //Occasionally yell
            ChargeTimer = 20000;
            ResetTimer = 0;
        }

        uint64 Midnight;
        uint8 Phase;
        uint32 CleaveTimer;
        uint32 CurseTimer;
        uint32 RandomYellTimer;
        uint32 ChargeTimer;                                     //only when mounted
        uint32 ResetTimer;

        void Reset() OVERRIDE
        {
            ResetTimer = 0;
        }

        void EnterEvadeMode() OVERRIDE
        {
            ScriptedAI::EnterEvadeMode();
            ResetTimer = 2000;
        }

        void EnterCombat(Unit* /*who*/) OVERRIDE {}

        void KilledUnit(Unit* /*victim*/) OVERRIDE
        {
            Talk(SAY_KILL);
        }

        void JustDied(Unit* /*killer*/) OVERRIDE
        {
            Talk(SAY_DEATH);
            if (Unit* midnight = Unit::GetUnit(*me, Midnight))
                midnight->Kill(midnight);
        }

        void UpdateAI(const uint32 diff) OVERRIDE;

        void SpellHit(Unit* /*source*/, const SpellInfo* spell) OVERRIDE
        {
            if (spell->Mechanic == MECHANIC_DISARM)
                Talk(SAY_DISARMED);
        }
    };
};

class boss_midnight : public CreatureScript
{
public:
    boss_midnight() : CreatureScript("boss_midnight") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_midnightAI(creature);
    }

    struct boss_midnightAI : public ScriptedAI
    {
        boss_midnightAI(Creature* creature) : ScriptedAI(creature) {}

        uint64 Attumen;
        uint8 Phase;
        uint32 Mount_Timer;

        void Reset() OVERRIDE
        {
            Phase = 1;
            Attumen = 0;
            Mount_Timer = 0;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetVisible(true);
        }

        void EnterCombat(Unit* /*who*/) OVERRIDE {}

        void KilledUnit(Unit* /*victim*/) OVERRIDE
        {
            if (Phase == 2)
            {
                if (Unit* unit = Unit::GetUnit(*me, Attumen))
                    Talk(SAY_MIDNIGHT_KILL, unit->GetGUID());
            }
        }

        void UpdateAI(const uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;

            if (Phase == 1 && HealthBelowPct(95))
            {
                Phase = 2;
				if (Creature* attumen = me->SummonCreature(SUMMON_ATTUMEN, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                {
                    Attumen = attumen->GetGUID();
                    SetMidnight(attumen, me->GetGUID());
                    attumen->AI()->AttackStart(me->getVictim());
                    Talk(SAY_APPEAR, Attumen);
                }
            }
            else if (Phase == 2 && HealthBelowPct(25))
            {
                if (Unit* pAttumen = Unit::GetUnit(*me, Attumen))
                    MoveToMount(pAttumen);
            }
            else if (Phase == 3)
            {
                if (Mount_Timer)
                {
                    if (Mount_Timer <= diff)
                    {
                        Mount_Timer = 0;
                        me->SetVisible(false);
                        me->GetMotionMaster()->MoveIdle();
                        if (Unit* pAttumen = Unit::GetUnit(*me, Attumen))
                        {
                            pAttumen->SetDisplayId(MOUNTED_DISPLAYID);
                            pAttumen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            if (pAttumen->getVictim())
                            {
                                pAttumen->GetMotionMaster()->MoveChase(pAttumen->getVictim());
                                pAttumen->SetTarget(pAttumen->getVictim()->GetGUID());
                            }
							pAttumen->SetFloatValue(OBJECT_FIELD_SCALE_X, 1);
                        }
                    } else Mount_Timer -= diff;
                }
            }

            if (Phase < 3)
                DoMeleeAttackIfReady();
        }

        void MoveToMount(Unit* pAttumen)
        {
			Phase = 4;
            Talk(SAY_MOUNT, pAttumen->GetGUID());
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pAttumen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

			Position pos;
			me->GetPosition(&pos);
			me->GetNearPosition(pos, me->GetDistance2d(pAttumen)/2, me->GetAngle(pAttumen));

            pAttumen->GetMotionMaster()->Clear();
			pAttumen->GetMotionMaster()->MovePoint(128, pos);

            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(128, pos);
        }

		void MovementInform(uint32 type, uint32 point)
		{
			if (point == 128)
			{
				Phase = 3;
				Mount_Timer = 100;
			}
		}

        void SetMidnight(Creature* pAttumen, uint64 value)
        {
            CAST_AI(boss_attumen::boss_attumenAI, pAttumen->AI())->Midnight = value;
        }
    };
};

void boss_attumen::boss_attumenAI::UpdateAI(uint32 diff)
{
    if (ResetTimer)
    {
        if (ResetTimer <= diff)
        {
            ResetTimer = 0;
            Unit* pMidnight = Unit::GetUnit(*me, Midnight);
            if (pMidnight)
            {
                pMidnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pMidnight->SetVisible(true);
            }
            Midnight = 0;
            me->SetVisible(false);
            me->Kill(me);
        } else ResetTimer -= diff;
    }

    //Return since we have no target
    if (!UpdateVictim())
        return;

    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
        return;

    if (CleaveTimer <= diff)
    {
        DoCastVictim(SPELL_SHADOWCLEAVE);
        CleaveTimer = urand(10000, 15000);
    } else CleaveTimer -= diff;

    if (CurseTimer <= diff)
    {
        DoCastVictim(SPELL_INTANGIBLE_PRESENCE);
        CurseTimer = 30000;
    } else CurseTimer -= diff;

    if (RandomYellTimer <= diff)
    {
        Talk(SAY_RANDOM);
        RandomYellTimer = urand(30000, 60000);
    } else RandomYellTimer -= diff;

    if (me->GetUInt32Value(UNIT_FIELD_DISPLAYID) == MOUNTED_DISPLAYID)
    {
        if (ChargeTimer <= diff)
        {
			if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, -5.0f, true))
				DoCast(target, SPELL_BERSERKER_CHARGE);
            ChargeTimer = 20000;
        } else ChargeTimer -= diff;
    }
    else
    {
        if (HealthBelowPct(25))
        {
            Creature* pMidnight = Unit::GetCreature(*me, Midnight);
            if (pMidnight && pMidnight->GetTypeId() == TYPEID_UNIT)
            {
                CAST_AI(boss_midnight::boss_midnightAI, (pMidnight->AI()))->MoveToMount(me);
                me->SetHealth(pMidnight->GetHealth());
                DoResetThreat();
            }
        }
    }

    DoMeleeAttackIfReady();
}

void AddSC_boss_attumen()
{
    new boss_attumen();
    new boss_midnight();
}
