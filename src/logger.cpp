#include <iostream>
#include <string_view>
#include <fstream>
#include <mutex>
#include <fmt/format.h>

#include "ts_file_handler.hpp"
#include "ostream_handler.hpp"
#include "status.hpp"

#define LOGGER_LIB_CPP_SOURCE

#include "LoggerLib/logger.hpp"

/* ----- Macro ----- */

#define DELETE_CONSTRUCTORS(objectName)\
	objectName() = delete;\
	objectName(const objectName&) = delete;\
	objectName& operator=(const objectName&) = delete

/* ----- Alias ----- */

using Logger = loggerlib::Logger;

using Status = loggerlib::detail::Status;
using TSFileWriter = loggerlib::detail::TSFileWriter;
using OstreamHandler = loggerlib::detail::OstreamHandler;
using LoggerParam = loggerlib::LoggerParam;

/* ----- Consts ----- */

namespace {
	namespace LogFormat {
		// log line | Log type | File name | Function name | Code line | Message
		inline constexpr std::string_view debug = "{:<5} | {:<9} | {:<30} | {:<30} | {:<5} | {}\n";

		// Log line | Log type | message
		inline constexpr std::string_view release = "{:<5} | {:<9} | {}\n";

		// log type | exception
		inline constexpr std::string_view loggerException = " ---- | {:<9} | {}\n";
	};

	namespace LogTypeString {
		inline constexpr std::string_view debug = "DEBUG";
		inline constexpr std::string_view info = "INFO";
		inline constexpr std::string_view exception = "EXCEPTION";
		inline constexpr std::string_view warning = "WARNING";
		inline constexpr std::string_view fatal = "FATAL";
	};

	namespace MsgFormat {
		// Cannot open log file "{file_path}": {exception_message}
		inline constexpr std::string_view cantOpenFile = "Cannot open file \"{}\"";

		// Cannot write to a file log "{file_path}": {exception_message}
		inline constexpr std::string_view cantWriteToFile = "Cannot write to file \"{}\" log:\n{}";

		// Cannot flush file "{file_path}"
		inline constexpr std::string_view cantFlush = "Cannot flush file \"{}\"";
	};

	namespace Msg {
		inline constexpr std::string_view ostreamCantBeNUll = "ostream cannot be nullptr or NULL";

		inline constexpr std::string_view startFunc = "START function";
		inline constexpr std::string_view stopFunc = "STOP function";
	};
}

/* ----- class LogFormatter ----- */

class LogFormatter {
public: // Публичные методы

	DELETE_CONSTRUCTORS(LogFormatter);

public: // Публичные статические методы

	static std::string formatDebugLog(
		uint32_t logLine,
		std::string_view logTypeStr,
		std::string_view fileName,
		std::string_view funcName,
		uint32_t codeLine,
		std::string_view msg
	) {
		return fmt::format(
			LogFormat::debug,
			logLine,
			logTypeStr,
			fileName,
			funcName,
			codeLine,
			msg
		);
	}

	static std::string formatReleaseLog(
		uint32_t logLine,
		std::string_view logTypeStr,
		std::string_view msg
	) {
		return fmt::format(
			LogFormat::release,
			logLine,
			logTypeStr,
			msg
		);
	}

	static std::string formatLoggerExceptionLog(
		std::string_view logTypeStr,
		std::string_view msg
	) {
		return fmt::format(
			LogFormat::loggerException,
			logTypeStr,
			msg
		);
	}

};

/* ----- class MsgFormater ----- */

class MsgFormater {
public: // Публичные методы

	DELETE_CONSTRUCTORS(MsgFormater);

public: // Публичные статические методы

	static std::string formatMsgCantOpenFile(
		std::string_view filePath
	) {
		return fmt::format(
			MsgFormat::cantOpenFile,
			filePath
		);
	}

	static std::string formatMsgCantWriteFile(
		std::string_view filePath,
		std::string_view log
	) {
		return fmt::format(
			MsgFormat::cantWriteToFile,
			filePath,
			log
		);
	}

	static std::string formatMsgCantFlush(
		std::string_view filePath
	) {
		return fmt::format(
			MsgFormat::cantFlush,
			filePath
		);
	}
};

/* ----- class Logger::Impl ----- */

class Logger::Impl {
private: // Приватные Вспомогательные структуры
	struct FileData {
		bool is_used;
		std::ofstream file;
		std::string filePath;
	};

