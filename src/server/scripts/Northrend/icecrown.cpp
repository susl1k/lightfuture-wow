/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
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
SDName: Icecrown
SD%Complete: 100
SDComment: Quest support: 12807
SDCategory: Icecrown
EndScriptData */

/* ContentData
npc_arete
EndContentData */

#include "ScriptPCH.h"
#include "Vehicle.h"
#include "GameObjectAI.h"
#include "MoveSplineInit.h"
#include "MoveSplineInitArgs.h"
#include "ScriptedEscortAI.h"

/*######
## npc_arete
######*/

#define GOSSIP_ARETE_ITEM1 "Lord-Commander, I would hear your tale."
#define GOSSIP_ARETE_ITEM2 "<You nod slightly but do not complete the motion as the Lord-Commander narrows his eyes before he continues.>"
#define GOSSIP_ARETE_ITEM3 "I thought that they now called themselves the Scarlet Onslaught?"
#define GOSSIP_ARETE_ITEM4 "Where did the grand admiral go?"
#define GOSSIP_ARETE_ITEM5 "That's fine. When do I start?"
#define GOSSIP_ARETE_ITEM6 "Let's finish this!"
#define GOSSIP_ARETE_ITEM7 "That's quite a tale, Lord-Commander."

enum eArete
{
    GOSSIP_TEXTID_ARETE1        = 13525,
    GOSSIP_TEXTID_ARETE2        = 13526,
    GOSSIP_TEXTID_ARETE3        = 13527,
    GOSSIP_TEXTID_ARETE4        = 13528,
    GOSSIP_TEXTID_ARETE5        = 13529,
    GOSSIP_TEXTID_ARETE6        = 13530,
    GOSSIP_TEXTID_ARETE7        = 13531,

    QUEST_THE_STORY_THUS_FAR    = 12807
};

class npc_arete : public CreatureScript
{
public:
    npc_arete() : CreatureScript("npc_arete") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_THE_STORY_THUS_FAR) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE1, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE4, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE5, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE6, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE7, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+7:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_THE_STORY_THUS_FAR);
                break;
        }

        return true;
    }
};

/*######
## npc_dame_evniki_kapsalis
######*/

enum eDameEnvikiKapsalis
{
    TITLE_CRUSADER    = 123
};

class npc_dame_evniki_kapsalis : public CreatureScript
{
public:
    npc_dame_evniki_kapsalis() : CreatureScript("npc_dame_evniki_kapsalis") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasTitle(TITLE_CRUSADER))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());
        return true;
    }
};

/*######
## npc_squire_david
######*/

enum eSquireDavid
{
    QUEST_THE_ASPIRANT_S_CHALLENGE_H                    = 13680,
    QUEST_THE_ASPIRANT_S_CHALLENGE_A                    = 13679,

    NPC_ARGENT_VALIANT                                  = 33448,

    GOSSIP_TEXTID_SQUIRE                                = 14407
};

#define GOSSIP_SQUIRE_ITEM_1 "I am ready to fight!"
#define GOSSIP_SQUIRE_ITEM_2 "How do the Argent Crusader raiders fight?"

class npc_squire_david : public CreatureScript
{
public:
    npc_squire_david() : CreatureScript("npc_squire_david") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_THE_ASPIRANT_S_CHALLENGE_H) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_THE_ASPIRANT_S_CHALLENGE_A) == QUEST_STATUS_INCOMPLETE)//We need more info about it.
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        }

        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_SQUIRE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->SummonCreature(NPC_ARGENT_VALIANT, 8575.451f, 952.472f, 547.554f, 0.38f);
        }
        return true;
    }
};

/*######
## npc_argent_valiant
######*/

enum eArgentValiant
{
    SPELL_CHARGE                = 63010,
    SPELL_SHIELD_BREAKER        = 65147,
    SPELL_KILL_CREDIT           = 63049,
    SPELL_THRUST                = 62544,
    SPELL_DEFEND                = 62719,

    NPC_ARGENT_VALIANT_CREDIT   = 24108,
    SPELL_GIVE_KILL_CREDIT_VALIANT = 63049,
};

class npc_argent_valiant : public CreatureScript
{
public:
    npc_argent_valiant() : CreatureScript("npc_argent_valiant") { }

    struct npc_argent_valiantAI : public ScriptedAI
    {
        npc_argent_valiantAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->GetMotionMaster()->MovePoint(0, 8599.258f, 963.951f, 547.553f);
            creature->setFaction(35); //wrong faction in db?
        }

        uint32 uiChargeTimer;
        uint32 uiShieldBreakerTimer;

        void Reset()
        {
            uiChargeTimer = 7000;
            uiShieldBreakerTimer = 10000;
        }

