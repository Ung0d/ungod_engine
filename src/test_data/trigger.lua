trigger = ungod.newStateBehavior("trigger")

function trigger.onInit(static, instance)
  instance.entity:add():SemanticsRigidbody()
  collider = ungod.makeRotatedRect(Vec2f.new(0,0), Vec2f.new(10,10), 0)
  instance.world:rigidbodySem():addCollider(instance.entity, collider)
end

function trigger.onCollisionEnter(static, instance, otherEntity)
  if otherEntity == ungod.tester then
    instance.check_enter = true
  end
end

function trigger.onCollision(static, instance, otherEntity)
    if otherEntity == ungod.tester then
      instance.check_coll = true
    end
end

function trigger.onCollisionExit(static, instance, otherEntity)
    if otherEntity == ungod.tester then
      instance.check_exit = true
    end
end
