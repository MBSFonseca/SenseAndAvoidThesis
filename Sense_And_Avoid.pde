import processing.serial.*;
import javax.swing.*;

int x=300; // change size()
int y=300;
int radius = 300;
int a = 0;
int nReceived = 0;
int typeofaircraft;
int[] receiver = new int[100];
int[] emitter = new int[100];
int[] intruder = new int[100];

class Last {
  boolean check = false;
  int a = 0;
  int[] cor = new int[3];
  int time = 0;
}
Last last;
Serial myPort;                       // The serial port

void setup() {
  size(300, 300, P2D);
  last = new Last();

  String d = (String)JOptionPane.showInputDialog(      //Frame for port name input
    frame, 
    "Please choose Port name:", 
    "Choose filename", 
    JOptionPane.WARNING_MESSAGE, 
    null, 
    Serial.list(), 
    Serial.list()[0]);
  try { 
    myPort = new Serial(this, d, 115200);      //Start connection
    print("communication done");
    myPort.bufferUntil(10);                    //buffer until New Line character (10 in ASCII) is received
  } 
  catch(Exception e) {
    println("Port not found");
    exit();
  }
  delay(100);          //allow Arduino to setup
}

void draw() {
  background(127);
  //ellipse(x/2, y/2, 20, 20);
  for (int i=0; i<10; i++) {
    line(x/2, y/2, x/2+cos(radians(360/8*i)+QUARTER_PI/2)*radius, y/2+sin(radians(360/8*i)+QUARTER_PI/2)*radius);
  }
  if (last.check) {
    if (nReceived > 1) {
      if (receiver[0] == receiver[1]-1) last.a = receiver[0]*2+6;
      else last.a = receiver[0]*2+4;
    } else last.a = receiver[0]*2+5;
    if (emitter[0]<4 || emitter[0]>12) {
      last.cor[0] = 255;
      last.cor[1] = 0;
      last.cor[2] = 0;
    } else {
      last.cor[0] = 255;
      last.cor[1] = 204;
      last.cor[2] = 0;
    }
    fill(color(last.cor[0], last.cor[1], last.cor[2]));
    triangle(
      x/2, 
      y/2, 
      x/2+cos(radians(360/8*last.a)+QUARTER_PI/2)*radius, 
      y/2+sin(radians(360/8*last.a)+QUARTER_PI/2)*radius, 
      x/2+cos(radians(360/8*(last.a+1))+QUARTER_PI/2)*radius, 
      y/2+sin(radians(360/8*(last.a+1))+QUARTER_PI/2)*radius);
    last.check = false;
    last.time = millis();
  } else if (last.a!=0 && millis() < last.time + 1000) {
    fill(color(last.cor[0], last.cor[1], last.cor[2]));
    triangle(
      x/2, 
      y/2, 
      x/2+cos(radians(360/8*last.a)+QUARTER_PI/2)*radius, 
      y/2+sin(radians(360/8*last.a)+QUARTER_PI/2)*radius, 
      x/2+cos(radians(360/8*(last.a+1))+QUARTER_PI/2)*radius, 
      y/2+sin(radians(360/8*(last.a+1))+QUARTER_PI/2)*radius);
  }
}

void serialEvent(Serial p) {
  String inString = p.readString();
  print(inString);
  int[] tot = int(split(inString, ','));
  //for(int i=0;i<tot.length;i++) println(tot[i]);
  nReceived = tot.length/3;
  //println(nReceived);
  int u = 0;
  for ( int i =0; i<nReceived*3; i=i+3) {
    receiver[u]=tot[i];
    //println("R"+str(receiver[u])+",");
    emitter[u]=tot[i+1];
    //println("E"+str(emitter[u])+",");
    intruder[u]=tot[i+2];
    //println("I"+str(intruder[u])+",");
    u++;
  }
  last.check = true;
}

void keyPressed() {
  if (key==DELETE) {
    exit();
  }
}