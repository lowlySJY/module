#define ENA 24
#define PUL 26                      //定义脉冲引脚为10号引脚
#define DIR 28                      //定义方向引脚为11号引脚
#define Direction 1                 //定义正方向（归零方向）
#define pulseperrev 200             //脉冲每转
#define XFS 16                      //定义细分数参数
#define XW 3                        //定义限位开关为3号引脚
#define BJZ 200                     //步距角360/1.8
#define DLCS 30                     //带轮齿数
#define DLKD 2                      //带轮宽度
#define LDBC 0                      //零点补偿
int nowread = 1;
int lastread = 1;
int count;
int i;
int num;
int TIME;
void setup(){
  pinMode(ENA,OUTPUT);
  pinMode(PUL,OUTPUT);   //设置pulse引脚为输出模式
  pinMode(DIR,OUTPUT);    //设置方向引脚为输出模式 
  pinMode(XW,INPUT);    
  digitalWrite(DIR,Direction);
  digitalWrite(ENA,LOW);        //使能端low驱动
}
void loop(){
  
    origin();
    delay(1000);
    stepper(200,0,6000);
    delay(1500);
    stepper(50,1,6000);
    delay(1500);
    stepper(150,0,6000);
    delay(1500);
    origin();
    delay(1000);
    
}

//stepper函数步进电机相对于前一个状态驱动，dis距离，dir方向，spd速度(毫米每分钟)
void stepper(double dis,int dir,double spd){
  digitalWrite(DIR,dir);
  TIME = int((30000000*DLCS*DLKD)/(double)(BJZ*XFS*spd));
  count = int((dis/(double)(DLCS*DLKD))*(XFS*BJZ));
  for(i=0;i<count;i++){
    digitalWrite(PUL,HIGH);     //脉冲引脚写高电平
    delayMicroseconds(TIME);   //延时200us
    digitalWrite(PUL,LOW);      //脉冲引脚写低电平
    delayMicroseconds(TIME);   //延时200us
    }
  }


void origin(){
  nowread = digitalRead(XW);
  if(nowread == 0){
    stepper(50,Direction^1,2000);
        
    while(digitalRead(XW)==1){
      digitalWrite(DIR,Direction);
      digitalWrite(PUL,HIGH);     //脉冲引脚写高电平
      delayMicroseconds(400);   //延时200us
      digitalWrite(PUL,LOW);      //脉冲引脚写低电平
      delayMicroseconds(400);   //延时200us
      }
    }
   else{ 
   while(digitalRead(XW)==1){
      digitalWrite(DIR,Direction);
      digitalWrite(PUL,HIGH);     //脉冲引脚写高电平
      delayMicroseconds(150);   //延时200us
      digitalWrite(PUL,LOW);      //脉冲引脚写低电平
      delayMicroseconds(150);   //延时200us
      }
      stepper(50,Direction^1,2000);
    while(digitalRead(XW)==1){
      digitalWrite(DIR,Direction);
      digitalWrite(PUL,HIGH);     //脉冲引脚写高电平
      delayMicroseconds(400);   //延时200us
      digitalWrite(PUL,LOW);      //脉冲引脚写低电平
      delayMicroseconds(400);   //延时200us
      }
      digitalWrite(PUL,LOW);
      delay(1000);
    }
  
      stepper(LDBC,0,200);
      delay(500);
      
}
 




  
