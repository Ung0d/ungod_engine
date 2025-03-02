namespace uedit
{
    template<typename DATA, typename ... PARAM>
    void ParticleSystemActions::setPositionDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mActionManager.action(std::function([=](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setPositionDist<DATA>(e, key, param...); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setSystem(e, old); }),
            e);
    }

    template<typename DATA, typename ... PARAM>
    void ParticleSystemActions::setVelocityDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mActionManager.action(std::function([=](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setVelocityDist<DATA>(e, key, param...); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setSystem(e, old); }),
            e);
    }

    template<typename DATA, typename ... PARAM>
    void ParticleSystemActions::setSpawnInterval(ungod::Entity e, const std::string& key, PARAM&& ... param)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mActionManager.action(std::function([=](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setSpawnInterval<DATA>(e, key, param...); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setSystem(e, old); }),
            e);
    }

    template<typename DATA, typename ... PARAM>
    void ParticleSystemActions::setLifetimeDist(ungod::Entity e, const std::string& key, PARAM&& ... param)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mActionManager.action(std::function([=](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setLifetimeDist<DATA>(e, key, param...); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setSystem(e, old); }),
            e);
    }

    template<typename DATA, typename ... PARAM>
    void ParticleSystemActions::addAffector(ungod::Entity e, const std::string& key, PARAM&& ... param)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mActionManager.action(std::function([=](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().addAffector<DATA>(e, key, param...); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setSystem(e, old); }),
            e);
    }

    template<typename DATA, typename ... PARAM>
    void ParticleSystemActions::setTexrectInitializer(ungod::Entity e, const std::string& key, PARAM&& ... param)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mActionManager.action(std::function([=](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setTexrectInitializer<DATA>(e, key, param...); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setSystem(e, old); }),
            e);
    }






    template<typename DATA, typename F>
    void ParticleSystemActions::alterPositionDist(ungod::Entity e, const F& func)
    {
        DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getPositionDist<DATA>();
        mActionManager.action(std::function([this, func](ungod::Entity e)
            {
                ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getPositionDist<DATA>(e);
                func(data);
            }),
            std::function([this, old](ungod::Entity e)
                {
                    ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getPositionDist<DATA>(e);
                    data.setData(old);
                }),
                e);
    }

    template<typename DATA, typename F>
    void ParticleSystemActions::alterVelocityDist(ungod::Entity e, const F& func)
    {
        DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getVelocityDist<DATA>();
        mActionManager.action(std::function([this, func](ungod::Entity e)
            {
                ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getVelocityDist<DATA>(e);
                func(data);
            }),
            std::function([this, old](ungod::Entity e)
                {
                    ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getVelocityDist<DATA>(e);
                    data.setData(old);
                }),
                e);
    }

    template<typename DATA, typename F>
    void ParticleSystemActions::alterSpawnInterval(ungod::Entity e, const F& func)
    {
        DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getSpawnInterval<DATA>();
        mActionManager.action(std::function([this, func](ungod::Entity e)
            {
                ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getSpawnInterval<DATA>(e);
                func(data);
            }),
            std::function([this, old](ungod::Entity e)
                {
                    ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getSpawnInterval<DATA>(e);
                    data.setData(old);
                }),
                e);

    }

    template<typename DATA, typename F>
    void ParticleSystemActions::alterLifetimeDist(ungod::Entity e, const F& func)
    {
        DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getLifetimeDist<DATA>();
        mActionManager.action(std::function([this, func](ungod::Entity e)
            {
                ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getLifetimeDist<DATA>(e);
                func(data);
            }),
            std::function([this, old](ungod::Entity e)
                {
                    ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getLifetimeDist<DATA>(e);
                    data.setData(old);
                }),
                e);
    }


    template<typename DATA, typename F>
    void ParticleSystemActions::alterTexrectInitializer(ungod::Entity e, const F& func)
    {
        DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<DATA>();
        mActionManager.action(std::function([this, func](ungod::Entity e)
            {
                ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getTexrectInitializer<DATA>(e);
                func(data);
            }),
            std::function([this, old](ungod::Entity e)
                {
                    ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getTexrectInitializer<DATA>(e);
                    data.setData(old);
                }),
                e);
    }


    template<typename DATA, typename F>
    void ParticleSystemActions::alterAffector(ungod::Entity e, std::size_t i, const F& func)
    {

        DATA old = e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<DATA>();
        mActionManager.action(std::function([this, func](ungod::Entity e, std::size_t i)
            {
                ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getAffector<DATA>(e, i);
                func(data);
            }),
            std::function([this, old](ungod::Entity e, std::size_t i)
                {
                    ungod::ScopedAccessor<DATA> data = e.getWorld().getParticleSystemHandler().getAffector<DATA>(e, i);
                    data.setData(old);
                }),
                e, i);
    }

}