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
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Player.h"

enum BWLEncounter
{
    BOSS_RAZORGORE              = 0,
    BOSS_VAELASTRAZ             = 1,
    BOSS_BROODLORD              = 2,
    BOSS_FIREMAW                = 3,
    BOSS_EBONROC                = 4,
    BOSS_FLAMEGOR               = 5,
    BOSS_CHROMAGGUS             = 6,
    BOSS_NEFARIAN               = 7
};

enum CreatureIds
{
    NPC_RAZORGORE               = 12435,
    NPC_BLACKWING_DRAGON        = 12422,
    NPC_BLACKWING_TASKMASTER    = 12458,
    NPC_BLACKWING_LEGIONAIRE    = 12416,
    NPC_BLACKWING_WARLOCK       = 12459,
    NPC_VAELASTRAZ              = 13020,
    NPC_BROODLORD               = 12017,
    NPC_FIRENAW                 = 11983,
    NPC_EBONROC                 = 14601,
    NPC_FLAMEGOR                = 11981,
    NPC_CHROMAGGUS              = 14020,
    NPC_VICTOR_NEFARIUS         = 10162,
    NPC_NEFARIAN                = 11583
};

enum Events
{
    // Victor Nefarius
    EVENT_SPAWN_ADD            = 1,
    EVENT_SHADOW_BOLT          = 2,
    EVENT_FEAR                 = 3,
    EVENT_MIND_CONTROL         = 4,
    // Nefarian
    EVENT_SHADOWFLAME          = 5,
    EVENT_VEILOFSHADOW         = 6,
    EVENT_CLEAVE               = 7,
    EVENT_TAILLASH             = 8,
    EVENT_CLASSCALL            = 9,
    // UBRS
    EVENT_CHAOS_1              = 10,
    EVENT_CHAOS_2              = 11,
    EVENT_PATH_2               = 12,
    EVENT_PATH_3               = 13,
    EVENT_SUCCESS_1            = 14,
    EVENT_SUCCESS_2            = 15,
    EVENT_SUCCESS_3            = 16,
};

enum Says
{
    // Nefarius
    // UBRS
    SAY_CHAOS_SPELL            = 9,
    SAY_SUCCESS                = 10,
    SAY_FAILURE                = 11,
    // BWL
    SAY_GAMESBEGIN_1           = 12,
    SAY_GAMESBEGIN_2           = 13,
 // SAY_VAEL_INTRO             = 14, Not used - when he corrupts Vaelastrasz

    // Nefarian
    SAY_RANDOM                 = 0,
    SAY_RAISE_SKELETONS        = 1,
    SAY_SLAY                   = 2,
    SAY_DEATH                  = 3,

    SAY_MAGE                   = 4,
    SAY_WARRIOR                = 5,
    SAY_DRUID                  = 6,
    SAY_PRIEST                 = 7,
    SAY_PALADIN                = 8,
    SAY_SHAMAN                 = 9,
    SAY_WARLOCK                = 10,
    SAY_HUNTER                 = 11,
    SAY_ROGUE                  = 12,
    SAY_DEATH_KNIGHT           = 13
};

enum Gossip
{
   GOSSIP_ID                   = 21332,
   GOSSIP_OPTION_ID            = 0
};

enum Paths
{
    NEFARIUS_PATH_2            = 1379671,
    NEFARIUS_PATH_3            = 1379672
};

enum GameObjects
{
    GO_PORTCULLIS_ACTIVE       = 164726,
    GO_PORTCULLIS_TOBOSSROOMS  = 175186
};

enum Creatures
{
    NPC_BRONZE_DRAKANOID       = 14263,
    NPC_BLUE_DRAKANOID         = 14261,
    NPC_RED_DRAKANOID          = 14264,
    NPC_GREEN_DRAKANOID        = 14262,
    NPC_BLACK_DRAKANOID        = 14265,
    NPC_CHROMATIC_DRAKANOID    = 14302,
    NPC_BONE_CONSTRUCT         = 14605,
    // UBRS
    NPC_GYTH                   = 10339
};

enum Spells
{
    // Victor Nefarius
    // UBRS Spells
    SPELL_CHROMATIC_CHAOS       = 16337, // Self Cast hits 10339
    SPELL_VAELASTRASZZ_SPAWN    = 16354, // Self Cast Depawn one sec after
    // BWL Spells
    SPELL_SHADOWBOLT            = 22677,
    SPELL_SHADOWBOLT_VOLLEY     = 22665,
    SPELL_SHADOW_COMMAND        = 22667,
    SPELL_FEAR                  = 22678,

