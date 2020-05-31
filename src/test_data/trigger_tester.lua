trigger_tester = ungod.newStateBehavior("trigger_tester")

function trigger_tester.onInit(static, instance)
  instance.entity:add():semanticsRigidbody()
  collider = ungod.makeRotatedRect(Vec2f.new(0,0), Vec2f.new(10,10), 0)
  instance.entity:semanticsRigidbody():addCollider(collider)
  ungod.tester = instance.entity
end

function trigger_tester.onCollisionEnter(static, instance, delta)

end

function trigger_tester.onCollision(static, instance, otherEntity)

end

function trigger_tester.onCollisionExit(static, instance, otherEntity)

end
