----------------------------------------------------------------------
-- CGAL svdlinf ipelet description
----------------------------------------------------------------------

label = "SVDLinf"

about = [[
This ipelet is based on the CGAL_ipelet package. See www.cgal.org.
]]

-- this variable will store the C++ ipelet when it has been loaded
ipelet = false

function run(model, num)
  if not ipelet then ipelet = assert(ipe.Ipelet(dllname)) end
  model:runIpelet(methods[num].label, ipelet, num)
end

methods = {
  { label="Segment VD Linf generall" },
  { label="Segment skeleton Linf general" },
  { label="Segment VD Linf axis-parallel" },
  { label="Segment skeleton Linf axis-parallel" },
  { label="Help" },
}

----------------------------------------------------------------------
