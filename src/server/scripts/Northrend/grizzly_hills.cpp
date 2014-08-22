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
SDName: Grizzly_Hills
SD%Complete: 80
SDComment: Quest support: 12231, 12247
SDCategory: Grizzly Hills
EndScriptData */

/* ContentData
npc_orsonn_and_kodian
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "Vehicle.h"
#include "Spell.h"

#define GOSSIP_ITEM1 "You're free to go Orsonn, but first tell me what's wrong with the furbolg."
#define GOSSIP_ITEM2 "What happened then?"
#define GOSSIP_ITEM3 "Thank you, Son of Ursoc. I'll see what can be done."
#define GOSSIP_ITEM4 "Who was this stranger?"
#define GOSSIP_ITEM5 "Thank you, Kodian. I'll do what I can."

enum eEnums
{
    GOSSIP_TEXTID_ORSONN1       = 12793,
    GOSSIP_TEXTID_ORSONN2       = 12794,
    GOSSIP_TEXTID_ORSONN3       = 12796,

    GOSSIP_TEXTID_KODIAN1       = 12797,
    GOSSIP_TEXTID_KODIAN2       = 12798,

    NPC_ORSONN                  = 27274,
    NPC_KODIAN                  = 27275,

    //trigger creatures
    NPC_ORSONN_CREDIT           = 27322,
    NPC_KODIAN_CREDIT           = 27321,

    QUEST_CHILDREN_OF_URSOC     = 12247,
    QUEST_THE_BEAR_GODS_OFFSPRING        = 12231
};

class npc_orsonn_and_kodian : public CreatureScript
{
public:
    npc_orsonn_and_kodian() : CreatureScript("npc_orsonn_and_kodian") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_CHILDREN_OF_URSOC) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_THE_BEAR_GODS_OFFSPRING) == QUEST_STATUS_INCOMPLETE)
        {
            switch (creature->GetEntry())
            {
                case NPC_ORSONN:
                    if (!player->GetReqKillOrCastCurrentCount(QUEST_CHILDREN_OF_URSOC, NPC_ORSONN_CREDIT) || !player->GetReqKillOrCastCurrentCount(QUEST_THE_BEAR_GODS_OFFSPRING, NPC_ORSONN_CREDIT))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN1, creature->GetGUID());
                        return true;
                    }
                    break;
                case NPC_KODIAN:
                    if (!player->GetReqKillOrCastCurrentCount(QUEST_CHILDREN_OF_URSOC, NPC_KODIAN_CREDIT) || !player->GetReqKillOrCastCurrentCount(QUEST_THE_BEAR_GODS_OFFSPRING, NPC_KODIAN_CREDIT))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_KODIAN1, creature->GetGUID());
                        return true;
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
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->CLOSE_GOSSIP_MENU();
                player->TalkedToCreature(NPC_ORSONN_CREDIT, creature->GetGUID());
                break;

            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_KODIAN2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->CLOSE_GOSSIP_MENU();
                player->TalkedToCreature(NPC_KODIAN_CREDIT, creature->GetGUID());
                break;
        }

        return true;
    }
};

/*######
## Quest 12027: Mr. Floppy's Perilous Adventure
######*/

enum eFloppy
{
    NPC_MRFLOPPY                = 26589,
    NPC_HUNGRY_WORG             = 26586,
    NPC_RAVENOUS_WORG           = 26590,   //RWORG
    NPC_EMILY                   = 26588,

    QUEST_PERILOUS_ADVENTURE    = 12027,

    SPELL_MRFLOPPY              = 47184,    //vehicle aura

