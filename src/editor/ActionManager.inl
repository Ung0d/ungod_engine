template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setPositionDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setPositionDist<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setVelocityDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setVelocityDist<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setSpawnInterval(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setSpawnInterval<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setLifetimeDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setLifetimeDist<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::addAffector(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().addAffector<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setSystem(e, old); } ),
                    e);
}

template<typename DATA, typename ... PARAM>
void WorldActionWrapper::setTexrectInitializer(ungod::Entity e, const std::string& key, PARAM&& ... param)
{
    ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
    mEFrame->action(std::function([=] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setTexrectInitializer<DATA>(e, key, param...); }),
                    std::function([this, old] (ungod::Entity e)
                                  { e.getWorld().getParticleSystemHandler().setSystem(e, old); } ),
                    e);
}






template<typename DATA, typename F>
void WorldActionWrapper::alterPositionDist(ungod::Entity e, const F& func)
{
    DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getPositionDist<DATA>();
    mEFrame->action(std::function([this, func] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getPositionDist<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getPositionDist<DATA>(e);
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
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getVelocityDist<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getVelocityDist<DATA>(e);
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
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getSpawnInterval<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getSpawnInterval<DATA>(e);
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
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getLifetimeDist<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getLifetimeDist<DATA>(e);
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
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getTexrectInitializer<DATA>(e);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getTexrectInitializer<DATA>(e);
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
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getAffector<DATA>(e, i);
                                      func( data );
                                  }),
                    std::function([this, old] (ungod::Entity e, std::size_t i)
                                  {
                                      ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getAffector<DATA>(e, i);
                                      data.setData(old);
                                  } ),
                    e, i);
}