        void MovementInform(uint32 uiType, uint32 /*uiId*/)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            me->setFaction(14);
        }

        void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
        {
            if (uiDamage > me->GetHealth() && pDoneBy->GetTypeId() == TYPEID_PLAYER)
            {
                uiDamage = 0;
                CAST_PLR(pDoneBy)->KilledMonsterCredit(NPC_ARGENT_VALIANT_CREDIT, 0);
                pDoneBy->CastSpell(pDoneBy, SPELL_GIVE_KILL_CREDIT_VALIANT, 0);
                me->setFaction(35);
                me->DespawnOrUnsummon(5000);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (uiChargeTimer <= uiDiff)
            {
                DoCastVictim(SPELL_CHARGE);
                uiChargeTimer = 7000;
            } else uiChargeTimer -= uiDiff;

            if (uiShieldBreakerTimer <= uiDiff)
            {
                DoCastVictim(SPELL_SHIELD_BREAKER);
                uiShieldBreakerTimer = 10000;
            } else uiShieldBreakerTimer -= uiDiff;

            if (me->isAttackReady())
            {
                DoCast(me->getVictim(), SPELL_THRUST, true);
                me->resetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_valiantAI(creature);
    }
};

/*######
## npc_alorah_and_grimmin
######*/

enum ealorah_and_grimmin
{
    SPELL_CHAIN                     = 68341,
    NPC_FJOLA_LIGHTBANE             = 36065,
    NPC_EYDIS_DARKBANE              = 36066,
    NPC_PRIESTESS_ALORAH            = 36101,
    NPC_PRIEST_GRIMMIN              = 36102
};

class npc_alorah_and_grimmin : public CreatureScript
{
public:
    npc_alorah_and_grimmin() : CreatureScript("npc_alorah_and_grimmin") { }

    struct npc_alorah_and_grimminAI : public ScriptedAI
    {
        npc_alorah_and_grimminAI(Creature* creature) : ScriptedAI(creature) {}

        bool uiCast;

        void Reset()
        {
            uiCast = false;
        }

        void UpdateAI(const uint32 /*uiDiff*/)
        {
            if (uiCast)
                return;
            uiCast = true;
            Creature* target = NULL;

            switch (me->GetEntry())
            {
                case NPC_PRIESTESS_ALORAH:
                    target = me->FindNearestCreature(NPC_EYDIS_DARKBANE, 10.0f);
                    break;
                case NPC_PRIEST_GRIMMIN:
                    target = me->FindNearestCreature(NPC_FJOLA_LIGHTBANE, 10.0f);
                    break;
            }
            if (target)
                DoCast(target, SPELL_CHAIN);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alorah_and_grimminAI(creature);
    }
};

/*######
## npc_guardian_pavilion
######*/

enum eGuardianPavilion
{
    SPELL_TRESPASSER_H                            = 63987,
    AREA_SUNREAVER_PAVILION                       = 4676,

    AREA_SILVER_COVENANT_PAVILION                 = 4677,
    SPELL_TRESPASSER_A                            = 63986,
};

class npc_guardian_pavilion : public CreatureScript
{
public:
    npc_guardian_pavilion() : CreatureScript("npc_guardian_pavilion") { }

    struct npc_guardian_pavilionAI : public Scripted_NoMovementAI
    {
        npc_guardian_pavilionAI(Creature* creature) : Scripted_NoMovementAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (me->GetAreaId() != AREA_SUNREAVER_PAVILION && me->GetAreaId() != AREA_SILVER_COVENANT_PAVILION)
                return;

            if (!who || who->GetTypeId() != TYPEID_PLAYER || !me->IsHostileTo(who) || !me->isInBackInMap(who, 5.0f))
                return;

            if (who->HasAura(SPELL_TRESPASSER_H) || who->HasAura(SPELL_TRESPASSER_A))
                return;

            if (who->ToPlayer()->GetTeamId() == TEAM_ALLIANCE)
                who->CastSpell(who, SPELL_TRESPASSER_H, true);
            else
                who->CastSpell(who, SPELL_TRESPASSER_A, true);

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_guardian_pavilionAI(creature);
    }
};

/*######
## npc_vereth_the_cunning
######*/

enum eVerethTheCunning
{
    NPC_GEIST_RETURN_BUNNY_KC   = 31049,
    NPC_LITHE_STALKER           = 30894,
    SPELL_SUBDUED_LITHE_STALKER = 58151,
};

class npc_vereth_the_cunning : public CreatureScript
{
public:
    npc_vereth_the_cunning() : CreatureScript("npc_vereth_the_cunning") { }

    struct npc_vereth_the_cunningAI : public ScriptedAI
    {
        npc_vereth_the_cunningAI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            ScriptedAI::MoveInLineOfSight(who);

            if (who->GetEntry() == NPC_LITHE_STALKER && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetCharmer())
                {
                    if (who->HasAura(SPELL_SUBDUED_LITHE_STALKER))
                    {
                        owner->ToPlayer()->KilledMonsterCredit(NPC_GEIST_RETURN_BUNNY_KC, 0);
                        who->ToCreature()->DisappearAndDie();
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vereth_the_cunningAI(creature);
    }
};

/*######
* npc_tournament_training_dummy
######*/
enum TournamentDummy
{
    NPC_CHARGE_TARGET         = 33272,
    NPC_MELEE_TARGET          = 33229,
    NPC_RANGED_TARGET         = 33243,

    SPELL_CHARGE_CREDIT       = 62658,
    SPELL_MELEE_CREDIT        = 62672,
    SPELL_RANGED_CREDIT       = 62673,

    SPELL_PLAYER_THRUST       = 62544,
    SPELL_PLAYER_BREAK_SHIELD = 62626,
    SPELL_PLAYER_CHARGE       = 62874,

    SPELL_RANGED_DEFEND       = 62719,
    SPELL_CHARGE_DEFEND       = 64100,
    SPELL_VULNERABLE          = 62665,

    SPELL_COUNTERATTACK       = 62709,

    EVENT_DUMMY_RECAST_DEFEND = 1,
    EVENT_DUMMY_RESET         = 2,
};

class npc_tournament_training_dummy : public CreatureScript
{
    public:
        npc_tournament_training_dummy(): CreatureScript("npc_tournament_training_dummy"){}

        struct npc_tournament_training_dummyAI : Scripted_NoMovementAI
        {
            npc_tournament_training_dummyAI(Creature* creature) : Scripted_NoMovementAI(creature) {}

            EventMap events;
            bool isVulnerable;

            void Reset()
            {
                me->SetControlled(true, UNIT_STATE_STUNNED);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                isVulnerable = false;

                // Cast Defend spells to max stack size
                switch (me->GetEntry())
                {
                    case NPC_CHARGE_TARGET:
                        DoCast(SPELL_CHARGE_DEFEND);
                        break;
                    case NPC_RANGED_TARGET:
                        me->CastCustomSpell(SPELL_RANGED_DEFEND, SPELLVALUE_AURA_STACK, 3, me);
                        break;
                }

                events.Reset();
                events.ScheduleEvent(EVENT_DUMMY_RECAST_DEFEND, 5000);
            }

            void EnterEvadeMode()
            {
                if (!_EnterEvadeMode())
                    return;

                Reset();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                damage = 0;
                events.RescheduleEvent(EVENT_DUMMY_RESET, 10000);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                switch (me->GetEntry())
                {
                    case NPC_CHARGE_TARGET:
                        if (spell->Id == SPELL_PLAYER_CHARGE)
                            if (isVulnerable)
                                DoCast(caster, SPELL_CHARGE_CREDIT, true);
                        break;
                    case NPC_MELEE_TARGET:
                        if (spell->Id == SPELL_PLAYER_THRUST)
                        {
                            DoCast(caster, SPELL_MELEE_CREDIT, true);

                            if (Unit* target = caster->GetVehicleBase())
                                DoCast(target, SPELL_COUNTERATTACK, true);
                        }
                        break;
                    case NPC_RANGED_TARGET:
                        if (spell->Id == SPELL_PLAYER_BREAK_SHIELD)
                            if (isVulnerable)
                                DoCast(caster, SPELL_RANGED_CREDIT, true);
                        break;
                }

                if (spell->Id == SPELL_PLAYER_BREAK_SHIELD)
                    if (!me->HasAura(SPELL_CHARGE_DEFEND) && !me->HasAura(SPELL_RANGED_DEFEND))
                        isVulnerable = true;
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_DUMMY_RECAST_DEFEND:
                        switch (me->GetEntry())
                        {
                            case NPC_CHARGE_TARGET:
                            {
                                if (!me->HasAura(SPELL_CHARGE_DEFEND))
                                    DoCast(SPELL_CHARGE_DEFEND);
                                break;
                            }
                            case NPC_RANGED_TARGET:
                            {
                                Aura* defend = me->GetAura(SPELL_RANGED_DEFEND);
                                if (!defend || defend->GetStackAmount() < 3 || defend->GetDuration() <= 8000)
                                    DoCast(SPELL_RANGED_DEFEND);
                                break;
                            }
                        }
                        isVulnerable = false;
                        events.ScheduleEvent(EVENT_DUMMY_RECAST_DEFEND, 5000);
                        break;
                    case EVENT_DUMMY_RESET:
                        if (UpdateVictim())
                        {
                            EnterEvadeMode();
                            events.ScheduleEvent(EVENT_DUMMY_RESET, 10000);
                        }
                        break;
                }

                if (!UpdateVictim())
                    return;

                if (!me->HasUnitState(UNIT_STATE_STUNNED))
                    me->SetControlled(true, UNIT_STATE_STUNNED);
            }

            void MoveInLineOfSight(Unit* /*who*/){}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tournament_training_dummyAI(creature);
        }

};

/*######
## npc_faction_valiant_champion
######*/

/*
UPDATE creature_template SET scriptname = 'npc_faction_valiant_champion' WHERE entry IN (33559,33562,33558,33564,33306,33285,33382,33561,33383,33384);
UPDATE creature_template SET scriptname = 'npc_faction_valiant_champion' WHERE entry IN (33738,33739,33740,33743,33744,33745,33746,33747,33748,33749);
*/

enum eFactionValiantChampion
{
    //SPELL_CHARGE                = 63010,
    //SPELL_SHIELD_BREAKER        = 65147,
    SPELL_REFRESH_MOUNT         = 66483,

    SPELL_GIVE_VALIANT_MARK_1   = 62724,
    SPELL_GIVE_VALIANT_MARK_2   = 62770,
    SPELL_GIVE_VALIANT_MARK_3   = 62771,
    SPELL_GIVE_VALIANT_MARK_4   = 62995,
    SPELL_GIVE_VALIANT_MARK_5   = 62996,

    SPELL_GIVE_CHAMPION_MARK    = 63596,

    QUEST_THE_GRAND_MELEE_0     = 13665,
    QUEST_THE_GRAND_MELEE_1     = 13745,
    QUEST_THE_GRAND_MELEE_2     = 13750,
    QUEST_THE_GRAND_MELEE_3     = 13756,
    QUEST_THE_GRAND_MELEE_4     = 13761,
    QUEST_THE_GRAND_MELEE_5     = 13767,
    QUEST_THE_GRAND_MELEE_6     = 13772,
    QUEST_THE_GRAND_MELEE_7     = 13777,
    QUEST_THE_GRAND_MELEE_8     = 13782,
    QUEST_THE_GRAND_MELEE_9     = 13787,

    QUEST_AMONG_THE_CHAMPIONS_0 = 13790,
    QUEST_AMONG_THE_CHAMPIONS_1 = 13793,
    QUEST_AMONG_THE_CHAMPIONS_2 = 13811,
    QUEST_AMONG_THE_CHAMPIONS_3 = 13814,

    SPELL_BESTED_DARNASSUS      = 64805,
    SPELL_BESTED_GNOMEREGAN     = 64809,
    SPELL_BESTED_IRONFORGE      = 64810,
    SPELL_BESTED_ORGRIMMAR      = 64811,
    SPELL_BESTED_SENJIN         = 64812,
    SPELL_BESTED_SILVERMOON     = 64813,
    SPELL_BESTED_STORMWIND      = 64814,
    SPELL_BESTED_EXODAR         = 64808,
    SPELL_BESTED_UNDERCITY      = 64816,
    SPELL_BESTED_THUNDERBLUFF   = 64815,

    SPELL_DEFEND_AURA_PERIODIC   = 64223, // 10sec
};

#define GOSSIP_MELEE_FIGHT      "I'am ready to fight!"

class npc_faction_valiant_champion : public CreatureScript
{
public:
    npc_faction_valiant_champion() : CreatureScript("npc_faction_valiant_champion") { }

    struct npc_faction_valiant_championAI : public ScriptedAI
    {
        npc_faction_valiant_championAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        uint32 uiChargeTimer;
        uint32 uiShieldBreakerTimer;
        uint64 guidAttacker;
        bool chargeing;

        void Reset()
        {
            uiChargeTimer = 7000;
            uiShieldBreakerTimer = 10000;

            me->setFaction(35);
        }

        void EnterCombat(Unit* attacker)
        {
            guidAttacker = attacker->GetGUID();
            DoCast(me,SPELL_DEFEND_AURA_PERIODIC,true);
            if(Aura* aur = me->AddAura(SPELL_DEFEND,me))
                aur->ModStackAmount(1);
        }

        void MovementInform(uint32 uiType, uint32 uiId)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            if(uiId != 1)
                return;

            chargeing = false;

            DoCastVictim(SPELL_CHARGE);
            if(me->getVictim())
                me->GetMotionMaster()->MoveChase(me->getVictim());
        }

        void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
        {
            if(pDoneBy && pDoneBy->GetGUID() != guidAttacker)
                uiDamage = 0;

            if (uiDamage > me->GetHealth() && pDoneBy->GetTypeId() == TYPEID_PLAYER)
            {
                uiDamage = 0;

                if(pDoneBy->HasAura(63034))
                {
                    switch(me->GetEntry())
                    {
                    case 33559: // Darnassus
                    case 33562: // Exodar
                    case 33558: // Gnomeregan
                    case 33564: // Ironforge
                    case 33306: // Orgrimmar
                    case 33285: // Sen'jin
                    case 33382: // Silvermoon
                    case 33561: // Stormwind
                    case 33383: // Thunder Bluff
                    case 33384: // Undercity
                        {
                            pDoneBy->CastSpell(pDoneBy,SPELL_GIVE_VALIANT_MARK_1,true);
                            break;
                        }
                    case 33738: // Darnassus
                    case 33739: // Exodar
                    case 33740: // Gnomeregan
                    case 33743: // Ironforge
                    case 33744: // Orgrimmar
                    case 33745: // Sen'jin
                    case 33746: // Silvermoon
                    case 33747: // Stormwind
                    case 33748: // Thunder Bluff
                    case 33749: // Undercity
                        {
                            pDoneBy->CastSpell(pDoneBy,SPELL_GIVE_CHAMPION_MARK,true);
                            break;
                        }
                    }

                    switch(me->GetEntry())
                    {
                        case 33738: // Darnassus
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_DARNASSUS,true); break;
                        case 33739: // Exodar
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_EXODAR,true); break;
                        case 33740: // Gnomeregan
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_GNOMEREGAN,true); break;
                        case 33743: // Ironforge
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_IRONFORGE,true); break;
                        case 33744: // Orgrimmar
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_ORGRIMMAR,true); break;
                        case 33745: // Sen'jin
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_SENJIN,true); break;
                        case 33746: // Silvermoon
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_SILVERMOON,true); break;
                        case 33747: // Stormwind
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_STORMWIND,true); break;
                        case 33748: // Thunder Bluff
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_THUNDERBLUFF,true); break;
                        case 33749: // Undercity
                            pDoneBy->CastSpell(pDoneBy,SPELL_BESTED_UNDERCITY,true); break;
                    }
                }

                me->setFaction(35);
                EnterEvadeMode();
                me->CastSpell(me,SPELL_REFRESH_MOUNT,true);
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (uiChargeTimer <= uiDiff)
            {
                chargeing = true;
                float x,y,z;
                me->GetNearPoint(me, x, y, z, 1.0f, 15.0f, float(M_PI*2*rand_norm()));
                me->GetMotionMaster()->MovePoint(1,x,y,z);

                uiChargeTimer = 15000;
            } else uiChargeTimer -= uiDiff;

            if (uiShieldBreakerTimer <= uiDiff)
            {
                DoCastVictim(SPELL_SHIELD_BREAKER);
                uiShieldBreakerTimer = 10000;
            } else uiShieldBreakerTimer -= uiDiff;

            if (me->isAttackReady())
            {
                DoCast(me->getVictim(), SPELL_THRUST, true);
                me->resetAttackTimer();
            }

            if(Player* plr = Player::GetPlayer(*me,guidAttacker))
                 if(!plr->HasAura(63034))
                     EnterEvadeMode();
        }
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_faction_valiant_championAI(creature);
    }

    bool CanMakeDuel(Player* player, uint32 npcEntry)
    {
        switch(npcEntry)
        {
        case 33738: // Darnassus
            return !player->HasAura(SPELL_BESTED_DARNASSUS);
        case 33739: // Exodar
            return !player->HasAura(SPELL_BESTED_EXODAR);
        case 33740: // Gnomeregan
            return !player->HasAura(SPELL_BESTED_GNOMEREGAN);
        case 33743: // Ironforge
            return !player->HasAura(SPELL_BESTED_IRONFORGE);
        case 33744: // Orgrimmar
            return !player->HasAura(SPELL_BESTED_ORGRIMMAR);
        case 33745: // Sen'jin
            return !player->HasAura(SPELL_BESTED_SENJIN);
        case 33746: // Silvermoon
            return !player->HasAura(SPELL_BESTED_SILVERMOON);
        case 33747: // Stormwind
            return !player->HasAura(SPELL_BESTED_STORMWIND);
        case 33748: // Thunder Bluff
            return !player->HasAura(SPELL_BESTED_THUNDERBLUFF);
        case 33749: // Undercity
            return !player->HasAura(SPELL_BESTED_UNDERCITY);
        }
        return true;
    }

    void AddMeleeFightGossip(Player* player)
    {
        if(!player)
            return;

        if( player->HasAura(63034) &&
            ((player->GetQuestStatus(QUEST_THE_GRAND_MELEE_0) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_1) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_2) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_3) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_4) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_5) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_6) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_7) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_8) == QUEST_STATUS_INCOMPLETE) ||
            (player->GetQuestStatus(QUEST_THE_GRAND_MELEE_9) == QUEST_STATUS_INCOMPLETE)))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MELEE_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
    }


    bool OnGossipHello(Player* player, Creature* creature)
    {
        switch(creature->GetEntry())
        {
        case 33559: // Darnassus
        case 33562: // Exodar
        case 33558: // Gnomeregan
        case 33564: // Ironforge
        case 33561: // Stormwind
            {
                if(player->GetTeamId() == TEAM_ALLIANCE)
                    AddMeleeFightGossip(player);
                break;
            }
        case 33306: // Orgrimmar
        case 33285: // Sen'jin
        case 33382: // Silvermoon
        case 33383: // Thunder Bluff
        case 33384: // Undercity
            {
                if(player->GetTeamId() == TEAM_HORDE)
                    AddMeleeFightGossip(player);
                break;
            }
        case 33738: // Darnassus
        case 33739: // Exodar
        case 33740: // Gnomeregan
        case 33743: // Ironforge
        case 33744: // Orgrimmar
        case 33745: // Sen'jin
        case 33746: // Silvermoon
        case 33747: // Stormwind
        case 33748: // Thunder Bluff
        case 33749: // Undercity
             {
                if( player->HasAura(63034) &&
                    ((player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_0) == QUEST_STATUS_INCOMPLETE) ||
                    (player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_1) == QUEST_STATUS_INCOMPLETE) ||
                    (player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_2) == QUEST_STATUS_INCOMPLETE) ||
                    (player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_3) == QUEST_STATUS_INCOMPLETE)))
                {
                    if(CanMakeDuel(player,creature->GetEntry()))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MELEE_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                }
                break;
            }
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            creature->setFaction(14);
            creature->AI()->AttackStart(player);
            return true;
        }

        if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
        {
            creature->setFaction(14);
            creature->AI()->AttackStart(player);
            return true;
        }
        return true;
    }
};

