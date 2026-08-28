#ifndef __LOGGER_LIB_LOGGER_HPP__
#define __LOGGER_LIB_LOGGER_HPP__

#include <ostream>
#include <memory>
#include <iostream>

/* ----- Logging modes ----- */

#define LOGGING_DEBUG 0
#define LOGGING_RELEASE 1

/* ----- Logging mod check ----- */

#ifdef LOGGING_DEFAULT
	#define LOGGING_MODE LOGGING_RELEASE
#endif

#ifndef LOGGER_LIB_CPP_SOURCE
	#ifndef LOGGING_MODE
		#error "You must define LOGGING_MODE: LOGGING_DEBUG or LOGGING_RELEASE"
	#endif
#endif

/* ----- Default instance name ---- */

#ifndef LOGGER_INSTANCE
	#define LOGGER_INSTANCE logger
#endif

/* ----- Public functions for user ----- */

// Имя текущего файла без пути до него
#define FILE_NAME loggerlib::extractFileName(std::string_view(__FILE__))

#if defined(LOGGING_MODE) && (LOGGING_MODE == LOGGING_DEBUG)

	#define FUNC_DEBUG()\
		loggerlib::Logger::FuncChecker funcChecker(\
			LOGGER_INSTANCE,\
			FILE_NAME,\
			std::string_view(__func__),\
			__LINE__\
		)

	#define LOG_DEBUG(msg)\
		LOGGER_INSTANCE.debug(\
			msg,\
			FILE_NAME,\
			std::string_view(__func__),\
			__LINE__\
		)

	#define LOG_INFO(msg)\
		LOGGER_INSTANCE.info(\
			msg,\
			FILE_NAME,\
			std::string_view(__func__),\
			__LINE__\
		)

	#define LOG_EXCEPTION(msg)\
		LOGGER_INSTANCE.exception(\
			msg,\
			FILE_NAME,\
			std::string_view(__func__),\
			__LINE__\
		)

	#define LOG_WARNING(msg)\
		LOGGER_INSTANCE.warning(\
			msg,\
			FILE_NAME,\
			std::string_view(__func__),\
			__LINE__\
		)

	#define LOG_FATAL(msg)\
		LOGGER_INSTANCE.fatal(\
			msg,\
			FILE_NAME,\
			std::string_view(__func__),\
			__LINE__\
		)

#endif

#if defined(LOGGING_MODE) && (LOGGING_MODE == LOGGING_RELEASE)

	#define LOG_DEBUG(msg) NULL
	#define LOG_INFO(msg) LOGGER_INSTANCE.info(msg)
	#define LOG_EXCEPTION(msg) LOGGER_INSTANCE.exception(msg)
	#define LOG_WARNING(msg) LOGGER_INSTANCE.warning(msg)
	#define LOG_FATAL(msg) LOGGER_INSTANCE.fatal(msg)

#endif

/* ----- Namespace ----- */

namespace loggerlib {

	enum class LoggerParam : uint32_t{
		none = 0,
		useExceptions = 1 << 0,
		useFlush = 1 << 1
	};

	/// @brief Вернуть имя файла из пути до этого файла
	/// @param filePath Путь до файла
	/// @return Имя файла без пути до этого файла
	constexpr std::string_view extractFileName(
		std::string_view filePath
	) {
		size_t pos = filePath.size();
		while (pos > 0) {
			pos--;

			if (filePath[pos] == '/' || filePath[pos] == '\\') {
				return filePath.substr(pos + 1);
			}
		}

		return filePath;
	}

	constexpr LoggerParam operator | (LoggerParam lhs, LoggerParam rhs) {
		using T = std::underlying_type_t<LoggerParam>;
		return static_cast<LoggerParam>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	constexpr LoggerParam operator & (LoggerParam lhs, LoggerParam rhs) {
		using T = std::underlying_type_t<LoggerParam>;
		return static_cast<LoggerParam>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}

	constexpr LoggerParam& operator |= (LoggerParam& lhs, LoggerParam rhs) {
		lhs = lhs | rhs;
		return lhs;
	}

	class Logger {
	private:

		class Impl;
		std::unique_ptr<Impl> m_impl;

	public:
		Logger();
		~Logger();

		/// @brief Установить данные файла для записи логов
		/// @param filePath Путь до файла
		/// @return Возможен выброс std::runtime_error
		void setLogFile(
			const std::string& filePath
		);

		/// @brief Установить данные потока вывода для логов
		/// @param out Указатель на поток вывода
		/// @return Возможен выброс std::runtime_error
		void setOstream(
			std::ostream* ostream
		);

		void addParam(
			LoggerParam param
		);

		void removeparam(
			LoggerParam param
		);

		bool hasParam(
			LoggerParam param
		) const;

	#if (defined(LOGGING_MODE) && (LOGGING_MODE == LOGGING_DEBUG)) || defined(LOGGER_LIB_CPP_SOURCE)

		class FuncChecker {
		private:
			class Impl;
			std::unique_ptr<Impl> m_impl;

		public:

			FuncChecker(
				const Logger& instance,
				std::string_view currentFileName,
				std::string_view currentFuncName,
				int32_t currentCodeLine
			);

			~FuncChecker();

		};

		void debugStartFunc(
			std::string_view currentFileName,
			std::string_view currentFuncName,
			int32_t currentCodeLine
		) const;

		void debugStopFunc(
			std::string_view currentFileName,
			std::string_view currentFuncName,
			int32_t currentCodeLine
		) const;

		void debug(
			std::string_view msg,
			std::string_view curentFileName,
			std::string_view currentFuncName,
			int32_t currentCodeLine
		);

		void info(
			std::string_view msg,
			std::string_view currentFileName,
			std::string_view currentFuncName,
			int32_t currentCodeLine
		) const ;

		void exception(
			std::string_view msg,
			std::string_view currentFileName,
			std::string_view currentFuncName,
			int32_t currentCodeLine
		);

		void warning(
			std::string_view msg,
			std::string_view currentFileName,
			std::string_view currentFuncName,
			int32_t currentCodeLine
		);

		void fatal(
			std::string_view msg,
			std::string_view currentFileName,
			std::string_view currentFuncName,
			int32_t currentCodeLine
		);

	#endif

	#if (defined(LOGGING_MODE) && (LOGGING_MODE == LOGGING_RELEASE)) || defined(LOGGER_LIB_CPP_SOURCE)

		void info(
			std::string_view msg
		);

		void exception(
			std::string_view msg
		);

		void warning(
			std::string_view msg
		);

		void fatal(
			std::string_view msg
		);

	#endif

	};
}

/* ----- END ----- */

#endif // __LOGGER_LIB_LOGGER_HPP__
