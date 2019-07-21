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
#define fastspd 6500                //快速进给速率
#define slowspd 2000                //慢速进给速率
int nowread = 1;
int lastread = 1;
int count;
int i;
int num;
int currentX;                       //X坐标
int lastdis;
int TIME;
char UART_RX;     //串口接收到的字符
int state;        //状态，0为接收到G，1为接收到X 2为接收到F
int readtype=0;      //读取状态，应该读英文时为0，读数字为1
int parameter[3]={0,0,0};  //参数，第一个放G结果，第二个放X结果，第三个放F结果
int current_number;
boolean flag = 0;          //检测输入有没有输错

void setup() {
  Serial.begin(9600);
  pinMode(ENA,OUTPUT);
  pinMode(PUL,OUTPUT);   //设置pulse引脚为输出模式
  pinMode(DIR,OUTPUT);    //设置方向引脚为输出模式 
  pinMode(XW,INPUT);    
  digitalWrite(DIR,Direction);
  digitalWrite(ENA,LOW);        //使能端low驱动
}

   //G01 X60 F5000
void loop() {
  flag = 0;
  readtype = 0;
  while(Serial.available()==0);           //等待上位机控制
  if(Serial.available()>0)UART_RX=Serial.read();
  do{   //当读取到的不是回车时（命令还没有结束）
    //if(UART_RX==' '&&readtype==0)continue;
    if(readtype==0){
        switch(UART_RX){
        case 'G':
        case 'g':state=0;parameter[state] =0;break;
        case 'X':
        case 'x':state=1;parameter[state] =0;break;
        case 'F':
        case 'f':state=2;parameter[state] =0;break;
        default:Serial.println(UART_RX);break;   //while(Serial.read()!='\n');flag = 1;   //如果发现有错误，一直读掉这句话
        }
    readtype = 1;
    }
    else if(readtype==1){
      if(UART_RX>='0'&&UART_RX<='9'){  
        current_number = UART_RX-'0';
        parameter[state] = parameter[state]*10+current_number;
      }
      else readtype = 0;
    }
  while(Serial.available()==0);
  }while((UART_RX=Serial.read())!='\n');
  switch(parameter[0]){
    case 0:Serial.print("fastmove to X= ");Serial.println(parameter[1]);
            stepper_X(parameter[1],fastspd,lastdis);
            lastdis = parameter[1];
            break;
    case 1:Serial.print("toolfeed to X= ");Serial.print(parameter[1]);
            Serial.print(" with feedrate f = ");Serial.println(parameter[2]);
            stepper_X(parameter[1],parameter[2],lastdis);
            lastdis = parameter[1];
            break;
    case 28:Serial.println("return to Origin");
            origin();delay(1500);
            lastdis = LDBC;
            break;
    case 92:Serial.print("set the current x position to ");Serial.println(parameter[1]);
            currentX = parameter[1];
            lastdis = currentX;
            break;
    default:Serial.println(parameter[0]);
  }
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

//stepper_X步进电机相对坐标原点运动
void stepper_X(double dis,double spd,int lastdis){
  if(dis < lastdis){digitalWrite(DIR,Direction);}
  else{digitalWrite(DIR,Direction^1);}
  TIME = int((30000000*DLCS*DLKD)/(double)(BJZ*XFS*spd));
  double s;
  s = dis-lastdis;
  count = int((s/(double)(DLCS*DLKD))*(XFS*BJZ));
    for(i=0;i<abs(count);i++){
    digitalWrite(PUL,HIGH);     //脉冲引脚写高电平
    delayMicroseconds(TIME);   //延时200us
    digitalWrite(PUL,LOW);      //脉冲引脚写低电平
    delayMicroseconds(TIME);   //延时200us
    }
  }
 
void origin(){
  nowread = digitalRead(XW);
  if(nowread == 0){
    stepper(50,Direction^1,slowspd);
        
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
      stepper(50,Direction^1,slowspd);
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
  
      stepper(LDBC,0,slowspd);
      delay(500);
      currentX = LDBC;
     
}
 
    
    
      
