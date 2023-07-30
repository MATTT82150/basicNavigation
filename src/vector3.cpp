class vector3 {

  public:

  int x;
  int y;
  int z;

  vector3(int x0, int y0, int z0) {
    x = x0;
    y = y0;
    z = z0;
  }

  vector3 difference(vector3 v) {
    return vector3(x-v.x, y-v.y, z-v.z);
  }

  vector3 magnitude() {
    return sqrt(x*x, y*y, z*z);
  }

  vector3 add(vector3 v) {
    vector3(x+v.x, y+v.y, z+v.z);
  }
}