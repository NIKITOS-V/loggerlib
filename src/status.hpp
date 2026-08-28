#ifndef __LOGGER_LIB_DETAIL_STATUS_HPP__
#define __LOGGER_LIB_DETAIL_STATUS_HPP__

namespace loggerlib::detail {
	class Status {
	public: // Public enums

		enum class StatusType {
			successful,

			runtimeException,
			nullPointerException,

			openFileException,
			writeFileException
		};

	private: // Private attributes

		StatusType m_status;

	public: // Public methods

		explicit Status(StatusType status) noexcept;
		Status(const Status& other) noexcept;
		Status(Status&& moved) noexcept;

		Status& operator = (const Status& other);

		~Status() = default;

		StatusType getStatus() const;

		bool isSuccessful() const;
		bool isRuntimeException() const;
		bool isOpenFileException() const;
		bool isWriteFileException() const;
		bool isNullPointerException() const;

	public: // Public static methods

		static Status successful();
		static Status runtimeException();
		static Status openFileException();
		static Status writeFileException();
		static Status nullPointerException();

	}; // class Status
} // namespace loggerlib::detail

#endif // __LOGGER_LIB_DETAIL_STATUS_HPP__
