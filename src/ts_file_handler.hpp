#pragma once

#include <memory>
#include <string>
#include <fstream>

#include "status.hpp"

namespace loggerlib::detail {
	class TSFileWriter {
	private: // Private attributes

		class Impl;
		std::unique_ptr<Impl> m_impl;

	public: // Public methods

		TSFileWriter();
		~TSFileWriter();

		/// @brief Записать текст в открытый файл
		/// @param file открытый файл для записи
		/// @param text Текст для записи
		/// @return Статус выполнения (successful, writeFileException)
		Status writeFile (
			std::ofstream& file,
			std::string_view text
		);

		/// @brief Открыть файл по указанному пути
		/// @param file Не открытый файл
		/// @param filePath Путь до файла
		/// @param mod Режим открытия файла
		/// @return Статус выполнения (successful, openFileException)
		Status openFile(
			std::ofstream& file,
			const std::string& filePath,
			std::ios_base::openmode mod
		);

		/// @brief Принудительно записать данные с буфера файла
		/// @param file Открытый файл
		/// @return Результат выполнения (successful, writeFileException)
		Status flush(
			std::ofstream& file
		);

	}; // class TSFileWriter

} // namespace loggerlib::detail

