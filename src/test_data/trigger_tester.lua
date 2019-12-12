trigger_tester = ungod.newStateBehavior("trigger_tester")

function trigger_tester.onInit(static, instance, states, entity, world, global)
  entity:add():SemanticsRigidbody()
  collider = ungod.makeRotatedRect(Vec2f.new(0,0), Vec2f.new(10,10), 0)
  world:rigidbodySem():addCollider(entity, collider)
  global.tester = entity
end

function trigger_tester.onCollisionEnter(static, instance, states, entity, delta, world, global)

end

function trigger_tester.onCollision(static, instance, states, entity, otherEntity, world, global)

end

function trigger_tester.onCollisionExit(static, instance, states, entity, otherEntity, world, global)

end
