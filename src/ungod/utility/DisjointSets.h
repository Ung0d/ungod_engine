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

#ifndef UNGOD_DISJOINT_SETS_H
#define UNGOD_DISJOINT_SETS_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <forward_list>
#include <algorithm>

namespace ungod
{
    /**
    * \brief Data-structure that maintains a set of disjoint sets.
    * Each set has a representative element that will be always the element of smallest index. Sets can be efficiently
    * retrieved (through any element in the requested set as key) and merged.
    */
    template<typename T>
    class DisjointSets
    {
    public:
    class SetElement;
        /** \brief Structure that manages a set of elments with a single-linked list. */
        struct Set
        {
            std::forward_list< SetElement* > clist;
            typename std::forward_list< SetElement* >::iterator tail; //<points always to the end of clist
            unsigned size; //< counts the elements in the set (since forward_list has no own count)

            Set( SetElement* element );
        };

        /** \brief Structure that holds a single element. */
        struct SetElement
        {
            const T& data;
            unsigned index;
            std::shared_ptr<Set> cset; //< pointer to the color-set the color belongs to, ownership can be shared between different colors

            SetElement(const unsigned cIndex, const T& t);
        };

        std::vector< SetElement > elements;  //< direct access to the data of all colors
        std::unordered_map< T, std::size_t> indexmap; //< maps elements to indices
        std::size_t setcounter;

    public:
        DisjointSets() : setcounter(0) {}

        /**
        * \brief Reserves internally a given number of elements.
        */
        void reserve(const unsigned initCount);

        /** \brief Inserts a new element. */
        void insertElement(const T& t);

        /**
        * \brief Returns the set that contains the the element.
        */
        Set* findSet(const T& t) const;

        /**
        * \brief Returns true if i = j or i and j are from the same set
        */
        bool areSame(const T& t1, const T& t2) const;

        /**
        * \brief Merges the two sets that contain the given elements if they exist.
        */
        void merge(const T& t1, const T& t2);

        /** \brief Returns the current number of elements. */
        unsigned elementCount() const;

        /** \brief Returns the current number of sets. */
        unsigned setCount() const;

        ~DisjointSets() {}
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    DisjointSets<T>::SetElement::SetElement(const unsigned cIndex, const T& t) : data(t), index(cIndex)
    {
        cset = std::make_shared<Set>(this);
    }

    template<typename T>
    DisjointSets<T>::Set::Set( SetElement* element ) : size(1)
    {
        clist.emplace_front(element);
        tail = clist.begin();
    }

    template<typename T>
    void DisjointSets<T>::reserve(const unsigned initCount)
    {
        elements.reserve(initCount);
    }

    template<typename T>
    void DisjointSets<T>::insertElement(const T& t)
    {
        auto res = indexmap.emplace(t, elements.size());
        if (res.second)
        {
            elements.emplace_back(elements.size(), res.first->first);
            setcounter++;
        }
    }

    template<typename T>
    typename DisjointSets<T>::Set* DisjointSets<T>::findSet(const T& t) const
    {
        auto res = indexmap.find(t);
        if (res != indexmap.end())
        {
            return elements[res->second].cset.get();
        }
        return nullptr;
    }


    template<typename T>
    bool DisjointSets<T>::areSame(const T& t1, const T& t2) const
    {
        return findSet(t1) == findSet(t2);
    }


    template<typename T>
    void DisjointSets<T>::merge(const T& t1, const T& t2)
    {
        //lookup indices
        auto res1 = indexmap.find(t1);
        auto res2 = indexmap.find(t2);

        if (res1 == indexmap.end() || res2 == indexmap.end())
            return;

        std::size_t i = res1->second;
        std::size_t j = res2->second;

        //append shorter list to the longer one, swap roles if neccessary
        if(elements[j].cset->size > elements[i].cset->size)
            std::swap(i,j);

        elements[i].cset->size += elements[j].cset->size;
        elements[i].cset->clist.splice_after( elements[i].cset->tail, elements[j].cset->clist );

        //update cset pointers of the j-set
        while(std::next(elements[i].cset->tail) != elements[i].cset->clist.end())
        {
            ++elements[i].cset->tail;
            (*(elements[i].cset->tail))->cset = elements[i].cset->clist.front()->cset;
        }

        //we have now one set less
        setcounter--;
    }

    template<typename T>
    unsigned DisjointSets<T>::elementCount() const
    {
        return elements.size();
    }

    template<typename T>
    unsigned DisjointSets<T>::setCount() const
    {
        return setcounter;
    }
}

#endif // UNGOD_DISJOINT_SETS_H
