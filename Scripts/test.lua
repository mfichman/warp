require "Scripts.Object"

a = Quaternion:new()
a:fromAngleAxis({0,1,0}, math.pi/4)
print(a[1], a[2], a[3], a[4])
angle, axis = a:toAngleAxis()
print(axis[1], axis[2], axis[3], angle)

rot = a:toRotationMatrix()

b = Quaternion:new()
b:fromRotationMatrix(rot)
print(b[1], b[2], b[3], b[4])

x,y,z = a:toAxes()
b:fromAxes(x,y,z)
print(b[1], b[2], b[3], b[4])

c = Quaternion:new()
c:fromAngleAxis({1,0,0}, math.pi/4)
c = c * b
print(c[1], c[2], c[3], c[4])

d = slerp(a,c,.1)
print(d[1], d[2], d[3], d[4])
