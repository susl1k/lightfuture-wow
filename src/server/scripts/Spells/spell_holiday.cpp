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
 * Spells used in holidays/game events that do not fit any other category.
 * Scriptnames in this file should be prefixed with "spell_#holidayname_".
 */

#include "ScriptPCH.h"

// 45102 Romantic Picnic
enum SpellsPicnic
{
    SPELL_BASKET_CHECK              = 45119, // Holiday - Valentine - Romantic Picnic Near Basket Check
    SPELL_MEAL_PERIODIC             = 45103, // Holiday - Valentine - Romantic Picnic Meal Periodic - effect dummy
    SPELL_MEAL_EAT_VISUAL           = 45120, // Holiday - Valentine - Romantic Picnic Meal Eat Visual
    //SPELL_MEAL_PARTICLE             = 45114, // Holiday - Valentine - Romantic Picnic Meal Particle - unused
    SPELL_DRINK_VISUAL              = 45121, // Holiday - Valentine - Romantic Picnic Drink Visual
    SPELL_ROMANTIC_PICNIC_ACHIEV    = 45123, // Romantic Picnic periodic = 5000
};

class spell_love_is_in_the_air_romantic_picnic : public SpellScriptLoader
{
    public:
        spell_love_is_in_the_air_romantic_picnic() : SpellScriptLoader("spell_love_is_in_the_air_romantic_picnic") { }

        class spell_love_is_in_the_air_romantic_picnic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_love_is_in_the_air_romantic_picnic_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->SetStandState(UNIT_STAND_STATE_SIT);
                target->CastSpell(target, SPELL_MEAL_PERIODIC, false);
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                // Every 5 seconds
                Unit* target = GetTarget();
                Unit* caster = GetCaster();

                // If our player is no longer sit, remove all auras
                if (target->getStandState() != UNIT_STAND_STATE_SIT)
                {
                    target->RemoveAura(SPELL_ROMANTIC_PICNIC_ACHIEV);
                    target->RemoveAura(GetAura());
                    return;
                }

                target->CastSpell(target, SPELL_BASKET_CHECK, false); // unknown use, it targets Romantic Basket
                target->CastSpell(target, RAND(SPELL_MEAL_EAT_VISUAL, SPELL_DRINK_VISUAL), false);

                bool foundSomeone = false;
                // For nearby players, check if they have the same aura. If so, cast Romantic Picnic (45123)
                // required by achievement and "hearts" visual
                std::list<Player*> playerList;
                Trinity::AnyPlayerInObjectRangeCheck checker(target, INTERACTION_DISTANCE*2);
                Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(target, playerList, checker);
                target->VisitNearbyWorldObject(INTERACTION_DISTANCE*2, searcher);
                for (std::list<Player*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    if ((*itr) != target && (*itr)->HasAura(GetId())) // && (*itr)->getStandState() == UNIT_STAND_STATE_SIT)
                    {
                        if (caster)
                        {
                            caster->CastSpell(*itr, SPELL_ROMANTIC_PICNIC_ACHIEV, true);
                            caster->CastSpell(target, SPELL_ROMANTIC_PICNIC_ACHIEV, true);
                        }
                        foundSomeone = true;
                        // break;
                    }
                }

                if (!foundSomeone && target->HasAura(SPELL_ROMANTIC_PICNIC_ACHIEV))
                    target->RemoveAura(SPELL_ROMANTIC_PICNIC_ACHIEV);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_love_is_in_the_air_romantic_picnic_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_love_is_in_the_air_romantic_picnic_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_love_is_in_the_air_romantic_picnic_AuraScript();
        }
};

enum HalloweenData
{
    NPC_STINKY_BOMB_CREDIT                  = 15415,

    GO_STINKY_BOMB_FLASK                    = 180449,
    GO_STINKY_BOMB_CLOUD                    = 180450,