/*######
npc_squire_danny
######*/

enum eSquireDanny
{
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_UNDERCITY = 13729,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SENJIN = 13727,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_THUNDERBLUFF = 13728,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SILVERMOON = 13731,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_ORGRIMMAR = 13726,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_DARNASSUS = 13725,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_IRONFORGE = 13713,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_GNOMEREGAN = 13723,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_EXODAR = 13724,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_STORMWIND = 13699,

    NPC_ARGENT_CHAMPION = 33707,
    SPELL_SUMMON_ARGENT_CHAMPION = 63171,

    GOSSIP_TEXTID_SQUIRE_DANNY = 14407
};

/*
UPDATE creature_template SET scriptname = 'npc_squire_danny' WHERE entry = 33518;
*/

class npc_squire_danny : public CreatureScript
{
public:
    npc_squire_danny() : CreatureScript("npc_squire_danny") { }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer,SPELL_SUMMON_ARGENT_CHAMPION,false);
        }
        //else
        //pPlayer->SEND_GOSSIP_MENU(???, pCreature->GetGUID()); Missing text
        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pPlayer->HasAura(63034)
            && ((pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_UNDERCITY) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SENJIN) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_THUNDERBLUFF) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SILVERMOON) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_ORGRIMMAR) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_DARNASSUS) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_IRONFORGE) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_GNOMEREGAN) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_EXODAR) == QUEST_STATUS_INCOMPLETE)
            || (pPlayer->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_STORMWIND) == QUEST_STATUS_INCOMPLETE)))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_SQUIRE_DANNY, pCreature->GetGUID());
    return true;
    }
};

/*######
## npc_argent_champion
######*/
// To Do Argent Valiant, Faction Valiant, Argent Champion and Faction Champion have the same script -> make one

/*
UPDATE creature_template SET scriptname = 'npc_argent_champion' WHERE entry = 33707;
*/

enum eArgentChampion
{
    //SPELL_CHARGE                = 63010,
    //SPELL_SHIELD_BREAKER        = 65147,

    SPELL_ARGENT_CRUSADE_CHAMPION   = 63501,
    SPELL_GIVE_KILL_CREDIT_CHAMPION = 63516,
};

class npc_argent_champion : public CreatureScript
{
public:
    npc_argent_champion() : CreatureScript("npc_argent_champion") { }

    struct npc_argent_championAI : public ScriptedAI
    {
        npc_argent_championAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->GetMotionMaster()->MovePoint(0, 8561.30f, 1113.30f, 556.9f);
            creature->setFaction(35); //wrong faction in db?
        }

        uint32 uiChargeTimer;
        uint32 uiShieldBreakerTimer;

        void Reset()
        {
            uiChargeTimer = 7000;
            uiShieldBreakerTimer = 10000;
        }

        void MovementInform(uint32 uiType, uint32 /*uiId*/)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            me->setFaction(14);
        }

        void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
        {
            if (uiDamage > me->GetHealth() && pDoneBy->GetTypeId() == TYPEID_PLAYER)
            {
                uiDamage = 0;
                if(pDoneBy->HasAura(63034))
                    pDoneBy->CastSpell(pDoneBy,SPELL_GIVE_KILL_CREDIT_CHAMPION,true);
                me->setFaction(35);
                me->DespawnOrUnsummon(5000);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (uiChargeTimer <= uiDiff)
            {
                DoCastVictim(SPELL_CHARGE);
                uiChargeTimer = 7000;
            } else uiChargeTimer -= uiDiff;

            if (uiShieldBreakerTimer <= uiDiff)
            {
                DoCastVictim(SPELL_SHIELD_BREAKER);
                uiShieldBreakerTimer = 10000;
            } else uiShieldBreakerTimer -= uiDiff;

            if (me->isAttackReady())
            {
                DoCast(me->getVictim(), SPELL_THRUST, true);
                me->resetAttackTimer();
            }
        }
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_argent_championAI(creature);
    }
};

/*######
npc_squire_cavin
######*/

enum eSquireCalvin
{
    QUEST_THE_BLACK_KNIGHTS_FALL                = 13664,
    NPC_BLACK_KNIGHT                            = 33785,
};

/*
UPDATE creature_template SET scriptname = 'npc_squire_cavin' WHERE entry = 33522;
*/

#define GOSSIP_SUMMON_BLACK_KNIGHT      "Ask Cavin to summon the Black Knight."

class npc_squire_cavin : public CreatureScript
{
public:
    npc_squire_cavin() : CreatureScript("npc_squire_cavin") { }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            Position pos;
            pCreature->GetPosition(&pos);
            {
                if(TempSummon* temp = pCreature->SummonCreature(NPC_BLACK_KNIGHT,pos,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,10000))
                    temp->AI()->SetGUID(pPlayer->GetGUID());
            }
            pPlayer->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pPlayer->HasAura(63034) && (pPlayer->GetQuestStatus(QUEST_THE_BLACK_KNIGHTS_FALL) == QUEST_STATUS_INCOMPLETE))
        {
            std::list<Creature*> checkList;
            pCreature->GetCreatureListWithEntryInGrid(checkList,NPC_BLACK_KNIGHT,100.0f);
            if(checkList.size() == 0)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SUMMON_BLACK_KNIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        }

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }
};


enum eBlackKnight
{
    SPELL_BK_CHARGE                 = 63003,
    //SPELL_SHIELD_BREAKER        = 65147,
    SPELL_DARK_SHIELD               = 64505,

};

// UPDATE creature_template SET scriptname = 'npc_the_black_knight' WHERE entry = 33785;

#define YELL_ATTACK_PHASE_1_END         "Get off that horse and fight me man-to-man!"
#define YELL_ATTACK_PHASE_2             "I will not fail you, master!"

class npc_the_black_knight : public CreatureScript
{
public:
    npc_the_black_knight() : CreatureScript("npc_the_black_knight") { }

    struct npc_the_black_knightAI : public ScriptedAI
    {
        npc_the_black_knightAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        uint32 uiChargeTimer;
        uint32 uiShieldBreakerTimer;
        uint64 guidAttacker;
        uint32 uireattackTimer;

        bool chargeing;

        bool mountDuel;
        bool handDuel;

        void Reset()
        {
            uiChargeTimer = 7000;
            uiShieldBreakerTimer = 10000;
            uireattackTimer = 10000;

            me->setFaction(35);

            mountDuel = false;
            handDuel = false;
        }

        void SetGUID(uint64 guid, int32)
        {
            if(Player* plr = Player::GetPlayer(*me,guid))
            {
                guidAttacker = guid;
                mountDuel = true;
                handDuel = false;
                me->setFaction(14);
                me->Mount(28652);
                AttackStart(plr);
                // Move Point

                me->SetMaxHealth(50000);
                me->SetHealth(50000);
            }
        }

        void JustDied(Unit* killer)
        {
            me->DespawnOrUnsummon(5000);
        }

        void EnterCombat(Unit* attacker)
        {
            DoCast(me,SPELL_DEFEND_AURA_PERIODIC,true);
            if(Aura* aur = me->AddAura(SPELL_DEFEND,me))
                aur->ModStackAmount(1);
        }

        void MovementInform(uint32 uiType, uint32 uiId)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            if(uiId == 1)
            {

                chargeing = false;

                DoCastVictim(SPELL_BK_CHARGE);
                if(me->getVictim())
                    me->GetMotionMaster()->MoveChase(me->getVictim());

            }else if(uiId == 2)
            {
                if(Player* plr = Player::GetPlayer(*me,guidAttacker))
                {
                    AttackStart(plr);
                }
            }
        }

        void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
        {
            if(pDoneBy && pDoneBy->GetGUID() != guidAttacker)
                uiDamage = 0;

            if(handDuel)
                return;
            if(!mountDuel)
                return;

            if (uiDamage > me->GetHealth() && pDoneBy->GetTypeId() == TYPEID_PLAYER)
            {
                uiDamage = 0;
                mountDuel = false;
                me->SetHealth(50000);
				me->Dismount();
                me->GetMotionMaster()->MoveIdle();
                me->RemoveAurasDueToSpell(SPELL_DEFEND_AURA_PERIODIC);
                me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                me->MonsterYell(YELL_ATTACK_PHASE_1_END,LANG_UNIVERSAL,guidAttacker);
                uireattackTimer = 10000;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if(mountDuel)
            {
                if (uiChargeTimer <= uiDiff)
                {
                    chargeing = true;
                    float x,y,z;
                    me->GetNearPoint(me, x, y, z, 1.0f, 15.0f, float(M_PI*2*rand_norm()));
                    me->GetMotionMaster()->MovePoint(1,x,y,z);

                    uiChargeTimer = 7000;
                } else uiChargeTimer -= uiDiff;

                if (uiShieldBreakerTimer <= uiDiff)
                {
                    DoCastVictim(SPELL_SHIELD_BREAKER);
                    uiShieldBreakerTimer = 10000;
                } else uiShieldBreakerTimer -= uiDiff;

                if (me->isAttackReady())
                {
                    DoCast(me->getVictim(), SPELL_THRUST, true);
                    me->resetAttackTimer();
                }
            }else if(handDuel)
            {
                if (uiShieldBreakerTimer <= uiDiff)
                {
                    DoCastVictim(SPELL_DARK_SHIELD);
                    uiShieldBreakerTimer = 30000;
                } else uiShieldBreakerTimer -= uiDiff;

                DoMeleeAttackIfReady();
            }else
            {
                if(uireattackTimer <= uiDiff)
                {
                    handDuel = true;
                    if(me->getVictim())
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                    me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);

                    if(Player* plr = Player::GetPlayer(*me,guidAttacker))
                        plr->ExitVehicle();

                    me->SetMaxHealth(12500);
                    me->SetHealth(12500);
                    me->MonsterYell(YELL_ATTACK_PHASE_2,LANG_UNIVERSAL,guidAttacker);
                    uireattackTimer = 99999999;
                }else uireattackTimer -= uiDiff;
            }
        }
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_the_black_knightAI(creature);
    }
};

// Flaming Spear Targeting
/*
DELETE FROM spell_script_names WHERE spell_id IN (66588);
INSERT INTO spell_script_names (spell_id,Scriptname)
VALUES
(66588,'spell_flaming_spear_targeting');
*/
class spell_flaming_spear_targeting : public SpellScriptLoader
{
    public:
        spell_flaming_spear_targeting() : SpellScriptLoader("spell_flaming_spear_targeting") { }

        class spell_flaming_spear_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_flaming_spear_targeting_SpellScript);

            bool Load()
            {
                return GetCaster()->ToPlayer() != NULL;
            }

            void GetTargets(std::list<Unit*>& targetList)
            {
                targetList.clear();

                std::list<Creature*> pTargetList;
                GetCaster()->GetCreatureListWithEntryInGrid(pTargetList,35092, 150.0f);
                GetCaster()->GetCreatureListWithEntryInGrid(pTargetList,34925, 150.0f);

                if(pTargetList.empty())
                    return;

                pTargetList.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));

                std::list<Creature*>::iterator itr = pTargetList.begin();
                uint8 i = 3;
                for (std::list<Creature*>::const_iterator itr = pTargetList.begin(); itr != pTargetList.end(); ++itr)
                {
                    if(i == 0)
                        break;
                    if((*itr)->isAlive())
                    {
                        targetList.push_back(*itr);
                        i--;
                    }
                }

            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_flaming_spear_targeting_SpellScript::GetTargets, EFFECT_0, TARGET_UNIT_TARGET_ANY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_flaming_spear_targeting_SpellScript();
        }
};

#define GOSSIP_TRADE        "Visit a trader."
#define GOSSIP_BANK         "Visit a bank."
#define GOSSIP_MAIL         "Visit a mailbox."

