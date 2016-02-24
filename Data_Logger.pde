
import processing.serial.*;
import javax.swing.*;

Serial myPort;                       // The serial port
PrintWriter output;
int x=600; // change size()
int y=600;
int radius = 600;
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
  int intruder = 0;
}
Last last;

void setup() {
  size(600, 600, P2D);
  last = new Last();
  String s = (String)JOptionPane.showInputDialog(
    frame, 
    "Please choose name for file:", 
    "Choose filename", 
    JOptionPane.WARNING_MESSAGE, 
    null, 
    null, 
    "filename");
  s += ".txt";
  try {
    output = createWriter(s);
  } 
  catch (Exception e) {
    println("Could not create file");
    exit();
  }
  String d = (String)JOptionPane.showInputDialog(
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
  delay(100);
}

void draw() {
  background(127);
  //ellipse(x/2, y/2, 20, 20);
  for (int i=0; i<10; i++) {
    line(x/2, y/2, x/2+cos(radians(360/8*i)+QUARTER_PI/2)*radius, y/2+sin(radians(360/8*i)+QUARTER_PI/2)*radius);
  }
  if (last.check) {
    last.intruder =  emitter[0];
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
    fill(0,0,0);
    textSize(100);
    textAlign(CENTER);
    text(last.intruder, x/2, y-50);
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
    fill(0,0,0);
    textSize(100);
    textAlign(CENTER);
    text(last.intruder, x/2, y-50);
  }
}

void serialEvent(Serial p) {
  String inString = p.readString();
  String time = str(hour())+","+str(minute())+","+str(second())+","+str(millis())+","+inString;
  print(time);
  output.print(time);
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
    myPort.clear();
    myPort.stop();
    if (output != null) {
      println("Closing File");
      try {
        output.close();
      } 
      catch(Exception e) {
        println("Error while closing the writer");
      }
    } else println("File is empty");
    exit();
  }
}