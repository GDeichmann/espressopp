/****************************************************************************
*                                                                           *
*  Author      : Thomas Brandes, SCAI, FhG                                  *
*  Copyright   : SCAI, FhG, St. Augustin, Germany                           *
*                MPI-P, Mainz, Germany                                      *
*  Date        : Sep 08                                                     *
*  Last Update : Sep 08                                                     *
*                                                                           *
*  This file is part of the Espresso++ software                             *
*                                                                           *
*  Module      : log4espp.hpp                                               *
*                                                                           *
*  Function    : Macros for logging with log4cxx|log4cpp|default            *
*                                                                           *
****************************************************************************/

#ifndef LOG4ESPP_H
#define LOG4ESPP_H

#include "acconfig.hpp"

/************************************************************************
*                                                                       *
*  FLAGS for disable/enable LOGGING at compile time                     *
*                                                                       *
*    make sure that the desired logging levels are enabled              *
*                                                                       *
*    LOG4ESPP_DEBUG_ENABLED  :  LOG4ESPP_DEBUG is done                  *
*    LOG4ESPP_INFO_ENABLED   :  LOG4ESPP_INFO is done                   *
*                                                                       *
************************************************************************/

#if defined(LOG4ESPP_LEVEL_TRACE)

#define LOG4ESPP_TRACE_ENABLED
#define LOG4ESPP_DEBUG_ENABLED
#define LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_FATAL_ENABLED

#elif defined(LOG4ESPP_LEVEL_DEBUG)

#undef LOG4ESPP_TRACE_ENABLED
#define LOG4ESPP_DEBUG_ENABLED
#define LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_FATAL_ENABLED

#elif defined(LOG4ESPP_LEVEL_INFO)

#undef LOG4ESPP_DEBUG_ENABLED
#undef LOG4ESPP_TRACE_ENABLED
#define LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_FATAL_ENABLED

#elif defined(LOG4ESPP_LEVEL_WARN)

#undef LOG4ESPP_DEBUG_ENABLED
#undef LOG4ESPP_TRACE_ENABLED
#undef LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_FATAL_ENABLED

#elif defined(LOG4ESPP_LEVEL_ERROR)

#undef LOG4ESPP_DEBUG_ENABLED
#undef LOG4ESPP_TRACE_ENABLED
#undef LOG4ESPP_INFO_ENABLED
#undef LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_FATAL_ENABLED

#elif defined(LOG4ESPP_LEVEL_FATAL)

#undef LOG4ESPP_DEBUG_ENABLED
#undef LOG4ESPP_TRACE_ENABLED
#undef LOG4ESPP_INFO_ENABLED
#undef LOG4ESPP_WARN_ENABLED
#undef LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_FATAL_ENABLED

#elif defined(LOG4ESPP_LEVEL_OFF)

#undef LOG4ESPP_DEBUG_ENABLED
#undef LOG4ESPP_TRACE_ENABLED
#undef LOG4ESPP_INFO_ENABLED
#undef LOG4ESPP_WARN_ENABLED
#undef LOG4ESPP_ERROR_ENABLED
#undef LOG4ESPP_FATAL_ENABLED

#else

#define LOG4ESPP_DEBUG_ENABLED
#define LOG4ESPP_TRACE_ENABLED
#define LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_FATAL_ENABLED

#endif

/************************************************************************
*                                                                       *
*   LOG4ESPP   <======    LOG4CPP                                       *
*                                                                       *
************************************************************************/

#if defined(HAVE_LOG4CPP) and defined(LOG4ESPP_USE_LOG4CPP)

#include <stdio.h>
#include "log4cpp/Portability.hh"
#ifdef LOG4CPP_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <iostream>
#include <sstream>
#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#ifdef LOG4CPP_HAVE_SYSLOG
#include "log4cpp/SyslogAppender.hh"
#endif
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/NDC.hh"
#include <log4cpp/SimpleConfigurator.hh>
#include <log4cpp/BasicConfigurator.hh>

  /*******************************************************
  *   LOG4ESPP_DEFINITION                                *
  *******************************************************/

#define LOG4ESPP_DEFINITION() 

  /*******************************************************
  *   LOG4ESPP_CONFIGURE                                 *
  *******************************************************/

