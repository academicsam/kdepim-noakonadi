// CalendarSystem abstract class, default derived kde gregorian class and factory class
// Provides support for different calendar types for kde calendar widget and related stuff
// Carlos Moro  <cfmoro@correo.uniovi.es>
// GNU-GPL v.2

#ifndef _CALSYSTEM
#define _CALSYSTEM

//#include <string>
#include <qdatetime.h>
#include <qstring.h>
#include <kled.h>


// Abstract base class
// Derived classes must be created through FactoryCalendar class
class KCalendarSystem
{
	public:
		// Default constructor, nothing
		KCalendarSystem() {}
		// Gets month name for date
		virtual QString getMonth(const QDate& date) = 0 ;
		// Gets year of date
		virtual int getYear(const QDate& date) = 0;
		// Gets day/month/year formatted date
		virtual QString getFormatDate(const QDate& date) = 0 ;
		// date + 1 month
		virtual void getNextMonthDate(QDate& date) = 0;
		// date -1 month
		virtual void getPreviousMonthDate(QDate& date) = 0;
		// date +1 year
		virtual void getNextYearDate(QDate& date) = 0;
		// date -1 year
		virtual void getPreviousYearDate(QDate& date) = 0;
		// Number of months in this year
		virtual int monthsInYear(int year) = 0;
		// Month name
		virtual QString getMonthName(int month) = 0;
		// Given a month for date in date's year, construct a valid date
		virtual void constructDateInMonth(QDate& date, int month) = 0;
		// Given a year, construct a valid date
		virtual void constructDateInYear(QDate& date, int year) = 0;
		// Convert day/month/year string to a valid date
		virtual QDate parseDate(QString text) = 0;
		// Short week day name
		virtual QString wDayName(int col) = 0;
		// Day of week number of date
		virtual int dayOfTheWeek(const QDate& date) = 0;
		// Lenght of the month
		virtual int numberOfDaysInMonth(const QDate& date) = 0;
		// Lenght of previous month
		virtual int numberOfDaysPrevMonth(const QDate& date) = 0;
		// Max limit of valid year supported (QDate, 8000)
		virtual int getMaxValidYear() = 0;
		// Get day for date
		virtual int getDay(const QDate& date) = 0;
		// Just for debug
		virtual void printType() = 0;
		// Default destructor
		virtual ~KCalendarSystem(){}
};


// Default derived, gregorian calendar class
class KCalendarGregorian : public KCalendarSystem
{
	public:
		KCalendarGregorian();
		QString getMonth(const QDate& date) ;
		int getYear(const QDate& date);
		QString getFormatDate(const QDate& date) ;
		void getNextMonthDate(QDate& date);
		void getPreviousMonthDate(QDate& date);
		void getNextYearDate(QDate& date);
		void getPreviousYearDate(QDate& date);
		int monthsInYear(int year);
		QString getMonthName(int month);
		void constructDateInMonth(QDate& date, int month);
	        void constructDateInYear(QDate& date, int year);
		QDate parseDate(QString text);
		QString wDayName(int col);
		int dayOfTheWeek(const QDate& date);
		int numberOfDaysInMonth(const QDate& date);
		int numberOfDaysPrevMonth(const QDate& date);
		int getMaxValidYear();
		int getDay(const QDate& date);
		void printType();
		virtual ~KCalendarGregorian();
};

// Factory class for calendar types
class KFactoryCalendar {

	private:
		// Supported calendar types
		static QString calTy[];

	public:
		// default constructor
		KFactoryCalendar();
		// Factory method for creating CalendarSystem specific classes
		static KCalendarSystem* createCalendar(const QString& calType = "gregorian");
		// default destructor
		~KFactoryCalendar();
};

#endif
// Best regards ;)
