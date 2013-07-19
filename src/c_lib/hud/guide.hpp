#include <hud/hud.hpp>

namespace guide
{

    //block descriptions
    static const char* regolith_desc =
    "Regolith is a kind of dust commonly found on the surface of astronomical objects,\n"
    "including asteroids. In Gnomescroll, the block is probably the most accessible\n"
    "building material, but also one of the weakest.\n";

    static const char* rock_desc =
    "Rock is a block found in the deeper parts of the asteroid. Its main use is as a\n"
    "strong building material. It is better to mine it using a tool, due to the time\n"
    "it takes to destroy a single rock block.\n";

    static const char* iridium_desc =
    "Iridium is a rare element used in nuclear reactors. In Gnomescroll, you can make the\n"
    "finest shovels out of this material, as well as use it for other crafting recipes\n"
    "such as lasers. After mining, you get the ore, which you need to smelt in a furnace.\n";

    static const char* iron_desc =
    "Iron is a metal used for nearly everything. In Gnomescroll, you need to smelt it\n"
    "in a furnace to create iron bars, ready for crafting.\n";

    static const char* copper_desc =
    "Copper is a golden-coloured mineral found underground. It is used mainly for copper wires,\n"
    "but also for armor and crafting, mainly of electronic components. As all ores, you need to\n"
    "smelt it in a furnace before crafting.\n";

    static const char* gallium_desc =
    "Gallium is a rare metal found deep underground, which melts very easily and is useful\n"
    "for all sorts of things. It is very soft and thus makes weak shovels, but its value is\n"
    "compensated by the varied range of circuits and components using it. Like the other ores,\n"
    "smelting is required to obtain a gallium bar.\n";

    static const char* silicon_desc =
    "Silicon is an extremely useful element found only in meteors. The main use is electonics,\n"
    "but the crystal dagger also requires silicon for its recipe.\n";

    static const char* methice_desc =
    "Methane ice is the crystalized form of CH4, an explosive gas used for smelting. If not put\n"
    "in a cryofreezer, methane ice will slowly evaporate out of containers and the inventory.\n"
    "Other uses include explosives and charge packs.\n";

    static const char* coal_desc =
    "Coal, in its crystal form - graphite, is a very useful smelting material, providing lots\n"
    "of heat for a long time. It does not have the annoying habit of evaporating like methane ice.\n";

    static const char* furnace_desc =
    "A furnace is a block used for smelting. Right-click it to open its inventory. Place your items\n"
    "in the slots by dragging them there with the left mouse button. Press E to close the inventory.\n"
    "When the furnace runs out of fuel, the fire inside will stop burning. When that happens, open\n"
    "the furnace inventory and take out your freshly-smelted items.";

    static const char* crafting_desc =
    "A crafting bench is used for creating one item out of another. Place your items in the\n"
    "item slots on the left and take out the item on the right. If nothing appears, your recipe\n"
    "is wrong and you should research it again. These are some basic recipes:\n"
    "any metal bar + another bar of the same metal -> a shovel\n"
    "any metal bar + any crystal -> a laser\n"
    "iron bar + iron bar + iron bar + iron bar -> a storage block\n"
    "any metal bar + methane ice -> a charge pack(right-click to consume)\n"
    "More recipes can be found online, or you can just ask other players.\n";

    //basic stuff descriptions
    static const char* inventory_desc =
    "This is your inventory. Inside, you can find the items that you have collected.\n"
    "The bottom slots are the items you can hold in your hand. To hold them,\n"
    "press the corresponding number key or scroll your mouse. To use them, right- or left-click\n"
    "while holding the desired item. You can move items between slots by dragging them with the mouse.\n"
    "In middle are the items you have, but cannot hold. You can move them to the lower section.\n"
    "In the upper part are the items you can buy using synthesizer chips. Next to each item, its cost is\n"
    "displayed. Synthesizer chips can be obtained as a reward by mining blocks, killing mobs etc.\n"
    //"They can also be bought online via our store.\n" //ready for premium!
    "You can see the current amount of synthesizer chips you have in a slot with them.\n"
    "If you die, all the items inside your inventory are dropped except for the ones in the lower bar.\n"
    //"If you are a premium user, you also get a safe pouch for some items that you can hide them in\n"
    //"so that nobody steals them.\n" //premium ready!
    "";

    static const char* guide_desc =
    "Welcome to Gnomescroll! In this guide, you will learn the basics of the ever-expanding universe\n"
    "of the game. Press the semicolon \";\" to get a description of something you're looking at.\n"
    "For a start, try to mine some blocks by pressing and holding your left mouse button. You can\n"
    "collect the item that falls out by simply walking on top of it, or next to it. An online\n"
    "tutorial is avaliable on gnomescroll.com/how-to-play, the wiki is on wiki.gnomescroll.com,\n"
    "and live help is avaliable on gnomescroll.com/contact.\n";

    CubeType regolith = t_map::get_cube_type("regolith");
    CubeType rock = t_map::get_cube_type("rock");
    CubeType iridium = t_map::get_cube_type("iridium_ore");
    CubeType iron = t_map::get_cube_type("iron_ore");
    CubeType copper = t_map::get_cube_type("copper_ore");
    CubeType gallium = t_map::get_cube_type("gallium_ore");
    CubeType silicon = t_map::get_cube_type("silicon_ore");
    CubeType methice = t_map::get_cube_type("methane_ice");
    CubeType coal = t_map::get_cube_type("coal");
    CubeType furnace = t_map::get_cube_type("smelter_basic");
    CubeType crafting = t_map::get_cube_type("crafting_bench_basic");

    void draw_guide()
    {
        using ClientState::hitscan;
        ItemType equipped_type = Toolbelt::get_selected_item_type();
        float range = Item::get_weapon_range(equipped_type);
        if (hitscan.distance > range || hitscan.type == HITSCAN_TARGET_NONE)
        return;

        switch (hitscan.type)
        {
            case HITSCAN_TARGET_VOXEL:
            if (hitscan.voxel_target.entity_type == ENTITY_AGENT)
            {
                Agents::Agent* agent = Agents::get_agent(AgentID(hitscan.voxel_target.entity_id));
                if (agent != NULL)
                Hud::set_prompt(agent->status.name);
            }
            else
            Hud::set_prompt(Entities::get_entity_name(hitscan.voxel_target.entity_type));
            break;

            case HITSCAN_TARGET_BLOCK:
            Hud::set_prompt(t_map::get_cube_pretty_name(hitscan.cube_type));
            break;

            case HITSCAN_TARGET_MECH:
            {
                MechType mtype = t_mech::get_mech_type(hitscan.mech_id);
                if (isValid(mtype))
                {
                    if (mtype == t_mech::get_mech_type("terminal_basic"))
                    {
                        const char* text = t_mech::get_mech_text(hitscan.mech_id);
                        Hud::set_prompt(text);
                    }
                    else
                    Hud::set_prompt(t_mech::get_mech_pretty_name(mtype));
                }
            }
            break;

            case HITSCAN_TARGET_SPRITE_MOB:
            {
                t_mob::SpriteMob* mob = t_mob::get_sprite_mob(hitscan.sprite_mob_id);
                if (mob != NULL)
                Hud::set_prompt(t_mob::get_mob_pretty_name(mob->type));
            }
            break;

            case HITSCAN_TARGET_NONE:
            break;
        }

        Hud::hud->target->draw();
    }
}//guide
