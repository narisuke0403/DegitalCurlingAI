Table a;
Table b;
Table answer;
int n = 0;
int t = 0;

void setup() {
  size(800, 800);
  a = loadTable("../../Log/csv/log_"+str(t)+".csv");
  b = loadTable("../out"+str(t)+".csv");
  answer = loadTable("../answer"+str(t)+".csv");
}

void draw() {
  background(0);
  Parse(t,n);
}

void Parse(int t,int n) {
  float[] x = new float[t+1];
  float[] y = new float[t+1];
  int[] c = new int[t+1];
  float[] _x = new float[t+1];
  float[] _y = new float[t+1];
  int[] _c = new int[t+1];
  int p = b.getInt(0,n);
  int number = 0;
  for (int i = 0; i < a.getColumnCount(); i+=8) {
    x[number] = a.getFloat(n, i)*(2.375/2)+2.375;
    y[number] = a.getFloat(n, i+1)*(4.88/2)+4.88;
    c[number] = a.getInt(n, i+2);
    if (p != -1) {
      _x[number] = answer.getFloat(p, i)*(2.375/2)+2.375;
      _y[number] = answer.getFloat(p, i+1)*(4.88/2)+4.88;
      _c[number] = answer.getInt(p, i+2);
    }
    number+=1;
  }
  print(n);print("は");
  print(p);println("に似ている");
  
  pushMatrix();
  translate(40, 0);
  scale(70);
  stroke(0);
  strokeWeight(1);
  fill(244);
  rect(0, 0, 4.75, 11.28);
  noStroke();
  fill(0, 0, 255);
  ellipse(2.375, 4.88, 3.66/2, 3.66/2);
  fill(-1);
  ellipse(2.375, 4.88, 2.44/2, 2.44/2);
  fill(255, 0, 0);
  ellipse(2.375, 4.88, 1.22/2, 1.22/2);
  fill(-1);
  ellipse(2.375, 4.88, 1.22/8, 1.22/8);
  stroke(0);
  strokeWeight(0.01);
  for (int i = 0; i < number; i++) {
    if (x[i] != 0.0 && y[i] != 0.0) {
      if (c[i] == -1) {
        fill(255, 0, 0);
      } else {
        fill( 255, 255, 0);
      }
      ellipse(x[i], y[i], 0.29/2, 0.29/2);
    }
  }
  popMatrix();
  
  pushMatrix();
  translate(440, 0);
  scale(70);
  stroke(0);
  strokeWeight(1);
  fill(244);
  rect(0, 0, 4.75, 11.28);
  noStroke();
  fill(0, 0, 255);
  ellipse(2.375, 4.88, 3.66/2, 3.66/2);
  fill(-1);
  ellipse(2.375, 4.88, 2.44/2, 2.44/2);
  fill(255, 0, 0);
  ellipse(2.375, 4.88, 1.22/2, 1.22/2);
  fill(-1);
  ellipse(2.375, 4.88, 1.22/8, 1.22/8);
  stroke(0);
  strokeWeight(0.01);
  for (int i = 0; i < number; i++) {
    if (_x[i] != 0.0 && _y[i] != 0.0) {
      if (_c[i] == -1) {
        fill(255, 0, 0);
      } else {
        fill( 255, 255, 0);
      }
      ellipse(_x[i], _y[i], 0.29/2, 0.29/2);
    }
  }
  popMatrix();
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == RIGHT && n < a.getRowCount()-1) {
      n += 1;
    } else if (keyCode == LEFT && n != 0) {
      n -= 1;
    }
  }
}