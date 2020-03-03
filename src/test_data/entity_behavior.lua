normal = ungod.newStateBehavior("normal")

function normal.onInit(static, instance)
    instance.check_update = false
    instance.check_collenter = false
    instance.check_coll = false
    instance.check_collexit = false
    ungod.test = 1

    instance.event_received = 0
    instance.delayed_event_received = 0

    event_link = ungod.listen(instance.entity, "event")
    delayed_event_link = ungod.listen(instance.entity, "delayed_event")
    --links can be used to disconnect again ( event_link:disconnect() )
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

function normal.onCustomEvent(static, instance, event)
  if event.type == "event" then
    instance.event_received = event.data.val
  elseif event.type == "delayed_event" then
    instance.delayed_event_received = event.data.val
  end
end