#define GOSSIP_ORC_PENNANT  "Orgrimmar Champion's Pennant"
#define GOSSIP_SEN_PENNANT  "Darkspear Champion's Pennant"
#define GOSSIP_UND_PENNANT  "Forsaken Champion's Pennant"
#define GOSSIP_SIL_PENNANT  "Silvermoon Champion's Pennant"
#define GOSSIP_TBL_PENNANT  "Thunder Bluff Champion's Pennant"

#define GOSSIP_STW_PENNANT  "Stormwind Champion's Pennant"
#define GOSSIP_IFR_PENNANT  "Ironforge Champion's Pennant"
#define GOSSIP_GNO_PENNANT  "Gnomeregan Champion's Pennant"
#define GOSSIP_DAR_PENNANT  "Darnassus Champion's Pennant"
#define GOSSIP_EXO_PENNANT  "Exodar Champion's Pennant"

enum eSquireGruntling
{
    SPELL_BANK_ERRAND_H             = 68849,
    SPELL_POSTMAN_H                 = 68850,
    SPELL_SHOP_H                    = 68851,
    SPELL_TIRED_H                   = 68852,

    SPELL_BANK_ERRAND_A             = 67368,
    SPELL_POSTMAN_A                 = 67376,
    SPELL_SHOP_A                    = 67377,
    SPELL_TIRED_A                   = 67401,

    SPELL_PEND_DAR                  = 63443,
    SPELL_PEND_GNO                  = 63442,
    SPELL_PEND_IRO                  = 63440,
    SPELL_PEND_ORG                  = 63444,
    SPELL_PEND_SEN                  = 63446,
    SPELL_PEND_SIL                  = 63438,
    SPELL_PEND_STO                  = 62727,
    SPELL_PEND_EXO                  = 63439,
    SPELL_PEND_UND                  = 63441,
    SPELL_PEND_THU                  = 63445,


    ACHIEVEMENT_CHAMP_DARNASSUS     = 2777,
    ACHIEVEMENT_CHAMP_GNOMEREGAN    = 2779,
    ACHIEVEMENT_CHAMP_IRONFORGE     = 2780,
    ACHIEVEMENT_CHAMP_ORGRIMMAR     = 2783,
    ACHIEVEMENT_CHAMP_SENJIN        = 2784,
    ACHIEVEMENT_CHAMP_SILVERMOON    = 2785,
    ACHIEVEMENT_CHAMP_STORMWIND     = 2781,
    ACHIEVEMENT_CHAMP_EXODAR        = 2778,
    ACHIEVEMENT_CHAMP_UNDERCITY     = 2787,
    ACHIEVEMENT_CHAMP_THUNDERBLUFF  = 2786,

    GOS_CHAMP_DAR                   = 1,
    GOS_CHAMP_GNO                   = 2,
    GOS_CHAMP_IRO                   = 3,
    GOS_CHAMP_ORG                   = 4,
    GOS_CHAMP_SEN                   = 5,
    GOS_CHAMP_SIL                   = 6,
    GOS_CHAMP_STO                   = 7,
    GOS_CHAMP_EXO                   = 8,
    GOS_CHAMP_UND                   = 9,
    GOS_CHAMP_THU                   = 10,

    ENTRY_SQUIRE                    = 33238,
    ENTRY_GRUNTLING                 = 33239,
};

// UPDATE creature_template SET scriptname = 'npc_argent_squire_gruntling', npcflag |= 1 WHERE entry in (33238,33239);

class npc_argent_squire_gruntling : public CreatureScript
{
public:
    npc_argent_squire_gruntling() : CreatureScript("npc_argent_squire_gruntling") { }

    bool canShowPostman(Creature* pCreature)
    {
        if(pCreature->HasAura(SPELL_POSTMAN_H) || pCreature->HasAura(SPELL_POSTMAN_A))
            return true;

        if(pCreature->HasAura(SPELL_BANK_ERRAND_H) || pCreature->HasAura(SPELL_BANK_ERRAND_A))
            return false;

        if(pCreature->HasAura(SPELL_SHOP_H) || pCreature->HasAura(SPELL_SHOP_A))
            return false;

        if(pCreature->HasAura(SPELL_TIRED_H) || pCreature->HasAura(SPELL_TIRED_A))
            return false;

        return true;
    }

    bool canShowShop(Creature* pCreature)
    {
        if(pCreature->HasAura(SPELL_POSTMAN_H) || pCreature->HasAura(SPELL_POSTMAN_A))
            return false;

        if(pCreature->HasAura(SPELL_BANK_ERRAND_H) || pCreature->HasAura(SPELL_BANK_ERRAND_A))
            return false;

        if(pCreature->HasAura(SPELL_SHOP_H) || pCreature->HasAura(SPELL_SHOP_A))
            return true;

        if(pCreature->HasAura(SPELL_TIRED_H) || pCreature->HasAura(SPELL_TIRED_A))
            return false;

        return true;
    }

    bool canShowBank(Creature* pCreature)
    {
        if(pCreature->HasAura(SPELL_POSTMAN_H) || pCreature->HasAura(SPELL_POSTMAN_A))
            return false;

        if(pCreature->HasAura(SPELL_BANK_ERRAND_H) || pCreature->HasAura(SPELL_BANK_ERRAND_A))
            return true;

        if(pCreature->HasAura(SPELL_SHOP_H) || pCreature->HasAura(SPELL_SHOP_A))
            return false;

        if(pCreature->HasAura(SPELL_TIRED_H) || pCreature->HasAura(SPELL_TIRED_A))
            return false;

        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if(!pPlayer)
            return true;

        //if(canShowBank(pCreature))
        //    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, GOSSIP_BANK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_BANK);

        //if(canShowShop(pCreature))
        //    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TRADE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        //if(canShowPostman(pCreature))
        //    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MAIL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_DARNASSUS))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DAR_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_DAR);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_GNOMEREGAN))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GNO_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_GNO);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_IRONFORGE))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_IFR_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_IRO);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_ORGRIMMAR))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ORC_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_ORG);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_SENJIN))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DAR_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_SEN);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_SILVERMOON))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SIL_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_SIL);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_STORMWIND))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_STO);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_EXODAR))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_EXO_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_EXO);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_UNDERCITY))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_UND_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_UND);

        if(pPlayer->HasAchieved(ACHIEVEMENT_CHAMP_THUNDERBLUFF))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TBL_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_THU);

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }

    void cleanUpAllAuras(Creature* pCreature)
    {
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_DAR);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_GNO);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_IRO);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_ORG);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_SEN);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_SIL);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_STO);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_EXO);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_UND);
        pCreature->RemoveAurasDueToSpell(SPELL_PEND_THU);
    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        switch(uiAction)
        {
        //case GOSSIP_ACTION_BANK:
        //    pPlayer->GetSession()->SendShowBank(pCreature->GetGUID());
        //    break;
        //case GOSSIP_ACTION_TRADE:
        //    pPlayer->GetSession()->SendListInventory(pCreature->GetGUID());
        //    break;
        //case GOSSIP_ACTION_INFO_DEF:
        //    pCreature->MonsterSay("Nein, das kann ich nicht",LANG_UNIVERSAL,pPlayer->GetGUID());
        //    break;

        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_DAR:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_DAR,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_GNO:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_GNO,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_IRO:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_IRO,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_ORG:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_ORG,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_SEN:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_SEN,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_SIL:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_SIL,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_STO:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_STO,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_EXO:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_EXO,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_UND:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_UND,true);
            break;
        case GOSSIP_ACTION_INFO_DEF+GOS_CHAMP_THU:
            cleanUpAllAuras(pCreature);
            pCreature->CastSpell(pCreature,SPELL_PEND_THU,true);
            break;
        }

        pPlayer->CLOSE_GOSSIP_MENU();
        return true;
    }

};
//----------------------done in smart scripts---------------------------------------
/*########
## npc_black_knights_grave
#########*/
/*
enum eGraveSpells
{
    SPELL_COMPLETE_QUEST        = 66785,

    ENTRY_CULT_ASSASSIN         = 35127,
    ENTRY_CULT_SABOTEUR         = 35116,
};
*/
//#define SAY_SABOTEUR_1              "What do you think you're doing?"
//#define SAY_SABOTEUR_2              "You're too late to stop our plan."
//#define SAY_SABOTEUR_3              "See to it that I'm not followed."
//#define SAY_ASSASSIN_1              "Gladly."
/*
class npc_black_knights_grave : public CreatureScript
{
public:
    npc_black_knights_grave() : CreatureScript("npc_black_knights_grave") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_black_knights_graveAI (pCreature);
    }

    struct npc_black_knights_graveAI : public Scripted_NoMovementAI
    {
        npc_black_knights_graveAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature), summons(pCreature) { me->setActive(true); }

        SummonList summons;
        bool bEventRunning;

        uint64 guidSaboteur;
        uint64 guidAssassin;

        uint32 uiSpeakTimer;
        uint32 uiSpeakPhase;

        void Reset()
        {
            bEventRunning = false;
            uiSpeakPhase = 0;
        }

        void StartEvent(uint64 attacker = 0)
        {
            if(bEventRunning)
                return;

            bEventRunning = true;

            Creature* cre = DoSummon(ENTRY_CULT_SABOTEUR,me,5.0f,30000,TEMPSUMMON_TIMED_DESPAWN);
            if(cre)
            {
                cre->SetStandState(UNIT_STAND_STATE_KNEEL);
                cre->SetReactState(REACT_PASSIVE);
                cre->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                cre->setFaction(14);
                guidSaboteur = cre->GetGUID();
            }

            cre = DoSummon(ENTRY_CULT_ASSASSIN,me,5.0f,40000,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
            if(cre)
            {
                cre->SetReactState(REACT_PASSIVE);
                cre->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                cre->setFaction(14);
                guidAssassin = cre->GetGUID();
            }

            uiSpeakTimer = 7000;
            uiSpeakPhase = 1;
        }

        void StopEvent()
        {
            DoCastAOE(SPELL_COMPLETE_QUEST,true);
            bEventRunning = false;
            summons.DespawnAll(2000);
            uiSpeakPhase = 0;
        }

        void JustSummoned(Creature* pSummoned)
        {
            summons.Summon(pSummoned);
        }

        void MoveInLineOfSight(Unit *who)
        {
            if(!who || !who->ToPlayer())
                return;

            if(who->IsWithinDist(me,20.0f))
                if(who->ToPlayer()->GetQuestStatus(14016) == QUEST_STATUS_INCOMPLETE)
                    StartEvent(who->GetGUID());

            return;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(bEventRunning)
            {
                if(uiSpeakTimer <= uiDiff)
                {
                    switch(uiSpeakPhase)
                    {
                    case 1:
                        if(Creature* cre = Creature::GetCreature(*me,guidSaboteur))
                            cre->MonsterSay(SAY_SABOTEUR_1,LANG_UNIVERSAL,0);
                        break;
                    case 2:
                        if(Creature* cre = Creature::GetCreature(*me,guidSaboteur))
                            cre->MonsterSay(SAY_SABOTEUR_2,LANG_UNIVERSAL,0);
                        break;
                    case 3:
                        if(Creature* cre = Creature::GetCreature(*me,guidSaboteur))
                            cre->MonsterSay(SAY_SABOTEUR_3,LANG_UNIVERSAL,0);
                        break;
                    case 4:
                        if(Creature* cre = Creature::GetCreature(*me,guidAssassin))
                        {
                            cre->MonsterSay(SAY_ASSASSIN_1,LANG_UNIVERSAL,0);
                            cre->SetReactState(REACT_AGGRESSIVE);
                            cre->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                            std::list<Player*> pList;
                            pList = cre->GetNearestPlayersList(20.0f,true);
                            if(pList.size() > 0)
                            {
                                std::list<Player*>::const_iterator itr = pList.begin();
                                if((*itr))
                                    cre->AI()->AttackStart((*itr));
                            }
                            summons.DespawnEntry(ENTRY_CULT_SABOTEUR,2000);
                        }
                        break;
                    }
                    uiSpeakPhase++;
                    uiSpeakTimer = 3000;
                }else uiSpeakTimer -= uiDiff;


                if(Creature* cre = Creature::GetCreature(*me,guidAssassin))
                {
                    if(cre->isDead())
                    {
                        StopEvent();
                    }
                }else
                {
                    bEventRunning = false;
                    summons.DespawnAll();
                }

            }
        }
    };
};
*/