#define LOG4ESPP_CONFIGURE() { char *logFile; \
   logFile = getenv("LOG4ESPP"); \
   if (logFile != NULL) {\
      printf ("LOG4ESPP: LOG4CPP property configuration with file %s\n", logFile); \
      log4cpp::SimpleConfigurator::configure(logFile); \
     } \
     else { \
      printf ("LOG4ESPP: LOG4CPP basic configuration\n"); \
      log4cpp::BasicConfigurator::configure(); \
     } \
   }

#define LOG4ESPP_ROOTLOGGER(aLogger) log4cpp::Category& aLogger = log4cpp::Category::getRoot()

#define LOG4ESPP_DECL_LOGGER(aLogger) log4cpp::Category& aLogger;
#define LOG4ESPP_LOGGER(aLogger,name) log4cpp::Category& aLogger = log4cpp::Category::getInstance(std::string(name))

  /*******************************************************
  *   LOG4ESPP_XXXXX_ON                                  *
  *******************************************************/

#define LOG4ESPP_TRACE_ON(logger) (logger.isPriorityEnabled(log4cpp::Priority::DEBUG))
#define LOG4ESPP_DEBUG_ON(logger) (logger.isPriorityEnabled(log4cpp::Priority::DEBUG))
#define LOG4ESPP_INFO_ON(logger) (logger.isPriorityEnabled(log4cpp::Priority::INFO))
#define LOG4ESPP_WARN_ON(logger) (logger.isPriorityEnabled(log4cpp::Priority::WARN))
#define LOG4ESPP_ERROR_ON(logger) (logger.isPriorityEnabled(log4cpp::Priority::ERROR))
#define LOG4ESPP_FATAL_ON(logger) (logger.isPriorityEnabled(log4cpp::Priority::FATAL))

  /*******************************************************
  *   LOG4ESPP_TRACE                                     *
  *******************************************************/

#ifdef LOG4ESPP_TRACE_ENABLED
#define LOG4ESPP_TRACE(logger,msg) { if (logger.isPriorityEnabled(log4cpp::Priority::DEBUG)) \
                 { std::ostringstream omsg; omsg << msg; logger.debug(omsg.str()); }}
#else 
#define LOG4ESPP_TRACE(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_DEBUG                                     *
  *******************************************************/

#ifdef LOG4ESPP_DEBUG_ENABLED
#define LOG4ESPP_DEBUG(logger,msg) { if (logger.isPriorityEnabled(log4cpp::Priority::DEBUG)) \
                 { std::ostringstream omsg; omsg << msg; logger.debug(omsg.str()); }}
#else 
#define LOG4ESPP_DEBUG(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_INFO                                     *
  *******************************************************/

#ifdef LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_INFO(logger,msg) { if (logger.isPriorityEnabled(log4cpp::Priority::INFO)) \
                 { std::ostringstream omsg; omsg << msg; logger.info(omsg.str()); }}
#else 
#define LOG4ESPP_INFO(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_WARN                                     *
  *******************************************************/

#ifdef LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_WARN(logger,msg) { if (logger.isPriorityEnabled(log4cpp::Priority::WARN)) \
                 { std::ostringstream omsg; omsg << msg; logger.warn(omsg.str()); }}
#else 
#define LOG4ESPP_WARN(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_ERROR                                     *
  *******************************************************/

#ifdef LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_ERROR(logger,msg) { if (logger.isPriorityEnabled(log4cpp::Priority::ERROR)) \
                 { std::ostringstream omsg; omsg << msg; logger.error(omsg.str()); }}
#else 
#define LOG4ESPP_ERROR(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_FATAL                                     *
  *******************************************************/

#ifdef LOG4ESPP_FATAL_ENABLED
#define LOG4ESPP_FATAL(logger,msg) { if (logger.isPriorityEnabled(log4cpp::Priority::FATAL)) \
                 { std::ostringstream omsg; omsg << msg; logger.fatal(omsg.str()); }}
#else 
#define LOG4ESPP_FATAL(logger,msg)
#endif

#define LOG4ESPP_PUSH(string) log4cpp::NDC::push(string)
#define LOG4ESPP_POP() log4cpp::NDC::pop()

/************************************************************************
*                                                                       *
*   LOG4ESPP   <======    LOG4CXX                                       *
*                                                                       *
************************************************************************/

#elif defined(HAVE_LOG4CXX) and defined(LOG4ESPP_USE_LOG4CXX)

