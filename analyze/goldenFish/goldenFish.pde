Table a;
Table b;
Table answer;
int n = 0;
int t = 0;

void setup() {
  size(475, 1128);
  a = loadTable("../../out.csv");
  strokeWeight(0.01);
}

void draw() {
  background(255);
  pushMatrix();
  scale(100);
  translate(2.375, 4.88);
  noFill();
  stroke(0);
  for (int i = 0; i < a.getColumnCount (); i++) {

    float mina = a.getFloat(0, i);
    float maxa = a.getFloat(1, i);
    float mind = a.getFloat(2, i);
    float maxd = a.getFloat(3, i);
    float min=radians(mina); 
    float max=radians(maxa);
    line(mind*cos(min), mind*sin(min), maxd*cos(min), maxd*sin(min));
    line(mind*cos(max), mind*sin(max), maxd*cos(max), maxd*sin(max));
    arc(0, 0, maxd*2, 2*maxd, radians(mina), radians(maxa));
    arc(0, 0, mind*2, 2*mind, radians(mina), radians(maxa));
  }
  //  fill(255, 9, 9);
  //  ellipse(3, 5, 0.29, 0.29);
  popMatrix();
}
/*
(x,y)
 x=rcosTheta
 y=rsinTheta
 (x-tx)^2+(y-ty)^2<=((0.29)/2)^2
 sinx=rsincos
 cosy=rsincos
 sinx=cosy
 y=tanx
 r=x/cos
 r=y/sin
 
 
 
 
 */