	struct OstreamData {
		bool is_used;
		std::ostream* ostreamPtr;
	};

private: // Приватные внутренние поля

	std::unique_ptr<TSFileWriter> m_fileHandler;
  	std::unique_ptr<OstreamHandler> m_ostreamHandler;

	int32_t m_logLine;
	LoggerParam m_params;

	FileData m_fileData;
	OstreamData m_ostreamData;

public: // Публичные методы класса

	Impl() {
		this->m_fileHandler = std::make_unique<TSFileWriter>();
		this->m_ostreamHandler = std::make_unique<OstreamHandler>();

		this->m_fileData.is_used = false;
		this->m_ostreamData.is_used = false;

		this->m_params = LoggerParam::none;
		this->m_logLine = 1;
	}

	~Impl() = default;

	/// @brief Установить данные потока вывода для логов
	/// @param out Указатель на поток вывода
	/// @return Возможен выброс std::runtime_error
	void setOstream(
		std::ostream* out
	) {
		if (out == nullptr || out == NULL) {
			std::string exceptionMsg = LogFormatter::formatLoggerExceptionLog(
				LogTypeString::fatal,
				Msg::ostreamCantBeNUll
			);

			this->m_forgetOstream();

			this->m_ostreamHandler->cerr(
				exceptionMsg
			);

			if (this->hasParam(LoggerParam::useExceptions)) {
				throw std::runtime_error(exceptionMsg);
			}

			return;
		}

		this->m_ostreamData.ostreamPtr = out;

		this->m_useOstream();
	}

	/// @brief Установить данные файла для записи логов
	/// @param filePath Путь до файла
	/// @return Возможен выброс std::runtime_error
	void setLogFile(
		const std::string& filePath
	) {
		if (this->m_fileData.is_used) {
			// Если уже используется какой-то файл, то сбрасываем содержимое на диск и закрываем файл

			Status status = this->m_fileHandler->flush(
				this->m_fileData.file
			);

			this->m_closeFile();
			this->m_forgetFile();

			if (status.isWriteFileException()) {
				std::string exceptionMsg = LogFormatter::formatLoggerExceptionLog(
					LogTypeString::fatal,
					MsgFormater::formatMsgCantFlush(this->m_fileData.filePath)
				);

				this->m_ostreamHandler->cerr(
					exceptionMsg
				);

				if (this->hasParam(LoggerParam::useExceptions)) {
					throw std::runtime_error(exceptionMsg);
				}

				return;
			}
		}

		// Устанавливаем новый файл

		this->m_fileData.filePath = filePath;

		Status status = this->m_fileHandler->openFile(
			this->m_fileData.file,
			this->m_fileData.filePath,
			std::ios_base::out
		);

		if (status.isOpenFileException()) {
			std::string exceptionMsg = LogFormatter::formatLoggerExceptionLog(
				LogTypeString::fatal,
				MsgFormater::formatMsgCantOpenFile(this->m_fileData.filePath)
			);

			this->m_forgetFile();

			this->m_ostreamHandler->cerr(
				exceptionMsg
			);

			if (this->hasParam(LoggerParam::useExceptions)) {
				throw std::runtime_error(exceptionMsg);
			}

			return;
		}

		this->m_useFile();
	}

	void debugStartFunc(
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) {
		this->m_log(
			LogFormatter::formatDebugLog(
				this->m_logLine++,
				LogTypeString::debug,
				currentFileName,
				currentFuncName,
				currentCodeLine,
				Msg::startFunc
			)
		);
	}

	void debugStopFunc(
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) {
		this->m_log(
			LogFormatter::formatDebugLog(
				this->m_logLine++,
				LogTypeString::debug,
				currentFileName,
				currentFuncName,
				currentCodeLine,
				Msg::stopFunc
			)
		);
	}

	void debug(
		std::string_view msg,
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) {
		this->m_log(
			LogFormatter::formatDebugLog(
				this->m_logLine++,
				LogTypeString::debug,
				currentFileName,
				currentFuncName,
				currentCodeLine,
				msg
			)
		);
	}

	void info(
		std::string_view msg,
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) {
		this->m_log(
			LogFormatter::formatDebugLog(
				this->m_logLine++,
				LogTypeString::info,
				currentFileName,
				currentFuncName,
				currentCodeLine,
				msg
			)
		);
	}

