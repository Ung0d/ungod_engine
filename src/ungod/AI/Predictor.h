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
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"

namespace ungod
{
    namespace AI
    {
        template<typename T>
        class Tensor
        {
        public:
            Tensor(T* data, const std::vector<int>& shape) : mData(data), mShape(shape) 
            {
                ungod::Logger::assertion(mShape.size() > 0, "Tensor requires at least one dimension.");
            }

            void set(const std::vector<int>& pos, T t);

        private:
            T* mData;
            std::vector<int> mShape;

        private:
            int dimsBelow(int dim);
        };


        /** 
        * \brief An predictor that feeds an input to an underlying machine learning model and returns the computed output.
        * Is also responsible for loading and interpreting a model before inference.
        */
        class Predictor
        {
        public:
            Predictor() = default;

            bool setModel(Model model);

            /** \brief Returns input tensors to set. Reallocates on first invoke and every invoke with dims unqual to the 
            * previous call. */
            template<typename T, int DIM>
            std::unordered_map<std::string, Tensor<T>> getInputTensors(const std::vector<std::string>& names, const std::vector<std::vector< int >>& dims);

            /** \brief Invokes the model and returns the computed output tensors. */


        private:
            Model mModel;
            std::unique_ptr<tflite::Interpreter> mInterpreter;
            int mCurNumNodes;
            int mCurNumEdges;
            std::unordered_map<std::string, std::vector< int >> mCurInputDims;

        private:
            int getInputTensorID(const std::string& name);
        };




        template<typename T>
        void Tensor<T>::set(const std::vector<int>& pos, T t)
        {
            T* toset = mData;
            for (int dim = 0; dim < mShape.size(); dim++)
                toset += pos[dim] * dimsBelow(dim);
            (*toset) = t;
        }

        template<typename T>
        int Tensor<T>::dimsBelow(int dim)
        {
            int dims = 1;
            for (int i = dim + 1; i < mShape.size(); i++)
                dims *= mShape[dim];
            return dims;
        }


        template<typename T>
        std::unordered_map<std::string, Tensor<T>> getInputTensors(const std::vector<std::string>& names, const std::vector<std::vector< int >>& dims)
        {
            ungod::Logger::assertion(names.size() == dims.size(), "names and batch_sizes need equal length");
            ungod::Logger::assertion(names.size() == this->mInterpreter->inputs().size(), "number of model inputs and requested tensors do not match");
            std::unordered_map<std::string, Tensor<T>> inputs;
            bool allocationRequired = false;
            std::vector<int> ids{ names.size() };
            for (int i = 0; i < names.size(); i++)
            {
                ids[i] = this->getInputTensorID(names[i]);
                ungod::Logger::assertion(ids[i] != -1, "no input tensor with name " + names[i]);
                auto batch = this->mCurInputDims.insert({ names[i], {-1} }).first;
                if (batch->second != dims[i])
                {
                    allocationRequired = true;
                    batch->second = dims[i];
                    this->mInterpreter->ResizeInputTensor(ids[i], dims[i]);
                }
            }
            if (allocationRequired)
                ungod::Logger::assertion(this->mInterpreter->AllocateTensors() == kTfLiteOk, "can not allocate tensors");
            for (int i = 0; i < names.size(); i++)
            {
                inputs.emplace(names[i], this->mInterpreter->typed_input_tensor<T>(ids[i]), dims[i]);
            }
            return inputs;
        }
    }
}


#endif // UNGOD_AI_PREDICTOR_H

