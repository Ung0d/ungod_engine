/*
* Quad - a efficient and reusable quad-tree implementation.
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

#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <list>
#include <memory>
#include <type_traits>
#include <functional>

namespace quad
{
    /** \brief Bundles 2 floats. */
    struct Vector2f
    {
        float x;
        float y;

        Vector2f() : x(0), y(0) {}
        Vector2f(float cx, float cy) : x(cx), y(cy) {}
    };

    /** \brief Models a rect in 2D space. */
    struct Bounds
    {
        Vector2f position;
        Vector2f size;

        Bounds() : position(0,0), size(0,0) {}
        Bounds(float x, float y, float width, float height) : position(x,y), size(width, height) {}
    };

    template <typename T> struct AlwaysFalse : std::false_type {};

    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL> class QuadTree;

    /** \brief Traits object. Must be specialized for every type T that shall be added to the quad-tree. */
    template<typename T, std::size_t MAX_CAPACITY = 5, std::size_t MAX_LEVEL = 16>
    struct ElementTraits
    {
        /**
        * \brief Give the quad-tree information about whether the inherited object is static or not.
        * The tree will (based on the return value of this method) separate static and non-static objects if
        * a pullRequest is started with QuadTree::DistinctPullResult type.
        */
        static bool isStatic(T)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return false;
        }

        /**
        * \brief Give the quad-tree information about the position of the inherited object.
        */
        static Vector2f getPosition(T)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return Vector2f();
        }

        /**
        * \brief Give the quad-tree information about the size of the inherited object.
        */
        static Vector2f getSize(T)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return Vector2f();
        }

        /**
        * \brief Provide a mapping to an unique id for each object.
        * IMPORTANT: All objects t that compare equal through their operator ==
        * must naturally map to the same id.
        * This is optional and only nessecary for the free functions.
        */
        static uint64_t getID(T)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return 0;
        }
    };

    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL> class QuadTreeRoot;

    /**
    * \brief Quadtree implementation to subdivide two-dimensional space.
    * This quad tree uses a few tweaks to gain maximum performance.
    * Every object is stored in exactly one node and it knows of the treenode it is stored in.
    * Template parameters:
    * - T ... type of the objects to store
    * - MAX_CAPACITY ... maximum number of objects a node can hold
    * - MAX_LEVEL ... deepest level the tree can grow
    *
    * Requirements on the elements:
    * - must be compareable with ==
    */
    template<typename T, std::size_t MAX_CAPACITY = 5, std::size_t MAX_LEVEL = 16>
    class QuadTreeNode
    {
    friend class QuadTree<T,MAX_CAPACITY,MAX_LEVEL>;
    private:
        QuadTreeNode(const Bounds& cBoundary, const unsigned cLevel = 0, QuadTreeNode* cFather = nullptr);

    public:
        QuadTreeNode(const QuadTreeNode& other) = delete;
        QuadTreeNode& operator=(const QuadTreeNode& other) = delete;

        /**
        * \brief Retrieves all stored elements of all nodes that contain the given bounds.
        * Elements are stores in ether a PullResult or a DistinctPullResult that separates
        * between static and non static.
        */
        template <template<typename, std::size_t, std::size_t> typename RESULT_TYPE>
        void retrieve(RESULT_TYPE<T, MAX_CAPACITY, MAX_LEVEL>& pullResult, const Bounds& bounds) const;

        /**
        * \brief Get the whole content of the tree and stores it in ether a ConstPullResult.
        */
        template <template<typename, std::size_t, std::size_t> typename RESULT_TYPE>
        void getContent(RESULT_TYPE<T, MAX_CAPACITY, MAX_LEVEL>& pullResult) const;

        /**
        * \brief Indicates whether the node is empty.
        */
        const bool empty() const;

        /**
        * \brief Returns the number of elements in the nodes subtree.
        */
		std::size_t size() const;

        /** \brief Invokes the callback for every subnode of the tree. Stops, if callback returns false.
        * Useful for example, if you want to render the bounds of the tree for debugging purposes. */
        void traverse(std::function<bool(const QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>&)> callback) const;

        /** \brief Returns the root node of the tree. */
        QuadTree<T,MAX_CAPACITY,MAX_LEVEL>& getRoot();

        /** \brief Returns the father of this node. */
        QuadTreeNode* getFather();

        /**
        * \brief Insert a new object in the tree if it fits. Returns success.
        * This call the ElementTraits::setOwnerNode method for the inserted object
        * an maybe also for other objects in the tree.
        */
        bool insert(QuadTree<T,MAX_CAPACITY,MAX_LEVEL>& root, T insertThis);

        /**
        * \brief Test whether a object fits in this node.
        */
        bool isInsideBounds(T element) const;

        /**
        * \brief Removes the given element from the tree.
        * Assumes that the element is contained exactly once.
        * This call the ElementTraits::setOwnerNode method for
        * the removed object. (new owner is nullptr)
        */
        bool remove(QuadTree<T,MAX_CAPACITY,MAX_LEVEL>& root, T deleteThis);

        /** \brief Removes the given object from this subnode of the tree. */
        std::tuple<bool, QuadTreeNode*> removeFromNode(QuadTree<T,MAX_CAPACITY,MAX_LEVEL>& root, T deleteThis);

        ~QuadTreeNode() {}

    private:
        enum quadrant {NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST};

        QuadTreeNode* mFather; ///<pointer to the father node or null if root (a node doesnt own its father)
        std::unique_ptr<QuadTreeNode> mChildren[4];  ///<pointer to the child nodes (a node ownes its children)
        std::vector< T > mContainer;
        unsigned mLevel; ///<indicates how deep we are

        /** \brief Helper method that splits the node into 4 equal pieces. */
        void subdivide();

        /** \brief Helper method that removes empty nodes after element-removal.
        * Returns the lowest node with elements left. */
        QuadTreeNode* upwardsCleanup();

    protected:
        Bounds mBounds;
    };

    /**
    * \brief Root node of a quad tree. The object the user is supposed to interact with.
    */
    template<typename T, std::size_t MAX_CAPACITY = 5, std::size_t MAX_LEVEL = 16>
    class QuadTree : public QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>
    {
    friend class QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>;
    public:
        QuadTree();
        QuadTree(const Bounds& cBoundary);

        /**
        * \brief Function that can be used to remove an element from its tree.
        * Requires a working mapping of element <-> unique id through the ElementTraits<T>.
        * Returns true if the element was successfully removed.
        */
        bool removeFromItsNode(T removeThis);

        /**
        * \brief Function that can be used to notify the quadtree, the given object is inserted in,
        * that the properties of the object have changed.
        * Generally, that changed are assumed to be "small", so if the object has to be reinserted,
        * the new node must be "near" the old node. This grants maximum performance. For small changes
        * only small parts of the tree, if any, must be traversed.
        * Requires a working mapping of element <-> unique id through the ElementTraits<T>.
        * Returns true if the operation was successful.
        */
        bool changedProperties(T t);

        /**
        * \brief Function that can be used to insert an element into the same quadtree as another element "hint".
        * The new inserted element is tried to be inserted into the same node as hint.
        * If it doesnt fit within this node, its successivly tried to be inserted into the father and so on.
        * Requires a working mapping of element <-> unique id through the ElementTraits<T>.
        */
        bool insertNearby(T insertThis, T hint);

        /** \brief Returns the owner node of the given object, if it is in the tree.
        * Constant time. */
        QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>* getOwner(T t) const;

        bool insert(T insertThis);

        bool remove(T deleteThis);

        std::tuple<bool, QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>*> removeFromNode(T deleteThis);

        /**
        * \brief Instantiates new boundaries. The whole content of the tree is removed and reinserted.
        */
        void setBoundary(const Bounds& bounds);

        /**
        * \brief Returns the bounds of the quad tree node represented through
        * a rect in 2D space.
        */
        const Bounds& getBoundary() const { return QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::mBounds; }

		/** \brief Clears the contents of this tree. */
		void clear();

    private:
        std::unordered_map<uint64_t, QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>*> mOwnerNodes; ///<maps element id to owner node

        /** \brief Registers a new owner node for element t. */
        void setOwner(T t, QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>* owner);
    };


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////IMPLEMENTATION//////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::QuadTreeNode(const Bounds& bounds, const unsigned level, QuadTreeNode* father) :
                          mFather(father), mLevel(level), mBounds(bounds)
    {
        mContainer.reserve(MAX_CAPACITY);
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    template <template<typename, std::size_t, std::size_t> typename RESULT_TYPE>
    void QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::retrieve(RESULT_TYPE<T, MAX_CAPACITY, MAX_LEVEL>& pullResult, const Bounds& bounds) const
    {
        if (bounds.position.x <= mBounds.position.x + mBounds.size.x &&
            bounds.position.x + bounds.size.x >= mBounds.position.x &&
            bounds.position.y <= mBounds.position.y + mBounds.size.y &&
            bounds.position.y + bounds.size.y >= mBounds.position.y)
        {
            for(auto& element : mContainer) { pullResult.quadTreeInsert(element); }

            if(mChildren[NORTHWEST]) //one check is enough as there are always all 4 childs active or none of them (quad-tree principle)
            {
                mChildren[NORTHWEST]->retrieve(pullResult, bounds);
                mChildren[NORTHEAST]->retrieve(pullResult, bounds);
                mChildren[SOUTHWEST]->retrieve(pullResult, bounds);
                mChildren[SOUTHEAST]->retrieve(pullResult, bounds);
            }
        }
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    template <template<typename, std::size_t, std::size_t> typename RESULT_TYPE>
    void QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::getContent(RESULT_TYPE<T, MAX_CAPACITY, MAX_LEVEL>& pullResult) const
    {
        for(const auto& element : mContainer) { pullResult.quadTreeInsert(element); }

        if(mChildren[NORTHWEST]) //one check is enough as there are always all 4 childs active or none of them (quad-tree principle)
        {
            mChildren[NORTHWEST]->getContent(pullResult);
            mChildren[NORTHEAST]->getContent(pullResult);
            mChildren[SOUTHWEST]->getContent(pullResult);
            mChildren[SOUTHEAST]->getContent(pullResult);
        }
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    const bool QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::empty() const
    {
        return !mChildren[NORTHWEST] && mContainer.empty();
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    std::size_t QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::size() const
    {
        if(mChildren[NORTHWEST])
            return mContainer.size() + mChildren[NORTHWEST]->size() + mChildren[NORTHEAST]->size() + mChildren[SOUTHEAST]->size() + mChildren[SOUTHWEST]->size();
        else
            return mContainer.size();
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    QuadTree<T,MAX_CAPACITY, MAX_LEVEL>& QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::getRoot()
    {
        QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>* curLvl = this;
        while(curLvl->mFather)
        {
            curLvl = curLvl->mFather;
        }
        return *static_cast<QuadTree<T,MAX_CAPACITY, MAX_LEVEL>*>(curLvl);
    }

    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    QuadTreeNode<T,MAX_CAPACITY, MAX_LEVEL>* QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::getFather()
    {
        return mFather;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    void QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::traverse(std::function<bool(const QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>&)> callback) const
    {
        if (callback( *this ) && mChildren[NORTHEAST])
        {
            mChildren[NORTHWEST]->processBounds(callback);
            mChildren[NORTHEAST]->processBounds(callback);
            mChildren[SOUTHWEST]->processBounds(callback);
            mChildren[SOUTHEAST]->processBounds(callback);
        }
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::insert(QuadTree<T,MAX_CAPACITY,MAX_LEVEL>& root, T insertThis)
    {
        if ( isInsideBounds(insertThis) )
        {
            if(mChildren[NORTHWEST])  //try to insert it in the childen first
            {
                if(mChildren[NORTHWEST]->insert(root, insertThis))
                    return true;
                if(mChildren[NORTHEAST]->insert(root, insertThis))
                    return true;
                if(mChildren[SOUTHWEST]->insert(root, insertThis))
                    return true;
                if(mChildren[SOUTHEAST]->insert(root, insertThis))
                    return true;
            }

            //couldnt insert in in one of the children

            root.setOwner(insertThis, this);
            mContainer.push_back(insertThis);

            if(mLevel <= MAX_LEVEL && mContainer.size() > MAX_CAPACITY)
            {
                if(!mChildren[NORTHWEST]) //one check is enough as there are always all 4 childs active or none of them (quad-tree principle)
                {
                    subdivide();
                }

                //rematch objects to new subnodes
                //we use inversed iteration for efficieny
                for(auto content = mContainer.rbegin(); content != mContainer.rend();)
                {
                    if( mChildren[NORTHWEST]->insert(root, *content) ||
                        mChildren[NORTHEAST]->insert(root, *content) ||
                        mChildren[SOUTHEAST]->insert(root, *content) ||
                        mChildren[SOUTHWEST]->insert(root, *content) )
                    {
                        auto temp = mContainer.erase( --content.base() );
                        content = typename std::vector<T>::reverse_iterator(temp);
                    }
                    else
                        ++content;
                }
            }
            return true;
        }
        else
            return false;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::isInsideBounds(T element) const
    {
        return ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getPosition(element).x +
                    ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getSize(element).x <= mBounds.position.x + mBounds.size.x &&
               ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getPosition(element).x >= mBounds.position.x &&
               ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getPosition(element).y +
                    ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getSize(element).y <= mBounds.position.y + mBounds.size.y &&
               ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getPosition(element).y >= mBounds.position.y;
    }

    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::remove(QuadTree<T,MAX_CAPACITY,MAX_LEVEL>& root, T deleteThis)
    {
        if ( isInsideBounds(deleteThis) )
        {
            //traverse down --- object must be in the deepest node, that contains its bounds
            if (mChildren[NORTHEAST])
            {
                if (mChildren[NORTHEAST]->remove(root, deleteThis) ||
                    mChildren[NORTHWEST]->remove(root, deleteThis) ||
                    mChildren[SOUTHEAST]->remove(root, deleteThis) ||
                    mChildren[SOUTHWEST]->remove(root, deleteThis))
                {
                    return true;
                }
            }
            return std::get<0>(removeFromNode(root, deleteThis));
        }
        return false;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    std::tuple<bool, QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>*> QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::removeFromNode(QuadTree<T,MAX_CAPACITY,MAX_LEVEL>& root, T deleteThis)
    {
        for(auto element = mContainer.begin(); element != mContainer.end(); ++element)
        {
            if(*element == deleteThis)
            {
                mContainer.erase(element);
                root.setOwner(deleteThis, nullptr);
                return std::make_tuple(true, upwardsCleanup());
            }
        }
        return std::make_tuple(false, this);
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    void QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::subdivide()
    {
        float boundaryX       = mBounds.position.x;
        float boundaryY       = mBounds.position.y;
        float boundaryWidth   = mBounds.size.x;
        float boundaryHeight  = mBounds.size.y;

        mChildren[NORTHWEST] = std::unique_ptr<QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>>
                                (new QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>(
                                    Bounds(boundaryX, boundaryY, boundaryWidth/2, boundaryHeight/2), mLevel + 1, this));
        mChildren[NORTHEAST] = std::unique_ptr<QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>>
                                (new QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>(
                                    Bounds(boundaryX + boundaryWidth/2, boundaryY, boundaryWidth/2, boundaryHeight/2), mLevel + 1, this));
        mChildren[SOUTHWEST] = std::unique_ptr<QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>>
                                (new QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>(
                                    Bounds(boundaryX, boundaryY + boundaryHeight/2, boundaryWidth/2, boundaryHeight/2), mLevel + 1, this));
        mChildren[SOUTHEAST] = std::unique_ptr<QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>>
                                (new QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>(
                                    Bounds(boundaryX + boundaryWidth/2, boundaryY + boundaryHeight/2, boundaryWidth/2, boundaryHeight/2), mLevel + 1, this));
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>* QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::upwardsCleanup()
    {
        if (!mChildren[NORTHWEST] ||
            (mChildren[NORTHWEST]->empty() &&
             mChildren[NORTHEAST]->empty() &&
             mChildren[SOUTHWEST]->empty() &&
             mChildren[SOUTHEAST]->empty()))
        {
			mChildren[NORTHWEST].reset();
			mChildren[NORTHEAST].reset();
			mChildren[SOUTHWEST].reset();
			mChildren[SOUTHEAST].reset();
            if (mFather)
                return mFather->upwardsCleanup();
        }
        return this;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::QuadTree() : QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>( {0,0,0,0} ) {}

    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::QuadTree(const Bounds& cBoundary) :
        QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>(cBoundary)
    {
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::removeFromItsNode(T removeThis)
    {
        auto* owner = getOwner(removeThis);
        if(owner) return owner->remove(*this, removeThis);
        else return false;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::changedProperties(T t)
    {
        auto* owner = getOwner(t);
        if (owner && owner != this)
        {
            if( !owner->isInsideBounds(t) ) //must reinsert the element
            {
                bool wasRemoved;
                std::tie(wasRemoved, owner) = owner->removeFromNode(*this, t);
                if( wasRemoved )
                {
                    while (owner->getFather())
                    {
                        if (owner->insert(*this, t))
                            return true;
                        owner = owner->getFather();
                    }
                    return static_cast<QuadTree<T, MAX_CAPACITY, MAX_LEVEL>*>(owner)->insert(t);
                }
            }
        }
        return false;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::insertNearby(T insertThis, T hint)
    {
        auto* hintNode = getOwner(hint);
        if (hintNode)
        {
            bool inserted = false;
            while (hintNode && !inserted)
            {
                inserted = hintNode->insert(*this, insertThis);
                hintNode = hintNode->getFather();
            }
            return inserted;
        }
        return false;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    void QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::setOwner(T t, QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>* owner)
    {
        uint64_t id = ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getID(t);
        mOwnerNodes[id] = owner;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>* QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::getOwner(T t) const
    {
        uint64_t id = ElementTraits<T,MAX_CAPACITY,MAX_LEVEL>::getID(t);
        auto res = mOwnerNodes.find(id);
        if (res != mOwnerNodes.end())
            return res->second;
        else
            return nullptr;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::insert(T insertThis)
    {
        //make sure the object is not added twice
        removeFromItsNode(insertThis);

        using QN = QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>;
        if (QN::mChildren[QN::NORTHWEST])  //try to insert it in the childen first
        {
            if (QN::mChildren[QN::NORTHWEST]->insert(*this, insertThis))
                return true;
            if (QN::mChildren[QN::NORTHEAST]->insert(*this, insertThis))
                return true;
            if (QN::mChildren[QN::SOUTHWEST]->insert(*this, insertThis))
                return true;
            if (QN::mChildren[QN::SOUTHEAST]->insert(*this, insertThis))
                return true;
        }

        //couldnt insert in in one of the children

        this->setOwner(insertThis, static_cast<QN*>(this));
        QN::mContainer.push_back(insertThis);

        if (!QN::mChildren[QN::NORTHWEST] && QN::mLevel <= MAX_LEVEL && QN::mContainer.size() > MAX_CAPACITY)
        {
            this->subdivide();

            //rematch objects to new subnodes
            //we use inversed iteration for efficieny (erase doesnt shift whole vector)
            for (auto content = QN::mContainer.rbegin(); content != QN::mContainer.rend();)
            {
                if ( QN::mChildren[QN::NORTHWEST]->insert(*this, *content) ||
                     QN::mChildren[QN::NORTHEAST]->insert(*this, *content) ||
                     QN::mChildren[QN::SOUTHEAST]->insert(*this, *content) ||
                     QN::mChildren[QN::SOUTHWEST]->insert(*this, *content) )
                {
                    auto temp = QN::mContainer.erase( --content.base() );
                    content = typename std::vector<T>::reverse_iterator(temp);
                }
                else
                    ++content;
            }
        }
        return true;
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    bool QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::remove(T deleteThis)
    {
        using QN = QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>;
        //traverse down --- object must be in the deepest node, that contains its bounds
        if (QN::mChildren[QN::NORTHEAST])
        {
            if (QN::mChildren[QN::NORTHEAST]->remove(*this, deleteThis) ||
                QN::mChildren[QN::NORTHWEST]->remove(*this, deleteThis) ||
                QN::mChildren[QN::SOUTHEAST]->remove(*this, deleteThis) ||
                QN::mChildren[QN::SOUTHWEST]->remove(*this, deleteThis))
            {
                return true;
            }
        }
        return std::get<0>(this->removeFromNode(deleteThis));
    }


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    std::tuple<bool, QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>*> QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::removeFromNode(T deleteThis)
    {
        return QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::removeFromNode(*this, deleteThis);
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////RESULT_TYPES////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    /**
    * \brief Class that represents the result of a quadTreePull.
    * Basically a wrapper of a list filled with the entities returned from
    * the pull. The class provides methods to retrieve these entities
    * one after another.
    * Usage in a pollLoop:
    * while(!result.empty())
    * {
    *   auto e = result.pollEntity();
    *   //do stuff with e
    * }
    */
    template<typename T, std::size_t MAX_CAPACITY = 5, std::size_t MAX_LEVEL = 16>
    class PullResult
    {
    friend class QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>;

    private:
        std::list<T> result;
        typename std::list<T>::iterator indexPtr; ///< points to the last unpolled position

        /** \brief Called by QuadTree (friend) during a QuadTreePull to fill the result with entities. */
        void quadTreeInsert(const T& en)
        {
            result.push_back( en );
            indexPtr = result.begin();
        }

    public:
        PullResult()
        {
            indexPtr = result.begin();
        }

        /** \brief Polls out the next entry in the result list. If done() is true
        when calling this method, behavior is undefined. */
        T& poll()
        {
            return *(indexPtr++);
        }

        /**
        * \brief Indicates whether the internal list is empty (so no
        * entity could be pulled from the world or all entries are
        * already polled out)
        */
        bool done() const
        {
            return indexPtr == result.end();
        }

        /**
        * \brief Resets the internal interator (restarts polling)
        */
        void reset()
        {
            indexPtr = result.begin();
        }

        /**
        * \brief Returns the underlying list of elements.
        */
        std::list<T>& getList()
        {
            return result;
        }

        /**
        * \brief Returns the underlying list of elements.
        */
        const std::list<T>& getList() const
        {
            return result;
        }
    };


    /**
    * \brief Works exactly like PullResult, but separates elements in
    * to distinct list according to whether they are static or not.
    * Polling of static and nonStatic elements works undependent of each other.
    */
    template<typename T, std::size_t MAX_CAPACITY = 5, std::size_t MAX_LEVEL = 16>
    class DistinctPullResult
    {
    friend class QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>;

    private:
        std::list<T> statics; ///<contains static elements
        std::list<T> nonStatics; ///<contains non-static elements
        typename std::list<T>::iterator staticIndex; ///<always points to the first unpolled element in statics list
        typename std::list<T>::iterator nonstaticIndex; ///<always points to the first unpolled element in non-statics list

        /** \brief Called by QuadTree (friend) during a QuadTreePull to fill the result with entities.  */
        void quadTreeInsert(const T& en)
        {
            if (ElementTraits<T>::isStatic(en))
            {
                statics.push_back( en );
                staticIndex = statics.begin();
            }
            else
            {
                nonStatics.push_back( en );
                nonstaticIndex = nonStatics.begin();
            }
        }

    public:
        DistinctPullResult() : staticIndex( statics.begin()) , nonstaticIndex( nonStatics.begin())  {}

        /**
        * \brief Polls out the next entry in the result list. If staticsDone() is true
        when calling this method, behavior is undefined. */
        T& pollStatic()
        {
            return *(staticIndex++);
        }

        /**
        * \brief Polls out the next entry in the result list. If nonStaticsDone() is true
        when calling this method, behavior is undefined. */
        T& pollNonStatic()
        {
            return *(nonstaticIndex++);
        }

        /**
        * \brief Indicates whether polling of static elements is done.
        * already polled out)
        */
        bool staticsDone() const
        {
            return staticIndex == statics.end();
        }

        /**
        * \brief Indicates whether the polling of non-static elements is done.
        */
        bool nonStaticsDone() const
        {
            return nonstaticIndex == nonStatics.end();
        }

        /**
        * \brief Returns the underlying list  for statics to perform actions (like sorting) on it.
        */
        std::list< T >& getStaticsList()
        {
            return statics;
        }

        /**
        * \brief Returns the underlying list for non statics to perform actions (like sorting) on it.
        */
        std::list< T >& getNonStaticsList()
        {
            return nonStatics;
        }

        /**
        * \brief Returns the underlying list  for statics to perform actions (like sorting) on it.
        */
        const std::list< T >& getStaticsList() const
        {
            return statics;
        }

        /**
        * \brief Returns the underlying list for non statics to perform actions (like sorting) on it.
        */
        const std::list< T >& getNonStaticsList() const
        {
            return nonStatics;
        }

        /**
        * \brief Resets the internal interator (restarts polling)
        */
        void resetStatic()
        {
            staticIndex = statics.begin();
        }

        /**
        * \brief Resets the internal interator (restarts polling)
        */
        void resetNonStatic()
        {
            nonstaticIndex = nonStatics.begin();
        }
    };


    template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
    void QuadTree<T,MAX_CAPACITY,MAX_LEVEL>::setBoundary(const Bounds& bounds)
    {
        PullResult<T,MAX_CAPACITY,MAX_LEVEL> pull;
        QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::getContent(pull);
        clear();
        QuadTreeNode<T,MAX_CAPACITY,MAX_LEVEL>::mBounds = bounds;
        while (!pull.done())
            insert(pull.poll());
    }


	template<typename T, std::size_t MAX_CAPACITY, std::size_t MAX_LEVEL>
	void QuadTree<T, MAX_CAPACITY, MAX_LEVEL>::clear()
	{
		QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::mContainer.clear();
		mOwnerNodes.clear();
		QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::mChildren[QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::NORTHWEST].reset();
		QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::mChildren[QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::NORTHEAST].reset();
		QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::mChildren[QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::SOUTHWEST].reset();
		QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::mChildren[QuadTreeNode<T, MAX_CAPACITY, MAX_LEVEL>::SOUTHEAST].reset();
	}
}

#endif // _QUADTREE__