    SAY_WORGHAGGRO1             = -1800001, //Um... I think one of those wolves is back...
    SAY_WORGHAGGRO2             = -1800002, //He's going for Mr. Floppy!
    SAY_WORGRAGGRO3             = -1800003, //Oh, no! Look, it's another wolf, and it's a biiiiiig one!
    SAY_WORGRAGGRO4             = -1800004, //He's gonna eat Mr. Floppy! You gotta help Mr. Floppy! You just gotta!
    SAY_RANDOMAGGRO             = -1800005, //There's a big meanie attacking Mr. Floppy! Help!
    SAY_VICTORY1                = -1800006, //Let's get out of here before more wolves find us!
    SAY_VICTORY2                = -1800007, //Don't go toward the light, Mr. Floppy!
    SAY_VICTORY3                = -1800008, //Mr. Floppy, you're ok! Thank you so much for saving Mr. Floppy!
    SAY_VICTORY4                = -1800009, //I think I see the camp! We're almost home, Mr. Floppy! Let's go!
    TEXT_EMOTE_WP1              = -1800010, //Mr. Floppy revives
    TEXT_EMOTE_AGGRO            = -1800011, //The Ravenous Worg chomps down on Mr. Floppy
    SAY_QUEST_ACCEPT            = -1800012, //Are you ready, Mr. Floppy? Stay close to me and watch out for those wolves!
    SAY_QUEST_COMPLETE          = -1800013  //Thank you for helping me get back to the camp. Go tell Walter that I'm safe now!
};

//emily
class npc_emily : public CreatureScript
{
public:
    npc_emily() : CreatureScript("npc_emily") { }

    struct npc_emilyAI : public npc_escortAI
    {
        npc_emilyAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 m_uiChatTimer;

        uint64 RWORGGUID;
        uint64 MrfloppyGUID;

        bool Completed;

        void JustSummoned(Creature* summoned)
        {
            if (Creature* Mrfloppy = GetClosestCreatureWithEntry(me, NPC_MRFLOPPY, 50.0f))
                summoned->AI()->AttackStart(Mrfloppy);
            else
                summoned->AI()->AttackStart(me->getVictim());
        }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;
            switch (i)
            {
                case 9:
                    if (Creature* Mrfloppy = GetClosestCreatureWithEntry(me, NPC_MRFLOPPY, 100.0f))
                        MrfloppyGUID = Mrfloppy->GetGUID();
                    break;
                case 10:
                    if (Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        DoScriptText(SAY_WORGHAGGRO1, me);
                        me->SummonCreature(NPC_HUNGRY_WORG, me->GetPositionX()+5, me->GetPositionY()+2, me->GetPositionZ()+1, 3.229f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
                    }
                    break;
                case 11:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                    break;
                case 17:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                    DoScriptText(SAY_WORGRAGGRO3, me);
                    if (Creature* RWORG = me->SummonCreature(NPC_RAVENOUS_WORG, me->GetPositionX()+10, me->GetPositionY()+8, me->GetPositionZ()+2, 3.229f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                    {
                        RWORG->setFaction(35);
                        RWORGGUID = RWORG->GetGUID();
                    }
                    break;
                case 18:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                            RWORG->GetMotionMaster()->MovePoint(0, Mrfloppy->GetPositionX(), Mrfloppy->GetPositionY(), Mrfloppy->GetPositionZ());
                        DoCast(Mrfloppy, SPELL_MRFLOPPY);
                    }
                    break;
                case 19:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Mrfloppy->HasAura(SPELL_MRFLOPPY, 0))
                        {
                            if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                                Mrfloppy->EnterVehicle(RWORG);
                        }
                    }
                    break;
                case 20:
                    if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                        RWORG->HandleEmoteCommand(34);
                    break;
                case 21:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                        {
                            RWORG->Kill(Mrfloppy);
                            Mrfloppy->ExitVehicle();
                            RWORG->setFaction(14);
                            RWORG->GetMotionMaster()->MovePoint(0, RWORG->GetPositionX()+10, RWORG->GetPositionY()+80, RWORG->GetPositionZ());
                            DoScriptText(SAY_VICTORY2, me);
                        }
                    }
                    break;
                case 22:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Mrfloppy->isDead())
                        {
                            if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                                RWORG->DisappearAndDie();
                            me->GetMotionMaster()->MovePoint(0, Mrfloppy->GetPositionX(), Mrfloppy->GetPositionY(), Mrfloppy->GetPositionZ());
                            Mrfloppy->setDeathState(ALIVE);
                            Mrfloppy->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            DoScriptText(SAY_VICTORY3, me);
                        }
                    }
                    break;
                case 24:
                    if (player)
                    {
                        Completed = true;
                        player->GroupEventHappens(QUEST_PERILOUS_ADVENTURE, me);
                        DoScriptText(SAY_QUEST_COMPLETE, me, player);
                    }
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    break;
                case 25:
                    DoScriptText(SAY_VICTORY4, me);
                    break;
                case 27:
                    me->DisappearAndDie();
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->DisappearAndDie();
                    break;
            }
        }

        void EnterCombat(Unit* /*Who*/)
        {
            DoScriptText(SAY_RANDOMAGGRO, me);
        }

        void Reset()
        {
            m_uiChatTimer = 4000;
            MrfloppyGUID = 0;
            RWORGGUID = 0;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (m_uiChatTimer <= uiDiff)
                {
                    m_uiChatTimer = 12000;
                }
                else
                    m_uiChatTimer -= uiDiff;
            }
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_PERILOUS_ADVENTURE)
        {
            DoScriptText(SAY_QUEST_ACCEPT, creature);
            if (Creature* Mrfloppy = GetClosestCreatureWithEntry(creature, NPC_MRFLOPPY, 180.0f))
            {
                Mrfloppy->GetMotionMaster()->MoveFollow(creature, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }

            if (npc_escortAI* pEscortAI = CAST_AI(npc_emily::npc_emilyAI, (creature->AI())))
                pEscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emilyAI(creature);
    }
};

