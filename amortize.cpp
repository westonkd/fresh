/***********************************************************************
 * Program:
 *    Exploration 2, Freshness
 *    Brother Neff, CS 238
 * Author:
 *    Weston Dransfield
 *    Gage Peterson
 * Summary:
 *    Amortization Calculating Program
 *    Exploring a practical recurrence relation.
 *
 ***********************************************************************/
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>
using namespace std;

#include "properties.h"

/***********************************************************************
 * See, for example:
 * http://www.bankrate.com/calculators/mortgages/mortgage-calculator.aspx
 ***********************************************************************/
class Amortize
{
   private:
      int mTermInMonths;              // term in months (n)

      double mRate;                   // annual interest rate (r)

      double mPeriodicRate;           // periodic (monthly) rate (i)

      double mPrincipal;              // principal (p)

      double mMonthlyPayment;         // monthly payment (m)

      double mExtraMonthlyPayment;    // extra monthly payment (x)

      bool mShowAmortizationSchedule; // verbose (v)

      int mStart;                     // extra payment starting month number (s)

      int mEnd;                       // extra payment ending month number (e)

      bool mHavePrincipal;            // p

      bool mHavePeriodicRate;         // r

      bool mHaveTermInMonths;         // n

      bool mHaveMonthlyPayment;       // m

      double mInitialGuess;

      /***********************************************************************
       * Find p given i, m, n.
       ***********************************************************************/
      void findPrincipal();

      /***********************************************************************
       * Find i given p, m, n.
       **********************************************************************/
      void findPeriodicRate();

      /***********************************************************************
       * Find m given p, i, n.
       **********************************************************************/
      void findMonthlyPayment();

      /***********************************************************************
       * Find n given p, i, m.
       **********************************************************************/
      void findTermInMonths();

      /***********************************************************************
       * Helper function to get the  principal to avoid duplicating code.
       **********************************************************************/
      double getPrincipal(double i, double m, double n);

      /***********************************************************************
       * Find all missing parameters.
       **********************************************************************/
      bool findAll()
      {
         if (!mHavePrincipal && mHaveMonthlyPayment &&
               mHavePeriodicRate && mHaveTermInMonths)
         {
            findPrincipal();
            mHavePrincipal = true;
         }

         if (!mHavePeriodicRate && mHavePrincipal &&
               mHaveMonthlyPayment && mHaveTermInMonths)
         {
            findPeriodicRate();
            mHavePeriodicRate = true;
         }

         if (!mHaveTermInMonths && mHavePrincipal &&
               mHaveMonthlyPayment && mHavePeriodicRate)
         {
            findTermInMonths();
            mHaveTermInMonths = true;
         }

         if (!mHaveMonthlyPayment && mHavePrincipal &&
               mHavePeriodicRate && mHaveTermInMonths)
         {
            findMonthlyPayment();
            mHaveMonthlyPayment = true;
         }

         if (mHavePrincipal && mHaveMonthlyPayment &&
               mHavePeriodicRate && !mHaveTermInMonths)
         {
            mHaveTermInMonths = true;
         }

         return (mHavePrincipal && mHaveMonthlyPayment &&
               mHavePeriodicRate && mHaveTermInMonths);
      }

      /****************************************************************
       * Check information for consistency:
       *
       * (E.g., the monthly payment as given should equal
       *  the monthly payment as calculated, and if
       *  the start month has been given but not the end month,
       *  set the end month to the term in months.
       *
       * Called after findAll() in run().
       ****************************************************************/
      bool checkConsistency()
      {
         double monthlyPayment = mMonthlyPayment;
         // calculate monthly payment to make sure it's consistent,
         // as it could have just been given, but not yet calculated.
         findMonthlyPayment();
         if (abs(mMonthlyPayment - monthlyPayment) > 0.001)
         {
            // not close enough, we have a problem...
            return false;
         }
         else
         {
            if (mStart > 0 && mEnd == 0)
            {
               mEnd = mTermInMonths;
            }
            return true;
         }
      }

      /****************************************************************
       * Displays the month, interest paid, principal paid and balance.
       *
       * Called when displaying the amortization schedule.
       ****************************************************************/
      void showAmortizationLine(int month, double interestPaid,
            double principalPaid,
            double balance)
      {
         cout << setw(5)  << month
            << setw(12) << interestPaid
            << setw(12) << principalPaid
            << setw(16) << balance
            << endl;
      }

