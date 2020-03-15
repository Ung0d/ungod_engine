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

namespace ungod
{
    namespace AI
    {
        /** \brief A node in a monte carlo search tree.
        */
       /* class MonteCarloNode
        {
        public:
            MonteCarloNode(float prior = 0.0f);

            bool isLeaf() const;

            float value() const;*/

            /** \brief Creates a new child node for each valid action and computes priors based on predicted logits. */
           /* void expand(const std::vector<float>& logits, const std::vector<bool>& valid);

            std::tuple<int, MonteCarloNode*> selectChild(float pbCBase, float pbCInit);

        private:
            int mVisitCount;
            float mPrior;
            float mValueSum;
            std::unordered_map<int, std::unique_ptr<MonteCarloNode>> mChildren;
        };*/

        /** 
        * \brief Class that runs monte carlo tree search with a predictor
        * to decide on actions for a scene of actors.
        */
       /* class TreeSearch
        {
        public:
            TreeSearch(Predictor& predictor) : mPredictor(predictor) {}

            void decide(const SceneSpecs& scene);

        private:
            Predictor mPredictor;

        private:
            void rolloutOne(const SceneSpecs& scene);
        };*/
    }
}


#endif // UNGOD_AI_TREE_SEARCH_H

