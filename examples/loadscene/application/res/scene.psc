<scene maincamera="camera">
  <root name="RootNode" transform="s: 1.00000, 1.00000, 1.00000,r: 0.00000,-0.00000, 0.00000,t: 0.00000, 0.00000, 0.00000" >
    <drawable name="cao" transform="s: 1.00000, 1.00000, 1.00000,r: 0.00000,-0.00000, 0.00000,t: 0.00000, 0.00000, 0.00000" >
      <geometry id="mesh/cao.pmh" />
      <material id="material/texture.pmt" texture="texture/cao_d_0.png" transparent="true" />
    </drawable>
    <drawable name="floor" transform="s: 1.00000, 1.00000, 1.00000,r: 0.00000,-0.00000, 0.00000,t: 0.00000, 0.00000, 0.00000" >
      <geometry id="mesh/floor.pmh" />
      <material id="material/texture.pmt" texture="texture/floor_d_0.png" />
    </drawable>
    <drawable name="mogu" transform="s: 1.00000, 1.00000, 1.00000,r: 0.00000,-0.00000, 0.00000,t: 0.37744, 0.00000, 0.20811" >
      <geometry id="mesh/mogu.pmh" />
      <material id="material/texture.pmt" texture="texture/mogu.png" />
    </drawable>
    <drawable name="water" transform="s: 1.00000, 1.00000, 1.00000,r: 0.00000,-0.00000, 0.00000,t:-1.84212, 0.00000, 0.00000" >
      <geometry id="mesh/water.pmh" />
      <material id="material/texture.pmt" texture="texture/water.png" />
    </drawable>
    <background name="bg" texture="texture/bg_d_0.png" />
    <camera name="camera" fixed="true" projection="perspective,fov:60.0,aspect:-1,znear:0.1,zfar:1000.0" lookat="eye:0,-80.0,40.0,center:0,0,30,up:0,1,0" />
  </root>
  <texture id="texture/floor_d_0.png" wrap="repeat" />
</scene>