#define SPELL_CHILLMAW_FROST_BREATH 65248
#define SPELL_CHILLMAW_WING_BUFFET 65260
#define NPC_CULTIST_BOMBARDIER 33695
#define SPELL_THROW_DYNAMITE 65128
#define SPELL_THROW_TIME_BOMB 65130

class npc_chillmaw : public CreatureScript
{
public:
    npc_chillmaw() : CreatureScript("npc_chillmaw") { }

    struct npc_chillmawAI : public ScriptedAI
    {
        npc_chillmawAI(Creature* creature) : ScriptedAI(creature), summons(creature) { }
		
		SummonList summons;
		
		uint32 frostBreathTimer;
		bool winged;
		uint32 dropped;
		uint32 dynamiteTimer;
		uint32 timeBombTimer;

        void Reset()
        {
			me->SetCanFly(true);
			me->SetDisableGravity(true);
			me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
			if (me->GetWaypointPath())
				me->GetMotionMaster()->MovePath(me->GetWaypointPath(), true);

			frostBreathTimer = urand(10000, 11000);
			winged = false;
			dropped = 0;
			dynamiteTimer = urand(5000, 6000)/(3-dropped);
			timeBombTimer = urand(13000, 15000)/(3-dropped);
			summons.DespawnAll();
        }
		
		void EnterCombat(Unit* who) override
		{
			me->GetMotionMaster()->Clear();
			me->GetMotionMaster()->MoveCharge(who->GetPositionX(), who->GetPositionY(), who->GetPositionZ());
		}

		void MovementInform(uint32 type, uint32 point) override
		{
			if (type == POINT_MOTION_TYPE && point == EVENT_CHARGE)
			{
				me->SetCanFly(false);
				me->SetDisableGravity(false);
				me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
			}
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == 2000140)
				return;

			if (!me->isInCombat())
				return;
			
			summon->SetInCombatState(false, me->getVictim());
			me->getVictim()->SetInCombatState(false, summon);
			
			summon->AddThreat(me->getVictim(), 1.0f);
			me->getVictim()->AddThreat(summon, 1.0f);
			
			summons.Summon(summon);
		}

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;
			
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			
			if (frostBreathTimer <= uiDiff)
			{
				DoCastAOE(SPELL_CHILLMAW_FROST_BREATH);
				frostBreathTimer = urand(10000, 11000);
				return;
			}
			else frostBreathTimer -= uiDiff;
			
			if (!HealthAbovePct(35) && !winged)
			{
				winged = true;
				DoCastAOE(SPELL_CHILLMAW_WING_BUFFET);
			}
			
			if (dropped < 3)
			{
				if (HealthBelowPct(75-dropped*25))
				{
					if (Unit* passanger = me->GetVehicleKit()->GetPassenger(dropped))
						if (passanger->isAlive())
						{
							passanger->ExitVehicle();
							passanger->ToCreature()->DespawnOrUnsummon(50);
						}
					me->SummonCreature(NPC_CULTIST_BOMBARDIER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
					dropped++;
				}
				
				if (dynamiteTimer <= uiDiff)
				{
					me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_THROW_DYNAMITE, true);
					dynamiteTimer = urand(5000, 6000)/(3-dropped);
				}
				else
					dynamiteTimer -= uiDiff;
				
				if (timeBombTimer <= uiDiff)
				{
					me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_THROW_TIME_BOMB, true);
					timeBombTimer = urand(13000, 15000)/(3-dropped);
				}
				else
					timeBombTimer -= uiDiff;
			}
			
			DoMeleeAttackIfReady();
        }
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_chillmawAI(creature);
    }
};

enum FlyToGunship
{
	NPC_GYROCOPTER = 2000200,

};

class npc_skyreaver_klum : public CreatureScript
{
public:
    npc_skyreaver_klum() : CreatureScript("npc_skyreaver_klum") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
		if (player->GetTeamId() == TEAM_ALLIANCE)
		{
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		if (Creature* gyrocopter = creature->SummonCreature(NPC_GYROCOPTER, *creature))
		{
			player->CLOSE_GOSSIP_MENU();
			player->Dismount();
			player->EnterVehicle(gyrocopter);

			Movement::PointsArray hordeWaypoints (19);
			hordeWaypoints[0] = G3D::Vector3(5841.087891f, 475.734131f, 657.640198f);
			hordeWaypoints[1] = G3D::Vector3(5819.568359f, 411.775635f, 681.437988f);
			hordeWaypoints[2] = G3D::Vector3(5827.815918f, 361.677979f, 668.690308f);
			hordeWaypoints[3] = G3D::Vector3(5879.711914f, 314.450317f, 654.848511f);
			hordeWaypoints[4] = G3D::Vector3(5952.648438f, 313.218567f, 644.767761f);
			hordeWaypoints[5] = G3D::Vector3(6040.539063f, 361.793915f, 641.533936f);
			hordeWaypoints[6] = G3D::Vector3(6339.972168f, 542.130249f, 644.289673f);
			hordeWaypoints[7] = G3D::Vector3(6721.421387f, 832.786560f, 657.804932f);
			hordeWaypoints[8] = G3D::Vector3(6955.448730f, 1062.298096f, 614.978455f);
			hordeWaypoints[9] = G3D::Vector3(7190.628906f, 1169.387451f, 578.993103f);
			hordeWaypoints[10] = G3D::Vector3(7301.608887f, 1214.732910f, 574.569763f);
			hordeWaypoints[11] = G3D::Vector3(7461.472656f, 1270.433105f, 616.889954f);
			hordeWaypoints[12] = G3D::Vector3(7555.871582f, 1336.021606f, 647.024841f);
			hordeWaypoints[13] = G3D::Vector3(7590.004395f, 1429.571289f, 658.405823f);
			hordeWaypoints[14] = G3D::Vector3(7559.906250f, 1497.666626f, 656.687378f);
			hordeWaypoints[15] = G3D::Vector3(7522.950195f, 1513.560303f, 646.460876f);
			hordeWaypoints[16] = G3D::Vector3(7490.181641f, 1457.558472f, 642.359314f);
			hordeWaypoints[17] = G3D::Vector3(7480.052246f, 1422.484009f, 644.490906f);
			hordeWaypoints[18] = G3D::Vector3(7506.947754f, 1400.743774f, 638.465942f);

			gyrocopter->GetMotionMaster()->MoveSmoothPath(1, hordeWaypoints, 30.0f, true);
			gyrocopter->DespawnOrUnsummon(87000);
		}

		return true;
    }
};

class npc_officer_van_rossem : public CreatureScript
{
public:
    npc_officer_van_rossem() : CreatureScript("npc_officer_van_rossem") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
		if (player->GetTeamId() == TEAM_HORDE)
		{
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		if (Creature* gyrocopter = creature->SummonCreature(NPC_GYROCOPTER, *creature))
		{
			player->CLOSE_GOSSIP_MENU();
			player->Dismount();
			player->EnterVehicle(gyrocopter);

			Movement::PointsArray allianceWaypoints (24);
			allianceWaypoints[0] = G3D::Vector3(5841.087891f, 475.734131f, 657.640198f);
			allianceWaypoints[1] = G3D::Vector3(5819.568359f, 411.775635f, 681.437988f);
			allianceWaypoints[2] = G3D::Vector3(5827.815918f, 361.677979f, 668.690308f);
			allianceWaypoints[3] = G3D::Vector3(5879.711914f, 314.450317f, 654.848511f);
			allianceWaypoints[4] = G3D::Vector3(5952.648438f, 313.218567f, 644.767761f);
			allianceWaypoints[5] = G3D::Vector3(6040.539063f, 361.793915f, 641.533936f);
			allianceWaypoints[6] = G3D::Vector3(6339.972168f, 542.130249f, 644.289673f);
			allianceWaypoints[7] = G3D::Vector3(6721.421387f, 832.786560f, 657.804932f);
			allianceWaypoints[8] = G3D::Vector3(6955.448730f, 1062.298096f, 614.978455f);
			allianceWaypoints[9] = G3D::Vector3(7190.628906f, 1169.387451f, 578.993103f);
			allianceWaypoints[10] = G3D::Vector3(7242.176758f, 1215.623779f, 576.078247f);
			allianceWaypoints[11] = G3D::Vector3(7315.382813f, 1290.768188f, 573.536377f);
			allianceWaypoints[12] = G3D::Vector3(7405.139648f, 1397.881958f, 574.835266f);
			allianceWaypoints[13] = G3D::Vector3(7466.253906f, 1482.482666f, 586.226746f);
			allianceWaypoints[14] = G3D::Vector3(7537.151855f, 1601.269165f, 605.752869f);
			allianceWaypoints[15] = G3D::Vector3(7582.723145f, 1733.833130f, 629.017273f);
			allianceWaypoints[16] = G3D::Vector3(7593.963379f, 1803.280396f, 644.811951f);
			allianceWaypoints[17] = G3D::Vector3(7556.053223f, 1876.133545f, 665.037781f);
			allianceWaypoints[18] = G3D::Vector3(7518.908691f, 1887.642822f, 672.107361f);
			allianceWaypoints[19] = G3D::Vector3(7484.753906f, 1882.426270f, 675.867310f);
			allianceWaypoints[20] = G3D::Vector3(7471.553711f, 1877.463745f, 676.562439f);
			allianceWaypoints[21] = G3D::Vector3(7456.784668f, 1821.872192f, 677.968445f);
			allianceWaypoints[22] = G3D::Vector3(7470.592773f, 1794.067383f, 666.910767f);
			allianceWaypoints[23] = G3D::Vector3(7474.917480f, 1788.022461f, 665.640503f);

			gyrocopter->GetMotionMaster()->MoveSmoothPath(1, allianceWaypoints, 30.0f, true);
			gyrocopter->DespawnOrUnsummon(96000);
		}

		return true;
    }
};

enum Assault_by_Ground
{
	QUEST_ASSAULT_BY_GROUND_A = 13284,
	QUEST_ASSAULT_BY_GROUND_H = 13301,

	ACTION_START_ESCORT = 5648,
	
	NPC_ME_A = 31737,
	NPC_ME_H = 31833,
	
	NPC_GUARD_A = 31701,
	NPC_GUARD_H = 31832,

	NPC_KILL_CREDIT_A = 59677,
	NPC_KILL_CREDIT_H = 59764,
};

Position AllianceSpawnGuardPosition[5] = 
{
	{7272.956055f, 1505.818359f, 320.238251f, 0.0f},
	{7270.692383f, 1505.494873f, 321.055939f, 0.0f},
	{7268.056152f, 1504.518677f, 321.514465f, 0.0f},
	{7266.677246f, 1505.679565f, 321.676025f, 0.0f},
	{7268.694336f, 1506.266968f, 321.340820f, 0.0f}
};
Position HordeSpawnGuardPosition[5] =
{
	{7509.982422f, 1798.844360f, 356.099701f, 0.0f},
	{7511.447266f, 1797.089600f, 356.111542f, 0.0f},
	{7511.735352f, 1795.161621f, 356.281677f, 0.0f},
	{7511.141113f, 1792.837769f, 356.581482f, 0.0f},
	{7510.681152f, 1795.544678f, 356.046326f, 0.0f}
};

class npc_assault_by_ground : public CreatureScript
{
public:
    npc_assault_by_ground() : CreatureScript("npc_assault_by_ground") { }

	bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
	{
		if (Creature* meClone = creature->SummonCreature(creature->GetEntry(), *creature, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000))
		{
			meClone->AI()->DoAction(ACTION_START_ESCORT);
			meClone->AI()->SetGUID((uint64)player, ACTION_START_ESCORT);
		}

		return true;
	}

	struct npc_assault_by_groundAI : public npc_escortAI
    {
        npc_assault_by_groundAI(Creature* creature) : npc_escortAI(creature), summons(creature) { }
		
		SummonList summons;
		Player* player;

		void SetGUID(uint64 guid, int32 type)
		{
			player = (Player*)guid;
		}
		
		void DoAction(const int32 action)
		{
			if (action == ACTION_START_ESCORT)
			{
				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

				for (int i = 0; i < 5 ; i++)
				{
					if (me->GetEntry() == NPC_ME_A)
						me->SummonCreature(NPC_GUARD_A, AllianceSpawnGuardPosition[i]);
					else
						me->SummonCreature(NPC_GUARD_H, HordeSpawnGuardPosition[i]);
				}

				Start();
			}
		}

