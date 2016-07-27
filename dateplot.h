//********************************************************************
// Plot data by day of year
//********************************************************************
#include <stdio.h>
#include <ctype.h>

//********************************************************************
// Return day of year or month from a date string
//  date = Date string in format YYYYMMDD, YYYY-MM-DD, or DD-MMM-YYYY
//  iflag = 0 return day of year; Jan 1 = day 1
//  iflag = 1 return month; Jan = month 0 
//********************************************************************
double getday( char *date, int iflag=0 );

// Number of chambers in each wheel
double monthedges[]  =  { 1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
int monthlen[]  =    { 31,28,31,30,31,30,31,31,30,31,30 };

// Labels for eta regions of wheels
const char *monthlabels[] = 
  {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec",""};

#ifndef dateplot_h
#define dateplot_h

//  Get day of year from date in format YYYY-MM-DD, YYYYMMDD, or DD-MMM-YYYY
//                                      2011-03-23, 20110323, or 23-Mar-2011 
double getday( char *date, int iflag ) {
  char MM[3],DD[3];
  int imm=0;

  if( date[2] == '-' ) {      //DD-MMM-YYYY
    sprintf(DD,"%c%c",date[0],date[1]);
    for( int i=0; i<12; i++ ) {
      if( toupper(date[3]) == monthlabels[i][0] && 
	  tolower(date[4]) == monthlabels[i][1] && 
	  tolower(date[5]) == monthlabels[i][2] ) {
        imm = i;
	break;
      }
    }
  } else if( date[4] == '-' ) { //YYYY-MM-DD
    sprintf(MM,"%c%c",date[5],date[6]);
    sprintf(DD,"%c%c",date[8],date[9]);
    imm = atoi(MM) - 1;
  } else {                    //YYYYMMDD
    sprintf(MM,"%c%c",date[4],date[5]);
    sprintf(DD,"%c%c",date[6],date[7]);
    imm = atoi(MM) - 1;
  }
  double dd = atof(DD) - 1.;

  if( iflag == 1 ) return (double) imm;
  return monthedges[imm]+dd;
}
#endif
