#include "ts_file_handler.hpp"

#include <mutex>
#include <iostream>

/* ----- Alias ----- */

using TSFileHandler = loggerlib::detail::TSFileWriter;
using Status = loggerlib::detail::Status;

/* ----- class TSFileHandler::Impl ----- */

class TSFileHandler::Impl {
private: // Private attributes

	std::mutex mtx;

public: // Public methods

	Status writeFile (
		std::ofstream& file,
		std::string_view text
	) {

		this->m_safeLockMutex();

		if (!file.is_open()) {
			return Status::writeFileException();
		}

		file.write(text.data(), text.size());

		if (file.fail()) {
			return Status::writeFileException();
		}

		return Status::successful();
	}

	Status openFile(
		std::ofstream& file,
		const std::string& filePath,
		std::ios_base::openmode mod
	) {
		this->m_safeLockMutex();

		if (file.is_open()) {
			return Status::successful();
		}

		file.open(
			filePath,
			mod
		);

		if (file.fail()) {
			return Status::openFileException();
		}

		return Status::successful();
	}

	/// @brief Принудительно записать данные с буфера файла
	/// @param file Открытый файл
	/// @return Результат выполнения (successful, writeFileException)
	Status flush(
		std::ofstream& file
	) {
		this->m_safeLockMutex();

		if (!file.is_open()) {
			return Status::writeFileException();
		}

		file.flush();

		if (file.fail()) {
			return Status::writeFileException();
		}

		return Status::successful();
	}

private: // Private methods

	/// @brief Безопасно заблокировать мьютекс
	void m_safeLockMutex() {
		std::lock_guard<std::mutex> lock(this->mtx);
	}
};

/* ----- class TSFileHandler. Public methods ----- */

TSFileHandler::TSFileWriter() : m_impl(std::make_unique<Impl>()) {}

TSFileHandler::~TSFileWriter() = default;

Status TSFileHandler::writeFile(
	std::ofstream& file,
	std::string_view text
) {
	Status s = this->m_impl->writeFile(file, text);
	return s;
}

Status TSFileHandler::openFile(
	std::ofstream& file,
	const std::string& filePath,
	std::ios_base::openmode mod
) {
	return this->m_impl->openFile(file, filePath, mod);
}

Status TSFileHandler::flush(
	std::ofstream& file
) {
    return this->m_impl->flush(file);
}

/* ----- END ----- */
