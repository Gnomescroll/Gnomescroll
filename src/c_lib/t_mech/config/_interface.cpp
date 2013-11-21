/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <t_mech/config/mech_dat.hpp>

namespace t_mech
{

void end_mech_dat()
{
    mech_attributes->done_loading();
    mech_attributes->set_pretty_names();
    apply_mech_dat_changes();   // handle renaming/removals
    verify_mech_dat();  // validate
    save_mech_names();  // safe to save names now
}

}   // t_mech
