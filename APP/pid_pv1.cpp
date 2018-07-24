/**********************************************************************************************
 * Arduino PID Library - Version 1.1.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/
#include "pid_pv1.h"
#include <rtthread.h>
/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up 
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
PID::PID(double* Input, double* Output, double* Setpoint,
        double Kp, double Ki, double Kd, int ControllerDirection)
{
	// 赋值控制量、被控量及设定值初始地址，注意这里是地址
    myOutput = Output;
    myInput = Input;
    mySetpoint = Setpoint;
	// 初始化auto模式为false
	inAuto = false;
	
	// 默认控制量限制在0到255，此函数可以根据实际系统需要修改控制量输出限制范围
	PID::SetOutputLimits(0, 255);				//default output limit corresponds to 
												//the arduino pwm limits
	// 默认采样周期为100ms，同样可以根据需求修改
    SampleTime = 100;							//default Controller Sample Time is 0.1 seconds

	// 设置输出的方向
    PID::SetControllerDirection(ControllerDirection);
	// 设置PID 控制参数
    PID::SetTunings(Kp, Ki, Kd);

	// 用于存储PID构造时，对应的系统运行时间
	// rt_tick_get()作用是获取当前系统运行时间（单位ms），此函数针对arduino；移植到别的系统，可以其他类似作用函数替代
	// 这里减去SampleTime是为了保证在构造后能力马上进行PID控制，而不需要等待到下一个SampleTime周期
    lastTime = rt_tick_get()-SampleTime;				
}
 
 
/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 *   此函数用于PID控制量计算，函数可以频繁的在进程中被调用。
 **********************************************************************************/ 
bool PID::Compute()
{
	// 如果没有开启PID返回 计算失败，退出；控制量不变，仍为上一次控制量
   if(!inAuto) return false;
   // 获取当前系统运行时间并求出相对上一次计算时间间隔
   unsigned long now = rt_tick_get();
   unsigned long timeChange = (now - lastTime);
   // 如果时间间隔大于或者等于采样时间，那么则计算,否则不满足采样条件，计算失败，退出；
   if(timeChange>=SampleTime)
   {
      /*Compute all the working error variables*/
	   // 保存当前被控量，如果是一个实时控制系统，此时被控量可能与构造时的被控量不一致
	  double input = *myInput;
	  // 求出设定值与当前被控量之间的偏差
      double error = *mySetpoint - input;
	  // 计算积分项 此处积分项和标准PID控制方程略微有差距
      ITerm+= (ki * error);
	  // 如果 积分项超过最大限制，那么设置积分项为最大限制；同样，最小限制也做同样处理
	  // 此处为何这么做一句两句说不清楚，主要是为了PID 控制量长时间超限后，突然降低设定值，能够让系统马上反应而不会产生一个时间滞后。
      if(ITerm > outMax) ITerm= outMax;
      else if(ITerm < outMin) ITerm= outMin;

	  // 求出两个被控量之间偏差，也就是在计算周期（这里不用采用周期是因为计算周期可能会超过采样周期）被控量的变化。
	  // 其实就是微分项的 因子，但是看起来和标准表达式也不一样啊！！！
	  // 。。。。一两句也说不清楚，总的来说是为了防止控制量和被控量突变
      double dInput = (input - lastInput);
 
      /*Compute PID Output*/
	  // PID 调节算式，这就不需要说明了
      double output = kp * error + ITerm- kd * dInput;

      // 这里做限制和ITerm做限制的作用是一样的。。
	  if(output > outMax) output = outMax;
      else if(output < outMin) output = outMin;
	  *myOutput = output;
	  
      /*Remember some variables for next time*/
      lastInput = input;
      lastTime = now;
	  return true;
   }
   else return false;
}


/* SetTunings(...)*************************************************************
 * This function allows the controller's dynamic performance to be adjusted. 
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 * 此函数用于设定PID调节参数
 ******************************************************************************/ 
