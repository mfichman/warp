-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

AI = Object:new()

function AI:trackFromSide(enemy)
    local proj = Level:getSpineProjection(100)
    local right = proj.forward:cross{0,1,0}
    enemy:setPosition(proj.position + right * 20)
    enemy:setVelocity{30,0,0}
    enemy:setOrientation(proj.forward)

    enemy.onTimeStep = function(self)
        local target_direction = Level:getPlayerPosition() - enemy:getPosition()
        target_direction:normalize()
        local cur_orientation = enemy:getOrientation()
        local normal = target_direction:cross(cur_orientation:getDirection())
        -- cross product is ab*sin(theta)n = sin(theta)n
        if (normal:squaredLength() > .1) then
            rotation = Quaternion:new()
            rotation:fromAngleAxis(normal, 4) -- adjust the angle between it and the player by .1 radians
            cur_orientation = rotation * cur_orientation
            enemy:setOrientation(cur_orientation)
            print (cur_orientation[1])
            print (cur_orientation[2])
            print (cur_orientation[3])
            print (cur_orientation[4])
            enemy:setVelocity(cur_orientation:getDirection() * 40)
        end
        
    end
end


function AI:flyFromBehind(enemy)
    local proj = Level:getSpineProjection(-10)
    local right = proj.forward:cross{0, 1, 0}
    enemy:setPosition(proj.position + right * 4)
    enemy:setVelocity(proj.forward * 60)
    
    local offsetv = Vector:new{0, math.random(-3, 3), 0}
    local offseth = math.random(-3, 3)
    Level:createTask(function()
        -- Wait 2 seconds
        Level:sleep(2)
        enemy.onTimeStep = function(self)
            local alpha = 0.99
            local proj = Level:getSpineProjection(40)
            local right = proj.forward:cross(Vector.UNIT_Y)
            proj.position = proj.position + offsetv
            proj.position = proj.position + right * offseth
            
            local dir = proj.position - enemy:getPosition()
            dir:normalize()
            local cur_vel = enemy:getVelocity()
            
            local vel = (cur_vel*alpha) + (dir*40*(1-alpha))
            enemy:setVelocity(vel)
        end
    end)
end