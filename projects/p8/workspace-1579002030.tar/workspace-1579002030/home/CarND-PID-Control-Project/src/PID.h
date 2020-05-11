#ifndef PID_H
#define PID_H
#include <vector>

class PID {
 public:
  /**
   * Constructor
   */
  PID();

  /**
   * Destructor.
   */
  virtual ~PID();

  /**
   * Initialize PID.
   * @param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_);

  /**
   * Update the PID error variables given cross track error.
   * @param cte The current cross track error
   * @output the control variable(steer value)
   */
  double RunPID(double cte);

  /**
   * Adjust the Kp parameter based on cte with the purpose
   * of reducing the sin wave trajectory amplitude.
   */  
  void adjustP(double cte);
  
  /**
   * Adjust the Kd parameter based on cte with the purpose
   * of reducing the sin wave trajectory frequency.
   */
  void adjustD(double cte);
  
  /**
   * Prints the values of Kp, Ki and Kd once the online 
   * adjustment had finnished.
   */
  void PrintParams(void);

 private:
  /**
   * PID Errors
   */
  double p_error;
  double i_error;
  double d_error;

  /**
   * PID Coefficients
   */ 
  double Kp;
  double Ki;
  double Kd;
  
   /**
   * PID output
   */ 
  double prev_cte;
  double int_cte;
};

#endif  // PID_H