    SPELL_NEFARIANS_BARRIER     = 22663,

    // Nefarian
    SPELL_SHADOWFLAME_INITIAL   = 22992,
    SPELL_SHADOWFLAME           = 22539,
    SPELL_BELLOWINGROAR         = 22686,
    SPELL_VEILOFSHADOW          = 7068,
    SPELL_CLEAVE                = 20691,
    SPELL_TAILLASH              = 23364,

    SPELL_MAGE                  = 23410,     // wild magic
    SPELL_WARRIOR               = 23397,     // beserk
    SPELL_DRUID                 = 23398,     // cat form
    SPELL_PRIEST                = 23401,     // corrupted healing
    SPELL_PALADIN               = 23418,     // syphon blessing
    SPELL_SHAMAN                = 23425,     // totems
    SPELL_WARLOCK               = 23427,     // infernals
    SPELL_HUNTER                = 23436,     // bow broke
    SPELL_ROGUE                 = 23414,     // Paralise
    SPELL_DEATH_KNIGHT          = 49576      // Death Grip

// 19484
// 22664
// 22674
// 22666
};

Position const DrakeSpawnLoc[2] = // drakonid
{
    {-7591.151855f, -1204.051880f, 476.800476f, 3.0f},
    {-7514.598633f, -1150.448853f, 476.796570f, 3.0f}
};

Position const NefarianLoc[2] =
{
    {-7449.763672f, -1387.816040f, 526.783691f, 3.0f}, // nefarian spawn
    {-7535.456543f, -1279.562500f, 476.798706f, 3.0f}  // nefarian move
};

uint32 const Entry[5] = {NPC_BRONZE_DRAKANOID, NPC_BLUE_DRAKANOID, NPC_RED_DRAKANOID, NPC_GREEN_DRAKANOID, NPC_BLACK_DRAKANOID};

class boss_victor_nefarius : public CreatureScript
{
public:
    boss_victor_nefarius() : CreatureScript("boss_victor_nefarius") { }

    struct boss_victor_nefariusAI : public BossAI
    {
        boss_victor_nefariusAI(Creature* creature) : BossAI(creature, BOSS_NEFARIAN) { }

        void Reset()
        {
            if (me->GetMapId() == 469)
            {
                if (!me->FindNearestCreature(NPC_NEFARIAN, 1000.0f, true))
                    _Reset();
                SpawnedAdds = 0;

                me->SetVisible(true);
                me->SetPhaseMask(1, true);
                me->SetUInt32Value(UNIT_NPC_FLAGS, 1);
                me->setFaction(35);
                me->SetStandState(UNIT_STAND_STATE_SIT_HIGH_CHAIR);
                me->RemoveAura(SPELL_NEFARIANS_BARRIER);
            }
        }

        void JustReachedHome()
        {
            Reset();
        }

        void BeginEvent(Player* target)
        {
            _EnterCombat();

            Talk(SAY_GAMESBEGIN_2);

            me->setFaction(103);
            me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            DoCast(me, SPELL_NEFARIANS_BARRIER);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            AttackStart(target);
            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(3000, 10000));
            events.ScheduleEvent(EVENT_FEAR, urand(10000, 20000));
            //events.ScheduleEvent(EVENT_MIND_CONTROL, urand(30000, 35000));
            events.ScheduleEvent(EVENT_SPAWN_ADD, 10000);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            if (summon->GetEntry() != NPC_NEFARIAN)
            {
                summon->UpdateEntry(NPC_BONE_CONSTRUCT);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                summon->SetReactState(REACT_PASSIVE);
                summon->SetStandState(UNIT_STAND_STATE_DEAD);
            }
        }

        void JustSummoned(Creature* /*summon*/) {}

