normal = ungod.newStateBehavior("normal")

function normal.onInit(static, instance)
    instance.check_update = false
    instance.check_collenter = false
    instance.check_coll = false
    instance.check_collexit = false
    ungod.test = 1
end

function normal.onUpdate(static, instance, delta)
    instance.check_update = true
end

function normal.onCreation(static, instance)
    instance.check_creation = true
end

function normal.onCollisionEnter(static, instance, delta)
    instance.check_collenter = true
end

function normal.onCollision(static, instance, otherEntity)
    instance.check_coll = true
end

function normal.onCollisionExit(static, instance, otherEntity)
    instance.check_collexit = true
end
