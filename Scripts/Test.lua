-- Criterium: CS248 Final Project
-- Copyright (c) 2010 Matt Fichman

crCreatePlane("Plane1", {})
--crCreatePlane("Plane2", {normal={ 1, 0, 0 }, depth=-1000})
--crCreatePlane("Plane3", {normal={ -1, 0, 0 }, depth=1000})

for i=1,10 do

crCreateBall("Ball"..i, { radius=10 })
crSetNode("Ball"..i, { position={math.random(-20, 20), math.random(0, 300), math.random(-20, 20)} })

end

coroutine.yield(100)

crSetLight("Light", {diffuse={1, 0, 0}})