    QUEST_CRASHING_WICKERMAN_FESTIVAL       = 1658,
    //costumes
    SPELL_PIRATE_COSTUME_MALE               = 24708,
    SPELL_PIRATE_COSTUME_FEMALE             = 24709,
    SPELL_NINJA_COSTUME_MALE                = 24711,
    SPELL_NINJA_COSTUME_FEMALE              = 24710,
    SPELL_LEPER_GNOME_COSTUME_MALE          = 24712,
    SPELL_LEPER_GNOME_COSTUME_FEMALE        = 24713,
    SPELL_GHOST_COSTUME_MALE                = 24735,
    SPELL_GHOST_COSTUME_FEMALE              = 24736,
    //wand spells
    SPELL_HALLOWEEN_WAND_PIRATE             = 24717,
    SPELL_HALLOWEEN_WAND_NINJA              = 24718,
    SPELL_HALLOWEEN_WAND_LEPER_GNOME        = 24719,
    SPELL_HALLOWEEN_WAND_RANDOM             = 24720,
    SPELL_HALLOWEEN_WAND_SKELETON           = 24724,
    SPELL_HALLOWEEN_WAND_WISP               = 24733,
    SPELL_HALLOWEEN_WAND_GHOST              = 24737,
    SPELL_HALLOWEEN_WAND_BAT                = 24741,

    SPELL_GRIM_VISAGE                       = 24705,

    // Headless Horseman fire event
    NPC_HEADLESS_FIRE                       = 23537,
    NPC_FIRE_DUMMY                          = 23686,
    NPC_SHADE_HORSEMAN                      = 23543,
    GO_FIRE_EFFIGY                          = 186720,
    GO_LARGE_JACK_O_LANTERN                 = 186887,


    SPELL_FIRE_CREATE_NODE                  = 42118,
    SPELL_WATER_SPOUT_VISUAL                = 42348,
    SPELL_FIRE_VISUAL_BUFF                  = 42074,
    SPELL_FIRE_SIZE_STACK                   = 42091,
    SPELL_FIRE_STARTING_SIZE                = 42096,
    SPELL_QUESTS_CREDITS                    = 42242,
    SPELL_CREATE_WATER_BUCKET               = 42349,

    SPELL_HORSEMAN_CONFLAGRATION_1          = 42380,
    SPELL_HORSEMAN_CONFLAGRATION_2          = 42869,
    SPELL_HORSEMAN_JACK_O_LANTERN           = 44185,
    SPELL_HORSEMAN_CLEAVE                   = 15496,

    SAY_HORSEMAN_SPAWN                      = 1,
    SAY_HORSEMAN_FIRES_OUT                  = 2,
    SAY_HORSEMAN_FIRES_FAIL                 = 3,
    SAY_HORSEMAN_LAUGHS                     = 4,
    SAY_DEATH                               = 5,

    QUEST_LET_THE_FIRES_COME_A              = 12135,
    QUEST_LET_THE_FIRES_COME_H              = 12139,
    QUEST_STOP_FIRES_A                      = 11131,
    QUEST_STOP_FIRES_H                      = 11219,
};

class spell_halloween_wand : public SpellScriptLoader
{
public:
    spell_halloween_wand() : SpellScriptLoader("spell_halloween_wand") {}

    class spell_halloween_wand_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_halloween_wand_SpellScript)

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_PIRATE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_NINJA))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_LEPER_GNOME))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_RANDOM))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_SKELETON))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_WISP))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_GHOST))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_BAT))
                return false;
            return true;
        }

        void HandleScriptEffect()
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitPlayer();

            if (!caster || !target)
                return;

            uint32 spellId = 0;
            uint8 gender = target->getGender();

            switch (GetSpellInfo()->Id)
            {
                case SPELL_HALLOWEEN_WAND_LEPER_GNOME:
                    spellId = gender ? SPELL_LEPER_GNOME_COSTUME_FEMALE : SPELL_LEPER_GNOME_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_PIRATE:
                    spellId = gender ? SPELL_PIRATE_COSTUME_FEMALE : SPELL_PIRATE_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_GHOST:
                    spellId = gender ? SPELL_GHOST_COSTUME_FEMALE : SPELL_GHOST_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_NINJA:
                    spellId = gender ? SPELL_NINJA_COSTUME_FEMALE : SPELL_NINJA_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_RANDOM:
                    spellId = RAND(SPELL_HALLOWEEN_WAND_PIRATE, SPELL_HALLOWEEN_WAND_NINJA, SPELL_HALLOWEEN_WAND_LEPER_GNOME, SPELL_HALLOWEEN_WAND_SKELETON, SPELL_HALLOWEEN_WAND_WISP, SPELL_HALLOWEEN_WAND_GHOST, SPELL_HALLOWEEN_WAND_BAT);
                    break;
            }
            caster->CastSpell(target, spellId, true);
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_halloween_wand_SpellScript::HandleScriptEffect);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_halloween_wand_SpellScript();
    }
};



