state1 = ungod.newStateBehavior("state1")

function state1.onInit(static, instance)
    instance.check = true
end

function state1.onEvent(static, instance)
    instance.states:switchTo("state2")
end

function state1.onUncheck(static, instance)
    instance.check = false
end

state2 = ungod.newStateBehavior("state2")

function state2.onInit(static, instance)
    instance.check_state2 = true
end

function state2.onEvent(static, instance)
    instance.states:returnToPrevious()
end

function state2.onUncheck(static, instance)
    instance.check_state2 = false
end