//mrfloppy
class npc_mrfloppy : public CreatureScript
{
public:
    npc_mrfloppy() : CreatureScript("npc_mrfloppy") { }

    struct npc_mrfloppyAI : public ScriptedAI
    {
        npc_mrfloppyAI(Creature* c) : ScriptedAI(c) {}

        uint64 EmilyGUID;
        uint64 RWORGGUID;
        uint64 HWORGGUID;

        void Reset() {}

        void EnterCombat(Unit* Who)
        {
            if (Creature* Emily = GetClosestCreatureWithEntry(me, NPC_EMILY, 50.0f))
            {
                switch (Who->GetEntry())
                {
                    case NPC_HUNGRY_WORG:
                        DoScriptText(SAY_WORGHAGGRO2, Emily);
                        break;
                    case NPC_RAVENOUS_WORG:
                        DoScriptText(SAY_WORGRAGGRO4, Emily);
                        break;
                    default:
                        DoScriptText(SAY_RANDOMAGGRO, Emily);
                }
            }
        }

        void EnterEvadeMode() {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mrfloppyAI(creature);
    }
};

// Outhouse Bunny

enum eOuthouseBunny
{
    SPELL_OUTHOUSE_GROANS           = 48382,
    SPELL_CAMERA_SHAKE              = 47533,
    SPELL_DUST_FIELD                = 48329
};

enum eSounds
{
    SOUND_FEMALE        = 12671,
    SOUND_MALE          = 12670
};

class npc_outhouse_bunny : public CreatureScript
{
public:
    npc_outhouse_bunny() : CreatureScript("npc_outhouse_bunny") { }

    struct npc_outhouse_bunnyAI : public ScriptedAI
    {
        npc_outhouse_bunnyAI(Creature* creature) : ScriptedAI(creature) {}

        uint8 m_counter;
        uint8 m_gender;

        void Reset()
        {
            m_counter = 0;
            m_gender = 0;
        }

        void SetData(uint32 uiType, uint32 uiData)
        {
            if (uiType == 1)
                m_gender = uiData;
        }

