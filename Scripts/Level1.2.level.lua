-- Takes a vector now

local distance = Level:getSpineNodeDistance()
while(true) do
distance = distance + 60
Level:waitForDistance(distance)

for i=1,8 do
Level:createAIEnemy("Banshee", AI.flyFromBehind)
Level:createAIEnemy("Banshee", AI.flyFromBehind)
distance = distance + 200
Level:waitForDistance(distance)
end

for i=1,8 do
Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam)
distance = distance + 80
Level:waitForDistance(distance)
end

distance = distance + 200
Level:waitForDistance(distance)

for i=1,2 do
Level:createEnemy("Shield"):left()
distance = distance + 50
Level:waitForDistance(distance)

Level:createEnemy("Shield"):right()
distance = distance + 50
Level:waitForDistance(distance)

Level:createEnemy("Shield"):top()
distance = distance + 50
Level:waitForDistance(distance)

Level:createEnemy("Shield"):bottom()
distance = distance + 50
Level:waitForDistance(distance)

end

end
