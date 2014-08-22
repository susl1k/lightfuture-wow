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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "blackwing_lair.h"
#include "Player.h"

#define OVERRIDE 

enum Say
{
    SAY_EGGS_BROKEN1        = 0,
    SAY_EGGS_BROKEN2        = 1,
    SAY_EGGS_BROKEN3        = 2,
    SAY_DEATH               = 3,
};

enum Spells
{
    SPELL_MINDCONTROL       = 42013,
    SPELL_CHANNEL           = 45537,
    SPELL_EGG_DESTROY       = 19873,

    SPELL_CLEAVE            = 22540,
    SPELL_WARSTOMP          = 24375,
    SPELL_FIREBALLVOLLEY    = 22425,
    SPELL_CONFLAGRATION     = 23023,

	SPELL_FULL_HEAL			= 23040,
};

enum Summons
{
    NPC_ELITE_DRACHKIN      = 12422,
    NPC_ELITE_WARRIOR       = 12458,
    NPC_WARRIOR             = 12416,
    NPC_MAGE                = 12420,
    NPC_WARLOCK             = 12459,

    GO_EGG                  = 177807
};

enum EVENTS
{
    EVENT_CLEAVE            = 1,
    EVENT_STOMP             = 2,
    EVENT_FIREBALL          = 3,
    EVENT_CONFLAGRATION     = 4
};

class boss_razorgore : public CreatureScript
{
public:
    boss_razorgore() : CreatureScript("boss_razorgore") { }

    struct boss_razorgoreAI : public BossAI
    {
        boss_razorgoreAI(Creature* creature) : BossAI(creature, BOSS_RAZORGORE) { }

        void Reset() OVERRIDE
        {

        }

		void JustReachedHome()
		{
			BossAI::JustReachedHome();

			_Reset();

            secondPhase = false;
            if (instance)
                instance->SetData(DATA_EGG_EVENT, NOT_STARTED);
		}

        void JustDied(Unit* /*killer*/) OVERRIDE
        {
            _JustDied();
            Talk(SAY_DEATH);

            if (instance)
                instance->SetData(DATA_EGG_EVENT, DONE);
        }

		void EnterCombat(Unit* who) OVERRIDE
		{
			_EnterCombat();

			if (instance)
				instance->SetData(DATA_EGG_EVENT, IN_PROGRESS);
		}

        void DoChangePhase()
        {
            events.ScheduleEvent(EVENT_CLEAVE, 15000);
            events.ScheduleEvent(EVENT_STOMP, 35000);
            events.ScheduleEvent(EVENT_FIREBALL, 7000);
            events.ScheduleEvent(EVENT_CONFLAGRATION, 12000);

            secondPhase = true;
            me->RemoveAllAuras();
            me->SetHealth(me->GetMaxHealth());
			summons.DespawnAll();
        }

        void DoAction(const int32 action) OVERRIDE
        {
            if (action == ACTION_PHASE_TWO)
                DoChangePhase();
        }

        void DamageTaken(Unit* /*who*/, uint32& damage) OVERRIDE
        {
            if (!secondPhase)
				if (me->HealthBelowPct(5) || damage > me->GetHealth())
					DoCast(me, SPELL_FULL_HEAL);
        }

        void UpdateAI(const uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLEAVE:
                        DoCastVictim(SPELL_CLEAVE);
                        events.ScheduleEvent(EVENT_CLEAVE, urand(7000, 10000));
                        break;
                    case EVENT_STOMP:
                        DoCastVictim(SPELL_WARSTOMP);
                        events.ScheduleEvent(EVENT_STOMP, urand(15000, 25000));
                        break;
                    case EVENT_FIREBALL:
                        DoCastVictim(SPELL_FIREBALLVOLLEY);
                        events.ScheduleEvent(EVENT_FIREBALL, urand(12000, 15000));
                        break;
                    case EVENT_CONFLAGRATION:
                        DoCastVictim(SPELL_CONFLAGRATION);
                        if (me->getVictim() && me->getVictim()->HasAura(SPELL_CONFLAGRATION))
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true))
                                me->TauntApply(target);
                        events.ScheduleEvent(EVENT_CONFLAGRATION, 30000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        bool secondPhase;
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_razorgoreAI(creature);
    }
};

class go_orb_of_domination : public GameObjectScript
{
public:
    go_orb_of_domination() : GameObjectScript("go_orb_of_domination") { }

    bool OnGossipHello(Player* player, GameObject* go) OVERRIDE
    {
        if (InstanceScript* instance = go->GetInstanceScript())
            if (instance->GetData(DATA_EGG_EVENT) != DONE)
			{
				Creature* razorgore = instance->instance->GetCreature(instance->GetData64(DATA_RAZORGORE_THE_UNTAMED));
				if (!razorgore)
                    razorgore = player->FindNearestCreature(NPC_RAZORGORE, 200.0f);
				if (!razorgore)
					return false;

				razorgore->Attack(player, true);
                player->CastSpell(razorgore, SPELL_MINDCONTROL, true);
			}

        return true;
    }
};

class spell_egg_event : public SpellScriptLoader
{
    public:
        spell_egg_event() : SpellScriptLoader("spell_egg_event") { }

        class spell_egg_eventSpellScript : public SpellScript
        {
            PrepareSpellScript(spell_egg_eventSpellScript);

            void HandleOnHit()
            {
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    instance->SetData(DATA_EGG_EVENT, SPECIAL);
            }

            void Register() OVERRIDE
            {
                OnHit += SpellHitFn(spell_egg_eventSpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const OVERRIDE
        {
            return new spell_egg_eventSpellScript();
        }
};

class npc_razorgore_controller : public CreatureScript
{
public:
    npc_razorgore_controller() : CreatureScript("npc_razorgore_controller") { }

	struct npc_razorgore_controllerAI : public AggressorAI
    {
        npc_razorgore_controllerAI(Creature* creature) : AggressorAI(creature) { }

		EventMap events;

		void Reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* who)
		{
			if (InstanceScript* instance = me->GetInstanceScript())
				if (Creature* razorgore = instance->instance->GetCreature(instance->GetData64(DATA_RAZORGORE_THE_UNTAMED)))
					DoZoneInCombat(razorgore, 200.0f);

			events.ScheduleEvent(1, 10000);
			events.ScheduleEvent(2, 12000);
			events.ScheduleEvent(3, 8000);
			events.ScheduleEvent(4, 16000);
		}
		
		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			switch (events.ExecuteEvent())
			{
				case 1:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, 22274);
					events.ScheduleEvent(1, urand(5000, 10000));
					break;
				case 2:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, 14515);
					events.ScheduleEvent(2, 20000);
					break;
				case 3:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, 13747);
					events.ScheduleEvent(3, 20000);
					break;
				case 4:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, 22273);
					events.ScheduleEvent(4, 12000);
					break;
			}

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_razorgore_controllerAI(creature);
    }
};

void AddSC_boss_razorgore()
{
    new boss_razorgore();
	new npc_razorgore_controller();
    new go_orb_of_domination();
    new spell_egg_event();
}
