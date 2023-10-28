#define ledpin  13  //test led
#define tinhieu1 3  //s1
#define tinhieu2 4  //s2
#define tinhieu3 5  //s3
#define tinhieu4 6  //s4
#define tinhieu5 7  //s5

#define in1 11       //right wheel ~
#define in2 12       //right wheel 
#define in3 8      //left wheel 
#define in4 9      //left wheel ~

#define trigPin  10   //Trigger pin for ultrasonic
#define echoPin  2  //Echo pin for ultrasonic

/*Commands*/
#define FW    1 //  Forward
#define SL    2 //  Slight Left
#define SR    3 //  Slight Right
#define LEFT  4 //  Left
#define RIGHT 5 //  Right
#define STP   6 //  Stop motors

/*commands for Wall-problem*/

#define W_U         11  //Wall problem U
#define W_L_Right   12  //Wall problem L turn right
#define W_L_Left    13  //Wall problem L turn left



#define MAX_SPEED 255
#define MED_SPEED 180
#define MIN_SPEED 10

#define MAX_T_PROBLEM 3  //Number of T-problems

#define MAX_WALL_PROBLEM 3  //Number of T-problems
#define WALL_DISTANCE   5    // distance to wall in cm

//Global variable
/*Identify action for each T-Problem - turning left or right*/
int t_problem_action[MAX_T_PROBLEM]={LEFT,FW,RIGHT};
int t_problem_speed[MAX_T_PROBLEM]={MAX_SPEED, MAX_SPEED ,MAX_SPEED};

/*Identify action for each Wall-Problem - turning left or right*/
int w_problem_action[MAX_WALL_PROBLEM]={W_U,W_U,W_U};
int w_problem_speed[MAX_WALL_PROBLEM]={MAX_SPEED, MAX_SPEED ,MAX_SPEED};

int problem_count;
int wall_count;
int car_speed;
boolean led_state;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(tinhieu1, INPUT);
  pinMode(tinhieu2, INPUT);
  pinMode(tinhieu3, INPUT);
  pinMode(tinhieu4, INPUT);
  pinMode(tinhieu5, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ledpin, OUTPUT); 
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  led_state = true;
  digitalWrite(ledpin,led_state);

  problem_count = 0;
  wall_count = 0;
  car_speed=MAX_SPEED;
  }
void forward()
  {
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    digitalWrite(in3,HIGH);
    digitalWrite(in4,LOW);
  }
void forward_v2(int spd)
  {
    analogWrite(in1,spd);
    digitalWrite(in2,LOW);
    digitalWrite(in3,HIGH);
    analogWrite(in4,255-spd);
  }
void backward()
  {
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
  }
 
void backward_v2(int spd)
  {
    analogWrite(in1,255-spd);
    digitalWrite(in2,HIGH);
    digitalWrite(in3,LOW);
    analogWrite(in4,spd);
  }
void turn_left(int spd){
    analogWrite(in1,spd);
    digitalWrite(in2,LOW);
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
    }
void turn_right(int spd){
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
    digitalWrite(in3,HIGH);
    analogWrite(in4,255-spd);
    }
void motor_stop(){
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
    delay(200);
  }
int distance(){
  // defines variables
  long duration;
  int dist;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  dist= duration*0.034/2;
  return dist;
}

void turn_right_90(){
  turn_right(255);
  delay(500);
}
void turn_left_90(){
  turn_left(255);
  delay(600);
}
void turn_wall_u_shape(){
  turn_right_90();
  forward_v2(255);
  delay(300);
  turn_left_90();
  forward_v2(255);
  delay(500);
  turn_left_90();
  forward_v2(255);
  delay(400);
  turn_right_90();
}
void test(){
  //turn_left(100);
  //delay(5000);
  //turn_left(255);
  //delay(1000);

  turn_right(100);
  delay(5000);
  turn_right(255);
  delay(1000);
  forward();
  delay(2000);
  backward();
  delay(2000);
}
  /*
   1, FW:     S3 == LOW
   2, SL:     S2 == LOW && S3 == LOW
   3, SR:     S3 == LOW && S4 == LOW
   4, Left:   S2 == LOW || S1 == LOW
   5, Right:  S4 == LOW || S5 == LOW
   */
int check_sensor(){
    int s1, s2, s3, s4, s5;
    int tmp = 0;
    //Serial.println(distance());
    if(distance() <= WALL_DISTANCE){
      motor_stop();
      delay(500);
      //double check the wall;
      for(int i = 0; i<1000; i++){
        tmp += distance();
      }
      //Serial.print("tmp: ");
      //Serial.println(tmp/1000);
      if (((tmp/1000) > 0) && ((tmp/1000) <= WALL_DISTANCE)){
        tmp=0;
        wall_count++;
        return W_U;//w_problem_action[(wall_count-1) % MAX_WALL_PROBLEM];
      }else{
        return FW;
      }
    }
    s1=digitalRead(tinhieu1);
    s2=digitalRead(tinhieu2);
    s3=digitalRead(tinhieu3);
    s4=digitalRead(tinhieu4);
    s5=digitalRead(tinhieu5);
    //Meet T-Problem - All sensor are LOW
    if(s1 == LOW && s2 == LOW && s3 == LOW && s4 == LOW && s5 == LOW){
      switch(t_problem_action[problem_count%MAX_T_PROBLEM]){  //Check which T-Problem is? Based on problem_count
        case LEFT:
          turn_left(MAX_SPEED);
          car_speed=t_problem_speed[problem_count%MAX_T_PROBLEM];
          delay(250);
          led_state = !led_state;
          digitalWrite(ledpin,led_state);
          problem_count++;
          return FW;
          break;
        case RIGHT:
          turn_right(MAX_SPEED);
          car_speed=t_problem_speed[problem_count%MAX_T_PROBLEM];
          delay(250);
          led_state = !led_state;
          digitalWrite(ledpin,led_state);
          problem_count++;
          return FW;
          break;
        case FW:
          car_speed=t_problem_speed[problem_count%MAX_T_PROBLEM];
          forward_v2(car_speed);
          delay(150);
          led_state = !led_state;
          digitalWrite(ledpin,led_state);
          problem_count++;
          return FW;
          break;
      }
    }else if((s1 == LOW && s2 == LOW && s3 == LOW)||(s1 == LOW && s3 == LOW) || (s1 == LOW && s2 == LOW)){  //Nga 3 phia ben TRAI, nhung di huong PHAI
      return RIGHT;
    }else if (s3 == LOW && s4 == LOW && s5 == LOW){ //Nga 3 phia ben PHAI, nhung di huong THANG
      return FW;
    }else if (s3 == LOW){
      return FW;
    }else if (s2 == LOW && s3 == LOW){
      return SL;
    }else if (s3 == LOW && s4 == LOW){
      return SR;
    }else if (s2 == LOW || s1 == LOW){
      return LEFT;
    }else if (s4 == LOW || s5 == LOW){
      return RIGHT;
    }else{
      return 0;
    }
  }

void loop() {
  // put your main code here, to run repeatedly:
  //Testing the engine
  //turn_wall_u_shape();
   
  switch(check_sensor()){
    case W_U: //Wall problem and turn U shape
      turn_wall_u_shape();
      break;
    case FW:
      forward_v2(car_speed);
      break;
    case SL:
      turn_left(100);
      break;
    case SR:
      turn_right(100);
      break;
    case LEFT:
      turn_left(250);
      break;
    case RIGHT:
      turn_right(250);
      break;
    default:
      backward_v2(180);
  }
  
}

