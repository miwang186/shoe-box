/********************************************************
 * PID RelayOutput Example
 * Same as basic example, except that this time, the output
 * is going to a digital pin which (we presume) is controlling
 * a relay.  the pid is designed to Output an analog value,
 * but the relay can only be On/Off.
 *
 *   to connect them together we use "time proportioning
 * control"  it's essentially a really slow version of PWM.
 * first we decide on a window size (5000mS say.) we then 
 * set the pid to adjust its output between 0 and that window
 * size.  lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the 
 * window being "Relay Off Time"
  
 PID继电器输出范例
 与基本范例相同，这一次输出是一个数字引脚控制的继电器。PID被设计成
 输出一个模拟值，但是继电器只有开关状态。
 为了联系上两者，我们使用时间比例控制，它本质上是一个很慢的PWM。
 首先我们决定一个窗口时间（比如5000ms）。
 然后设置PID适应它的输出在0到窗口时间的范围。
 最后我们添加一些逻辑，把PID输出转换成“继电器接通时间”和剩余的
 “继电器断开时间”
 ********************************************************/
 
#include <PID_v1.h>
#define RelayPin 8
// 定义我们将要使用的变量
//Define Variables we'll be connecting to 
double Setpoint, Input, Output;
//指定链接和最初的调优参数
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,2,5,1, DIRECT);
 
int WindowSize = 2000;
unsigned long windowStartTime;
void setup()
{
  windowStartTime = millis();
  //初始化变量
  //initialize the variables we're linked to
  Setpoint = 100;
  //告诉PID在从0到窗口大小的范围内取值
  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);
  //开启PID
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}
 
void loop()
{
  Input = analogRead(0);
  myPID.Compute();
 
  /************************************************
   * turn the output pin on/off based on pid output 基于PID输出，打开或关闭端口输出
   ************************************************/
  if(millis() - windowStartTime>WindowSize)
  { //time to shift the Relay Window 继电器窗口时间
    windowStartTime += WindowSize;
  }
  if(Output < millis() - windowStartTime) digitalWrite(RelayPin,HIGH);
  else digitalWrite(RelayPin,LOW);
 
}