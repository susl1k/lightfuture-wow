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
#include "naxxramas.h"

#define SPELL_MORTAL_WOUND      25646
#define SPELL_ENRAGE            RAID_MODE(28371, 54427)
#define SPELL_DECIMATE          RAID_MODE(28374, 54426)
#define SPELL_BERSERK           26662
#define SPELL_INFECTED_WOUND    29306

#define MOB_ZOMBIE  16360

const Position PosSummon[3] =
{
    {3267.9f, -3172.1f, 297.42f, 0.94f},
    {3253.2f, -3132.3f, 297.42f, 0},
    {3308.3f, -3185.8f, 297.42f, 1.58f},
};

enum Events
{
    EVENT_NONE,
    EVENT_WOUND,
    EVENT_ENRAGE,
    EVENT_DECIMATE,
    EVENT_BERSERK,
    EVENT_SUMMON,
};

#define EMOTE_NEARBY    "Gluth spots a nearby zombie to devour!"

class boss_gluth : public CreatureScript
{
public:
    boss_gluth() : CreatureScript("boss_gluth") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gluthAI (creature);
    }

    struct boss_gluthAI : public BossAI
    {
        boss_gluthAI(Creature* creature) : BossAI(creature, BOSS_GLUTH)
        {
            // Do not let Gluth be affected by zombies' debuff
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_INFECTED_WOUND, true);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetEntry() == MOB_ZOMBIE && me->IsWithinDistInMap(who, 7))
            {
				me->Kill(who);
                me->ModifyHealth(int32(me->CountPctFromMaxHealth(5)));
                me->MonsterTextEmote(EMOTE_NEARBY, 0, true);
            }
            else
                BossAI::MoveInLineOfSight(who);
        }

		void DamageTaken(Unit* damager, uint32& damage)
		{
			if (damager == me)
				damage = 0;
			ScriptedAI::DamageTaken(damager, damage);
		}

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
            events.ScheduleEvent(EVENT_WOUND, 10000);
            events.ScheduleEvent(EVENT_ENRAGE, 15000);
            events.ScheduleEvent(EVENT_DECIMATE, 105000);
            events.ScheduleEvent(EVENT_BERSERK, 8*60000);
            events.ScheduleEvent(EVENT_SUMMON, 15000);
        }

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == MOB_ZOMBIE)
				summon->GetMotionMaster()->MoveChase(me);
            summons.Summon(summon);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictimWithGaze() || !CheckInRoom())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_WOUND:
                        DoCast(me->getVictim(), SPELL_MORTAL_WOUND);
                        events.ScheduleEvent(EVENT_WOUND, 10000);
                        break;
                    case EVENT_ENRAGE:
                        // TODO : Add missing text
                        DoCast(me, SPELL_ENRAGE);
                        events.ScheduleEvent(EVENT_ENRAGE, 15000);
                        break;
                    case EVENT_DECIMATE:
                        // TODO : Add missing text
                        DoCastAOE(SPELL_DECIMATE);
                        events.ScheduleEvent(EVENT_DECIMATE, 105000);
                        break;
                    case EVENT_BERSERK:
                        DoCast(me, SPELL_BERSERK);
                        events.ScheduleEvent(EVENT_BERSERK, 5*60000);
                        break;
                    case EVENT_SUMMON:
                        for (int32 i = 0; i < RAID_MODE(1, 2); ++i)
                            DoSummon(MOB_ZOMBIE, PosSummon[rand() % RAID_MODE(1, 3)]);
                        events.ScheduleEvent(EVENT_SUMMON, 10000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

};

class spell_gluth_decimate : public SpellScriptLoader
{
	public:
		spell_gluth_decimate() : SpellScriptLoader("spell_gluth_decimate") { }

		class spell_gluth_decimate_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_gluth_decimate_SpellScript);

			void HandleScript(SpellEffIndex /*effIndex*/)
			{
				if (GetHitPlayer() || (GetHitCreature() && GetHitCreature()->GetEntry() == MOB_ZOMBIE))
					if (GetHitUnit()->GetHealthPct() > 5.0f)
					{
						uint32 newHealth = GetHitUnit()->GetMaxHealth() * 5 / 100;
						GetHitUnit()->SetHealth(newHealth);
						if (GetHitPlayer())
							return;
						
						Creature* zombie = GetHitCreature();
						zombie->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
						zombie->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
						zombie->GetMotionMaster()->MoveChase(GetCaster());
						zombie->SetTarget(GetCaster()->GetGUID());
					}
			}

			void Register()
			{
				OnEffectHitTarget += SpellEffectFn(spell_gluth_decimate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
			}
		};

		SpellScript* GetSpellScript() const
		{
			return new spell_gluth_decimate_SpellScript();
		}
};

void AddSC_boss_gluth()
{
    new boss_gluth();
	new spell_gluth_decimate();
}