	void info(
		std::string_view msg
	) {
		this->m_log(
			LogFormatter::formatReleaseLog(
				this->m_logLine++,
				LogTypeString::info,
				msg
			)
		);
	}

	void exception(
		std::string_view msg,
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) {
		this->m_log(
			LogFormatter::formatDebugLog(
				this->m_logLine++,
				LogTypeString::exception,
				currentFileName,
				currentFuncName,
				currentCodeLine,
				msg
			)
		);
	}

	void exception(
		std::string_view msg
	) {
		this->m_log(
			LogFormatter::formatReleaseLog(
				this->m_logLine++,
				LogTypeString::exception,
				msg
			)
		);
	}

	void warning (
		std::string_view msg,
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) {
		this->m_log(
			LogFormatter::formatDebugLog(
				this->m_logLine++,
				LogTypeString::warning,
				currentFileName,
				currentFuncName,
				currentCodeLine,
				msg
			)
		);
	}

	void warning (
		std::string_view msg
	) {
		this->m_log(
			LogFormatter::formatReleaseLog(
				this->m_logLine++,
				LogTypeString::warning,
				msg
			)
		);
	}

	void fatal(
		std::string_view msg,
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) {
		this->m_log(
			LogFormatter::formatDebugLog(
				this->m_logLine++,
				LogTypeString::fatal,
				currentFileName,
				currentFuncName,
				currentCodeLine,
				msg
			)
		);
	}

	void fatal(
		std::string_view msg
	) {
		this->m_log(
			LogFormatter::formatReleaseLog(
				this->m_logLine++,
				LogTypeString::fatal,
				msg
			)
		);
	}

	void addParam(LoggerParam param) {
		this->m_params |= param;
	}

	void removeParam(LoggerParam param) {
		using T = std::underlying_type_t<LoggerParam>;

		this->m_params = static_cast<LoggerParam>(
			static_cast<T>(this->m_params) & ~static_cast<T>(param)
		);
	}

	bool hasParam(LoggerParam param) const {
        return (m_params & param) == param;
    }

private: // Приватные методы класса

	/// @brief Общая функция логирования
	/// @param log Готовый к записи лог
	void m_log(
		std::string_view log
	) {

		if (this->m_ostreamData.is_used) {
			this->m_writeLogInOstream(log);
		}

		if (this->m_fileData.is_used) {
			Status status = this->m_writeLogInFile(log);

			if (status.isWriteFileException()) {
				std::string exceptionMsg = LogFormatter::formatLoggerExceptionLog(
					LogTypeString::fatal,
					MsgFormater::formatMsgCantWriteFile(
						this->m_fileData.filePath,
						log
					)
				);

				this->m_closeFile();

				this->m_ostreamHandler->cerr(
					exceptionMsg
				);

				if (this->hasParam(LoggerParam::useExceptions)) {
					throw std::runtime_error(exceptionMsg);
				}
			}
		}
	}

	void m_writeLogInOstream(
		std::string_view log
	) {
		if (!this->m_ostreamData.is_used) {
			return;
		}

		this->m_ostreamHandler->writeOstream(
			this->m_ostreamData.ostreamPtr,
			log
		);
	}

	/// @brief Записать лог в рабочий файл
	/// @param data Данные-лог для записи
	/// @return Статус выполнения (successful, writeFileException)
	Status m_writeLogInFile(
		std::string_view log
	) {
		Status status = this->m_fileHandler->writeFile(
			this->m_fileData.file,
			log
		);

		if (!status.isSuccessful()) {
			return status;
		}

		if (this->hasParam(LoggerParam::useFlush)) {
			status = this->m_fileHandler->flush(
				this->m_fileData.file
			);
		}

		return status;
	}

	void m_useFile() {
		this->m_fileData.is_used = true;
	}

	void m_forgetFile() {
		this->m_fileData.is_used = false;
	}

	void m_useOstream() {
		this->m_ostreamData.is_used = true;
	}

	void m_forgetOstream() {
		this->m_ostreamData.is_used = false;
	}

	void m_closeFile() {
		if (!this->m_fileData.is_used) {
			return;
		}

		this->m_fileData.file.flush();
		this->m_fileData.file.close();

		this->m_forgetFile();
	}
};

/* ----- class Logger::FuncChecker::Impl ----- */

class Logger::FuncChecker::Impl {
private: 

	std::string_view m_currentFuncName;
	std::string_view m_currentFileName;
	int32_t m_currentCodeLine;
	const Logger& m_instance;

public:

