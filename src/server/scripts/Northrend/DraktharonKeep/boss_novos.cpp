/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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
#include "SpellScript.h"
#include "ScriptedCreature.h"
#include "drak_tharon_keep.h"

enum Yells
{
    SAY_AGGRO                       = 0,
    SAY_KILL                        = 1,
    SAY_DEATH                       = 2,
    SAY_SUMMONING_ADDS              = 3, // unused
    SAY_ARCANE_FIELD                = 4,
    EMOTE_SUMMONING_ADDS            = 5  // unused
};

enum Spells
{
    SPELL_BEAM_CHANNEL              = 52106,
    SPELL_ARCANE_FIELD              = 47346,

    SPELL_SUMMON_RISEN_SHADOWCASTER = 49105,
    SPELL_SUMMON_FETID_TROLL_CORPSE = 49103,
    SPELL_SUMMON_HULKING_CORPSE     = 49104,
    SPELL_SUMMON_CRYSTAL_HANDLER    = 49179,
    SPELL_SUMMON_COPY_OF_MINIONS    = 59933,

    SPELL_ARCANE_BLAST              = 49198,
    SPELL_BLIZZARD                  = 49034,
    SPELL_FROSTBOLT                 = 49037,
    SPELL_WRATH_OF_MISERY           = 50089,
    SPELL_SUMMON_MINIONS            = 59910
};

enum Misc
{
    ACTION_RESET_CRYSTALS,
    ACTION_ACTIVATE_CRYSTAL,
    ACTION_DEACTIVATE,
    EVENT_ATTACK,
    EVENT_SUMMON_MINIONS,
    DATA_NOVOS_ACHIEV
};

#define MAX_Y_COORD_OH_NOVOS        -771.95f

static Position AddSpawnPoint = { -379.20f, -816.76f, 59.70f, 0.0f };
static Position CrystalHandlerSpawnPoint = { -326.626343f, -709.956604f, 27.813314f, 0.0f };
static Position AddDestinyPoint = { -379.314545f, -772.577637f, 28.58837f, 0.0f };

class boss_novos : public CreatureScript
{
public:
    boss_novos() : CreatureScript("boss_novos") { }

    struct boss_novosAI : public BossAI
    {
        boss_novosAI(Creature* creature) : BossAI(creature, DATA_NOVOS) { }

        void Reset() OVERRIDE
        {
            _Reset();

            _ohNovos = true;
            _crystalHandlerCount = 0;

            SetCrystalsStatus(false);
            SetBubbled(false);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell) OVERRIDE
		{
			BossAI::SpellHit(caster, spell);
			
			if (spell->Id == 57908)
				if (caster->ToPlayer())
					caster->CastSpell(caster, 57915, true);
		}

        void EnterCombat(Unit* /* victim */) OVERRIDE
        {
            _EnterCombat();
            Talk(SAY_AGGRO);
			
			events.ScheduleEvent(100, 15000);
			events.ScheduleEvent(101, 5000);
			events.ScheduleEvent(102, 30000);
			events.ScheduleEvent(103, 30000);

            SetCrystalsStatus(true);
            SetBubbled(true);
        }

        void AttackStart(Unit* target) OVERRIDE
        {
            if (!target)
                return;

            if (me->Attack(target, true))
                DoStartNoMovement(target);
        }

