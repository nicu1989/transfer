#include "PID.h"
#include <iostream>


/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */
#define SP 0
#define HUGE_A 1
#define H_ADJ 0.01
#define MEDIUM_A 0.6
#define M_ADJ 0.005
#define SMALL_A 0.2
#define S_ADJ 0.001
#define BIG_FREQ 20
#define FREQ_ADJ 0.05

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_) {
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */
  Kp = Kp_;
  Ki = Ki_;
  Kd = Kd_;
}

double PID::RunPID(double cte) {
  /**
   * TODO: Update PID errors based on cte.
   */
  double controlVariable = 0;
  p_error = 0;
  d_error = 0;
  i_error = 0;
  
  // proportional component
  p_error = SP - (cte * Kp);
  
  // derivative component
  double diff_cte = cte - prev_cte;
  prev_cte = cte;
  d_error = SP - (diff_cte * Kd);
  
  // integral component
  int_cte = int_cte + cte;
  i_error = SP - (int_cte * Ki);
  
  //sum of P,I and D outputs
  controlVariable = p_error + d_error + i_error;
  
  return controlVariable;
}

void PID::adjustP(double cte)
{
  static double minCTE = 0;
  static double maxCTE = 0;
  static double CTEAmplitude = 0;
  
  // get the minimum of the sin curve
  if (minCTE > cte)
  {
    minCTE = cte;
  }
  
  // get the maximum of the sin curve
  if (maxCTE < cte)
  {
    maxCTE = cte;
  }
  
  // We have a max and min for the sin, now we are close to 0 with the sin curve,
  // meaning that the sin is complete. Do the P adjustment based on the amplitude of the sin.
  if ((cte < 0.01 || cte > -0.01) && minCTE < -0.01 && maxCTE > 0.01)
  {
    // lets see how big the sin amplitude is
    CTEAmplitude = maxCTE - minCTE;

    maxCTE = 0;
    minCTE = 0;

    //if the amplitude is HUGE, do a huge substraction
    if(CTEAmplitude > HUGE_A)
    {
      Kp = Kp - H_ADJ; 
    }
  
    //if the amplitude is medium, do a medium substraction
    if (CTEAmplitude > MEDIUM_A)
    {
      Kp = Kp - M_ADJ;
    }
  
    //if the amplitude is small, do a small substraction
    if (CTEAmplitude > SMALL_A)
    {
      Kp = Kp - S_ADJ; 
    }
    
    std::cout<< "Adjusted P:" << Kp << std::endl; 
  }
}

void PID::adjustD(double cte)
{
  static int freqCounter = 0;
  static int sinCross = 0;
  
  // how many times do we cross the ideal trajectory axis
  if (cte < 0.01 || cte > -0.01)
  {
    sinCross++;
  }
  
  //have we cross it 2 times? ===> the sin is complete
  if (sinCross>=2)
  {
    sinCross = 0;
    
    //decrease the D parameter since we are completing the sin too fast
    if (freqCounter < BIG_FREQ)
    {
      Kd = Kd - FREQ_ADJ;
      std::cout<< "Adjusted D:" << Kd << std::endl; 
    }
    
  }          
  freqCounter++;
}

void PID::PrintParams(void)
{
  std::cout<< "Adjustment finished!" << std::endl; 
  std::cout<< "The parameter P is: " << Kp << std::endl; 
  std::cout<< "The parameter I is: " << Ki << std::endl; 
  std::cout<< "The parameter D is: " << Kd << std::endl; 
   
}