	Impl(
		const Logger& instance,
		std::string_view currentFileName,
		std::string_view currentFuncName,
		int32_t currentCodeLine
	) : m_instance(instance),
		m_currentFileName(currentFileName),
		m_currentFuncName(currentFuncName),
		m_currentCodeLine(currentCodeLine) {}

	~Impl() = default;

	void debugStartFunc() {
		this->m_instance.debugStartFunc(
			m_currentFileName,
			m_currentFuncName,
			m_currentCodeLine
		);
	}

	void debugStopFunc() {
		this->m_instance.debugStopFunc(
			this->m_currentFileName,
			this->m_currentFuncName,
			this->m_currentCodeLine
		);
	}
};


/* ----- class Logger. Public functions ----- */

Logger::Logger() : m_impl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::setLogFile(
	const std::string& filePath
) {
	this->m_impl->setLogFile(filePath);
}

void Logger::setOstream(
	std::ostream* ostream
) {
	this->m_impl->setOstream(ostream);
}

void Logger::addParam(
    LoggerParam param)
{
    this->m_impl->addParam(param);
}

void Logger::removeparam(
	LoggerParam param
) {
	this->m_impl->removeParam(param);
}

bool Logger::hasParam(
	LoggerParam param
) const {
	return this->m_impl->hasParam(param);
}

/* ----- class Logger::FuncChecker. Public logging debug mod class ---- */

Logger::FuncChecker::FuncChecker(
	const Logger& instance,
	std::string_view currentFileName,
	std::string_view currentFuncName,
	int32_t currentCodeLine
) {
	this->m_impl = std::make_unique<Impl>(
		instance,
		currentFileName,
		currentFuncName,
		currentCodeLine
	);

	this->m_impl->debugStartFunc();
}

Logger::FuncChecker::~FuncChecker() {
	this->m_impl->debugStopFunc();
}

/* ----- class Logger. Public logging debug mod functions ----- */

void Logger::debugStartFunc(
	std::string_view currentFileName,
	std::string_view currentFuncName,
	int32_t currentCodeLine
) const {
	this->m_impl->debugStartFunc(
		currentFileName,
		currentFuncName,
		currentCodeLine
	);
}

void Logger::debugStopFunc(
	std::string_view currentFileName,
	std::string_view currentFuncName,
	int32_t currentCodeLine
) const {
	this->m_impl->debugStopFunc(
		currentFileName,
		currentFuncName,
		currentCodeLine
	);
}

void Logger::debug(
    std::string_view msg,
    std::string_view curentFileName,
    std::string_view currentFuncName,
    int32_t currentCodeLine)
{
    this->m_impl->debug(
		msg,
		curentFileName,
		currentFuncName,
		currentCodeLine
	);
}

void Logger::info(
	std::string_view msg,
	std::string_view currentFileName,
	std::string_view currentFuncName,
	int32_t currentCodeLine
) const {
	this->m_impl->info(
		msg,
		currentFileName,
		currentFuncName,
		currentCodeLine
	);
}

void Logger::exception(
	std::string_view msg,
	std::string_view currentFileName,
	std::string_view currentFuncName,
	const int32_t currentCodeLine
) {
	this->m_impl->exception(
		msg,
		currentFileName,
		currentFuncName,
		currentCodeLine
	);
}

void Logger::warning (
	std::string_view msg,
	std::string_view currentFileName,
	std::string_view currentFuncName,
	const int32_t currentCodeLine
) {
	this->m_impl->warning(
		msg,
		currentFileName,
		currentFuncName,
		currentCodeLine
	);
}

void Logger::fatal(
	std::string_view msg,
	std::string_view currentFileName,
	std::string_view currentFuncName,
	const int32_t currentCodeLine
) {
	this->m_impl->fatal(
		msg,
		currentFileName,
		currentFuncName,
		currentCodeLine
	);
}

/* ----- class Logger. Public logging release mog functions ----- */

void Logger::info(
	std::string_view msg
) {
	this->m_impl->info(msg);
}

void Logger::exception(
	std::string_view msg
) {
	this->m_impl->exception(msg);
}

void Logger::warning (
	std::string_view msg
) {
	this->m_impl->warning(msg);
}

void Logger::fatal(
	std::string_view msg
) {
	this->m_impl->fatal(msg);
}

/* ----- END ----- */
