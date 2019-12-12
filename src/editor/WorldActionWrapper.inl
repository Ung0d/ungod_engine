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



template<std::size_t CONTEXT>
void WorldActionWrapper::addCollider(ungod::Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rot, int i)
{
    e.getWorld().getRigidbodyManager().addCollider<CONTEXT>(e, upleft, downright, rot, i);
}

template<std::size_t CONTEXT>
void WorldActionWrapper::removeCollider(ungod::Entity e, unsigned i)
{
    e.getWorld().getRigidbodyManager().removeCollider<CONTEXT>(e, i);
}


template <std::size_t CONTEXT>
void WorldActionWrapper::setRectDownRight(ungod::Entity e, const sf::Vector2f& downright)
{
    ungod::RotatedRectConstAggregator rrca{ e.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider() };
    auto old = sf::Vector2f{ rrca.getDownRightX(), rrca.getDownRightY() };
    mEFrame->action(std::function([downright](ungod::Entity e)
        { 
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectDownRight(e, downright);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectDownRight(e, downright);
        }),
        std::function([old](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectDownRight(e, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectDownRight(e, old);
            }),
        e);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setRectDownRight(ungod::Entity e, std::size_t index, const sf::Vector2f& downright)
{
    ungod::RotatedRectConstAggregator rrca{ e.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index).getCollider() };
    auto old = sf::Vector2f{ rrca.getDownRightX(), rrca.getDownRightY() };
    mEFrame->action(std::function([downright, i](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectDownRight(e, i, downright);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectDownRight(e, i, downright);
        }),
        std::function([old, i](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectDownRight(e, i, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectDownRight(e, i, old);
            }),
            e);
}
template <std::size_t CONTEXT>
void WorldActionWrapper::setRectUpLeft(ungod::Entity e, const sf::Vector2f& upleft)
{
    ungod::RotatedRectConstAggregator rrca{ e.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider() };
    auto old = sf::Vector2f{ rrca.getUpLeftX(), rrca.getUpLeftY() };
    mEFrame->action(std::function([upleft](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectUpLeft(e, upleft);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectUpLeft(e, upleft);
        }),
        std::function([old](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectUpLeft(e, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectUpLeft(e, old);
            }),
            e);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setRectUpLeft(ungod::Entity e, std::size_t index, const sf::Vector2f& upleft)
{
    ungod::RotatedRectConstAggregator rrca{ e.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index).getCollider() };
    auto old = sf::Vector2f{ rrca.getUpLeftX(), rrca.getUpLeftY() };
    mEFrame->action(std::function([upleft, i](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectUpLeft(e, i, upleft);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectUpLeft(e, i, upleft);
        }),
        std::function([old, i](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectUpLeft(e, i, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectUpLeft(e, i, old);
            }),
            e);
}


template <std::size_t CONTEXT>
void WorldActionWrapper::rotateRect(ungod::Entity e, float rotation)
{
    auto old = ungod::RotatedRectConstAggregator{e.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider()}.getRotation();
    mEFrame->action(std::function([rotation](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().rotateRect(e, rotation);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().rotateRect(e, rotation);
        }),
        std::function([old](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectRotation(e, old);
            }),
            e);
}
template <std::size_t CONTEXT>
void WorldActionWrapper::rotateRect(ungod::Entity e, std::size_t index, float rotation)
{
    auto old = ungod::RotatedRectConstAggregator{ e.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index).getCollider() }.getRotation();
    mEFrame->action(std::function([rotation, i](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().rotateRect(e, i, rotation);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().rotateRect(e, i, rotation);
        }),
        std::function([old, i](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, i, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectRotation(e, i, old);
            }),
            e);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setRectRotation(ungod::Entity e, float rotation)
{
    auto old = ungod::RotatedRectConstAggregator{ e.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider() }.getRotation();
    mEFrame->action(std::function([rotation](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, rotation);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectRotation(e, rotation);
        }),
        std::function([old](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectRotation(e, old);
            }),
            e);
}
template <std::size_t CONTEXT>
void WorldActionWrapper::setRectRotation(ungod::Entity e, std::size_t index, float rotation)
{
    auto old = ungod::RotatedRectConstAggregator{ e.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index).getCollider() }.getRotation();
    mEFrame->action(std::function([rotation, i](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, i, rotation);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setRectRotation(e, i, rotation);
        }),
        std::function([old, i](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setRectRotation(e, i, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setRectRotation(e, i, old);
            }),
            e);
}


template <std::size_t CONTEXT>
void WorldActionWrapper::setColliderPoint(ungod::Entity e, const sf::Vector2f& p, unsigned i)
{
    ungod::PointSetConstAggregator psca{ e.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider() };
    auto old = sf::Vector2f{ psca.getPointX(i), psca.getPointY(i) };
    mEFrame->action(std::function([p,i](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setPoint(e, i, p);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setPoint(e, i, p);
        }),
        std::function([old, i](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setPoint(e, i, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setPoint(e, i, old);
            }),
            e);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setColliderPoint(ungod::Entity e, std::size_t multiIndex, const sf::Vector2f& p, unsigned i)
{
    ungod::PointSetConstAggregator psca{ e.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(multiIndex).getCollider() };
    auto old = sf::Vector2f{ psca.getPointX(i), psca.getPointY(i) };
    mEFrame->action(std::function([p, i, multiIndex](ungod::Entity e)
        {
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                e.getWorld().getSemanticsRigidbodyManager().setPoint(e, multiIndex, i, p);
            else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                e.getWorld().getMovementRigidbodyManager().setPoint(e, multiIndex, i, p);
        }),
        std::function([old, i, multiIndex](ungod::Entity e)
            {
                if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                    e.getWorld().getSemanticsRigidbodyManager().setPoint(e, multiIndex, i, old);
                else if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
                    e.getWorld().getMovementRigidbodyManager().setPoint(e, multiIndex, i, old);
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