      /****************************************************************
       * When the balance goes negative, correctly adjust the
       * principal paid and the total paid.
       *
       * Return true if the balance did go negative, false otherwise.
       *
       * Called when iterating through the amortization calculations.
       ****************************************************************/
      bool balanceGoesNegative(double balance, double &principalPaid,
            double &totalPaid)
      {
         if (balance < 0)
         {
            principalPaid += balance;
            totalPaid += (mMonthlyPayment + balance);
            return true;
         }
         else
         {
            return false;
         }
      }

   public:
      Amortize();

      void display();

      void run()
      {
         if (findAll() && checkConsistency())
         {
            display();
         }
         else
         {
            cerr << "Bad or missing information.\n";
         }
      }
};

/****************************************************************
 * Default Constructor
 * Intializes the user-specified values and boolean indicators
 ****************************************************************/
Amortize::Amortize() 
{
   //Initialize member variables

   //get 'p' as the principal
   mPrincipal = atof(System.getProperty("p", "-1.0").c_str());
   mHavePrincipal = mPrincipal != -1.0;

   //get 'r' as the APR. If it is specified divide it by 12
   //to get the periodic rate
   mRate = atof(System.getProperty("r", "-1.0").c_str());
   if (mRate > 0)
   {
      //mRate /= 100;
      mPeriodicRate = mRate / 1200;
      mHavePeriodicRate = true;
   }

   //get 'n' as the term in months
   mTermInMonths = atof(System.getProperty("n", "-1.0").c_str());
   mHaveTermInMonths = mTermInMonths != -1.0;

   //get 'm' as the monthly payment.
   mMonthlyPayment = atof(System.getProperty("m", "-1.0").c_str());
   mHaveMonthlyPayment = mMonthlyPayment != -1.0; 

   //get 'v' to check for verbose mode
   mShowAmortizationSchedule = (System.getProperty("v", "false")  == "false") ? false : true;

   //check for extra monthly payments (x)
   mExtraMonthlyPayment = atof(System.getProperty("x", "-1.0").c_str());

   //check for start date
   mStart = atoi(System.getProperty("s", "-1").c_str());

   //check for an end date
   mEnd = atoi(System.getProperty("e", "0").c_str());

}

/****************************************************************
 * Find principal given monthly rate, monthly payments, and term 
 * in months.
 ****************************************************************/
double Amortize::getPrincipal(double i, double m, double n)
{
   double temp = 1 - pow(1 + i, -n);
   return temp * (m / i);
}

void Amortize::findPrincipal()
{
   mPrincipal = getPrincipal(mPeriodicRate, mMonthlyPayment, mTermInMonths);
   mHavePrincipal = true;
}

/****************************************************************
 * Find i given p, m, n. using Newton's Method
 ****************************************************************/
void Amortize::findPeriodicRate()
{
   //start our guess at 10%
   double guess = .1;
   double principalCheck = getPrincipal(guess, mMonthlyPayment, mTermInMonths);

   //stop checking when the current guess is within an acceptable range.
   while (!(principalCheck < mPrincipal + .001 && principalCheck > mPrincipal - .001))
   {
      double temp = mMonthlyPayment - mMonthlyPayment * pow(1 + guess, mTermInMonths * -1) - guess * mPrincipal;
      temp /= mTermInMonths * pow(1 + guess, mTermInMonths * -1 - 1) - mPrincipal;

      guess = guess - temp;
      principalCheck = getPrincipal(guess, mMonthlyPayment, mTermInMonths);
   }

   //set the rates
   mPeriodicRate = guess;
   mRate = guess * 12;

   mHavePeriodicRate = true;
}

/****************************************************************
 * Find term in months given principal, interest rate, and monthly
 * payment.
 ****************************************************************/
void Amortize::findTermInMonths()
{
   double temp = mPrincipal - mMonthlyPayment / mPeriodicRate;
   temp /= -1 * mMonthlyPayment / mPeriodicRate;
   mTermInMonths = -1 * log(temp) / log(1 + mPeriodicRate) + 1;
}

/****************************************************************
 * Find m given p, i, n
 ****************************************************************/
void Amortize::findMonthlyPayment()
{
   double temp = mPeriodicRate / (1 - pow(1 + mPeriodicRate, -1 * mTermInMonths));
   mMonthlyPayment = temp * mPrincipal;
}

// DO NOT CHANGE ANYTHING BELOW THIS LINE!

