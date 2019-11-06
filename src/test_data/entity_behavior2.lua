normal = ungod.newStateBehavior("normal")

function normal.onInit(static, instance, states, entity, world, global)
    global.test = global.test +1
end
