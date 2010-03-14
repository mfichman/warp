-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg
for i=1,5 do
    local enemy = Level:createAIEnemy("SnakeCrab", AI.outsideCrawl)
    enemy.on_death_sound = 5
    enemy.gain = 4
    Level:sleep(.3)
end

for i=1,2 do
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 2
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 2
    Level:sleep(.5)

    Level:sleep(5)

    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 3
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 3
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 3
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 3
    Level:sleep(.6)

    Level:sleep(5)
end

while true do

Level:startLoop{id=3}
Level:startLoop{id=4}
for i=1,5 do
    Level:createAIEnemy("Banshee", AI.flyFromBehind).on_death_sound = 3
    Level:createAIEnemy("Banshee", AI.flyFromBehind).on_death_sound = 3
    Level:createAIEnemy("Banshee", AI.flyFromBehind).on_death_sound = 3

    Level:sleep(4)
end

Level:stopLoop{id=3}
Level:startLoop{id=5}

Level:createTask(function()
    Level:waitForBeatSet()
    Level:waitForBeat()
    Level:stopLoop{id=4}
    Level:startLoop{id=6}
end)

for i=1,2 do
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 10).on_death_sound = 8
    Level:sleep(.5)
    
    Level:createAIEnemy("Seeker", AI.spiral, 100, 20).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 20).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 20).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 20).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 20).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 20).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 20).on_death_sound = 8
    Level:sleep(.5)

    Level:createAIEnemy("Seeker", AI.spiral, 100, 30).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 30).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 30).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 30).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 30).on_death_sound = 8
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 30).on_death_sound = 9
    Level:sleep(.5)
    Level:createAIEnemy("Seeker", AI.spiral, 100, 30).on_death_sound = 8
    Level:sleep(.5)
end

end

