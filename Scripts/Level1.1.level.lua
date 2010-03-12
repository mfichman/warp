-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg
Level:sleep(3)
while (true) do
for i=1,3 do
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

    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)
    Level:createAIEnemy("StarBlazer", AI.spiral, 100, 14).on_death_sound = 2
    Level:sleep(.6)

    Level:sleep(5)
end

for i=1,4 do
    Level:createAIEnemy("Banshee", AI.flyFromBehind)
    Level:createAIEnemy("Banshee", AI.flyFromBehind)
    Level:createAIEnemy("Banshee", AI.flyFromBehind)

    Level:sleep(3)
end
end

