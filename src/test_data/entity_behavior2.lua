normal = ungod.newStateBehavior("normal")

function normal.onInit(static, instance)
    ungod.test = ungod.test +1

    ungod.emit("event", {val = 5})
    ungod.emit("delayed_event", {val = 13}, 0.4)
end