// 24750 Trick
enum TrickSpells
{
    SPELL_SKELETON_COSTUME              = 24723,
    SPELL_TRICK_BUFF                    = 24753,
};

class spell_hallow_end_trick : public SpellScriptLoader
{
    public:
        spell_hallow_end_trick() : SpellScriptLoader("spell_hallow_end_trick") { }

        class spell_hallow_end_trick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_trick_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE)
                    || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE)
                    || !sSpellMgr->GetSpellInfo(SPELL_SKELETON_COSTUME) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_TRICK_BUFF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    uint8 gender = target->getGender();
                    uint32 spellId = SPELL_TRICK_BUFF;
                    switch (urand(0, 5))
                    {
                        case 1:
                            spellId = gender ? SPELL_LEPER_GNOME_COSTUME_FEMALE : SPELL_LEPER_GNOME_COSTUME_MALE;
                            break;
                        case 2:
                            spellId = gender ? SPELL_PIRATE_COSTUME_FEMALE : SPELL_PIRATE_COSTUME_MALE;
                            break;
                        case 3:
                            spellId = gender ? SPELL_GHOST_COSTUME_FEMALE : SPELL_GHOST_COSTUME_MALE;
                            break;
                        case 4:
                            spellId = gender ? SPELL_NINJA_COSTUME_FEMALE : SPELL_NINJA_COSTUME_MALE;
                            break;
                        case 5:
                            spellId = SPELL_SKELETON_COSTUME;
                            break;
                        default:
                            break;
                    }

                    caster->CastSpell(target, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hallow_end_trick_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hallow_end_trick_SpellScript();
        }
};

// 24751 Trick or Treat
enum TrickOrTreatSpells
{
    SPELL_TRICK                 = 24714,
    SPELL_TREAT                 = 24715,
    SPELL_TRICKED_OR_TREATED    = 24755,
    SPELL_TRICKY_TREAT_SPEED    = 42919,
    SPELL_TRICKY_TREAT_TRIGGER  = 42965,
    SPELL_UPSET_TUMMY           = 42966
};

class spell_hallow_end_trick_or_treat : public SpellScriptLoader
{
    public:
        spell_hallow_end_trick_or_treat() : SpellScriptLoader("spell_hallow_end_trick_or_treat") {}

        class spell_hallow_end_trick_or_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_trick_or_treat_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICK) || !sSpellMgr->GetSpellInfo(SPELL_TREAT) || !sSpellMgr->GetSpellInfo(SPELL_TRICKED_OR_TREATED))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    caster->CastSpell(target, roll_chance_i(50) ? SPELL_TRICK : SPELL_TREAT, true);
                    caster->CastSpell(target, SPELL_TRICKED_OR_TREATED, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hallow_end_trick_or_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hallow_end_trick_or_treat_SpellScript();
        }
};

class spell_hallow_end_tricky_treat : public SpellScriptLoader
{
    public:
        spell_hallow_end_tricky_treat() : SpellScriptLoader("spell_hallow_end_tricky_treat") { }

        class spell_hallow_end_tricky_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hallow_end_tricky_treat_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_SPEED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_TRIGGER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_UPSET_TUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->HasAura(SPELL_TRICKY_TREAT_TRIGGER) && caster->GetAuraCount(SPELL_TRICKY_TREAT_SPEED) > 3 && roll_chance_i(33))
                    caster->CastSpell(caster, SPELL_UPSET_TUMMY, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hallow_end_tricky_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hallow_end_tricky_treat_SpellScript();
        }
};

enum Mistletoe
{
    SPELL_CREATE_MISTLETOE          = 26206,
    SPELL_CREATE_HOLLY              = 26207,
    SPELL_CREATE_SNOWFLAKES         = 45036
};

class spell_winter_veil_mistletoe : public SpellScriptLoader
{
    public:
        spell_winter_veil_mistletoe() : SpellScriptLoader("spell_winter_veil_mistletoe") { }

