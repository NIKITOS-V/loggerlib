#include "status.hpp"

/* ----- Alias ----- */

using Status = loggerlib::detail::Status;

/* ----- class Status. Public methods ----- */

// Базовый конструктор
Status::Status(
    StatusType status
) noexcept : m_status(status) {}

// Конструктор копирования
Status::Status(
    const Status& other
) noexcept : m_status(other.m_status) {}

// Конструктор перемещения (просто чтобы был)
Status::Status(
    Status&& moved
) noexcept : m_status(moved.m_status) {}

Status& Status::operator = (
    const Status& other
) {
    if (this == &other) {
        return *this;
    }

    this->m_status = other.m_status;

    return *this;
}

Status::StatusType Status::getStatus() const {
    return this->m_status;
}

bool Status::isSuccessful() const {
    return this->m_status == StatusType::successful;
}

bool Status::isRuntimeException() const {
    return this->m_status == StatusType::runtimeException;
}

bool Status::isOpenFileException() const {
    return this->m_status == StatusType::openFileException;
}

bool Status::isWriteFileException() const {
    return this->m_status == StatusType::writeFileException;
}

bool loggerlib::detail::Status::isNullPointerException() const
{
    return this->m_status == StatusType::nullPointerException;
}

/* ----- class Status. Public static methods ----- */

Status Status::successful() {
    return Status(StatusType::successful);
}

Status Status::runtimeException() {
    return Status(StatusType::runtimeException);
}

Status Status::openFileException() {
    return Status(StatusType::openFileException);
}

Status Status::writeFileException() {
    return Status(StatusType::writeFileException);
}

Status Status::nullPointerException()
{
    return Status(StatusType::nullPointerException);
}

/* ----- END ----- */
