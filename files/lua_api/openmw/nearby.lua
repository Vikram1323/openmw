---
-- `openmw.nearby` provides read-only access to the nearest area of the game world.
-- Can be used only from local scripts.
-- @module nearby
-- @usage local nearby = require('openmw.nearby')



---
-- List of nearby activators.
-- @field [parent=#nearby] openmw.core#ObjectList activators

---
-- List of nearby actors.
-- @field [parent=#nearby] openmw.core#ObjectList actors

---
-- List of nearby containers.
-- @field [parent=#nearby] openmw.core#ObjectList containers

---
-- List of nearby doors.
-- @field [parent=#nearby] openmw.core#ObjectList doors

---
-- Everything that can be picked up in the nearby.
-- @field [parent=#nearby] openmw.core#ObjectList items

---
-- List of nearby players. Currently (since multiplayer is not yet implemented) always has one element.
-- @field [parent=#nearby] openmw.core#ObjectList players

---
-- Return an object by RefNum/FormId.
-- Note: the function always returns @{openmw.core#GameObject} and doesn't validate that
-- the object exists in the game world. If it doesn't exist or not yet loaded to memory),
-- then `obj:isValid()` will be `false`.
-- @function [parent=#nearby] getObjectByFormId
-- @param #string formId String returned by `core.getFormId`
-- @return openmw.core#GameObject
-- @usage local obj = nearby.getObjectByFormId(core.getFormId('Morrowind.esm', 128964))

---
-- @type COLLISION_TYPE
-- @field [parent=#COLLISION_TYPE] #number World
-- @field [parent=#COLLISION_TYPE] #number Door
-- @field [parent=#COLLISION_TYPE] #number Actor
-- @field [parent=#COLLISION_TYPE] #number HeightMap
-- @field [parent=#COLLISION_TYPE] #number Projectile
-- @field [parent=#COLLISION_TYPE] #number Water
-- @field [parent=#COLLISION_TYPE] #number Default Used by default: World+Door+Actor+HeightMap
-- @field [parent=#COLLISION_TYPE] #number AnyPhysical : World+Door+Actor+HeightMap+Projectile+Water
-- @field [parent=#COLLISION_TYPE] #number Camera Objects that should collide only with camera
-- @field [parent=#COLLISION_TYPE] #number VisualOnly Objects that were not intended to be part of the physics world

---
-- Collision types that are used in `castRay`.
-- Several types can be combined with @{openmw_util#util.bitOr}.
-- @field [parent=#nearby] #COLLISION_TYPE COLLISION_TYPE

---
-- Result of raycasing
-- @type RayCastingResult
-- @field [parent=#RayCastingResult] #boolean hit Is there a collision? (true/false)
-- @field [parent=#RayCastingResult] openmw.util#Vector3 hitPos Position of the collision point (nil if no collision)
-- @field [parent=#RayCastingResult] openmw.util#Vector3 hitNormal Normal to the surface in the collision point (nil if no collision)
-- @field [parent=#RayCastingResult] openmw.core#GameObject hitObject The object the ray has collided with (can be nil)

---
-- A table of parameters for @{#nearby.castRay}
-- @type CastRayOptions
-- @field openmw.core#GameObject ignore An object to ignore (specify here the source of the ray)
-- @field #number collisionType Object types to work with (see @{openmw.nearby#COLLISION_TYPE})
-- @field #number radius The radius of the ray (zero by default). If not zero then castRay actually casts a sphere with given radius.
--  NOTE: currently `ignore` is not supported if `radius>0`.

---
-- Cast ray from one point to another and return the first collision.
-- @function [parent=#nearby] castRay
-- @param openmw.util#Vector3 from Start point of the ray.
-- @param openmw.util#Vector3 to End point of the ray.
-- @param #CastRayOptions options An optional table with additional optional arguments
-- @return #RayCastingResult
-- @usage if nearby.castRay(pointA, pointB).hit then print('obstacle between A and B') end
-- @usage local res = nearby.castRay(self.position, enemy.position, {ignore=self})
-- if res.hitObject and res.hitObject ~= enemy then obstacle = res.hitObject end
-- @usage local res = nearby.castRay(self.position, targetPos, {
--     collisionType=nearby.COLLISION_TYPE.HeightMap + nearby.COLLISION_TYPE.Water,
--     radius = 10,
-- })

---
-- Cast ray from one point to another and find the first visual intersection with anything in the scene.
-- As opposite to `castRay` can find an intersection with an object without collisions.
-- In order to avoid threading issues can be used only in player scripts only in `onFrame` or
-- in engine handlers for user input. In other cases use `asyncCastRenderingRay` instead.
-- @function [parent=#nearby] castRenderingRay
-- @param openmw.util#Vector3 from Start point of the ray.
-- @param openmw.util#Vector3 to End point of the ray.
-- @return #RayCastingResult

---
-- Asynchronously cast ray from one point to another and find the first visual intersection with anything in the scene.
-- @function [parent=#nearby] asyncCastRenderingRay
-- @param openmw.async#Callback callback The callback to pass the result to (should accept a single argument @{openmw.nearby#RayCastingResult}).
-- @param openmw.util#Vector3 from Start point of the ray.
-- @param openmw.util#Vector3 to End point of the ray.

---
-- @type NAVIGATOR_FLAGS
-- @field [parent=#NAVIGATOR_FLAGS] #number Walk allow agent to walk on the ground area;
-- @field [parent=#NAVIGATOR_FLAGS] #number Swim allow agent to swim on the water surface;
-- @field [parent=#NAVIGATOR_FLAGS] #number OpenDoor allow agent to open doors on the way;
-- @field [parent=#NAVIGATOR_FLAGS] #number UsePathgrid allow agent to use predefined pathgrid imported from ESM files.

---
-- @type COLLISION_SHAPE_TYPE
-- @field [parent=#CCOLLISION_SHAPE_TYPE] #number Aabb Axis-Aligned Bounding Box is used for NPC and symmetric
-- Creatures;
-- @field [parent=#COLLISION_SHAPE_TYPE] #number RotatingBox is used for Creatures with big difference in width and
-- height;
-- @field [parent=#COLLISION_SHAPE_TYPE] #number Cylinder is used for NPC and symmetric Creatures.

---
-- @type FIND_PATH_STATUS
-- @field [parent=#FIND_PATH_STATUS] #number Success Path is found;
-- @field [parent=#FIND_PATH_STATUS] #number PartialPath Last path point is not a destination but a nearest position
-- among found;
-- @field [parent=#FIND_PATH_STATUS] #number NavMeshNotFound Provided `agentBounds` don't have corresponding navigation
-- mesh. For interior cells it means an agent with such `agentBounds` is present on the scene. For exterior cells only
-- default `agentBounds` is supported;
-- @field [parent=#FIND_PATH_STATUS] #number StartPolygonNotFound `source` position is too far from available
-- navigation mesh. The status may appear when navigation mesh is not fully generated or position is outside of covered
-- area;
-- @field [parent=#FIND_PATH_STATUS] #number EndPolygonNotFound `destination` position is too far from available
-- navigation mesh. The status may appear when navigation mesh is not fully generated or position is outside of covered
-- area;
-- @field [parent=#FIND_PATH_STATUS] #number MoveAlongSurfaceFailed Found path couldn't be smoothed due to imperfect
-- algorithm implementation or bad navigation mesh data;
-- @field [parent=#FIND_PATH_STATUS] #number FindPathOverPolygonsFailed Path over navigation mesh from `source` to
-- `destination` does not exist or navigation mesh is not fully generated to provide the path;
-- @field [parent=#FIND_PATH_STATUS] #number InitNavMeshQueryFailed Couldn't initialize required data due to bad input
-- or bad navigation mesh data.

---
-- Find path over navigation mesh from source to destination with given options. Result is unstable since navigation
-- mesh generation is asynchronous.
-- @function [parent=#nearby] findPath
-- @param openmw.util#Vector3 source Initial path position.
-- @param openmw.util#Vector3 destination Final path position.
-- @param #table options An optional table with additional optional arguments. Can contain:
--
--   * `agentBounds` - a table identifying which navmesh to use, can contain:
--
--     * `shapeType` - one of @{#COLLISION_SHAPE_TYPE} values;
--     * `halfExtents` - @{openmw.util#Vector3} defining agent bounds size;
--   * `stepSize` - a floating point number to define frequency of path points
--     (default: `2 * math.max(halfExtents:x, halfExtents:y)`)
--   * `includeFlags` - allowed areas for agent to move, a sum of @{#NAVIGATOR_FLAGS} values
--     (default: @{#NAVIGATOR_FLAGS.Walk} + @{#NAVIGATOR_FLAGS.Swim} +
--     @{#NAVIGATOR_FLAGS.OpenDoor} + @{#NAVIGATOR_FLAGS.UsePathgrid});
--   * `areaCosts` - a table defining relative cost for each type of area, can contain:
--
--     * `ground` - a floating point number >= 0, used in combination with @{#NAVIGATOR_FLAGS.Walk} (default: 1);
--     * `water` - a floating point number >= 0, used in combination with @{#NAVIGATOR_FLAGS.Swim} (default: 1);
--     * `door` - a floating point number >= 0, used in combination with @{#NAVIGATOR_FLAGS.OpenDoor} (default: 2);
--     * `pathgrid` - a floating point number >= 0, used in combination with @{#NAVIGATOR_FLAGS.UsePathgrid}
--       (default: 1);
--   * `destinationTolerance` - a floating point number representing maximum allowed distance between destination and a
--     nearest point on the navigation mesh in addition to agent size (default: 1);
-- @return #FIND_PATH_STATUS
-- @return #list<openmw.util#Vector3>
-- @usage local status, path = nearby.findPath(source, destination)
-- @usage local status, path = nearby.findPath(source, destination, {
--     includeFlags = nearby.NAVIGATOR_FLAGS.Walk + nearby.NAVIGATOR_FLAGS.OpenDoor,
--     areaCosts = {
--         door = 1.5,
--     },
-- })
-- @usage local status, path = nearby.findPath(source, destination, {
--     agentBounds = Actor.getPathfindingAgentBounds(self),
-- })

---
-- Returns random location on navigation mesh within the reach of specified location.
-- The location is not exactly constrained by the circle, but it limits the area.
-- @function [parent=#nearby] findRandomPointAroundCircle
-- @param openmw.util#Vector3 position Center of the search circle.
-- @param #number maxRadius Approximate maximum search distance.
-- @param #table options An optional table with additional optional arguments. Can contain:
--
--   * `agentBounds` - a table identifying which navmesh to use, can contain:
--
--     * `shapeType` - one of @{#COLLISION_SHAPE_TYPE} values;
--     * `halfExtents` - @{openmw.util#Vector3} defining agent bounds size;
--   * `includeFlags` - allowed areas for agent to move, a sum of @{#NAVIGATOR_FLAGS} values
--     (default: @{#NAVIGATOR_FLAGS.Walk} + @{#NAVIGATOR_FLAGS.Swim} +
--     @{#NAVIGATOR_FLAGS.OpenDoor} + @{#NAVIGATOR_FLAGS.UsePathgrid});
-- @return openmw.util#Vector3, #nil
-- @usage local position = nearby.findRandomPointAroundCircle(position, maxRadius)
-- @usage local position = nearby.findRandomPointAroundCircle(position, maxRadius, {
--     includeFlags = nearby.NAVIGATOR_FLAGS.Walk,
-- })
-- @usage local position = nearby.findRandomPointAroundCircle(position, maxRadius, {
--     agentBounds = Actor.getPathfindingAgentBounds(self),
-- })

---
-- Finds a nearest to the ray target position starting from the initial position with resulting curve drawn on the
-- navigation mesh surface.
-- @function [parent=#nearby] castNavigationRay
-- @param openmw.util#Vector3 from Initial ray position.
-- @param openmw.util#Vector3 to Target ray position.
-- @param #table options An optional table with additional optional arguments. Can contain:
--
--   * `agentBounds` - a table identifying which navmesh to use, can contain:
--
--     * `shapeType` - one of @{#COLLISION_SHAPE_TYPE} values;
--     * `halfExtents` - @{openmw.util#Vector3} defining agent bounds size;
--   * `includeFlags` - allowed areas for agent to move, a sum of @{#NAVIGATOR_FLAGS} values
--     (default: @{#NAVIGATOR_FLAGS.Walk} + @{#NAVIGATOR_FLAGS.Swim} +
--     @{#NAVIGATOR_FLAGS.OpenDoor} + @{#NAVIGATOR_FLAGS.UsePathgrid});
-- @return openmw.util#Vector3, #nil
-- @usage local position = nearby.castNavigationRay(from, to)
-- @usage local position = nearby.castNavigationRay(from, to, {
--     includeFlags = nearby.NAVIGATOR_FLAGS.Swim,
-- })
-- @usage local position = nearby.castNavigationRay(from, to, {
--     agentBounds = Actor.getPathfindingAgentBounds(self),
-- })

return nil
