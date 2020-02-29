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

#ifndef UNGOD_AI_TREE_SEARCH_H
#define UNGOD_AI_TREE_SEARCH_H

#include <tuple>
#include "ungod/AI/Predictor.h"
#include "ungod/AI/SceneSpecs.h"
#include "ungod/serialization/Serializable.h"

namespace ungod
{
    namespace AI
    {
        /** \brief Extracts SceneSpecs from a world and passes them to a TreeSearch predictor. */
        class SceneDirector
        {
        public:
            SceneDirector(const World& world) : mWorld(world) {}

            /** \brief Periodically checks whether a thinking phase is over and if so,  */

        private:
            const World& mWorld;
        };

        /**
        * \ingroup Components
        * \brief A component for entities, that are affected by AI. The engine handles entities with this component as
        * "mutable" when creating a SceneSimulator. Dynamic objects are copied since they will change their state when the AI
        * system tries to look ahead through a tree search based simulation. */
        class DynamicObjectComponent : public Serializable<DynamicObjectComponent>
        {
        }

        /** \brief A resetable view on a scene with dynamic objects that can be used to conduct a simulation that looks ahead
        * to decide on actions. Does not modify the underlying world and its contents when simulating. */
        class SceneSimulator
        {
        public:
            SceneSimulator() = default;

            /** \brief Resets the simulator to the state where it was constructed. */
            void reset();

            /** \brief Applies the given actions to all actors. */
            void applyActions(const std::vector<int>& actions);

        private:

        };
    }
}


#endif // UNGOD_AI_TREE_SEARCH_H

