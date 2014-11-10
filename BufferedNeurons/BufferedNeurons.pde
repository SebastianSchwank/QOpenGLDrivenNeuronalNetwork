 int sizeX = 200;
 int sizeY = 200;
 
 float weight1 = -0.3;
 float weight2 = 0.7;
 
 float input1;
 float input2;
 
 float output = 0.0;
 
 float x = -1.0;
 float y = -1.0;
 
 int i = 0;
 
void setup()
{
  size(sizeX, sizeY);
  background(0);
  
}

void keyPressed(){
  for(int i = 0; i < 20; i++){
    train();
  }
  println("Net Trained 20 steps");
}

void train(){
  
  x = random(200)/100.0 - 1.0;
  y = random(200)/100.0 - 1.0;
  
  float desiredOutput;
  if( x+y < 0){
    desiredOutput = 0.0;
  }
  else{
    desiredOutput = 1.0;
  }
  
  float summe = 0.0;
  summe = weight1 * x;
  summe += weight2 * y;
  
  output = (1.0/(1.0 + exp(-2.0 * summe))); // ACTIVATION FUNCTION
  
  float error = desiredOutput - output;
  
  //println(error);
  
  //fill(output*255);
  //stroke(output*255);
  //ellipse((x+1)*100, (y+1) * 100, 2,2);
  
  //BackPropagation
    float deltaWeight1 = x * error;
    float deltaWeight2 = y * error;
  
    weight1 = weight1 + deltaWeight1;
    weight2 = weight2 + deltaWeight2;
}
 
void draw() {
  
  
  
  if(x > 1.0){ x = -1.0; y+=1.0/100;}
  if(y > 1.0){ y = -1.0; background(0); train();}
  if(i >= 100*100) { i = 0;}
  
  float summe = 0.0;
  summe = weight1 * x;
  summe += weight2 * y;
  
  output = (1.0/(1.0 + exp(-2.0 * summe))); // ACTIVATION FUNCTION
  
  //println(error);
  
  pixels[i] = (int)output*255;
  
  x += 1/100.0;
  i += 1;
  
}
