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
void WorldActionWrapper::setColliderDownRight(ungod::Entity e, std::size_t index, const sf::Vector2f& downright)
{
    e.getWorld().getRigidbodyManager().setColliderDownRight<CONTEXT>(e, index, downright);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setColliderUpLeft(ungod::Entity e, std::size_t index, const sf::Vector2f& upleft)
{
    e.getWorld().getRigidbodyManager().setColliderUpLeft<CONTEXT>(e, index, upleft);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::rotateCollider(ungod::Entity e, std::size_t index, float rotation)
{
    e.getWorld().getRigidbodyManager().rotateCollider<CONTEXT>(e, index, rotation);
}

template <std::size_t CONTEXT>
void WorldActionWrapper::setColliderRotation(ungod::Entity e, std::size_t index, float rotation)
{
    e.getWorld().getRigidbodyManager().setColliderRotation<CONTEXT>(e, index, rotation);
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