        void SpellHit(Unit* pCaster, const SpellInfo* pSpell)
        {
             if (pSpell->Id == SPELL_OUTHOUSE_GROANS)
            {
                ++m_counter;
                if (m_counter < 5)
                    DoCast(pCaster, SPELL_CAMERA_SHAKE, true);
                else
                    m_counter = 0;
                DoCast(me, SPELL_DUST_FIELD, true);
                switch (m_gender)
                {
                    case GENDER_FEMALE: DoPlaySoundToSet(me, SOUND_FEMALE); break;
                    case GENDER_MALE: DoPlaySoundToSet(me, SOUND_MALE); break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_outhouse_bunnyAI(creature);
    }
};

// Tallhorn Stage

enum etallhornstage
{
    OBJECT_HAUNCH                   = 188665
};

class npc_tallhorn_stag : public CreatureScript
{
public:
    npc_tallhorn_stag() : CreatureScript("npc_tallhorn_stag") { }

    struct npc_tallhorn_stagAI : public ScriptedAI
    {
        npc_tallhorn_stagAI(Creature* creature) : ScriptedAI(creature) {}

        uint8 m_uiPhase;

        void Reset()
        {
            m_uiPhase = 1;
        }

        void UpdateAI(const uint32 /*uiDiff*/)
        {
            if (m_uiPhase == 1)
            {
                if (me->FindNearestGameObject(OBJECT_HAUNCH, 2.0f))
                {
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                }
                m_uiPhase = 0;
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tallhorn_stagAI(creature);
    }
};

// Amberpine Woodsman

enum eamberpinewoodsman
{
    TALLHORN_STAG                   = 26363
};

class npc_amberpine_woodsman : public CreatureScript
{
public:
    npc_amberpine_woodsman() : CreatureScript("npc_amberpine_woodsman") { }

    struct npc_amberpine_woodsmanAI : public ScriptedAI
    {
        npc_amberpine_woodsmanAI(Creature* creature) : ScriptedAI(creature) {}

        uint8 m_uiPhase;
        uint32 m_uiTimer;

        void Reset()
        {
            m_uiTimer = 0;
            m_uiPhase = 1;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            // call this each update tick?
            if (me->FindNearestCreature(TALLHORN_STAG, 0.2f))
            {
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
            }
            else
                if (m_uiPhase)
                {
                    if (m_uiTimer <= uiDiff)
                    {
                        switch (m_uiPhase)
                        {
                            case 1:
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_LOOT);
                                m_uiTimer = 3000;
                                m_uiPhase = 2;
                                break;
                            case 2:
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ATTACK1H);
                                m_uiTimer = 4000;
                                m_uiPhase = 1;
                                break;
                        }
                    }
                    else
                    m_uiTimer -= uiDiff;
                }
                ScriptedAI::UpdateAI(uiDiff);

            UpdateVictim();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amberpine_woodsmanAI(creature);
    }
};
/*######
## Quest 12288: Overwhelmed!
######*/

enum eSkirmisher
{
    SPELL_RENEW_SKIRMISHER  = 48812,
    CREDIT_NPC              = 27466,

    RANDOM_SAY_1             =  -1800044,        //Ahh..better..
    RANDOM_SAY_2             =  -1800045,        //Whoa.. i nearly died there. Thank you, $Race!
    RANDOM_SAY_3             =  -1800046         //Thank you. $Class!
};

class npc_wounded_skirmisher : public CreatureScript
{
public:
    npc_wounded_skirmisher() : CreatureScript("npc_wounded_skirmisher") { }

    struct npc_wounded_skirmisherAI : public ScriptedAI
    {
        npc_wounded_skirmisherAI(Creature* c) : ScriptedAI(c) {}

        uint64 uiPlayerGUID;

        uint32 DespawnTimer;

        void Reset ()
        {
            DespawnTimer = 5000;
            uiPlayerGUID = 0;
        }

        void MovementInform(uint32, uint32 id)
        {
            if (id == 1)
                me->DespawnOrUnsummon(DespawnTimer);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_RENEW_SKIRMISHER && caster->GetTypeId() == TYPEID_PLAYER
                && caster->ToPlayer()->GetQuestStatus(12288) == QUEST_STATUS_INCOMPLETE)
            {
                caster->ToPlayer()->KilledMonsterCredit(CREDIT_NPC, 0);
                DoScriptText(RAND(RANDOM_SAY_1, RANDOM_SAY_2, RANDOM_SAY_3), caster);
                if (me->IsStandState())
                    me->GetMotionMaster()->MovePoint(1, me->GetPositionX()+7, me->GetPositionY()+7, me->GetPositionZ());
                else
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->DespawnOrUnsummon(DespawnTimer);
                }

            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wounded_skirmisherAI(creature);
    }
};

/*Lightning Sentry - if you kill it when you have your Minion with you, you will get a quest credit*/
enum eSentry
{
    QUEST_OR_MAYBE_WE_DONT_A                     = 12138,
    QUEST_OR_MAYBE_WE_DONT_H                     = 12198,

    NPC_LIGHTNING_SENTRY                         = 26407,
    NPC_WAR_GOLEM                                = 27017,

    SPELL_CHARGED_SENTRY_TOTEM                   = 52703,
};

class npc_lightning_sentry : public CreatureScript
{
public:
    npc_lightning_sentry() : CreatureScript("npc_lightning_sentry") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lightning_sentryAI(creature);
    }

    struct npc_lightning_sentryAI : public ScriptedAI
    {
        npc_lightning_sentryAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 uiChargedSentryTotem;

        void Reset()
        {
            uiChargedSentryTotem = urand(10000, 12000);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (uiChargedSentryTotem <= uiDiff)
            {
                DoCast(SPELL_CHARGED_SENTRY_TOTEM);
                uiChargedSentryTotem = urand(10000, 12000);
            }
            else
                uiChargedSentryTotem -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (killer->ToPlayer() && killer->ToPlayer()->GetTypeId() == TYPEID_PLAYER)
            {
                if (me->FindNearestCreature(NPC_WAR_GOLEM, 10.0f, true))
                {
                    if (killer->ToPlayer()->GetQuestStatus(QUEST_OR_MAYBE_WE_DONT_A) == QUEST_STATUS_INCOMPLETE ||
                        killer->ToPlayer()->GetQuestStatus(QUEST_OR_MAYBE_WE_DONT_H) == QUEST_STATUS_INCOMPLETE)
                        killer->ToPlayer()->KilledMonsterCredit(NPC_WAR_GOLEM, 0);
                }
            }
        }
    };
};

/*Venture co. Straggler - when you cast Smoke Bomb, he will yell and run away*/
enum eSmokeEmOut
{
    SAY_SEO1                                     = -1603535,
    SAY_SEO2                                     = -1603536,
    SAY_SEO3                                     = -1603537,
    SAY_SEO4                                     = -1603538,
    SAY_SEO5                                     = -1603539,
    QUEST_SMOKE_EM_OUT_A                         = 12323,
    QUEST_SMOKE_EM_OUT_H                         = 12324,
    SPELL_SMOKE_BOMB                             = 49075,
    SPELL_CHOP                                   = 43410,
    NPC_VENTURE_CO_STABLES_KC                    = 27568,
};

class npc_venture_co_straggler : public CreatureScript
{
public:
    npc_venture_co_straggler() : CreatureScript("npc_venture_co_straggler") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venture_co_stragglerAI(creature);
    }

    struct npc_venture_co_stragglerAI : public ScriptedAI
    {
        npc_venture_co_stragglerAI(Creature* creature) : ScriptedAI(creature) { }

        uint64 uiPlayerGUID;
        uint32 uiRunAwayTimer;
        uint32 uiTimer;
        uint32 uiChopTimer;

        void Reset()
        {
            uiPlayerGUID = 0;
            uiTimer = 0;
            uiChopTimer = urand(10000, 12500);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (uiRunAwayTimer <= uiDiff)
            {
                if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                {
                    switch (uiTimer)
                    {
                        case 0:
                            if (player->GetQuestStatus(QUEST_SMOKE_EM_OUT_A) == QUEST_STATUS_INCOMPLETE ||
                                player->GetQuestStatus(QUEST_SMOKE_EM_OUT_H) == QUEST_STATUS_INCOMPLETE)
                                player->KilledMonsterCredit(NPC_VENTURE_CO_STABLES_KC, 0);
                            me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-7, me->GetPositionY()+7, me->GetPositionZ());
                            uiRunAwayTimer = 2500;
                            ++uiTimer;
                            break;
                        case 1:
                            DoScriptText(RAND(SAY_SEO1, SAY_SEO2, SAY_SEO3, SAY_SEO4, SAY_SEO5), me);
                            me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-7, me->GetPositionY()-5, me->GetPositionZ());
                            uiRunAwayTimer = 2500;
                            ++uiTimer;
                            break;
                        case 2:
                            me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-5, me->GetPositionY()-5, me->GetPositionZ());
                            uiRunAwayTimer = 2500;
                            ++uiTimer;
                            break;
                        case 3:
                            me->DisappearAndDie();
                            uiTimer = 0;
                            break;
                    }
                }
            }
            else
                uiRunAwayTimer -= uiDiff;