		void JustSummoned(Creature* summon)
		{
			summons.Summon(summon);
		}

		void EnterCombat(Unit* who)
		{
			Talk(1);
		}

		void WaypointReached(uint32 point)
		{
			if (point == 0)
				SetRun();
			else if (me->GetEntry() == NPC_ME_H && point == 10 || me->GetEntry() == NPC_ME_A && point == 14)
				Talk(0);
			else if (me->GetEntry() == NPC_ME_A && point == 24)
			{
				if (player)
					for (std::list<uint64>::iterator i = summons.begin(); i != summons.end(); i++)
					{
						if (Unit* guard = Unit::GetUnit(*me, *i))
							if (guard->isAlive())
								player->CastSpell(player, NPC_KILL_CREDIT_A, true);
					}
				summons.DespawnAll();
			}
			else if (me->GetEntry() == NPC_ME_H && point == 16)
			{
				if (player)
					for (std::list<uint64>::iterator i = summons.begin(); i != summons.end(); i++)
					{
						if (Unit* guard = Unit::GetUnit(*me, *i))
							if (guard->isAlive())
								player->CastSpell(player, NPC_KILL_CREDIT_H, true);
					}
				summons.DespawnAll();
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_assault_by_groundAI(creature);
    }
};

class npc_assault_by_ground_guard : public CreatureScript
{
public:
    npc_assault_by_ground_guard() : CreatureScript("npc_assault_by_ground_guard") { }

	struct npc_assault_by_ground_guardAI : public AggressorAI
    {
        npc_assault_by_ground_guardAI(Creature* creature) : AggressorAI(creature), owner(NULL) { }
		
		Unit* owner;
		float distance;
		float angle;

		void IsSummonedBy(Unit* summoner)
		{
			owner = summoner;
			distance = me->GetExactDist(summoner);
			angle = me->GetAngle(summoner);
			me->GetMotionMaster()->MoveFollow(summoner, distance, angle);
		}

		void EnterEvadeMode()
		{
			if (owner)
				me->GetMotionMaster()->MoveFollow(owner, distance, angle);
			else
				AggressorAI::EnterEvadeMode();
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_assault_by_ground_guardAI(creature);
    }
};

enum Lithe_Stalker
{
	SPELL_LEAP = 57882,
	SPELL_IRON_CHAIN = 58203,
	SPELL_ROCK = 58283,

	SPELL_BOMB_EXPLOSION = 75545,

	NPC_ABOMINATION = 30920,
	NPC_WORKER = 30921,
	NPC_UMBRAL_BRUTE = 30922,
	NPC_SCOURGE_BOMB = 31075,
	NPC_MATCHMAKING_KILL_CREDIT = 31481,
};

class npc_lithe_stalker : public CreatureScript
{
public:
    npc_lithe_stalker() : CreatureScript("npc_lithe_stalker") { }

	struct npc_lithe_stalkerAI : public NullCreatureAI
    {
        npc_lithe_stalkerAI(Creature* creature) : NullCreatureAI(creature) { bomb = NULL; player = NULL; }

		Unit* bomb;
		Player* player;
		uint32 timer;

		void PassengerBoarded(Unit* who, int8 seat, bool apply)
		{
			if (apply)
			{
				me->setFaction(35);
				timer = 1000;
				player = who->ToPlayer();
			}
			else
				player = NULL;
		}

		void SpellHitTarget(Unit* target, const SpellInfo* spell)
		{
			switch (spell->Id)
			{
				case SPELL_LEAP:
					 me->CombatStop();
					 break;
				case SPELL_IRON_CHAIN:
					if (target->GetEntry() == NPC_SCOURGE_BOMB)
					{
						target->GetMotionMaster()->MoveFollow(me, me->GetExactDist(target), M_PI);
						bomb = target;
					}
					break;
				case SPELL_ROCK:
					if (target->GetEntry() == NPC_UMBRAL_BRUTE)
					{
						if (Creature* worker = target->FindNearestCreature(NPC_WORKER, 15.0f))
						{
							target->setFaction(35);
							target->GetAI()->AttackStart(worker);
							worker->AI()->AttackStart(target);
							target->AddThreat(worker, 1000.0f);
							worker->AddThreat(target, 1000.0f);
							if (player)
								player->RewardPlayerAndGroupAtEvent(NPC_MATCHMAKING_KILL_CREDIT, player);
						}
					}
					break;
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (bomb)
			{
				if (Creature* abomination = bomb->FindNearestCreature(NPC_ABOMINATION, 5.0f))
				{
					if (player)
						player->KilledMonsterCredit(NPC_SCOURGE_BOMB, 0);
					abomination->AI()->Talk(0);
					abomination->DespawnOrUnsummon(500);
					bomb->CastSpell(bomb, SPELL_BOMB_EXPLOSION);
					bomb->ToCreature()->DespawnOrUnsummon(500);
					me->RemoveAurasDueToSpell(SPELL_IRON_CHAIN);
					bomb = NULL;
				}
			}

			if (timer)
			{
				if (timer < diff)
				{
					me->setFaction(35);
					timer = 0;
				}
				else
					timer -= diff;
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_lithe_stalkerAI(creature);
    }
};

Position assaultByAirHorde [60] = 
{
	{7505.462402f, 1708.332520f, 352.100708f, 0.0f},
	{7552.013184f, 1774.214233f, 391.995453f, 0.0f},
	{7587.781738f, 1858.157959f, 443.553253f, 0.0f},
	{7599.592285f, 1916.644287f, 476.117920f, 0.0f},
	{7582.763672f, 2014.891846f, 518.800049f, 0.0f},
	{7535.300293f, 2069.375977f, 535.392761f, 0.0f},
	{7447.589844f, 2123.454834f, 547.069275f, 0.0f},
	{7356.140137f, 2171.848877f, 564.788025f, 0.0f},
	{7294.131836f, 2200.511230f, 580.017456f, 0.0f},
	{7199.529785f, 2236.684814f, 607.548279f, 0.0f},
	{7135.259766f, 2255.693115f, 627.691406f, 0.0f},
	{7037.139648f, 2263.425781f, 659.818420f, 0.0f},
	{6996.263184f, 2230.152832f, 675.780029f, 0.0f},
	{6990.527832f, 2188.892578f, 683.687256f, 0.0f},
	{7010.324219f, 2170.970703f, 683.006897f, 0.0f},
	{7051.245605f, 2148.846680f, 678.267456f, 0.0f},
	{7090.620605f, 2135.905762f, 666.140564f, 0.0f},
	{7129.385254f, 2133.334717f, 645.886230f, 0.0f},
	{7190.267578f, 2126.888428f, 612.028687f, 0.0f},
	{7252.813965f, 2101.016846f, 582.974731f, 0.0f},
	{7291.639160f, 2039.398804f, 576.634399f, 0.0f},
	{7306.675781f, 1969.830322f, 576.187561f, 0.0f},
	{7306.587402f, 1915.053345f, 572.299744f, 0.0f},
	{7290.509766f, 1823.485718f, 568.694824f, 0.0f},
	{7266.987305f, 1792.035522f, 559.106567f, 0.0f},
	{7237.327148f, 1778.882324f, 547.345764f, 0.0f},
	{7217.165039f, 1774.053955f, 539.241699f, 0.0f},
	{7130.429199f, 1759.322632f, 524.886353f, 0.0f},
	{7086.025879f, 1756.672729f, 534.398499f, 0.0f},
	{7036.826660f, 1753.249634f, 548.847473f, 0.0f},
	{6999.713379f, 1740.064209f, 563.472961f, 0.0f},
	{6951.496582f, 1716.660889f, 575.080688f, 0.0f},
	{6908.096191f, 1700.440308f, 578.379089f, 0.0f},
	{6872.883789f, 1706.430420f, 582.968018f, 0.0f},
	{6821.141113f, 1732.718140f, 596.280945f, 0.0f},
	{6803.167480f, 1758.847900f, 607.798401f, 0.0f},
	{6780.899902f, 1809.688965f, 629.613892f, 0.0f},
	{6757.810547f, 1853.122925f, 650.944031f, 0.0f},
	{6721.059570f, 1901.681763f, 685.406860f, 0.0f},
	{6679.867188f, 1950.507568f, 726.441284f, 0.0f},
	{6618.414551f, 1962.332520f, 735.411926f, 0.0f},
	{6554.330566f, 1929.379517f, 727.290283f, 0.0f},
	{6518.295898f, 1871.383789f, 716.514221f, 0.0f},
	{6521.637695f, 1802.310547f, 702.091064f, 0.0f},
	{6544.385742f, 1748.035889f, 688.112610f, 0.0f},
	{6576.101563f, 1687.943359f, 671.399231f, 0.0f},
	{6609.750488f, 1631.388550f, 655.954224f, 0.0f},
	{6674.610840f, 1548.206055f, 620.345398f, 0.0f},
	{6720.595215f, 1502.653442f, 596.728210f, 0.0f},
	{6808.810059f, 1458.869751f, 560.952332f, 0.0f},
	{6872.014160f, 1442.252441f, 536.151917f, 0.0f},
	{6936.749512f, 1432.026733f, 511.581421f, 0.0f},
	{7046.719727f, 1427.668335f, 471.680420f, 0.0f},
	{7144.518066f, 1441.676880f, 436.194031f, 0.0f},
	{7243.164551f, 1458.747559f, 404.685211f, 0.0f},
	{7343.150391f, 1482.715698f, 383.901764f, 0.0f},
	{7449.182129f, 1536.538330f, 367.180450f, 0.0f},
	{7481.479980f, 1589.867310f, 369.422882f, 0.0f},
	{7503.958496f, 1674.003662f, 366.177124f, 0.0f},
	{7505.743652f, 1710.369995f, 349.810028f, 0.0f}
};

Position assaultByAirAlliance [73] = 
{
	{7303.661621f, 1521.339233f, 324.614563f, 0.0f},
	{7248.548828f, 1502.182617f, 345.180878f, 0.0f},
	{7189.770996f, 1471.901855f, 368.095306f, 0.0f},
	{7132.054199f, 1438.368286f, 389.152069f, 0.0f},
	{7052.859863f, 1402.070801f, 416.144836f, 0.0f},
	{6997.691895f, 1391.874878f, 432.307709f, 0.0f},
	{6930.173340f, 1389.585083f, 450.477020f, 0.0f},
	{6863.300293f, 1397.020996f, 469.592499f, 0.0f},
	{6798.270508f, 1413.452637f, 489.395966f, 0.0f},
	{6750.998535f, 1437.714111f, 507.819244f, 0.0f},
	{6704.709473f, 1480.839722f, 537.484375f, 0.0f},
	{6666.550293f, 1527.586426f, 572.880432f, 0.0f},
	{6633.567383f, 1576.595947f, 610.413940f, 0.0f},
	{6592.921875f, 1649.140137f, 665.383484f, 0.0f},
	{6582.096191f, 1688.551147f, 687.830872f, 0.0f},
	{6569.411621f, 1752.355225f, 713.466980f, 0.0f},
	{6560.557617f, 1819.237305f, 731.962158f, 0.0f},
	{6568.145508f, 1893.513916f, 743.965942f, 0.0f},
	{6587.057617f, 1928.118042f, 747.186096f, 0.0f},
	{6657.966797f, 1960.296631f, 749.424316f, 0.0f},
	{6700.177734f, 1943.976318f, 739.594177f, 0.0f},
	{6716.016113f, 1924.989258f, 726.483276f, 0.0f},
	{6758.114258f, 1850.023804f, 666.323120f, 0.0f},
	{6785.142578f, 1794.862305f, 633.049622f, 0.0f},
	{6812.912598f, 1751.118774f, 610.908936f, 0.0f},
	{6855.007813f, 1716.614868f, 590.352966f, 0.0f},
	{6890.120117f, 1700.042236f, 576.974365f, 0.0f},
	{6932.010254f, 1700.392578f, 565.500488f, 0.0f},
	{6957.491211f, 1713.331543f, 559.752075f, 0.0f},
	{7015.190430f, 1748.348511f, 548.073425f, 0.0f},
	{7057.873535f, 1752.142456f, 543.626953f, 0.0f},
	{7102.490723f, 1747.284302f, 542.498047f, 0.0f},
	{7137.416992f, 1749.345093f, 543.040466f, 0.0f},
	{7172.203613f, 1753.157349f, 543.278381f, 0.0f},
	{7225.264648f, 1765.505859f, 544.634277f, 0.0f},
	{7277.627441f, 1793.093994f, 548.829407f, 0.0f},
	{7307.786621f, 1825.138916f, 553.704895f, 0.0f},
	{7321.992676f, 1868.687744f, 559.589966f, 0.0f},
	{7320.801758f, 1892.828125f, 563.358826f, 0.0f},
	{7304.529297f, 1959.722168f, 575.785522f, 0.0f},
	{7294.790039f, 1992.925781f, 581.024902f, 0.0f},
	{7275.367188f, 2042.717163f, 588.566406f, 0.0f},
	{7243.946777f, 2089.168213f, 599.294739f, 0.0f},
	{7212.111816f, 2118.375000f, 609.077148f, 0.0f},
	{7167.530762f, 2136.329590f, 622.610291f, 0.0f},
	{7127.076172f, 2136.984863f, 637.516174f, 0.0f},
	{7102.582031f, 2131.007080f, 649.851685f, 0.0f},
	{7072.099609f, 2126.355957f, 666.379211f, 0.0f},
	{7056.873535f, 2130.320801f, 676.596680f, 0.0f},
	{7039.457520f, 2148.312012f, 691.259583f, 0.0f},
	{7019.905273f, 2190.982178f, 698.301270f, 0.0f},
	{7011.011230f, 2238.417725f, 698.233337f, 0.0f},
	{7027.689941f, 2289.247803f, 687.849731f, 0.0f},
	{7071.177246f, 2325.672607f, 672.343750f, 0.0f},
	{7121.820801f, 2334.410645f, 657.881042f, 0.0f},
	{7154.212891f, 2330.623779f, 648.946777f, 0.0f},
	{7201.572754f, 2315.943359f, 635.616028f, 0.0f},
	{7255.290039f, 2287.016113f, 619.925415f, 0.0f},
	{7332.564941f, 2220.683838f, 594.875549f, 0.0f},
	{7405.977051f, 2144.523193f, 569.278625f, 0.0f},
	{7495.756348f, 2041.847900f, 537.830383f, 0.0f},
	{7553.821777f, 1966.037598f, 514.375977f, 0.0f},
	{7571.510742f, 1904.375366f, 490.639404f, 0.0f},
	{7574.049316f, 1838.523560f, 467.298126f, 0.0f},
	{7569.990723f, 1784.115723f, 449.577911f, 0.0f},
	{7553.332031f, 1718.584717f, 431.688660f, 0.0f},
	{7527.717285f, 1655.559082f, 415.417603f, 0.0f},
	{7498.411133f, 1602.943970f, 400.278961f, 0.0f},
	{7453.426758f, 1552.385010f, 382.610992f, 0.0f},
	{7414.329590f, 1526.371704f, 368.300415f, 0.0f},
	{7378.090332f, 1516.074463f, 353.957916f, 0.0f},
	{7344.053223f, 1516.285400f, 341.473938f, 0.0f},
	{7301.285645f, 1520.532349f, 322.236847f, 0.0f},
};

enum Assault_By_Air
{
	QUEST_ASSAULT_BY_AIR_A = 13309,
	QUEST_ASSAULT_BY_AIR_H = 13310,

	NPC_ASSAULT_BY_AIR_TURRET_A = 32227,
	NPC_ASSAULT_BY_AIR_TURRET_H = 31884,

	NPC_ASSAULT_BY_AIR_CREDIT_A = 32224,
	NPC_ASSAULT_BY_AIR_CREDIT_H = 31888,

	NPC_SPEARGUN = 31280,
	SPELL_SUPPRESSION_CHARGE = 59880,

	EVENT_DROP = 1,
	EVENT_PERIODIC_HARPOON_DAMAGE = 2,
	EVENT_END_OF_DANGER = 3,
};

class npc_assault_by_air_turret : public CreatureScript
{
public:
    npc_assault_by_air_turret() : CreatureScript("npc_assault_by_air_turret") { }

	struct npc_assault_by_air_turretAI : public NullCreatureAI
    {
        npc_assault_by_air_turretAI(Creature* creature) : NullCreatureAI(creature), player(NULL) { }

		Player* player;

		void PassengerBoarded(Unit* unit, int8 seat, bool apply)
		{
			if (apply)
			{
				if (me->GetVehicleBase())
					if (player = unit->ToPlayer())
					{
						if (player->GetQuestStatus(QUEST_ASSAULT_BY_AIR_H) == QUEST_STATUS_INCOMPLETE)
						{
							events.Reset();
							events.ScheduleEvent(EVENT_PERIODIC_HARPOON_DAMAGE, 60000);
							events.ScheduleEvent(EVENT_DROP, 61000);
							events.ScheduleEvent(EVENT_DROP, 76000);
							events.ScheduleEvent(EVENT_DROP, 101000);
							events.ScheduleEvent(EVENT_DROP, 115000);
							events.ScheduleEvent(EVENT_END_OF_DANGER, 117000);
							//me->GetVehicleBase()->GetMotionMaster()->MoveSmoothPath(1, assaultByAirHorde, 60, 20.0f, true);
							me->GetVehicleBase()->GetMotionMaster()->MoveSmoothPath(1, me->GetVehicleBase()->GetEntry(), 20.0f, true);
						}
						else if (player->GetQuestStatus(QUEST_ASSAULT_BY_AIR_A) == QUEST_STATUS_INCOMPLETE)
						{
							events.Reset();
							events.ScheduleEvent(EVENT_PERIODIC_HARPOON_DAMAGE, 78000);
							events.ScheduleEvent(EVENT_DROP, 79000);
							events.ScheduleEvent(EVENT_DROP, 92000);
							events.ScheduleEvent(EVENT_DROP, 119000);
							events.ScheduleEvent(EVENT_DROP, 133000);
							events.ScheduleEvent(EVENT_END_OF_DANGER, 135000);
							//me->GetVehicleBase()->GetMotionMaster()->MoveSmoothPath(1, assaultByAirAlliance, 73, 20.0f, true);
							me->GetVehicleBase()->GetMotionMaster()->MoveSmoothPath(1, me->GetVehicleBase()->GetEntry(), 20.0f, true);
						}
					}
			}
			else
			{
				if (unit->ToPlayer())
				{
					if (unit->ToPlayer()->GetQuestStatus(QUEST_ASSAULT_BY_AIR_A) == QUEST_STATUS_INCOMPLETE)
						unit->ToPlayer()->FailQuest(QUEST_ASSAULT_BY_AIR_A);
					if (unit->ToPlayer()->GetQuestStatus(QUEST_ASSAULT_BY_AIR_H) == QUEST_STATUS_INCOMPLETE)
						unit->ToPlayer()->FailQuest(QUEST_ASSAULT_BY_AIR_H);
				}
				player = NULL;
			}
		}

		void JustDied(Unit* killer)
		{
			me->GetVehicleBase()->ToCreature()->DespawnOrUnsummon();
		}

		void UpdateAI(uint32 const diff)
		{
			if (events.Empty())
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_DROP:
						if (player)
						{
							if (me->GetEntry() == NPC_ASSAULT_BY_AIR_TURRET_A)
								player->RewardPlayerAndGroupAtEvent(NPC_ASSAULT_BY_AIR_CREDIT_A, player);
							else if (me->GetEntry() == NPC_ASSAULT_BY_AIR_TURRET_H)
								player->RewardPlayerAndGroupAtEvent(NPC_ASSAULT_BY_AIR_CREDIT_H, player);
						}
						break;
					case EVENT_PERIODIC_HARPOON_DAMAGE:
						{
							std::list<Creature*> spearGunList;
							me->GetCreatureListWithEntryInGrid(spearGunList, NPC_SPEARGUN, 100.0f);
							for (std::list<Creature*>::iterator itr = spearGunList.begin(); itr != spearGunList.end(); ++itr)
							{
								if (Creature* target = *itr)
									if (!target->HasAura(SPELL_SUPPRESSION_CHARGE))
										target->DealDamage(me, 1500);
							}
						}
						events.ScheduleEvent(EVENT_PERIODIC_HARPOON_DAMAGE, 5000);
						break;
					case EVENT_END_OF_DANGER:
						events.Reset();
						me->ModifyHealth(int32(me->GetMaxHealth()));
						break;
				}
			}
		}

		EventMap events;
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_assault_by_air_turretAI(creature);
    }
};

enum Bombarding_Icecrown
{
	SPELL_ENGINEERING = 59193,
	SPELL_BOMBARDMENT = 59194,
	SPELL_ANTIAIR = 59196,
	SPELL_FIGHT_FIRE = 61093,
	SPELL_CHARGE_SHIELD = 59061,
	SPELL_INFRA_GREEN_SHIELD = 59288,
	SPELL_BURNING = 61171,