/****************************************************************
 * Output the details of the principal, term, annual rate,
 * periodic rate, monthly payment, and extra payment (if any).
 *
 * Optionally display the amortization schedule.
 ****************************************************************/
void Amortize::display()
{
   double totalInterestPaid = 0.0;
   double totalPaid = 0.0;
   double totalExtra = 0.0;
   double totalSavings = 0.0;
   double balance = mPrincipal;
   cout << fixed << endl;
   cout << "##############################################\n\n";
   cout << "      Principal: " << setw(8) << setprecision(2) << mPrincipal
      << endl;
   cout << "           Term: " << mTermInMonths << " months\n";
   cout << "    Annual Rate: " << setprecision(3) << mRate << "%\n";
   cout << "  Periodic Rate: " << setprecision(7) << mPeriodicRate << endl;
   cout << endl;
   cout << "Monthly Payment: " << setw(8) << setprecision(2) << mMonthlyPayment
      << endl;
   if (mExtraMonthlyPayment > 0)
   {
      cout << "  Extra Payment: " << setw(8) << mExtraMonthlyPayment << endl;
   }
   cout << endl;

   if (mShowAmortizationSchedule)
   {
      cout << "Month     Interest   Principal       Balance\n";
   }

   int month;
   for (month = 1; month <= mTermInMonths; month++)
   {
      double interestPaid = balance * mPeriodicRate;
      totalInterestPaid += interestPaid;
      double principalPaid = mMonthlyPayment - interestPaid;
      balance -= principalPaid;

      if (balanceGoesNegative(balance, principalPaid, totalPaid))
      {
         if (mShowAmortizationSchedule)
         {
            showAmortizationLine(month, interestPaid, principalPaid, 0.0);
         }
         break;
      }

      totalPaid += mMonthlyPayment;

      if (month >= mStart && month <= mEnd)
      {
         balance -= mExtraMonthlyPayment;
         principalPaid += mExtraMonthlyPayment;
         totalExtra += mExtraMonthlyPayment;
      }
      if (mShowAmortizationSchedule)
      {
         showAmortizationLine(month, interestPaid, principalPaid, balance);
      }
   }

   if (mShowAmortizationSchedule)
   {
      cout << endl;
   }

   if (totalExtra > 0)
   {
      cout << "   Reduced Term: " << month << " months (shorter by "
           << (mTermInMonths - month) << " month(s) = "
           << (mTermInMonths - month) / 12 << " year(s) "
           << (mTermInMonths - month) % 12 << " month(s))\n";
   }

   cout << " Total Payments:" << setw(10) << totalPaid + totalExtra << endl;

   if (totalExtra > 0)
   {
      cout << " Extra Payments:" << setw(10) << totalExtra << endl;
   }

   cout << " Total Interest:" << setw(10)
        << totalInterestPaid << endl;

   if (totalExtra > 0)
   {
      totalSavings =
         ((mTermInMonths * mMonthlyPayment) - totalPaid - totalExtra);
      cout << "  Total Savings:" << setw(10)
           << totalSavings << endl;
   }

   cout << "  Intrst/Prncpl:" << setw(7)
        << (totalInterestPaid / mPrincipal * 100)
        << "%\n";
}

/****************************************************************
 * Main reads command-line argument(s) as follows:
 *
 *  Name of a file containing parameter settings, or
 *  (3 of the following 4 parameters are required)
 *
 *      p= principal (amount loaned)
 *      n= term in months (number of payments)
 *      r= annual interest rate (in percent, e.g. 3.75)
 *      m= monthly payment amount
 *
 *      v= (optional) 1 = view amortization schedule
 *      x= (optional) extra monthly payment
 *      s= (optional) month extra payment starts
 *      e= (optional) month extra payment ends (default = n)
 ****************************************************************/
int main(int argc, char* argv[])
{
   if (argc <= 1)
   {
      cout << "Usage: " << argv[0] << " <filename> or <parameters>" << endl;
      return 0;
   }

   if (argc == 2)
   {
      const char* filename = argv[1];
      string data;
      ifstream ifs(filename, ios_base::in);

      if (ifs)
      {
         while (true)
         {
            ifs >> data;

            if (ifs.eof() || ifs.fail())
            {
               break;
            }
            System.setProperty(data);
         }
         ifs.close();
      }
   }
   else
   {
      for (int i = 1; i < argc; i++)
      {
         string data = argv[i];
         System.setProperty(data);
      }
   }

   Amortize().run();

   return 0;
}