            if (!UpdateVictim())
                return;

            if (uiChopTimer <= uiDiff)
            {
                DoCast(me->getVictim(), SPELL_CHOP);
                uiChopTimer = urand(10000, 12000);
            }
            else
                uiChopTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void SpellHit(Unit* pCaster, const SpellInfo* pSpell)
        {
            if (pCaster && pCaster->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_SMOKE_BOMB)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_PASSIVE);
                me->CombatStop(false);
                uiPlayerGUID = pCaster->GetGUID();
                uiRunAwayTimer = 3500;
            }
        }
    };
};

/*####
## npc_lake_frog
####*/

enum eLakeFrog
{
    SPELL_WARTS                 = 62581,
    SPELL_WARTS_B_GONE          = 62574,
    SPELL_TRANSFORM             = 62550,
    SPELL_SUMMON_ASHWOOD_BRAND  = 62554,
    SPELL_FROG_LOVE             = 62537,

    ENTRY_NOT_MAIDEN_FROG       = 33211,
    ENTRY_MAIDEN_FROG           = 33224,
};

/*
UPDATE creature_template SET scriptname = 'npc_lake_frog' WHERE entry IN (33211,33224);
*/

#define SAY_FREED               "Can it really be? Free after all these years?"
#define GOSSIP_TEXT_GET_WEAPON  "Glad to help, my lady. I'm told you were once the guardian of a fabled sword. Do you know where I might find it?"

