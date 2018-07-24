#ifndef PID_v1_h
#define PID_v1_h
#define LIBRARY_VERSION	1.1.1

class PID
{
public:

	//Constants used in some of the functions below
	// 这里定义的两个变量分别指代两种工作模式：AUTOMATIC 对应 PID控制开启； MANUAL 对应PID控制关闭
	#define AUTOMATIC	1
	#define MANUAL	0
	// 这里定义两个变量分别指代控制量与被控量方向：DIRECT 对应两者同向； REVERSE 对应两者反向
	// 其中同向指： 如果控制量增大，那么被控量也会增大；反之亦然。
	// 其中反向指： 如果控制量增大，那么被控量缺减小；反之亦然。
	#define DIRECT  0
	#define REVERSE  1

	//commonly used functions **************************************************************************
	//构造函数
	PID(double*, double*, double*,        // * constructor.  links the PID to the Input, Output, and 
		double, double, double, int);     //   Setpoint.  Initial tuning parameters are also set here

	// 设置自动模式还是手动模式，两者区别目前还未清楚
	void SetMode(int Mode);               // * sets PID to either Manual (0) or Auto (non-0)

	// 计算PID， 在每个计算周期都应当调用 ，计算频率和是否计算可以在setMode和SetSampleTime中指定
	bool Compute();                       // * performs the PID calculation.  it should be
										  //   called every time loop() cycles. ON/OFF and
										  //   calculation frequency can be set using SetMode
										  //   SetSampleTime respectively

	//指定输出的范围，其中0-255，表示可限制的输出范围
	void SetOutputLimits(double, double); //clamps the output to a specific range. 0-255 by default, but
										  //it's likely the user will want to change this depending on
										  //the application



	//available but not commonly used functions ********************************************************
	// 设定P、I、D参数，可以在运行的时间周期内，指定运行需要的参数
	void SetTunings(double, double,       // * While most users will set the tunings once in the 
		double);         	  //   constructor, this function gives the user the option
							  //   of changing tunings during runtime for Adaptive control

	// 设定控制器的方向，限制输出的正反向，仅需要在开始的时候设置一次
	void SetControllerDirection(int);	  // * Sets the Direction, or "Action" of the controller. DIRECT
										  //   means the output will increase when error is positive. REVERSE
										  //   means the opposite.  it's very unlikely that this will be needed
										  //   once it is set in the constructor.

	// 采样周期，以毫秒作为设置单位，默认为10
	void SetSampleTime(int);              // * sets the frequency, in Milliseconds, with which 
										  //   the PID calculation is performed.  default is 100



	 //Display functions ****************************************************************
	// 获取PID运行参数
	double GetKp();						  // These functions query the pid for interal values.
	double GetKi();						  //  they were created mainly for the pid front-end,
	double GetKd();						  // where it's important to know what is actually 
	// 获取运行模式
	int GetMode();						  //  inside the PID.
	//获取PID 方向
	int GetDirection();					  //

private:
	// 此函数初始化，还不知什么用，需要参考CPP
	void Initialize();

	double dispKp;				// * we'll hold on to the tuning parameters in user-entered 
	double dispKi;				//   format for display purposes
	double dispKd;				//

	double kp;                  // * (P)roportional Tuning Parameter
	double ki;                  // * (I)ntegral Tuning Parameter
	double kd;                  // * (D)erivative Tuning Parameter

	int controllerDirection;

	// 其中包含了INput、 OUTput以及setPoint
	double *myInput;              // * Pointers to the Input, Output, and Setpoint variables
	double *myOutput;             //   This creates a hard link between the variables and the 
	double *mySetpoint;           //   PID, freeing the user from having to constantly tell us
								  //   what these values are.  with pointers we'll just know.
	// 此3个参数需要参考CPP才知道		  
	unsigned long lastTime;
	double ITerm, lastInput;

	unsigned long SampleTime;
	double outMin, outMax;
	// 是否自动参数的标志
	bool inAuto;
};
#endif
