/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "bone_tree.hpp"

namespace t_mob
{

BoneTree::~BoneTree()
{
    if (s != NULL) SDL_FreeSurface(s);
    if (pScene != NULL) aiReleaseImport(pScene);
    if (nl != NULL) delete[] nl;
    if (ml != NULL) delete[] ml;
    if (tvl != NULL) delete[] tvl;
    if (vll != NULL) delete[] vll;
    if (vln != NULL) delete[] vln;
    if (bvl != NULL) delete[] bvl;
    if (tbvl != NULL) delete[] tbvl;
    if (bvlo != NULL) delete[] bvlo;
    if (bvln != NULL) delete[] bvln;
    if (bvll != NULL) delete[] bvll;
}


}   // t_mob
