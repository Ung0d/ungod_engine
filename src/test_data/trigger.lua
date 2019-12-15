trigger = ungod.newStateBehavior("trigger")

function trigger.onInit(static, instance, states, entity, world, global)
  entity:add():SemanticsRigidbody()
  collider = ungod.makeRotatedRect(Vec2f.new(0,0), Vec2f.new(10,10), 0)
  world:rigidbodySem():addCollider(entity, collider)
end

function trigger.onCollisionEnter(static, instance, states, entity, otherEntity, world, global)
  if otherEntity == global.tester then
    instance.check_enter = true
  end
end

function trigger.onCollision(static, instance, states, entity, otherEntity, world, global)
    if otherEntity == global.tester then
      instance.check_coll = true
    end
end

function trigger.onCollisionExit(static, instance, states, entity, otherEntity, world, global)
    if otherEntity == global.tester then
      instance.check_exit = true
    end
end