        void SetData(uint32 type, uint32 data)
        {
            if (instance && type == 1 && data == 1)
            {
                me->StopMoving();
                events.ScheduleEvent(EVENT_PATH_2, 9000);
            }

            if (instance && type == 1 && data == 2)
            {
                events.ScheduleEvent(EVENT_SUCCESS_1, 5000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PATH_2:
                            me->GetMotionMaster()->MovePath(NEFARIUS_PATH_2, false);
                            events.ScheduleEvent(EVENT_CHAOS_1, 7000);
                            break;
                        case EVENT_CHAOS_1:
                            if (Creature* gyth = me->FindNearestCreature(NPC_GYTH, 75.0f, true))
                            {
                                me->SetFacingToObject(gyth);
                                Talk(SAY_CHAOS_SPELL);
                            }
                            events.ScheduleEvent(EVENT_CHAOS_2, 2000);
                            break;
                        case EVENT_CHAOS_2:
                            DoCast(SPELL_CHROMATIC_CHAOS);
                            me->SetFacingTo(1.570796f);
                            break;
                        case EVENT_SUCCESS_1:
                            if (Unit* player = me->SelectNearestPlayer(60.0f))
                            {
                                me->SetInFront(player);
                                me->SendMovementFlagUpdate();
                                Talk(SAY_SUCCESS);
                                if (GameObject* portcullis1 = me->FindNearestGameObject(GO_PORTCULLIS_ACTIVE, 65.0f))
                                    portcullis1->SetGoState(GO_STATE_ACTIVE);
                                if (GameObject* portcullis2 = me->FindNearestGameObject(GO_PORTCULLIS_TOBOSSROOMS, 80.0f))
                                    portcullis2->SetGoState(GO_STATE_ACTIVE);
                            }
                            events.ScheduleEvent(EVENT_SUCCESS_2, 4000);
                            break;
                        case EVENT_SUCCESS_2:
                            DoCast(me, SPELL_VAELASTRASZZ_SPAWN);
                            me->DespawnOrUnsummon(1000);
                            break;
                        case EVENT_PATH_3:
                            me->GetMotionMaster()->MovePath(NEFARIUS_PATH_3, false);
                            break;
                        default:
                            break;
                    }
                }
                return;
            }

            // Only do this if we haven't spawned nefarian yet
            if (UpdateVictim() && SpawnedAdds <= 42)
            {
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_BOLT:
                            switch (urand(0, 1))
                            {
                                case 0:
                                    DoCastVictim(SPELL_SHADOWBOLT_VOLLEY);
                                    break;
                                case 1:
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
                                        DoCast(target, SPELL_SHADOWBOLT);
                                    break;
                            }
                            DoResetThreat();
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(3000, 10000));
                            break;
                        case EVENT_FEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
                                DoCast(target, SPELL_FEAR);
                            events.ScheduleEvent(EVENT_FEAR, urand(10000, 20000));
                            break;
                        case EVENT_MIND_CONTROL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
                                DoCast(target, SPELL_SHADOW_COMMAND);
                            events.ScheduleEvent(EVENT_MIND_CONTROL, urand(30000, 35000));
                            break;
                        case EVENT_SPAWN_ADD:
                            for (uint8 i=0; i<2; ++i)
                            {
                                uint32 CreatureID;
                                if (urand(0, 2) == 0)
                                    CreatureID = NPC_CHROMATIC_DRAKANOID;
                                else
                                    CreatureID = Entry[urand(0, 4)];
                                if (Creature* dragon = me->SummonCreature(CreatureID, DrakeSpawnLoc[i]))
                                {
                                    dragon->setFaction(103);
                                    dragon->AI()->AttackStart(me->getVictim());
                                }

                                if (++SpawnedAdds >= 42)
                                {
                                    if (Creature* nefarian = me->SummonCreature(NPC_NEFARIAN, NefarianLoc[0]))
                                    {
                                        nefarian->setActive(true);
                                        nefarian->SetCanFly(true);
                                        nefarian->SetDisableGravity(true);
                                        nefarian->AI()->DoCastAOE(SPELL_SHADOWFLAME_INITIAL);
                                        nefarian->GetMotionMaster()->MovePoint(1, NefarianLoc[1]);
                                    }
                                    events.CancelEvent(EVENT_MIND_CONTROL);
                                    events.CancelEvent(EVENT_FEAR);
                                    events.CancelEvent(EVENT_SHADOW_BOLT);
                                    me->SetVisible(false);
                                    return;
                                }
                            }
                            events.ScheduleEvent(EVENT_SPAWN_ADD, 4000);
                            break;
                    }
                }
            }
        }

        void sGossipSelect(Player* player, uint32 sender, uint32 action)
        {
            if (sender == GOSSIP_ID && action == GOSSIP_OPTION_ID)
            {
                player->CLOSE_GOSSIP_MENU();
                Talk(SAY_GAMESBEGIN_1);
                BeginEvent(player);
            }
        }

        private:
            uint32 SpawnedAdds;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_victor_nefariusAI(creature);
    }
};

void AddSC_boss_victor_nefarius()
{
    new boss_victor_nefarius();
}