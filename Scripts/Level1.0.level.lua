-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Takes a vector now
Level:setCompositor("Glass")
Level:setGravity{0, 0, 0}
local distance = Level:getSpineNodeDistance()

distance = distance + 60
Level:waitForDistance(distance)

for i=1,5 do
    local enemy = Level:createAIEnemy("SnakeCrab", AI.outsideCrawl)
    enemy.on_death_sound = 5
    enemy.gain = 4
    distance = distance + 50
    Level:waitForDistance(distance)
end

while (true) do
    Level:waitForDistance(distance)

    for i=1,2 do

        Level:createAIEnemy("FireCycle", AI.outsideFloat).on_death_sound = 4
        Level:createAIEnemy("FireCycle", AI.outsideFloat).on_death_sound = 4
        Level:createAIEnemy("FireCycle", AI.outsideFloat).on_death_sound = 4

        distance = distance + 200
        Level:waitForDistance(distance)
    end


    for i=1,3 do

        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam).on_death_sound = 4
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam).on_death_sound = 4
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam).on_death_sound = 4

        distance = distance + 200
        Level:waitForDistance(distance)
    end

    for i=1,2 do

        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam).on_death_sound = 4
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam).on_death_sound = 4
        Level:createAIEnemy("FireCycle", AI.flyFromBehindAndRam).on_death_sound = 4

        distance = distance + 200
        Level:waitForDistance(distance)

        Level:createEnemy("Shield"):left().on_death_sound = 7
        distance = distance + 100
        Level:waitForDistance(distance)

        Level:createEnemy("Shield"):right().on_death_sound = 7
        distance = distance + 250
        Level:waitForDistance(distance)

    end
end