	NPC_BOMBARDMENT_PLANE = 31406,
	NPC_ANTIAIR = 31407,
	NPC_BOMBARDMENT = 31408,
	NPC_ENGINEERING = 31409,
	
	ACTION_BOMBARDMENT = 568,
	ACTION_ANTIAIR,
	ACTION_ENGINEERING,
	ACTION_FIGHT_FIRE = 61093,
	ACTION_CHARGE_SHIELD = 59061,
	ACTION_START_BOMBARDMENT = 5234,
	
	QUEST_BOMBARDMENT_FIRST_A = 13380,
	QUEST_BOMBARDMENT_FIRST_DAILY_A = 13404,
	QUEST_BOMBARDMENT_SECOND_A = 13381,
	QUEST_BOMBARDMENT_SECOND_DAILY_A = 13382,
};

class npc_bombardment_plane : public CreatureScript
{
public:
    npc_bombardment_plane() : CreatureScript("npc_bombardment_plane") { }

	struct npc_bombardment_planeAI : public NullCreatureAI
    {
        npc_bombardment_planeAI(Creature* creature) : NullCreatureAI(creature), player(NULL) { }

		Player* player;

		void SetGUID(uint64 guid, int32)
		{
			player = Unit::GetPlayer(*me, guid);
		}

		void DoAction(const int32 action)
		{
			switch (action)
			{
				case ACTION_BOMBARDMENT:
				case ACTION_ANTIAIR:
				case ACTION_ENGINEERING:
					if (player)
					{
						player->ExitVehicle();
						player->EnterVehicle(me->GetVehicleKit()->GetPassenger(action - ACTION_BOMBARDMENT));
					}
					break;

				case ACTION_FIGHT_FIRE:
					me->RemoveAurasDueToSpell(SPELL_BURNING);
					for (int i = 0; i < 3; i++)
						if (me->GetVehicleKit()->GetPassenger(i))
							me->GetVehicleKit()->GetPassenger(i)->RemoveAurasDueToSpell(SPELL_BURNING);
					break;
				case ACTION_CHARGE_SHIELD:
					me->CastCustomSpell(SPELL_INFRA_GREEN_SHIELD, SPELLVALUE_AURA_STACK, 30, me);
					break;
				case ACTION_START_BOMBARDMENT:
					me->GetMotionMaster()->MoveSmoothPath(1, me->GetEntry(), 10.0f, true);
					break;
			}
		}

		void MovementInform(uint32 type, uint32 id)
		{
			if (type != EFFECT_MOTION_TYPE)
				return;

			if (player)
			{
				player->ExitVehicle();
				player->CastSpell(player, VEHICLE_SPELL_PARACHUTE);
				me->DespawnOrUnsummon(1000);
			}
		}

		void SpellHit(Unit* target, const SpellInfo* spell)
		{
			switch (spell->Id)
			{
				case SPELL_BURNING:
					for (int i = 0; i < 3; i++)
						if (me->GetVehicleKit()->GetPassenger(i))
							me->AddAura(spell->Id, me->GetVehicleKit()->GetPassenger(i));
					break;
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_bombardment_planeAI(creature);
    }
};

class npc_bombardment_turret : public CreatureScript
{
public:
    npc_bombardment_turret() : CreatureScript("npc_bombardment_turret") { }

	struct npc_bombardment_turretAI : public NullCreatureAI
    {
        npc_bombardment_turretAI(Creature* creature) : NullCreatureAI(creature) { }

