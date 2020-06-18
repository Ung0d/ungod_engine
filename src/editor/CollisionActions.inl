namespace uedit
{
    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::addCollider(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const ungod::Collider& c)
    {
        auto old = rb.getCollider();
        mActionManager.action(std::function([c, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().addCollider(e, rb, c);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().addCollider(e, rb, c);
            }),
            std::function([old, &rb](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().addCollider(e, rb, old);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().addCollider(e, rb, old);
                }),
                e);
    }

    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::setRectDownRight(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& downright)
    {
        ungod::RotatedRectConstAggregator rrca{ rb.getCollider() };
        auto old = sf::Vector2f{ rrca.getDownRightX(), rrca.getDownRightY() };
        mActionManager.action(std::function([downright, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().setRectDownRight(e, rb, downright);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().setRectDownRight(e, rb, downright);
            }),
            std::function([old, &rb](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().setRectDownRight(e, rb, old);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().setRectDownRight(e, rb, old);
                }),
                e);
    }

    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::setRectUpLeft(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& upleft)
    {
        ungod::RotatedRectConstAggregator rrca{ rb.getCollider() };
        auto old = sf::Vector2f{ rrca.getUpLeftX(), rrca.getUpLeftY() };
        mActionManager.action(std::function([upleft, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().setRectUpLeft(e, rb, upleft);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().setRectUpLeft(e, rb, upleft);
            }),
            std::function([old, &rb](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().setRectUpLeft(e, rb, old);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().setRectUpLeft(e, rb, old);
                }),
                e);
    }


    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::rotateRect(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, float rotation)
    {
        auto old = ungod::RotatedRectConstAggregator{ rb.getCollider() }.getRotation();
        mActionManager.action(std::function([rotation, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().rotateRect(e, rb, rotation);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().rotateRect(e, rb, rotation);
            }),
            std::function([old, &rb](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().setRectRotation(e, rb, old);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().setRectRotation(e, rb, old);
                }),
                e);
    }

    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::setRectRotation(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, float rotation)
    {
        auto old = ungod::RotatedRectConstAggregator{ rb.getCollider() }.getRotation();
        mActionManager.action(std::function([rotation, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().setRectRotation(e, rb, rotation);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().setRectRotation(e, rb, rotation);
            }),
            std::function([old, &rb](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().setRectRotation(e, rb, old);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().setRectRotation(e, rb, old);
                }),
                e);
    }


    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::setColliderPoint(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& p, unsigned i)
    {
        ungod::PointSetConstAggregator psca{ rb.getCollider() };
        auto old = sf::Vector2f{ psca.getPointX(i), psca.getPointY(i) };
        mActionManager.action(std::function([p, i, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().setPoint(e, rb, i, p);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().setPoint(e, rb, i, p);
            }),
            std::function([old, i, &rb](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().setPoint(e, rb, i, old);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().setPoint(e, rb, i, old);
                }),
                e);
    }


    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::addColliderPoint(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& p)
    {
        auto oldCollider = rb.getCollider();
        mActionManager.action(std::function([p, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().addPoint(e, rb, p);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().addPoint(e, rb, p);
            }),
            std::function([oldCollider, &rb](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().addCollider(e, rb, oldCollider);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().addCollider(e, rb, oldCollider);
                }),
                e);
    }



    template <std::size_t CONTEXT>
    void CollisionActions<CONTEXT>::moveCollider(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& vec)
    {
        mActionManager.action(std::function([&rb, vec](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyHandler().moveCollider(e, rb, vec);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyHandler().moveCollider(e, rb, vec);
            }),
            std::function([&rb, vec](ungod::Entity e)
                {
                    if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                        e.getWorld().getSemanticsRigidbodyHandler().moveCollider(e, rb, -vec);
                    else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                        e.getWorld().getMovementRigidbodyHandler().moveCollider(e, rb, -vec);
                }),
                e);
    }

}