-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Note: most of this code is translated from the ogre source ogrequaternion.cpp

Quaternion = Object:new()

-- rotates a vector
-- adapted from wikipedia pseudo-code http://en.wikipedia.org/wiki/Quaternion_rotation
function Quaternion:__mul(v)
    if (v[4]) then
        return self:quat_mult(v)
    else
        local t2  =  self[4] * self[1]
        local t3  =  self[4] * self[2]
        local t4  =  self[4] * self[3]
        local t5  = -self[1] * self[1]
        local t6  =  self[1] * self[2]
        local t7  =  self[1] * self[3]
        local t8  = -self[2] * self[2]
        local t9  =  self[2] * self[3]
        local t10 = -self[3] * self[3]
        local result = Vector:new()
        result[1] = 2*( (t8 + t10) * v[1] + (t6 -  t4) * v[2] + (t3 + t7) * v[3]) + v[1]
        result[2] = 2*( (t4 +  t6) * v[1] + (t5 + t10) * v[2] + (t9 - t2) * v[3]) + v[2]
        result[3] = 2*( (t7 -  t3) * v[1] + (t2 +  t9) * v[2] + (t5 + t8) * v[3]) + v[3]
        return result
    end
end

-- rotate another quaternion by this one and return the result
function Quaternion:quat_mult(other)
    result = Quaternion:new()
--   w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
--   w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
--   w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
--   w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
    result[4] = self[4] * other[4] - self[1] * other[1] - self[2] * other[2] - self[3] * other[3]
    result[1] = self[4] * other[1] + self[1] * other[4] + self[2] * other[3] - self[3] * other[2]
    result[2] = self[4] * other[2] + self[2] * other[4] + self[3] * other[1] - self[1] * other[3]
    result[3] = self[4] * other[3] + self[3] * other[4] + self[1] * other[2] - self[2] * other[1]
    return result
end

function Quaternion:toAngleAxis() 
    local sqr_length = self[1] * self[1] + self[2] * self[2] + self[3] * self[3]
    local angle = 0
    local axis = Vector:new()
    if (sqr_length > 0) then
        angle = 2 * math.acos(self[4])
        local inv_length = 1 / math.sqrt(sqr_length)
        axis[1] = self[1] * sqr_length
        axis[2] = self[2] * sqr_length
        axis[3] = self[3] * sqr_length
    else
        axis:set(1,0,0)
    end
    return angle, axis
end

function Quaternion:getDirection()
    -- a little hacky. assume that meshes point down -z axis by default
    -- default behavior should probably be the positive z axis
    return self * {0,0,-1}
end

function Quaternion:fromAngleAxis(vector, angle)
    local half_angle = 0.5*angle
    local f_sin = math.sin(half_angle)
    self[4] = math.cos(half_angle);
    self[1] = f_sin*vector[1];
    self[2] = f_sin*vector[2];
    self[3] = f_sin*vector[3];
end

function Quaternion:fromForwardUp(forward, up)
    forward = Vector:new(forward)
    left = forward:cross(up)
    -- unfinished
end

function Vector:__tostring()
    return self[1]..", "..self[2]..", "..self[3]..", "..self[4]
end
