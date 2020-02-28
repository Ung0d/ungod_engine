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

#include "ungod/AI/TreeSearch.h"
#include <cmath>
#include <limits>

namespace ungod
{
    namespace AI
    {
        MonteCarloNode::MonteCarloNode(float prior) : mVisitCount(0), mPrior(prior), mValueSum(0.0f) {}

        bool MonteCarloNode::isLeaf() const
        {
            return mChildren.empty();
        }

        float MonteCarloNode::value() const
        {
            if mVisitCount == 0
                return 0.0f;
            else
                return mValueSum / mVisitCount;
        }

        void MonteCarloNode::expand(const std::vector<float>& logits, const std::vector<bool>& valid)
        {
            ungod::Logger::assertion(mChildren.empty(), "Can not expand a non leaf.");
            ungod::Logger::assertion(logits.size() == valid.size(), "Logit and valid vectors do not have matching sizes.");
            float expSum = 0.0f;
            for (int i = 0; i < logits.size(); i++)
            {
                if (valid[i])
                {
                    float expLogit = exp(logits[i]);
                    mChildren.emplace(i, std::make_unique<MonteCarloNode>(expLogit));
                    expSum += expLogit;
                }
            }
            for (auto& element : mChildren)
                element.second->scalePrior(1/ expSum);
        }

        std::tuple<int, MonteCarloNode*> MonteCarloNode::selectChild(float pbCBase, float pbCInit)
        {   
            //compute UCB score for each child and return the node with the highest value
            int action = -1; 
            MonteCarloNode* child = nullptr;
            float bestScore = std::numeric_limits<float>::lowest();
            for (auto& element : mChildren)
            {
                float pbc = log((mVisitCount + pbCBase + 1) / pbCBase) + pbCInit;
                pbc *= sqrt(mVisitCount / (element.second->mVisitCount + 1));
                float ucbScore = pbc * element.second->mPrior + element.second->value();

                if (ucbScore > bestScore)
                {
                    action = element.first;
                    child = element.second.get();
                    bestScore = ucbScore;
                }
            }
            return { action, child };
        }

        void MonteCarloNode::scalePrior(float scale)
        {
            mPrior *= scale;
        }
    }
}
