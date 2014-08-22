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

/*
 * Comment: Missing AI for Twisted Visages
 */

#include "ScriptPCH.h"
#include "ahnkahet.h"

enum Spells
{
    SPELL_INSANITY                                = 57496, //Dummy
    INSANITY_VISUAL                               = 57561,
    SPELL_INSANITY_TARGET                         = 57508,
    SPELL_MIND_FLAY                               = 57941,
    SPELL_SHADOW_BOLT_VOLLEY                      = 57942,
    SPELL_SHIVER                                  = 57949,
    SPELL_CLONE_PLAYER                            = 57507, //casted on player during insanity
    SPELL_INSANITY_PHASING_1                      = 57508,
    SPELL_INSANITY_PHASING_2                      = 57509,
    SPELL_INSANITY_PHASING_3                      = 57510,
    SPELL_INSANITY_PHASING_4                      = 57511,
    SPELL_INSANITY_PHASING_5                      = 57512
};

enum Creatures
{
    MOB_TWISTED_VISAGE                            = 30625
};

//not in db
enum Yells
{
    SAY_AGGRO                                     = -1619030,
    SAY_SLAY_1                                    = -1619031,
    SAY_SLAY_2                                    = -1619032,
    SAY_SLAY_3                                    = -1619033,
    SAY_DEATH_1                                   = -1619034,
    SAY_DEATH_2                                   = -1619035,
    SAY_PHASE                                     = -1619036
};

enum Achievements
{
    ACHIEV_QUICK_DEMISE_START_EVENT               = 20382,
};

class boss_volazj : public CreatureScript
{
public:
    boss_volazj() : CreatureScript("boss_volazj") { }

    struct boss_volazjAI : public ScriptedAI
    {
        boss_volazjAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 uiMindFlayTimer;
        uint32 uiShadowBoltVolleyTimer;
        uint32 uiShiverTimer;
        bool insanityHandled;
		uint32 insanityCount;
        SummonList Summons;

        void DamageTaken(Unit* /*pAttacker*/, uint32 &damage)
        {
            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                damage = 0;
			
			if (insanityCount == 0 && me->HealthBelowPct(66) || insanityCount == 1 && me->HealthBelowPct(33))
			{
				insanityCount++;
                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_INSANITY, false);
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_INSANITY)
            {
				if (!insanityHandled)
				{
					insanityHandled = true;
					DoCast(me, INSANITY_VISUAL, true);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetControlled(true, UNIT_STATE_STUNNED);
					
					Map::PlayerList const &players = me->GetMap()->GetPlayers();
					uint8 j = 0;
					for (Map::PlayerList::const_iterator ite1 = players.begin(); ite1 != players.end(); ++ite1)
					{
						Player* player = ite1->getSource();
						if (!player || !player->isAlive())
							continue;
						
						player->CastSpell(player, SPELL_INSANITY_TARGET+j, true);
						
						for (Map::PlayerList::const_iterator ite2 = players.begin(); ite2 != players.end(); ++ite2)
						{
							Player* partyMember = ite2->getSource();
							if (player != partyMember)
							{
								// Summon clone
								if (Creature* summon = me->SummonCreature(MOB_TWISTED_VISAGE, partyMember->GetPositionX(), partyMember->GetPositionY(), partyMember->GetPositionZ(), partyMember->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 0))
								{
									// clone
									// set phase
									summon->SetPhaseMask((1<<(4+j)), true);
									summon->AI()->DoZoneInCombat();
									partyMember->AddAura(SPELL_CLONE_PLAYER, summon);
								}
							}
						}
						j++;
					}
				}
            }
        }

        void ResetPlayersPhaseMask()
        {
            Map::PlayerList const &players = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                Player* player = i->getSource();
				for (uint8 j = 0; j < 5; j++)
					player->RemoveAurasDueToSpell(SPELL_INSANITY_PHASING_1+j);
            }
        }

        void Reset()
        {
            uiMindFlayTimer = 8*IN_MILLISECONDS;
            uiShadowBoltVolleyTimer = 5*IN_MILLISECONDS;
            uiShiverTimer = 15*IN_MILLISECONDS;

            if (instance)
            {
                instance->SetData(DATA_HERALD_VOLAZJ, NOT_STARTED);
                instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_QUICK_DEMISE_START_EVENT);
            }

            // Visible for all players in insanity
            me->SetPhaseMask((1|16|32|64|128|256), true);
			insanityCount = 0;
			insanityHandled = false;

            ResetPlayersPhaseMask();

            // Cleanup
            Summons.DespawnAll();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetControlled(false, UNIT_STATE_STUNNED);
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoScriptText(SAY_AGGRO, me);

            if (instance)
            {
                instance->SetData(DATA_HERALD_VOLAZJ, IN_PROGRESS);
                instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_QUICK_DEMISE_START_EVENT);
            }
        }

        void JustSummoned(Creature* summon)
        {
            Summons.Summon(summon);
        }

        uint32 GetSpellForPhaseMask(uint32 phase)
        {
            uint32 spell = 0;
            switch (phase)
            {
                case 16:
                    spell = SPELL_INSANITY_PHASING_1;
                    break;
                case 32:
                    spell = SPELL_INSANITY_PHASING_2;
                    break;
                case 64:
                    spell = SPELL_INSANITY_PHASING_3;
                    break;
                case 128:
                    spell = SPELL_INSANITY_PHASING_4;
                    break;
                case 256:
                    spell = SPELL_INSANITY_PHASING_5;
                    break;
            }
            return spell;
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            uint32 phase = summon->GetPhaseMask();
            uint32 nextPhase = 0;
            Summons.Despawn(summon);

            // Check if all summons in this phase killed
            for (SummonList::const_iterator iter = Summons.begin(); iter != Summons.end(); ++iter)
            {
                if (Creature* visage = Unit::GetCreature(*me, *iter))
                {
                    // Not all are dead
                    if (phase == visage->GetPhaseMask())
                        return;
                    else
                        nextPhase = visage->GetPhaseMask();
                }
            }

            // Roll Insanity
            uint32 spell = GetSpellForPhaseMask(phase);
            uint32 spell2 = GetSpellForPhaseMask(nextPhase);

            Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
            if (!PlayerList.isEmpty())
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (Player* player = i->getSource())
                        if (player->HasAura(spell))
                        {
                            player->RemoveAurasDueToSpell(spell);
                            if (spell2) // if there is still some different mask cast spell for it
                                player->CastSpell(player, spell2, true);
                        }
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (insanityHandled)
            {
                if (!Summons.empty())
                    return;

                insanityHandled = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetControlled(false, UNIT_STATE_STUNNED);
                me->RemoveAurasDueToSpell(INSANITY_VISUAL);
				ResetPlayersPhaseMask();
            }

            if (uiMindFlayTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_MIND_FLAY);
                uiMindFlayTimer = 20*IN_MILLISECONDS;
            } else uiMindFlayTimer -= diff;

            if (uiShadowBoltVolleyTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
                uiShadowBoltVolleyTimer = 5*IN_MILLISECONDS;
            } else uiShadowBoltVolleyTimer -= diff;

            if (uiShiverTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_SHIVER);
                uiShiverTimer = 15*IN_MILLISECONDS;
            } else uiShiverTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(SAY_DEATH_1, me);

            if (instance)
                instance->SetData(DATA_HERALD_VOLAZJ, DONE);

            Summons.DespawnAll();
            ResetPlayersPhaseMask();
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_volazjAI(creature);
    }
};

void AddSC_boss_volazj()
{
    new boss_volazj;
}