        void KilledUnit(Unit* who) OVERRIDE
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_KILL);
        }

        void JustDied(Unit* /*killer*/) OVERRIDE
        {
            _JustDied();
            Talk(SAY_DEATH);
        }

        void UpdateAI(uint32 const diff) OVERRIDE
        {
            if (!UpdateVictim() && !_bubbled)
                return;

            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
					case 100:
						if (Creature* summon = me->SummonCreature(27600, AddSpawnPoint, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20*IN_MILLISECONDS))
							summon->GetMotionMaster()->MovePath(NPC_NOVOS*10, false);
						events.ScheduleEvent(100, 15000);
						break;
					case 101:
						if (Creature* summon = me->SummonCreature(27598, AddSpawnPoint, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20*IN_MILLISECONDS))
							summon->GetMotionMaster()->MovePath(NPC_NOVOS*10, false);
						events.ScheduleEvent(101, 5000);
						break;
					case 102:
						if (Creature* summon = me->SummonCreature(27597, AddSpawnPoint, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20*IN_MILLISECONDS))
							summon->GetMotionMaster()->MovePath(NPC_NOVOS*10, false);
						events.ScheduleEvent(102, 30000);
						break;
					case 103:
						DoZoneInCombat(me->SummonCreature(26627, CrystalHandlerSpawnPoint, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20*IN_MILLISECONDS));
						events.ScheduleEvent(103, 30000);
						break;
                    case EVENT_SUMMON_MINIONS:
                        DoCast(SPELL_SUMMON_MINIONS);
                        events.ScheduleEvent(EVENT_SUMMON_MINIONS, 15000);
                        break;
                    case EVENT_ATTACK:
                        if (Unit* victim = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(victim, RAND(SPELL_ARCANE_BLAST, SPELL_BLIZZARD, SPELL_FROSTBOLT, SPELL_WRATH_OF_MISERY));
                        events.ScheduleEvent(EVENT_ATTACK, 3000);
                        break;
                    default:
                        break;
                }
            }
        }

        void DoAction(int32 const action) OVERRIDE
        {
            if (action == ACTION_CRYSTAL_HANDLER_DIED)
                CrystalHandlerDied();

        }

		void JustSummoned(Creature* summon)
		{
			summons.Summon(summon);
		}

        void MoveInLineOfSight(Unit* who) OVERRIDE
        {
            BossAI::MoveInLineOfSight(who);

			if (_bubbled || !_ohNovos || !who || who->GetTypeId() != TYPEID_UNIT || who->GetPositionY() > MAX_Y_COORD_OH_NOVOS)
                return;

            uint32 entry = who->GetEntry();
            if (entry == NPC_HULKING_CORPSE || entry == NPC_RISEN_SHADOWCASTER || entry == NPC_FETID_TROLL_CORPSE)
                _ohNovos = false;
        }

        uint32 GetData(uint32 type) const OVERRIDE
        {
            return type == DATA_NOVOS_ACHIEV && _ohNovos ? 1 : 0;
        }

    private:
        void SetBubbled(bool state)
        {
            _bubbled = state;
            if (!state)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    me->CastStop();
            }
            else
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                DoCast(SPELL_ARCANE_FIELD);
            }
        }

        void SetCrystalsStatus(bool active)
        {
			for (uint8 i = 0; i < 4; i++)
				//if (uint64 guid = instance->GetData64(DATA_NOVOS_CRYSTAL_1 + i))
					//if (GameObject* crystal = instance->instance->GetGameObject(guid))
					if (GameObject* crystal = me->FindNearestGameObject(GO_NOVOS_CRYSTAL_1 + i, 50.0f))
						SetCrystalStatus(crystal, active);
        }

        void SetCrystalStatus(GameObject* crystal, bool active)
        {
            crystal->SetGoState(active ? GO_STATE_ACTIVE : GO_STATE_READY);
            if (Creature* crystalChannelTarget = crystal->FindNearestCreature(NPC_CRYSTAL_CHANNEL_TARGET, 50.0f))
            {
                if (active)
					crystalChannelTarget->AI()->DoCastAOE(SPELL_BEAM_CHANNEL);
                else if (crystalChannelTarget->HasUnitState(UNIT_STATE_CASTING))
					crystalChannelTarget->CastStop();
            }
        }

        void CrystalHandlerDied()
        {
			if (GameObject* crystal = me->FindNearestGameObject(GO_NOVOS_CRYSTAL_1 + _crystalHandlerCount, 50.0f))
            //if (uint64 guid = instance->GetData64(DATA_NOVOS_CRYSTAL_1 + _crystalHandlerCount))
                //if (GameObject* crystal = instance->instance->GetGameObject(guid))
                    if (crystal->GetGoState() == GO_STATE_ACTIVE)
                        SetCrystalStatus(crystal, false);

            if (++_crystalHandlerCount >= 4)
            {
				events.Reset();
                Talk(SAY_ARCANE_FIELD);
                SetBubbled(false);
				SetCrystalsStatus(false);
                events.ScheduleEvent(EVENT_ATTACK, 3000);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SUMMON_MINIONS, 15000);
            }
        }

        uint8 _crystalHandlerCount;
        bool _ohNovos;
        bool _bubbled;
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_novosAI(creature);
    }
};

struct npc_novos_spells
{
	uint32 entry;
	uint32 spellN;
	uint32 spellH;
	uint32 CDmin;
	uint32 CDmax;
};

static npc_novos_spells npcSpells[2] = 
{
	{27600, 51363, 59016, 5000, 7500}, 
	{27597, 60019, 60019, 10000, 15000}
};

class npc_novos_stair_add : public CreatureScript
{
public:
    npc_novos_stair_add() : CreatureScript("npc_novos_stair_add") { }

	struct npc_novos_stair_addAI : public AggressorAI
    {
        npc_novos_stair_addAI(Creature* creature) : AggressorAI(creature) { }

		EventMap events;
		
		void MovementInform(uint32 type, uint32 point)
		{
			if (point == 5)
				DoZoneInCombat();
		}

		void EnterCombat(Unit* who)
		{
			me->GetMotionMaster()->Clear();
			me->GetMotionMaster()->MovementExpired();
			me->GetMotionMaster()->MoveChase(who);

			AggressorAI::EnterCombat(who);

			for (int i = 0; i < 3; i++)
			{
				if (npcSpells[i].entry == me->GetEntry())
					events.ScheduleEvent(i+1, urand(npcSpells[i].CDmin, npcSpells[i].CDmax));
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (uint32 eventId = events.ExecuteEvent())
			{
				DoCastVictim(me->GetMap()->IsHeroic() ? npcSpells[eventId - 1].spellH : npcSpells[eventId - 1].spellN);

				events.ScheduleEvent(eventId, urand(npcSpells[eventId - 1].CDmin, npcSpells[ eventId - 1].CDmax));
			}
			else
				DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new npc_novos_stair_addAI(creature);
    }
};

class achievement_oh_novos : public AchievementCriteriaScript
{
public:
    achievement_oh_novos() : AchievementCriteriaScript("achievement_oh_novos") { }

    bool OnCheck(Player* /*player*/, Unit* target) OVERRIDE
    {
        return target && target->GetTypeId() == TYPEID_UNIT && target->ToCreature()->AI()->GetData(DATA_NOVOS_ACHIEV);
    }
};

class spell_novos_summon_minions : public SpellScriptLoader
{
    public:
        spell_novos_summon_minions() : SpellScriptLoader("spell_novos_summon_minions") { }

        class spell_novos_summon_minions_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_novos_summon_minions_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_COPY_OF_MINIONS))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                for (uint8 i = 0; i < 5; ++i)
                    GetCaster()->CastSpell((Unit*)NULL, SPELL_SUMMON_COPY_OF_MINIONS, true);
            }

            void Register() OVERRIDE
            {
                OnEffectHitTarget += SpellEffectFn(spell_novos_summon_minions_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const OVERRIDE
        {
            return new spell_novos_summon_minions_SpellScript();
        }
};

void AddSC_boss_novos()
{
    new boss_novos();
    new npc_novos_stair_add();
    new spell_novos_summon_minions();
    new achievement_oh_novos();
}
