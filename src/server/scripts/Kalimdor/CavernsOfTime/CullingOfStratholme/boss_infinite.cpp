/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
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
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_CORRUPTING_BLIGHT                     = 60588,
    SPELL_VOID_STRIKE                           = 60590
};

enum Yells
{
    SAY_AGGRO                                   = -1595045,
    SAY_FAIL                                    = -1595046,
    SAY_DEATH                                   = -1595047
};

enum Achievements
{
    ACHIV_CULLING_OF_TIME                       = 1817,
};

class boss_infinite_corruptor : public CreatureScript
{
public:
    boss_infinite_corruptor() : CreatureScript("boss_infinite_corruptor") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_infinite_corruptorAI(creature);
    }

    struct boss_infinite_corruptorAI : public ScriptedAI
    {
        boss_infinite_corruptorAI(Creature *c) : ScriptedAI(c)
        {
            instance = c->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 uiBlightTimer;
        uint32 uiEscapeTimer;
        uint32 uiVoidStrikeTimer;

        bool bEscaping;

        void Reset()
        {
            uiEscapeTimer = 2000;
            uiBlightTimer = urand(7000, 9000);
            uiVoidStrikeTimer = urand(6000, 10000);

            me->SetReactState(REACT_AGGRESSIVE);
            bEscaping = false;

            if (instance)
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiEscapeTimer <= diff)
            {
                if (!bEscaping && !instance->GetData(DATA_COUNTDOWN))
                {
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->MovePoint(0, 2335.93f, 1278.89f, 132.89f);
                    bEscaping = true;
                }
                uiEscapeTimer = 2000;
            } else uiEscapeTimer -= diff;

            if (bEscaping)
                if (me->GetDistance(2335.93f, 1278.89f, 132.89f) < 1.0f)
        {
            if (instance)
                    {
                        instance->SetData(DATA_INFINITE_EVENT, DONE);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);
                    }
                    me->DisappearAndDie();
                }

            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (uiBlightTimer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(pTarget, SPELL_CORRUPTING_BLIGHT, false);
                uiBlightTimer = urand(7000, 9000);
            } else uiBlightTimer -= diff;

            if (uiVoidStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_VOID_STRIKE, false);
                uiVoidStrikeTimer = urand(6000, 10000);
            } else uiVoidStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
            {
                instance->SetData(DATA_INFINITE_EVENT, DONE);
                instance->DoCompleteAchievement(ACHIV_CULLING_OF_TIME);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_BLIGHT);
            }
        }
    };
};


void AddSC_boss_infinite_corruptor()
{
    new boss_infinite_corruptor();
}