        class spell_winter_veil_mistletoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_winter_veil_mistletoe_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_MISTLETOE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_CREATE_HOLLY) ||
                    !sSpellMgr->GetSpellInfo(SPELL_CREATE_SNOWFLAKES))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* target = GetHitPlayer())
                {
                    uint32 spellId = RAND(SPELL_CREATE_HOLLY, SPELL_CREATE_MISTLETOE, SPELL_CREATE_SNOWFLAKES);
                    GetCaster()->CastSpell(target, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_winter_veil_mistletoe_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_winter_veil_mistletoe_SpellScript();
        }
};

// 26275 - PX-238 Winter Wondervolt TRAP
enum PX238WinterWondervolt
{
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_1  = 26157,
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_2  = 26272,
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_3  = 26273,
    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_4  = 26274
};

class spell_winter_veil_px_238_winter_wondervolt : public SpellScriptLoader
{
    public:
        spell_winter_veil_px_238_winter_wondervolt() : SpellScriptLoader("spell_winter_veil_px_238_winter_wondervolt") { }

        class spell_winter_veil_px_238_winter_wondervolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_winter_veil_px_238_winter_wondervolt_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_1) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_2) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_3) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_4))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                uint32 const spells[4] =
                {
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_1,
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_2,
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_3,
                    SPELL_PX_238_WINTER_WONDERVOLT_TRANSFORM_4
                };

                if (Unit* target = GetHitUnit())
                {
                    for (uint8 i = 0; i < 4; ++i)
                        if (target->HasAura(spells[i]))
                            return;

                    target->CastSpell(target, spells[urand(0, 3)], true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_winter_veil_px_238_winter_wondervolt_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }

        private:

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_winter_veil_px_238_winter_wondervolt_SpellScript();
        }
};

class spell_racer_rocket_slam : public SpellScriptLoader
{
public:
	spell_racer_rocket_slam() : SpellScriptLoader("spell_racer_rocket_slam") { }

	class spell_racer_rocket_slam_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_racer_rocket_slam_SpellScript);

		void HandleScript()
		{
			Unit* caster = GetCaster();
			if (!caster->ToCreature())
				return;
			if (caster == caster->FindNearestCreature(caster->GetEntry(), 25.0f, true))
				sLog->outError("nachazim spatny NPC");
			std::list<Creature*> list;
			caster->GetCreatureListWithEntryInGrid(list, caster->GetEntry(), 25.0f);
			sLog->outError("hledam %d", list.size());
			for (std::list<Creature*>::iterator i = list.begin(); i != list.end(); i++)
			{
				if (*i != caster)
					if (caster->HasInArc(0.15f, *i))
					{
						caster->GetMotionMaster()->MoveCharge((*i)->GetPositionX(), (*i)->GetPositionY(), (*i)->GetPositionZ());
						if (caster->ToTempSummon())
							if (Unit* owner = caster->ToTempSummon()->GetSummoner())
								if (Player* player = owner->ToPlayer())
								{
									player->AddAura(49444, player);
									if (!player->HasAchieved(1295))
										if (player->GetAura(49444)->GetStackAmount() >= 25)
										{
											AchievementEntry const* pAE = sAchievementStore.LookupEntry(1295);
											player->CompletedAchievement(pAE);
										}
								}
						caster->Kill(*i);
						return;
					}
			}

			Position charge;
			caster->GetNearPosition(charge, 25.0f, 0.0f);
			caster->GetMotionMaster()->MoveCharge(charge.GetPositionX(), charge.GetPositionY(), charge.GetPositionZ());
		}

		void Register()
		{
			OnCast += SpellCastFn(spell_racer_rocket_slam_SpellScript::HandleScript);
			//OnEffectHitTarget += SpellEffectFn(spell_racer_rocket_slam_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_racer_rocket_slam_SpellScript();
	}
};

void AddSC_holiday_spell_scripts()
{
    new spell_love_is_in_the_air_romantic_picnic();
	// Hallow's End
	new spell_halloween_wand();
	new spell_hallow_end_trick();
    new spell_hallow_end_trick_or_treat();
    new spell_hallow_end_tricky_treat();
    // Winter Veil
    new spell_winter_veil_mistletoe();
    new spell_winter_veil_px_238_winter_wondervolt();
	new spell_racer_rocket_slam();
}
