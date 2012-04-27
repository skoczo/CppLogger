/** @mainpage package templates
* @authors Adrian Skoczylas
*
*#CppLogger
*<br/>
*<br/>
* Very simple (but powerful :) ) library to log infomations about program work to file.
* You can log two types of events (error and debug).
* When you log an error event this information was also log in debug file as error.<br/>
* Tested on Debian and Windows 7. Should work on all linux and windows systems.
*<br/>
*<br/>
* Example use:<br/>
* 	CppLogger::logDebug("log something to debug file");<br/>
* 	CppLogger::logError("log something to error and debug file");<br/>
* 	<br/>
* 	<br/>
* 	Log file format:
* 	- log type [DEBUG] or [ERROR]
* 	- time, example: 2012-4-27 19:16:27
* 	- message
* 	<br/><br/>
*
* Example log lines:
* 	- [DEBUG] 2012-4-27 19:16:27: log something to debug file
* 	- [ERROR] 2012-4-27 19:16:27: log something to error and debug file
*/
