//  T0, Tmax, and ADC fit functions from Athena
Double_t mt_t0_fermi(Double_t *x , Double_t *par);
Double_t mt_tmax_fermi(Double_t *x, Double_t *par);
Double_t skewnormal(Double_t *x, Double_t *par);

// T0 fit function
//  par[0] = T0
//  par[1] = T0 slope
//  par[2] = background
//  par[0] = amplitude
Double_t mt_t0_fermi(Double_t *x , Double_t *par) {
  const Double_t &t(x[0]), &t_0(par[0]), &T(par[1]), &back(par[2]),&A(par[3]);
  return (back + A/(1+exp(-(t-t_0)/T)));
 //  return (par[2] + par[3]/(1+exp(-(x[0]-par[0])/par[1])));
}	

// MTT0 Tmax fit function:
//  par[0] = Tmax
//  par[1] = Tmax slope
//  par[2] = background
//  par[3] = A
//  par[4] = B
//  par[5] = T0
Double_t mt_tmax_fermi(Double_t *x, Double_t *par) {
  Double_t &t(x[0]);
  Double_t &t_max(par[0]), &T(par[1]), &back(par[2]), &a(par[3]), &b(par[4]), &t_0(par[5]);
  return (back + (exp(a+b*(t-t_0)))/(1+exp((t-t_max)/T)));
  //  return (par[2] + (exp(par[4]+par[3]*(x[0]-par[5])))/(1+exp((x[0]-par[0])/par[1])));
}
// Skewnormal distribution from MTT0
Double_t skewnormal(Double_t *x, Double_t *par) {
//par[0] = skew gauss norm
//par[1] = skew gauss mean (i.e. mu)
//par[2] = skew gauss sigma (i.e sigma)
//par[3] = skew factor  (i.e. alpha)
// Numeric constants - hardcode these to improve speed
//  invsq2pi = 0.3989422804014327    //1/sqrt(2*pi)
//  twoPi    = 6.2831853071795862
  Double_t delta_value  = par[3]/(TMath::Sqrt(1.+par[3]*par[3]));
  Double_t omega_square = (par[2]*par[2])/(1. - delta_value*delta_value*0.63661977236758137);  //0.63.. = 4/twopi
  Double_t omega_value  = TMath::Sqrt(omega_square);
  Double_t xi_value     = par[1] - delta_value*omega_value*0.7978845608028654;   //*0.79..=2*invsq2pi
  Double_t Gauss_part   = (0.3989422804014327/omega_value)*TMath::Exp(-((x[0] - xi_value)*(x[0] - xi_value))/(2.0*omega_square));  //phi(x)
  Double_t Erf_part     = 1. + TMath::Erf(par[3]*(x[0]-xi_value)/omega_value);
  return par[0]*Gauss_part*Erf_part;
}