void PID::SetTunings(double Kp, double Ki, double Kd)
{
	// 如果PID参数中有小于0的参数，那么设定失败，直接退出，仍然沿用原来的参数
   if (Kp<0 || Ki<0 || Kd<0) return;
	// 仅做显示用。
   dispKp = Kp; dispKi = Ki; dispKd = Kd;
   
   // 获取采样时间，由ms转为s
   double SampleTimeInSec = ((double)SampleTime)/1000;  
   // 调整PID参数， I 和 D 参数的调节主要是为了满足采样周期改变带导致的影响，
   // 主要是 积分项和 微分项是和时间有关的参数，所以采样周期改变会导致这两项需要重新计算，这里为了减少这些工作，将采样周期变换转换我I D参数变化
   // 至于为什么可以这么做，是因为前面做了特殊处理，修改了PID标准表达式，使每一次计算对历史依赖较小
   kp = Kp;
   ki = Ki * SampleTimeInSec;
   kd = Kd / SampleTimeInSec;
 
	//  设定PID调节方向
  if(controllerDirection ==REVERSE)
   {
      kp = (0 - kp);
      ki = (0 - ki);
      kd = (0 - kd);
   }
}
  
/* SetSampleTime(...) *********************************************************
 * sets the period, in Milliseconds, at which the calculation is performed	
 ******************************************************************************/
//更新新的采样时间，同时按照比例更新ID参数
void PID::SetSampleTime(int NewSampleTime)
{
   if (NewSampleTime > 0)
   {
      double ratio  = (double)NewSampleTime
                      / (double)SampleTime;
      ki *= ratio;
      kd /= ratio;
      SampleTime = (unsigned long)NewSampleTime;
   }
}
 
/* SetOutputLimits(...)****************************************************
 *     This function will be used far more often than SetInputLimits.  while
 *  the input to the controller will generally be in the 0-1023 range (which is
 *  the default already,)  the output will be a little different.  maybe they'll
 *  be doing a time window and will need 0-8000 or something.  or maybe they'll
 *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 *  here.
 * 此函数容易产生控制量的突变，在运行过程中，尽量不要缩小范围
 **************************************************************************/
void PID::SetOutputLimits(double Min, double Max)
{
	// 赋值限制
   if(Min >= Max) return;
   outMin = Min;
   outMax = Max;
 
   if(inAuto)
   {
	   if(*myOutput > outMax) *myOutput = outMax;
	   else if(*myOutput < outMin) *myOutput = outMin;
	 
	   if(ITerm > outMax) ITerm= outMax;
	   else if(ITerm < outMin) ITerm= outMin;
   }
}

/* SetMode(...)****************************************************************
 * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 ******************************************************************************/ 
void PID::SetMode(int Mode)
{
    bool newAuto = (Mode == AUTOMATIC);
	// 如果模式不一样，那么则重新初始化
    if(newAuto == !inAuto)
    {  /*we just went from manual to auto*/
        PID::Initialize();
    }
    inAuto = newAuto;
}
 
/* Initialize()****************************************************************
 *	does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/ 
void PID::Initialize()
{
   ITerm = *myOutput;
   lastInput = *myInput;
   if(ITerm > outMax) ITerm = outMax;
   else if(ITerm < outMin) ITerm = outMin;
}

/* SetControllerDirection(...)*************************************************
 * The PID will either be connected to a DIRECT acting process (+Output leads 
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 ******************************************************************************/
void PID::SetControllerDirection(int Direction)
{
   if(inAuto && Direction !=controllerDirection)
   {
	  kp = (0 - kp);
      ki = (0 - ki);
      kd = (0 - kd);
   }   
   controllerDirection = Direction;
}

/* Status Funcions*************************************************************
 * Just because you set the Kp=-1 doesn't mean it actually happened.  these
 * functions query the internal state of the PID.  they're here for display 
 * purposes.  this are the functions the PID Front-end uses for example
 ******************************************************************************/
double PID::GetKp(){ return  dispKp; }
double PID::GetKi(){ return  dispKi;}
double PID::GetKd(){ return  dispKd;}
int PID::GetMode(){ return  inAuto ? AUTOMATIC : MANUAL;}
int PID::GetDirection(){ return controllerDirection;}