class npc_lake_frog : public CreatureScript
{
public:
    npc_lake_frog() : CreatureScript("npc_lake_frog") { }

    struct npc_lake_frogAI : public ScriptedAI
    {
        npc_lake_frogAI(Creature* c) : ScriptedAI(c) { alreadykissed = false;}

        bool alreadykissed;

        void ReceiveEmote(Player* player, uint32 emote)
        {
            if (emote == TEXT_EMOTE_KISS)
            {
                 alreadykissed = true;

                if(urand(0,3) == 0)
                {
                    DoCast(me,SPELL_TRANSFORM,true);
                    me->MonsterSay(SAY_FREED,LANG_UNIVERSAL,player->GetGUID());
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
                else
                {
                    if (!player->HasAura(SPELL_WARTS_B_GONE))
                    {
                        me->CastSpell(player,SPELL_WARTS);
                    }
                    else
                        player->RemoveAurasDueToSpell(SPELL_WARTS_B_GONE);

                        me->CastSpell(me,SPELL_FROG_LOVE,true);
                        me->GetMotionMaster()->MoveFollow(player,1,float(rand_norm()*2*M_PI));
                }

                me->DespawnOrUnsummon(15000);
            }
        }
    };

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,GOSSIP_TEXT_GET_WEAPON , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();

        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            creature->CastSpell(player,SPELL_SUMMON_ASHWOOD_BRAND,true);
        }
       
        return true;
    }


    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_lake_frogAI(creature);
    }
};

/*######
## Quest: Escape From Silverbrook (12308)
## NPC: Ducal's Horse (27409)
## NPC: Freed Alliance Scout (27411)
######*/
 
