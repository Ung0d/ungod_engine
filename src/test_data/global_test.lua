test = ungod.newGlobalBehavior("test")

function test.onStaticConstr(static)
    if static.constr == nil then
        static.constr = 37
    else
        static.constr = 41
    end
end

function test.onStaticDestr(static)
    static.destr = 256
end

function test.onInit(static, instance)
    instance.check1 = true
end

function test.onUncheck(static, instance)
    instance.check1 = false
    instance.check2 = true
end