#include <log4cxx/logstring.h>
#include <stdlib.h>
#include <log4cxx/logger.h>
#include <log4cxx/defaultconfigurator.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/ndc.h>
#include <locale.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

  /*******************************************************
  *   LOG4ESPP_DEFINITION                                *
  *******************************************************/

#define LOG4ESPP_DEFINITION() 

  /*******************************************************
  *   LOG4ESPP_CONFIGURE                                 *
  *******************************************************/

#define LOG4ESPP_CONFIGURE() { char *logFile; \
   logFile = getenv("LOG4ESPP"); \
   if (logFile != NULL) {\
      printf ("LOG4ESPP: LOG4CXX property configuration with file %s\n", logFile); \
      PropertyConfigurator::configure(logFile); \
     } \
     else { \
      printf ("LOG4ESPP: LOG4CXX basic configuration\n"); \
      /* DefaultConfigurator::configure(LogManager::getLoggerRepository()); */ \
      BasicConfigurator::configure(); \
     } \
   }

  /*******************************************************
  *   LOG4ESPP_LOGGER                                    *
  *******************************************************/

#define LOG4ESPP_ROOTLOGGER(aLogger) log4cxx::LoggerPtr aLogger = Logger::getRootLogger()
#define LOG4ESPP_LOGGER(aLogger,name) log4cxx::LoggerPtr aLogger = log4cxx::Logger::getLogger(name)
#define LOG4ESPP_DECL_LOGGER(aLogger) log4cxx::LoggerPtr aLogger

  /*******************************************************
  *   LOG4ESPP_XXXXX_ON                                  *
  *******************************************************/

#define LOG4ESPP_TRACE_ON(logger)  (LOG4CXX_UNLIKELY(logger->isTraceEnabled()))
#define LOG4ESPP_DEBUG_ON(logger) (LOG4CXX_UNLIKELY(logger->isDebugEnabled()))
#define LOG4ESPP_INFO_ON(logger)  (LOG4CXX_UNLIKELY(logger->isInfoEnabled()))
#define LOG4ESPP_WARN_ON(logger)  (LOG4CXX_UNLIKELY(logger->isWarnEnabled()))
#define LOG4ESPP_ERROR_ON(logger)  (LOG4CXX_UNLIKELY(logger->isErrorEnabled()))
#define LOG4ESPP_FATAL_ON(logger)  (LOG4CXX_UNLIKELY(logger->isFatalEnabled()))

  /*******************************************************
  *   LOG4ESPP_DEBUG                                     *
  *******************************************************/

#ifdef LOG4ESPP_DEBUG_ENABLED
#define LOG4ESPP_DEBUG(logger,msg) LOG4CXX_DEBUG(logger,msg)
#else
#define LOG4ESPP_DEBUG(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_TRACE                                     *
  *******************************************************/

#ifdef LOG4ESPP_TRACE_ENABLED
#define LOG4ESPP_TRACE(logger,msg) LOG4CXX_TRACE(logger,msg)
#else
#define LOG4ESPP_TRACE(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_WARN                                     *
  *******************************************************/

#ifdef LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_WARN(logger,msg) LOG4CXX_WARN(logger,msg)
#else
#define LOG4ESPP_WARN(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_INFO                                     *
  *******************************************************/

#ifdef LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_INFO(logger,msg) LOG4CXX_INFO(logger,msg)
#else
#define LOG4ESPP_INFO(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_ERROR                                     *
  *******************************************************/

#ifdef LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_ERROR(logger,msg) LOG4CXX_ERROR(logger,msg)
#else
#define LOG4ESPP_ERROR(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_FATAL                                     *
  *******************************************************/

#ifdef LOG4ESPP_FATAL_ENABLED
#define LOG4ESPP_FATAL(logger,msg) LOG4CXX_FATAL(logger,msg)
#else
#define LOG4ESPP_FATAL(logger,msg)
#endif

#define LOG4ESPP_PUSH(string) NDC::push(string)
#define LOG4ESPP_POP()        NDC::pop()

#else 

/************************************************************************
*                                                                       *
*  Default Logger (if neither log4cxx nor log4cpp are available)        *
*                                                                       *
************************************************************************/

#include <iostream>
#include <ctype.h>
#include <string.h>   /* strncasecmp */
#include <stdlib.h>   /* getenv      */

