test = ungod.EntityBehavior()

function test.onInit(self, e)
    self.isInit = true
    self.isClicked = false
    self.isDestroyed = false
    self.
end

function test.onMouseClicked(self, e)
    self.isClicked = true
end

function test.onDestroy(self, e)
    self.isDestroyed = true
end