		void SpellHit(Unit* target, const SpellInfo* spell)
		{
			switch (spell->Id)
			{
				case SPELL_BOMBARDMENT:
					if (me->GetEntry() == NPC_BOMBARDMENT)
						break;
					me->GetVehicleBase()->GetAI()->DoAction(ACTION_BOMBARDMENT);
					break;
				case SPELL_ANTIAIR:
					if (me->GetEntry() == NPC_ANTIAIR)
						break;
					me->GetVehicleBase()->GetAI()->DoAction(ACTION_ANTIAIR);
					break;
				case SPELL_ENGINEERING:
					if (me->GetEntry() == NPC_ENGINEERING)
						break;
					me->GetVehicleBase()->GetAI()->DoAction(ACTION_ENGINEERING);
					break;

				case SPELL_FIGHT_FIRE:
				case SPELL_CHARGE_SHIELD:
					me->GetVehicleBase()->GetAI()->DoAction(spell->Id);
					break;
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_bombardment_turretAI(creature);
    }
};

class npc_bombardment_start : public CreatureScript
{
public:
    npc_bombardment_start() : CreatureScript("npc_bombardment_start") { }

	bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32)
	{
		if (player->GetQuestStatus(QUEST_BOMBARDMENT_FIRST_A) == QUEST_STATUS_INCOMPLETE ||
			player->GetQuestStatus(QUEST_BOMBARDMENT_FIRST_DAILY_A) == QUEST_STATUS_INCOMPLETE ||
			player->GetQuestStatus(QUEST_BOMBARDMENT_SECOND_A) == QUEST_STATUS_INCOMPLETE ||
			player->GetQuestStatus(QUEST_BOMBARDMENT_SECOND_DAILY_A) == QUEST_STATUS_INCOMPLETE)
			if (Unit* plane = creature->SummonCreature(NPC_BOMBARDMENT_PLANE, *player))
				if (Unit* turret = plane->GetVehicleKit()->GetPassenger(1))
				{
					player->EnterVehicle(turret);
					plane->GetAI()->SetGUID(player->GetGUID());
					plane->GetAI()->DoAction(ACTION_START_BOMBARDMENT);
					player->CLOSE_GOSSIP_MENU();
				}

		return true;
	}
};

class vehicle_black_knights_gryphon : public CreatureScript
{
public:
    vehicle_black_knights_gryphon() : CreatureScript("vehicle_black_knights_gryphon") { }

	struct vehicle_black_knights_gryphonAI : public NullCreatureAI
    {
        vehicle_black_knights_gryphonAI(Creature* creature) : NullCreatureAI(creature), passenger(NULL), completeTimer(0) { }

		Player* passenger;
		uint32 completeTimer;

		void PassengerBoarded(Unit* who, int8 seat, bool apply)
		{
			if (apply)
			{
				me->GetMotionMaster()->MoveSmoothPath(12, me->GetEntry(), 20.0f, true);
				completeTimer = 111000;
				passenger = who->ToPlayer();
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (completeTimer)
			{
				if (completeTimer <= diff)
				{
					if (passenger)
						passenger->RewardPlayerAndGroupAtEvent(33519, passenger);
					me->DespawnOrUnsummon(1000);
					completeTimer = 0;
				}
				else
					completeTimer -= diff;
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new vehicle_black_knights_gryphonAI(creature);
    }
};

enum Quel_Dallar
{
	QUEST_MYRALION = 24558,
	QUEST_CALADIS = 24454,

	NPC_MYRALION = 36642,
	NPC_CALADIS = 36624,
	NPC_LANATHEL = 37846,
	NPC_HORDE_DEFENDERS = 36657,
	NPC_ALLIANCE_DEFENDERS = 36656,

	ACTION_START,

	EVENT_START,
	EVENT_CHARGE_DEFENDERS,
	EVENT_FREEZE_DEFENDERS,
	EVENT_TALK_1,
	EVENT_TALK_2,
	EVENT_TALK_3,
	EVENT_TALK_4,
	EVENT_TALK_5,
	EVENT_TALK_6,
	EVENT_TALK_7,
	EVENT_TALK_8,
	EVENT_TALK_9,
	EVENT_TALK_10,


	POINT_DISMOUNT,
	POINT_HOME,

	SPELL_FROST_BLOCK = 62766,
};

static const Position quelDellarPositions [] =
{
	{8120.35f, 776.2f, 482.03f, 2.0f},//move horde
	{8118.55f, 781.52f, 481.7f, 4.8f},//move alliance
	{8107.133789f, 783.730591f, 481.072632f, 0.0f},//lanathel (de)spawn
	{8114.567383f, 780.695251f, 481.538879f, 0.0f}//lanathel walk
};

class npc_quel_lanathel : public CreatureScript
{
public:
	npc_quel_lanathel() : CreatureScript("npc_quel_lanathel") { }

	struct npc_quel_lanathelAI : public NullCreatureAI
    {
        npc_quel_lanathelAI(Creature* creature) : NullCreatureAI(creature) { }

		EventMap events;
		std::list<Creature*> hordeList;
		std::list<Creature*> allianceList;

		void ChargeDefenders()
		{
			bool talked = false;
			hordeList.clear();
			allianceList.clear();
			me->GetCreatureListWithEntryInGrid(hordeList, NPC_HORDE_DEFENDERS, 50.0f);
			for (std::list<Creature*>::iterator i = hordeList.begin(); i != hordeList.end(); i++)
			{
				if (!talked)
				{
					talked = true;
					(*i)->MonsterSay("You are not welcome here, minion of the Lich King!", LANG_UNIVERSAL, 0);
				}
				Position nearPos;
				me->GetNearPosition(nearPos, 10.0f, me->GetAngle(*i));
				(*i)->GetMotionMaster()->MovePoint(0, nearPos);
			}
			me->GetCreatureListWithEntryInGrid(allianceList, NPC_ALLIANCE_DEFENDERS, 50.0f);
			for (std::list<Creature*>::iterator i = allianceList.begin(); i != allianceList.end(); i++)
			{
				Position nearPos;
				me->GetNearPosition(nearPos, 10.0f, me->GetAngle(*i));
				(*i)->GetMotionMaster()->MovePoint(0, nearPos);
			}
		}

		void FreezeDefenders()
		{
			for (std::list<Creature*>::iterator i = hordeList.begin(); i != hordeList.end(); i++)
				(*i)->AddAura(SPELL_FROST_BLOCK, *i);
			for (std::list<Creature*>::iterator i = allianceList.begin(); i != allianceList.end(); i++)
				(*i)->AddAura(SPELL_FROST_BLOCK, *i);
		}

		void EvadeDefenders()
		{
			for (std::list<Creature*>::iterator i = hordeList.begin(); i != hordeList.end(); i++)
			{
				(*i)->RemoveAurasDueToSpell(SPELL_FROST_BLOCK);
				(*i)->GetMotionMaster()->MoveTargetedHome();
			}
			for (std::list<Creature*>::iterator i = allianceList.begin(); i != allianceList.end(); i++)
			{
				(*i)->RemoveAurasDueToSpell(SPELL_FROST_BLOCK);
				(*i)->GetMotionMaster()->MoveTargetedHome();
			}

		}

		void MovementInform(uint32 type, uint32 id)
		{
			if (id == POINT_DISMOUNT)
			{
				Talk(0);
				events.Reset();
				events.ScheduleEvent(EVENT_CHARGE_DEFENDERS, 2000);
			}
			else if (id == POINT_HOME)
			{
				EvadeDefenders();
				if (me->ToTempSummon())
					if (Unit* summoner = me->ToTempSummon()->GetSummoner())
						summoner->GetMotionMaster()->MovePoint(POINT_HOME, summoner->ToCreature()->GetHomePosition());
				me->DespawnOrUnsummon(100);
			}
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 e = events.ExecuteEvent())
			{
				switch (e)
				{
					case EVENT_CHARGE_DEFENDERS:
						ChargeDefenders();
						events.ScheduleEvent(EVENT_FREEZE_DEFENDERS, 2000);
						break;
					case EVENT_FREEZE_DEFENDERS:
						FreezeDefenders();
						Talk(1);
						events.ScheduleEvent(EVENT_TALK_1, 5000);
						break;
					case EVENT_TALK_1:
						Talk(2);
						events.ScheduleEvent(EVENT_TALK_2, 10000);
						break;
					case EVENT_TALK_2:
						Talk(3);
						events.ScheduleEvent(EVENT_TALK_3, 6000);
						break;
					case EVENT_TALK_3:
						Talk(4);
						events.ScheduleEvent(EVENT_TALK_4, 10000);
						break;
					case EVENT_TALK_4:
						Talk(5);
						events.ScheduleEvent(EVENT_TALK_5, 7000);
						break;
					case EVENT_TALK_5:
						Talk(6);
						events.ScheduleEvent(EVENT_TALK_6, 13000);
						break;
					case EVENT_TALK_6:
						Talk(7);
						events.ScheduleEvent(EVENT_TALK_7, 9000);
						break;
					case EVENT_TALK_7:
						Talk(8);
						events.ScheduleEvent(EVENT_TALK_8, 5000);
						break;
					case EVENT_TALK_8:
						Talk(9);
						events.ScheduleEvent(EVENT_TALK_9, 10000);
						break;
					case EVENT_TALK_9:
						Talk(10);
						me->GetMotionMaster()->MovePoint(POINT_HOME, quelDellarPositions[2]);
						break;
				}
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_quel_lanathelAI(creature);
    }
};

class npc_myralion_caladis : public CreatureScript
{
public:
    npc_myralion_caladis() : CreatureScript("npc_myralion_caladis") { }

	bool OnQuestReward(Player* player, Creature* me, Quest const* quest, uint32 opt)
	{
		if (me->GetEntry() == NPC_MYRALION && quest->GetQuestId() == QUEST_MYRALION || me->GetEntry() == NPC_CALADIS && quest->GetQuestId() == QUEST_CALADIS)
			me->AI()->DoAction(ACTION_START);

		return CreatureScript::OnQuestReward(player, me, quest, opt);
	}

	struct npc_myralion_caladisAI : public NullCreatureAI
    {
        npc_myralion_caladisAI(Creature* creature) : NullCreatureAI(creature) { }

		void DoAction(int32 const action)
		{
			switch (action)
			{
				case ACTION_START:
					Talk(0);
					me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
					{
						Position pos = quelDellarPositions[me->GetEntry() == NPC_MYRALION ? 0 : 1];
						me->GetMotionMaster()->MoveCharge(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 2.0f, POINT_DISMOUNT);
					}
					break;
			}

		}

		void MovementInform(uint32 type, uint32 id)
		{
			if (id == POINT_DISMOUNT)
			{
				me->Dismount();
				Talk(1);
				Position pos = quelDellarPositions[3];
				me->SummonCreature(NPC_LANATHEL, quelDellarPositions[2])->GetMotionMaster()->MoveCharge(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 4.0f, POINT_DISMOUNT);
			}
			else if (id == POINT_HOME)
			{
				if (CreatureAddon const* cainfo = me->GetCreatureAddon())
					if (cainfo->mount != 0)
						me->Mount(cainfo->mount);
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
				Talk(2);
				me->SetOrientation(me->GetEntry() == NPC_MYRALION ? 4.3f : 1.5f);
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_myralion_caladisAI(creature);
    }
};

void AddSC_icecrown()
{
    new npc_arete;
    new npc_dame_evniki_kapsalis;
    new npc_squire_david;
    new npc_argent_valiant;
    new npc_alorah_and_grimmin;
    new npc_guardian_pavilion;
    new npc_vereth_the_cunning;
    new npc_tournament_training_dummy;
    new npc_faction_valiant_champion();
    new npc_argent_champion();
    new npc_squire_danny();
    new npc_squire_cavin();
    new npc_the_black_knight();
    new spell_flaming_spear_targeting();
    //new npc_argent_squire_gruntling();
    //new npc_black_knights_grave();
	new npc_chillmaw();
	new npc_skyreaver_klum();
	new npc_officer_van_rossem();
	new npc_assault_by_ground();
	new npc_assault_by_ground_guard();
	new npc_lithe_stalker();
	new npc_assault_by_air_turret();
	new npc_bombardment_plane();
	new npc_bombardment_turret();
	new npc_bombardment_start();
	new vehicle_black_knights_gryphon();
	
	new npc_quel_lanathel();
	new npc_myralion_caladis();
}