class LogClass {
   public: static int logLevel;
};

  /*******************************************************
  *   LOG4ESPP_DEFINITION                                *
  *******************************************************/

#define LOG4ESPP_DEFINITION() int LogClass::logLevel = 2; 

#define LOG4ESPP_CONFIGURE() { char *logLevel; \
   printf ("configure logger\n"); \
   logLevel = getenv("LOG4ESPP"); \
   if (logLevel != NULL) { \
      printf ("logLevel = %s\n", logLevel); \
      if (strncasecmp(logLevel,"TRACE",3)==0) LogClass::logLevel = 6; \
      if (strncasecmp(logLevel,"DEBUG",3)==0) LogClass::logLevel = 5; \
      if (strncasecmp(logLevel,"INFO",3)==0) LogClass::logLevel = 4; \
      if (strncasecmp(logLevel,"WARN",3)==0) LogClass::logLevel = 3; \
      if (strncasecmp(logLevel,"ERROR",3)==0) LogClass::logLevel = 2; \
      if (strncasecmp(logLevel,"FATAL",3)==0) LogClass::logLevel = 1; \
      if (strncasecmp(logLevel,"OFF",3)==0) LogClass::logLevel = 0; \
    } else { \
      printf ("no logging level specified (use e.g. LOG4ESPP=DEBUG), take default WARN\n"); \
   } }

#define LOG4ESPP_ROOTLOGGER(aLogger) 
#define LOG4ESPP_LOGGER(aLogger,name) 
#define LOG4ESPP_DECL_LOGGER(aLogger) LogClass aLogger;

  /*******************************************************
  *   LOG4ESPP_XXXXX_ON                                  *
  *******************************************************/

#define LOG4ESPP_TRACE_ON(aLogger) (LogClass::logLevel >= 6)
#define LOG4ESPP_DEBUG_ON(aLogger) (LogClass::logLevel >= 5)
#define LOG4ESPP_INFO_ON(aLogger)  (LogClass::logLevel >= 4)
#define LOG4ESPP_WARN_ON(aLogger)  (LogClass::logLevel >= 3)
#define LOG4ESPP_ERROR_ON(aLogger) (LogClass::logLevel >= 2)
#define LOG4ESPP_FATAL_ON(aLogger) (LogClass::logLevel >= 1)

  /*******************************************************
  *   LOG4ESPP_TRACE                                     *
  *******************************************************/

#ifdef LOG4ESPP_TRACE_ENABLED
#define LOG4ESPP_TRACE(logger,msg) { if (LogClass::logLevel >= 6) \
			       std::cout << "DEBUG: " << msg << std::endl; }
#else
#define LOG4ESPP_TRACE(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_DEBUG                                     *
  *******************************************************/

#ifdef LOG4ESPP_DEBUG_ENABLED
#define LOG4ESPP_DEBUG(logger,msg) { if (LogClass::logLevel >= 5) \
			       std::cout << "DEBUG: " << msg << std::endl; }
#else
#define LOG4ESPP_DEBUG(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_INFO                                      *
  *******************************************************/

#ifdef LOG4ESPP_INFO_ENABLED
#define LOG4ESPP_INFO(logger,msg) { if (LogClass::logLevel >= 4) \
			       std::cout << "INFO: " << msg << std::endl; }
#else
#define LOG4ESPP_INFO(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_WARN                                      *
  *******************************************************/

#ifdef LOG4ESPP_WARN_ENABLED
#define LOG4ESPP_WARN(logger,msg) { if (LogClass::logLevel >= 3) \
			       std::cout << "WARN: " << msg << std::endl; }
#else
#define LOG4ESPP_WARN(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_ERROR                                     *
  *******************************************************/

#ifdef LOG4ESPP_ERROR_ENABLED
#define LOG4ESPP_ERROR(logger,msg) { if (LogClass::logLevel >= 2) \
			       std::cout << "ERROR: " << msg << std::endl; }
#else
#define LOG4ESPP_ERROR(logger,msg)
#endif

  /*******************************************************
  *   LOG4ESPP_FATAL                                     *
  *******************************************************/

#ifdef LOG4ESPP_FATAL_ENABLED
#define LOG4ESPP_FATAL(logger,msg) { if (LogClass::logLevel >= 1) \
			       std::cout << "FATAL: " << msg << std::endl; }
#else
#define LOG4ESPP_FATAL(logger,msg)
#endif


#endif

#endif