enum eEscapeFromSilverbrook
{
    QUEST_ESCAPE_FROM_SILVERBROOK           = 12308,
    SPELL_SUMMON_SILVERBROOK_WORGEN         = 48681,
    SPELL_ESCAPE_FROM_SILVERBROOK_PERIODIC  = 48683,
    SPELL_ESCAPE_FROM_SILVERBROOK_CREDIT    = 50473,
    NPC_DUCAL_HORSE                         = 27409
};
class npc_ducal_horse : public CreatureScript
{
public:
    npc_ducal_horse() : CreatureScript("npc_ducal_horse") { }
 
    struct npc_ducal_horseAI : public npc_escortAI
    {
        npc_ducal_horseAI(Creature* pCreature) : npc_escortAI(pCreature)
        {
            bSummon = false;
            bPlayerBoarded = false;
            bNpcBoarded = false;
        }
 
        bool bPlayerBoarded;
        bool bNpcBoarded;
        bool bSummon;
		bool jump1;
		bool jump2;
        uint64 uiPlayerGUID;
        uint32 uiSummonTimer;
 
        void PassengerBoarded(Unit *who, int8 seatId, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER && seatId == 1)
            {
                uiPlayerGUID = who->GetGUID();
                bPlayerBoarded = apply;
            }
            if (who->GetTypeId() == TYPEID_UNIT && seatId == 0)
            {
                bNpcBoarded = apply;
            }
            if (bPlayerBoarded && bNpcBoarded)
            {
                if (!HasEscortState(STATE_ESCORT_ESCORTING))
                {
                    bSummon = true;
                    uiSummonTimer = urand(5000, 10000);
					//me->setFaction(33);
                    Start(false, true, uiPlayerGUID);
                    //DoCast(SPELL_ESCAPE_FROM_SILVERBROOK_PERIODIC);
                }
            }
            if (!apply && seatId == 0)
                if (who->GetTypeId() == TYPEID_UNIT)
                    who->ToCreature()->DespawnOrUnsummon();
        }
 
        void JustDied(Unit* pKiller)
        {
			npc_escortAI::JustDied(pKiller);

            if (Vehicle* pVehicle = me->GetVehicleKit())
                if (Unit* pPassenger = pVehicle->GetPassenger(0))
                    if (pPassenger->GetTypeId() == TYPEID_UNIT)
                        pPassenger->ToCreature()->DespawnOrUnsummon();
        }
 
        void WaypointReached(uint32 uiWP)
        {
            switch (uiWP)
            {
				case 36:
					jump1 = true;
					break;
				case 74:
					jump2 = true;
					break;
				case 97:
					bSummon = false;
					break;
				case 102:
					if (Vehicle* pVehicle = me->GetVehicleKit())
						if (Unit* pPassenger = pVehicle->GetPassenger(0))
							pPassenger->ExitVehicle();
					if (Player* pPlayer = GetPlayerForEscort())
					{
						pPlayer->ExitVehicle();
						pPlayer->CastSpell(pPlayer, SPELL_ESCAPE_FROM_SILVERBROOK_CREDIT, true);
					}
					break;
            }
        }
 
        void JustSummoned(Creature* pSummoned)
        {
            pSummoned->AI()->AttackStart(me);
			pSummoned->AddThreat(me, 10000000.0f);
			me->AddThreat(pSummoned, 10000000.0f);
			pSummoned->DespawnOrUnsummon(60000);
        }
 
        void OnCharmed(bool /*apply*/)
        {
        }
 
        void UpdateEscortAI(const uint32 uiDiff)
        {
			if (jump1)
			{
				jump1 = false;
				me->GetMotionMaster()->MoveJump(4063.238525f, -2261.991211f, 215.988922f, 15.90f, 12.0f);
			}
			else if (jump2)
			{
				jump2 = false;
				me->GetMotionMaster()->MoveJump(3900.396484f, -2743.329346f, 219.152481f, 16.70f, 17.0f);
			}
            /*if (bSummon)
            {
                if (uiSummonTimer <= uiDiff)
                {
                    DoCast(SPELL_SUMMON_SILVERBROOK_WORGEN);
                    uiSummonTimer = urand(5000, 10000);
                } else uiSummonTimer -= uiDiff;
            }*/
        }
	};
 
    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_ducal_horseAI (pCreature);
    }
};
 
