normal = ungod.newStateBehavior("normal")

function normal.onInit(static, instance, states, entity, world, global)
    instance.check_update = false
    instance.check_collenter = false
    instance.check_coll = false
    instance.check_collexit = false
    global.test = 1
end

function normal.onUpdate(static, instance, states, entity, delta, world, global)
    instance.check_update = true
end

function normal.onCreation(static, instance, states, entity, world, global)
    instance.check_creation = true
end

function normal.onCollisionEnter(static, instance, states, entity, delta, world, global)
    instance.check_collenter = true
end

function normal.onCollision(static, instance, states, entity, otherEntity, world, global)
    instance.check_coll = true
end

function normal.onCollisionExit(static, instance, states, entity, otherEntity, world, global)
    instance.check_collexit = true
end
