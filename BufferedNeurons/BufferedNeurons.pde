//*Binäre Ereignisse
 
class wuerfel{
  float p;
  
  wuerfel(float p){
    this.p = p;
  }
  
  boolean wurf(){
    float ergebniss = (random(10000)%(10000))/10000.0;
    if(ergebniss > p) return true;
    return false;
  }
  
}

 int sizeX = 800;
 int sizeY = 800;
 
 float weight = 0.1;
 
 float input;
 
 float outbuffer = 0;
 
int cycleCounter = 0;
 
void setup()
{
  size(sizeX, sizeY);
  background(0);
}
 
void draw() {
  
  if(cycleCounter == 0) input = 5;
  else input = 0;
  
  input = sin(cycleCounter/10.0);
  //input = cycleCounter/10.0;
  
  outbuffer = (outbuffer + input)*weight;
  
  //desired output
  float odes = sin(cycleCounter/10.0);
  //Learning Function
  weight =  (odes-outbuffer);
  
  stroke(255,0,0);
  point(cycleCounter,outbuffer*5+400);
  
  stroke(0,255,0);
  point(cycleCounter,input*5+200);
  
  cycleCounter += 1;
   
}
