/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#ifndef UNGOD_FLOOD_FILL_H
#define UNGOD_FLOOD_FILL_H

#include "ungod/content/TileMap.h"
#include "ungod/base/Utility.h"

namespace ungod
{
    /** \brief Performs floot fill on the given tilemap with a given init-position.
    * This will replace the ids of all tiles in a connected area of tiles with the same id.
    * Selects randomly from the given vector of ids to fill the flootable space.
    * If the appropriate flag is set, inactive tiles will automatically be activated.*/
    void floodFill(TileMap& tilemap, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs);
}

#endif // UNGOD_FLOOD_FILL_H
