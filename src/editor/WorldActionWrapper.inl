template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setPositionDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setPositionDist<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setVelocityDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setVelocityDist<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setSpawnInterval(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setSpawnInterval<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setLifetimeDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setLifetimeDist<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::addAffector(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().addAffector<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setTexrectInitializer(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setTexrectInitializer<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                    e);
}




template <std::size_t CONTEXT>
void WorldActionWrapper::addCollider(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const ungod::Collider& c)
{
    auto old = rb.getCollider();
    mEFrame->action(std::function([c, &rb](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().addCollider(e, rb, c);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().addCollider(e, rb, c);
        }),
        std::function([old, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().addCollider(e, rb, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().addCollider(e, rb, old);
            }),
            e);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setRectDownRight(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& downright)
{
    ungod::RotatedRectConstAggregator rrca{ rb.getCollider() };
    auto old = sf::Vector2f{ rrca.getDownRightX(), rrca.getDownRightY() };
    mEFrame->action(std::function([downright, &rb](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectDownRight(e, rb, downright);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectDownRight(e, rb, downright);
        }),
        std::function([old, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectDownRight(e, rb, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectDownRight(e, rb, old);
            }),
            e);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setRectUpLeft(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& upleft)
{
    ungod::RotatedRectConstAggregator rrca{ rb.getCollider() };
    auto old = sf::Vector2f{ rrca.getUpLeftX(), rrca.getUpLeftY() };
    mEFrame->action(std::function([upleft, &rb](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectUpLeft(e, rb, upleft);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectUpLeft(e, rb, upleft);
        }),
        std::function([old, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectUpLeft(e, rb, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectUpLeft(e, rb, old);
            }),
            e);
}


template <std::size_t CONTEXT>
void WorldActionWrapper::rotateRect(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, float rotation)
{
    auto old = ungod::RotatedRectConstAggregator{rb.getCollider()}.getRotation();
    mEFrame->action(std::function([rotation, &rb](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().rotateRect(e, rb, rotation);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().rotateRect(e, rb, rotation);
        }),
        std::function([old, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, rb, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectRotation(e, rb, old);
            }),
            e);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setRectRotation(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, float rotation)
{
    auto old = ungod::RotatedRectConstAggregator{ rb.getCollider() }.getRotation();
    mEFrame->action(std::function([rotation, &rb](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, rb, rotation);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectRotation(e, rb, rotation);
        }),
        std::function([old, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, rb, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectRotation(e, rb, old);
            }),
            e);
}


template <std::size_t CONTEXT>
void WorldActionWrapper::setColliderPoint(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& p, unsigned i)
{
    ungod::PointSetConstAggregator psca{rb.getCollider() };
    auto old = sf::Vector2f{ psca.getPointX(i), psca.getPointY(i) };
    mEFrame->action(std::function([p,i,&rb](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setPoint(e, rb, i, p);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setPoint(e, rb, i, p);
        }),
        std::function([old, i, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setPoint(e, rb, i, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setPoint(e, rb, i, old);
            }),
            e);
}


template <std::size_t CONTEXT>
void WorldActionWrapper::addColliderPoint(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& p)
{
    auto oldCollider = rb.getCollider();
    mEFrame->action(std::function([p, &rb](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().addPoint(e, rb, p);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().addPoint(e, rb, p);
        }),
        std::function([oldCollider, &rb](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().addCollider(e, rb, oldCollider);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().addCollider(e, rb, oldCollider);
            }),
            e);
}



template <std::size_t CONTEXT>
void WorldActionWrapper::moveCollider(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& vec)
{
    mEFrame->action(std::function([&rb, vec](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().moveCollider(e, rb, vec);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().moveCollider(e, rb, vec);
        }),
        std::function([&rb, vec](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().moveCollider(e, rb, -vec);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().moveCollider(e, rb, -vec);
            }),
            e);
}


template<typename DATA, typename F>
void WorldActionWrapper::alterPositionDist(ungod::Entity e, const F& func)
{
    DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getPositionDist<DATA>();
    mEFrame->action(std::function([this, func] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getPositionDist<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getPositionDist<DATA>(e);
                                      data.setData(old);
                                  } ),
                    e);
}

template<typename DATA, typename F>
void WorldActionWrapper::alterVelocityDist(ungod::Entity e, const F& func)
{
    DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getVelocityDist<DATA>();
    mEFrame->action(std::function([this, func] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getVelocityDist<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getVelocityDist<DATA>(e);
                                      data.setData(old);
                                  } ),
                    e);
}

template<typename DATA, typename F>
void WorldActionWrapper::alterSpawnInterval(ungod::Entity e, const F& func)
{
    DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getSpawnInterval<DATA>();
    mEFrame->action(std::function([this, func] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getSpawnInterval<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getSpawnInterval<DATA>(e);
                                      data.setData(old);
                                  } ),
                    e);

}

template<typename DATA, typename F>
void WorldActionWrapper::alterLifetimeDist(ungod::Entity e, const F& func)
{
    DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getLifetimeDist<DATA>();
    mEFrame->action(std::function([this, func] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getLifetimeDist<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getLifetimeDist<DATA>(e);
                                      data.setData(old);
                                  } ),
                    e);
}


template<typename DATA, typename F>
void WorldActionWrapper::alterTexrectInitializer(ungod::Entity e, const F& func)
{
    DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<DATA>();
    mEFrame->action(std::function([this, func] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getTexrectInitializer<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getTexrectInitializer<DATA>(e);
                                      data.setData(old);
                                  } ),
                    e);
}


template<typename DATA, typename F>
void WorldActionWrapper::alterAffector(ungod::Entity e, std::size_t i, const F& func)
{

    DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<DATA>();
    mEFrame->action(std::function([this, func] (ungod::Entity e, std::size_t i)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getAffector<DATA>(e, i);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e, std::size_t i)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemManager().getAffector<DATA>(e, i);
                                      data.setData(old);
                                  } ),
                    e, i);
}
