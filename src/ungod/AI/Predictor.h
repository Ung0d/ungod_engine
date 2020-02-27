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

#ifndef UNGOD_AI_PREDICTOR_H
#define UNGOD_AI_PREDICTOR_H

#include "ungod/AI/Model.h"

namespace ungod
{
    namespace AI
    {
        template<typename T, int DIM>
        class Tensor
        {
        static_assert(DIM > 0, "Tensor requires at least one dimension.");
        public:
            Tensor(T* data, const std::array<T, DIM>& shape) : mData(data), mShape(shape) {}

            void set(const std::array<T, DIM>& pos, T t);

        private:
            T* mData;
            std::array<T, DIM> mShape;

        private:
            int dimsBelow(int dim);
        };


        /** 
        * \brief An async predictor that feeds an input to an underlying machine learning model
        * and returns a future for the computed output.
        */
        class Predictor
        {
        public:
            Predictor() = default;

            Predictor(Model model);

            void setModel(Model model);

            /** \brief Returns an input tensor by name */
            template<typename T, int DIM>
            Tensor<T, DIM> getInputTensor(const std::string& name);

        private:
            Model mModel;
        };




        template<typename T, int DIM>
        void Tensor<T, DIM>::set(const std::array<T, DIM>& pos, T t)
        {
            T* toset = mData;
            for (int dim = 0; dim < DIM; dim++)
                toset += pos[dim] * dimsBelow(dim);
            (*toset) = t;
        }

        template<typename T, int DIM>
        int Tensor<T, DIM>::dimsBelow(int dim)
        {
            int dims = 1;
            for (int i = dim + 1; i < DIM; i++)
                dims *= mShape[dim];
            return dims;
        }


        template<typename T, int DIM>
        Tensor<T, DIM> Predictor::getInputTensor(const std::string& name)
        {

        }
    }
}


#endif // UNGOD_AI_PREDICTOR_H