class npc_freed_alliance_scout : public CreatureScript
{
public:
    npc_freed_alliance_scout() : CreatureScript("npc_freed_alliance_scout") { }
 
    struct npc_freed_alliance_scoutAI : public npc_escortAI
    {
		npc_freed_alliance_scoutAI(Creature* pCreature) : npc_escortAI(pCreature), uiDucalHorseGUID(0) { }
 
        uint64 uiDucalHorseGUID;
        bool bTalk;
        uint8 uiTalkStage;
        uint32 uiTalkTimer;
 
        void WaypointReached(uint32 PointId)
        {
            if (PointId == 6)
            {
               if (Creature* pHorse = me->SummonCreature(NPC_DUCAL_HORSE, 4428.630371f, -2488.344238f, 246.493561f, 5.788373f, TEMPSUMMON_MANUAL_DESPAWN))
			   {
                    uiDucalHorseGUID = pHorse->GetGUID();
                    me->EnterVehicle(pHorse, 0);
			   }
                uiTalkTimer = 0;
                bTalk = true;
            }
        }
 
        void JustDied(Unit* pKiller)
        {
            if (Creature* pHorse = Unit::GetCreature(*me, uiDucalHorseGUID))
                pHorse->DespawnOrUnsummon();
        }
 
        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);
 
            if (bTalk)
            {
                if (uiTalkTimer <= uiDiff)
                {
                    switch (uiTalkStage)
                    {
                    case 0:
                        Talk(0);
                        uiTalkTimer = 5000;
                        break;
                    case 1:
                        Talk(1);
                        uiTalkTimer = 8000;
                        break;
                    case 2:
                        Talk(2);
                        Start(false, true);
                        SetDespawnAtEnd(false);
                        bTalk = false;
                        break;
                    case 3:
                        Talk(3);
                        uiTalkTimer = 5000;
                        bTalk = false;
                        break;
                    case 4:
                        Talk(4);
                        uiTalkTimer = 5000;
                        bTalk = false;
                        break;
                    }
                    uiTalkStage++;
                } else uiTalkTimer -= uiDiff;
            }
 
            if (!UpdateVictim())
                return;
 
            DoMeleeAttackIfReady();
        }
 
        void IsSummonedBy(Unit* owner)
        {
            if (owner->GetEntry() == 27499)
            {
                bTalk = true;
                uiTalkTimer = 0;
                uiTalkStage = 0;
            }
        }
    };
 
    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_freed_alliance_scoutAI (pCreature);
    }
};

enum ShredderDelivery
{
    NPC_BROKEN_DOWN_SHREDDER               = 27354
};


class spell_shredder_delivery : public SpellScriptLoader
{
    public:
        spell_shredder_delivery() : SpellScriptLoader("spell_shredder_delivery") { }


        class spell_shredder_delivery_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shredder_delivery_SpellScript);


            bool Load() 
            {
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }


            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->ToCreature()->GetEntry() == NPC_BROKEN_DOWN_SHREDDER)
                    GetCaster()->ToCreature()->DespawnOrUnsummon();
            }


            void Register() 
            {
                OnEffectHitTarget += SpellEffectFn(spell_shredder_delivery_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };


        SpellScript* GetSpellScript() const 
        {
            return new spell_shredder_delivery_SpellScript();
        }
};

void AddSC_grizzly_hills()
{
    new npc_orsonn_and_kodian;
    new npc_emily;
    new npc_mrfloppy;
    new npc_outhouse_bunny;
    new npc_tallhorn_stag;
    new npc_amberpine_woodsman;
    new npc_wounded_skirmisher;
    new npc_lightning_sentry();
    new npc_venture_co_straggler();
    new npc_lake_frog();
	new npc_ducal_horse();
    new npc_freed_alliance_scout();
    new spell_shredder_delivery();
}
