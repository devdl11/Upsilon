#include <ion.h>
#include <string.h>
#include <assert.h>
#include <new>
#include "ion/storage.h"

#if ION_STORAGE_LOG
#include<iostream>
#endif

namespace Ion {

/* We want to implement a simple singleton pattern, to make sure the storage is
 * initialized on first use, therefore preventing the static init order fiasco.
 * That being said, we rely on knowing where the storage resides in the device's
 * memory at compile time. Indeed, we want to advertise the static storage's
 * memory address in the PlatformInfo structure (so that we can read and write
 * it in DFU).
 * Using a "static Storage storage;" variable makes it a local symbol at best,
 * preventing the PlatformInfo from retrieving its address. And making the
 * Storage variable global yields the static init fiasco issue. We're working
 * around both issues by creating a global staticStorageArea buffer, and by
 * placement-newing the Storage into that area on first use. */

uint32_t staticStorageArea[sizeof(Storage)/sizeof(uint32_t)] = {0};

constexpr char Storage::expExtension[];
constexpr char Storage::funcExtension[];
constexpr char Storage::seqExtension[];
constexpr char Storage::eqExtension[];
constexpr char Storage::examPrefix[];

Storage * Storage::sharedStorage() {
  static Storage * storage = new (staticStorageArea) Storage();
  return storage;
}

// RECORD

Storage::Record::Record(const char * fullName) {
  if (fullName == nullptr) {
    m_fullNameCRC32 = 0;
    return;
  }
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  // If no extension, return empty record
  if (*dotChar == 0 || *(dotChar+1) == 0) {
    m_fullNameCRC32 = 0;
    return;
  }
  new (this) Record(fullName, dotChar - fullName, dotChar+1, (fullName + strlen(fullName)) - (dotChar+1));
}

Storage::Record::Record(const char * baseName, const char * extension) {
  if (baseName == nullptr) {
    assert(extension == nullptr);
    m_fullNameCRC32 = 0;
    return;
  }
  new (this) Record(baseName, strlen(baseName), extension, strlen(extension));
}


#if ION_STORAGE_LOG

void Storage::Record::log() {
  std::cout << "Name: " << fullName() << std::endl;
  std::cout << "        Value (" << value().size << "): " << (char *)value().buffer << "\n\n" << std::endl;
}
#endif

uint32_t Storage::Record::checksum() {
  uint32_t crc32Results[2];
  crc32Results[0] = m_fullNameCRC32;
  Data data = value();
  crc32Results[1] = Ion::crc32Byte((const uint8_t *)data.buffer, data.size);
  return Ion::crc32Word(crc32Results, 2);
}

Storage::Record::Record(const char * basename, int basenameLength, const char * extension, int extensionLength) {
  assert(basename != nullptr);
  assert(extension != nullptr);

  // We compute the CRC32 of the CRC32s of the basename and the extension
  uint32_t crc32Results[2];
  crc32Results[0] = Ion::crc32Byte((const uint8_t *)basename, basenameLength);
  crc32Results[1] = Ion::crc32Byte((const uint8_t *)extension, extensionLength);
  m_fullNameCRC32 = Ion::crc32Word(crc32Results, 2);
}

// STORAGE

#if ION_STORAGE_LOG
void Storage::log() {
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    Record(currentName).log();
  }
}

void Storage::logMessage(const char * message) {
  std::cout << message << std::endl;
}
#endif

size_t Storage::availableSize() {
  /* TODO maybe do: availableSize(char ** endBuffer) to get the endBuffer if it
   * is needed after calling availableSize */
  assert(k_storageSize >= (endBuffer() - m_buffer) + sizeof(record_size_t));
  return k_storageSize-(endBuffer()-m_buffer)-sizeof(record_size_t);
}

size_t Storage::putAvailableSpaceAtEndOfRecord(Storage::Record r) {
  char * p = pointerOfRecord(r);
  size_t previousRecordSize = sizeOfRecordStarting(p);
  size_t availableStorageSize = availableSize();
  char * nextRecord = p + previousRecordSize;
  memmove(nextRecord + availableStorageSize,
      nextRecord,
      (m_buffer + k_storageSize - availableStorageSize) - nextRecord);
  size_t newRecordSize = previousRecordSize + availableStorageSize;
  overrideSizeAtPosition(p, (record_size_t)newRecordSize);
  return newRecordSize;
}

void Storage::getAvailableSpaceFromEndOfRecord(Record r, size_t recordAvailableSpace) {
  char * p = pointerOfRecord(r);
  size_t previousRecordSize = sizeOfRecordStarting(p);
  char * nextRecord = p + previousRecordSize;
  memmove(nextRecord - recordAvailableSpace,
      nextRecord,
      m_buffer + k_storageSize - nextRecord);
  overrideSizeAtPosition(p, (record_size_t)(previousRecordSize - recordAvailableSpace));
}

uint32_t Storage::checksum() {
  return Ion::crc32Byte((const uint8_t *) m_buffer, endBuffer()-m_buffer);
}

void Storage::notifyChangeToDelegate(const Record record) const {
  m_lastRecordRetrieved = Record(nullptr);
  m_lastRecordRetrievedPointer = nullptr;
  if (m_delegate != nullptr) {
    m_delegate->storageDidChangeForRecord(record);
  }
}

Storage::Record::ErrorStatus Storage::notifyFullnessToDelegate() const {
  if (m_delegate != nullptr) {
    m_delegate->storageIsFull();
  }
  return Record::ErrorStatus::NotEnoughSpaceAvailable;
}

Storage::Record::ErrorStatus Storage::createRecordWithFullName(const char * fullName, const void * data, size_t size) {
  setFullNameBufferWithPrefix(examPrefix, fullName);
  if (!fullNameAuthorized(fullName)) {
    return Record::ErrorStatus::NameTaken;
  }
  if (m_quarantine) {
    fullName = m_fullNameBuffer;
  }
  size_t recordSize = sizeOfRecordWithFullName(fullName, size);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return notifyFullnessToDelegate();
  }
  if (isFullNameTaken(fullName) || (!m_quarantine && !fullNameAuthorized(fullName))) {
    return Record::ErrorStatus::NameTaken;
  }
  // Find the end of data
  char * newRecordAddress = endBuffer();
  char * newRecord = newRecordAddress;
  // Fill totalSize
  newRecord += overrideSizeAtPosition(newRecord, (record_size_t)recordSize);
  // Fill name
  newRecord += overrideFullNameAtPosition(newRecord, fullName);
  // Fill data
  newRecord += overrideValueAtPosition(newRecord, data, size);
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  Record r = Record(fullName);
  notifyChangeToDelegate(r);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = newRecordAddress;
  #if ION_STORAGE_LOG
  logMessage("createRecordWithFullName start");
  log();
  r.log();
  logMessage("createRecordWithFullName end");
  #endif
  return Record::ErrorStatus::None;
}

Storage::Record::ErrorStatus Storage::createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size) {
  setFullNameBufferWithPrefix(examPrefix, baseName);
  if (!fullNameAuthorized(baseName)) {
    return Record::ErrorStatus::NameTaken;
  }
  #if ION_STORAGE_LOG
  logMessage("createRecordWithExtension start");
  logMessage(baseName);
  #endif
  if (m_quarantine) {
    baseName = m_fullNameBuffer;
  }
  size_t recordSize = sizeOfRecordWithBaseNameAndExtension(baseName, extension, size);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return notifyFullnessToDelegate();
  }
  if (isBaseNameWithExtensionTaken(baseName, extension) || (!m_quarantine && !fullNameAuthorized(baseName))) {
    return Record::ErrorStatus::NameTaken;
  }
  // Find the end of data
  char * newRecordAddress = endBuffer();
  char * newRecord = newRecordAddress;
  // Fill totalSize
  newRecord += overrideSizeAtPosition(newRecord, (record_size_t)recordSize);
  // Fill name
  newRecord += overrideBaseNameWithExtensionAtPosition(newRecord, baseName, extension);
  // Fill data
  newRecord += overrideValueAtPosition(newRecord, data, size);
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  Record r = Record(fullNameOfRecordStarting(newRecordAddress));
  notifyChangeToDelegate(r);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = newRecordAddress;
  #if ION_STORAGE_LOG
  log();
  r.log();
  logMessage("createRecordWithExtension end");
  #endif
  return Record::ErrorStatus::None;
}

int Storage::numberOfRecordsWithExtension(const char * extension) {
  int count = 0;
  size_t extensionLength = strlen(extension);
  #if ION_STORAGE_LOG
  logMessage("numberOfRecordsWithExtension start");
  #endif
  for (char * p : *this) {
    const char * name = fullNameOfRecordStarting(p);
    #if ION_STORAGE_LOG
    logMessage(name);
    #endif
    if (FullNameHasExtension(name, extension, extensionLength) && !(m_quarantine && !strstr(name, examPrefix))) {
      count++;
      #if ION_STORAGE_LOG
      logMessage("Counted !");
      #endif
    }
  }
  #if ION_STORAGE_LOG
  logMessage("numberOfRecordsWithExtension end");
  #endif
  return count;
}

int Storage::numberOfRecords(bool system) {
  int count = 0;
  #if ION_STORAGE_LOG
  logMessage("numberOfRecords start");
  #endif
  for (char * p : *this) {
    const char * name = fullNameOfRecordStarting(p);
    #if ION_STORAGE_LOG
    logMessage(name);
    #endif
    if(!(m_quarantine && !strstr(name, examPrefix)) || system){
      count++;
      #if ION_STORAGE_LOG
      logMessage("counted !");
      #endif
    }
  }
  #if ION_STORAGE_LOG
  logMessage("numberOfRecords end");
  #endif
  return count;
}

Storage::Record Storage::recordAtIndex(int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  char * recordAddress = nullptr;
  #if ION_STORAGE_LOG
  logMessage("recordAtIndex start");
  #endif
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    #if ION_STORAGE_LOG
    logMessage(currentName);
    #endif
    if (m_quarantine && !strstr(currentName, examPrefix)) {
      continue;
    }
    currentIndex++;
    #if ION_STORAGE_LOG
    logMessage("currentIndex ++");
    #endif
    if (currentIndex == index) {
      recordAddress = p;
      name = currentName;
      #if ION_STORAGE_LOG
      logMessage("found !");
      #endif
      break;
    }
  }
  #if ION_STORAGE_LOG
  logMessage("recordAtIndex end");
  #endif
  if (name == nullptr) {
    return Record();
  }
  Record r = Record(name);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = recordAddress;
  return Record(name);
}

Storage::Record Storage::recordWithExtensionAtIndex(const char * extension, int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  size_t extensionLength = strlen(extension);
  char * recordAddress = nullptr;
  #if ION_STORAGE_LOG
  logMessage("recordWithExtensionAtIndex start");
  #endif
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    #if ION_STORAGE_LOG
    logMessage(currentName);
    #endif
    if (m_quarantine && !strstr(currentName, examPrefix)) {
      continue;
    }
    if (FullNameHasExtension(currentName, extension, extensionLength)) {
      #if ION_STORAGE_LOG
      logMessage("currentIndex ++");
      #endif
      currentIndex++;
    }
    if (currentIndex == index) {
      #if ION_STORAGE_LOG
      logMessage("found !");
      #endif
      recordAddress = p;
      name = currentName;
      break;
    }
  }
  #if ION_STORAGE_LOG
  logMessage("recordWithExtensionAtIndex end");
  #endif
  if (name == nullptr) {
    return Record();
  }
  Record r = Record(name);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = recordAddress;
  return Record(name);
}

Storage::Record Storage::recordNamed(const char * fullName) {
  if (fullName == nullptr) {
    return Record();
  }
  #if ION_STORAGE_LOG
  logMessage("recordNamed start");
  logMessage(fullName);
  #endif 
  Record r = Record(fullName);
  char * p = pointerOfRecord(r);
  if (p != nullptr && (!m_quarantine || strstr(fullName, examPrefix))) {
    #if ION_STORAGE_LOG
    logMessage("First return");
    logMessage("recordNamed end");
    #endif
    return r;
  }else if (p == nullptr && m_quarantine) {
    setFullNameBufferWithPrefix(examPrefix, fullName);
    r = Record(m_fullNameBuffer);
    p = pointerOfRecord(r);
    #if ION_STORAGE_LOG
    logMessage("Second if");
    #endif
    if (p != nullptr) {
      #if ION_STORAGE_LOG
      logMessage("Second return");
      logMessage("recordNamed end");
      #endif
      return r;
    }
  }
  #if ION_STORAGE_LOG
  logMessage("recordNamed end");
  #endif
  return Record();
}

Storage::Record Storage::recordBaseNamedWithExtension(const char * baseName, const char * extension) {
  const char * extensions[1] = {extension};
  return recordBaseNamedWithExtensions(baseName, extensions, 1);
}

Storage::Record Storage::recordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions) {
  return privateRecordAndExtensionOfRecordBaseNamedWithExtensions(baseName, extensions, numberOfExtensions);
}

const char * Storage::extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions) {
  const char * result = nullptr;
  privateRecordAndExtensionOfRecordBaseNamedWithExtensions(baseName, extensions, numberOfExtensions, &result, baseNameLength);
  return result;
}

void Storage::destroyAllRecords() {
  overrideSizeAtPosition(m_buffer, 0);
  notifyChangeToDelegate();
}

void Storage::destroyRecordWithBaseNameAndExtension(const char * baseName, const char * extension) {
  recordBaseNamedWithExtension(baseName, extension).destroy();
}

void Storage::destroyRecordsWithExtension(const char * extension) {
  size_t extensionLength = strlen(extension);
  char * currentRecordStart = (char *)m_buffer;
  bool didChange = false;
  while (currentRecordStart != nullptr && sizeOfRecordStarting(currentRecordStart) != 0) {
    const char * currentFullName = fullNameOfRecordStarting(currentRecordStart);
    if (FullNameHasExtension(currentFullName, extension, extensionLength)) {
      Record currentRecord(currentFullName);
      currentRecord.destroy();
      didChange = true;
      continue;
    }
    currentRecordStart = *(RecordIterator(currentRecordStart).operator++());
  }
  if (didChange) {
    notifyChangeToDelegate();
  }
}

// PRIVATE

Storage::Storage() :
  m_magicHeader(Magic),
  m_buffer(),
  m_magicFooter(Magic),
  m_delegate(nullptr),
  m_lastRecordRetrieved(nullptr),
  m_lastRecordRetrievedPointer(nullptr),
  m_quarantine(false),
  m_examNumber(0),
  m_fullNameBuffer()
{
  assert(m_magicHeader == Magic);
  assert(m_magicFooter == Magic);
  // Set the size of the first record to 0
  overrideSizeAtPosition(m_buffer, 0);
}

const char * Storage::fullNameOfRecord(const Record record, bool system) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    const char * name = fullNameOfRecordStarting(p);
    return m_quarantine && !system && Storage::strstr(name, examPrefix) ? name + strlen(examPrefix): name;
  }
  return nullptr;
}

Storage::Record::ErrorStatus Storage::setFullNameOfRecord(const Record record, const char *fullName, bool system) {
  #if ION_STORAGE_LOG
  logMessage("setFullNameOfRecord start");
  logMessage(fullName);
  #endif
  if (m_quarantine && !system) {
    setFullNameBufferWithPrefix(examPrefix, fullName); 
    fullName = m_fullNameBuffer;
  }
  if (!FullNameCompliant(fullName) || (!m_quarantine && !fullNameAuthorized(fullName))) {
    #if ION_STORAGE_LOG
    logMessage("NonCompliant return");
    logMessage("setFullNameOfRecord end");
    #endif
    return Record::ErrorStatus::NonCompliantName;
  }
  if (isFullNameTaken(fullName, &record)) {
    #if ION_STORAGE_LOG
    logMessage("Taken return");
    logMessage("setFullNameOfRecord end");
    #endif
    return Record::ErrorStatus::NameTaken;
  }
  #if ION_STORAGE_LOG
  logMessage("After conditions");
  logMessage(fullName);
  #endif
  size_t nameSize = strlen(fullName) + 1;
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    size_t previousNameSize = strlen(fullNameOfRecordStarting(p))+1;
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
      #if ION_STORAGE_LOG
      logMessage("notify return");
      logMessage("setFullNameOfRecord end");
      #endif
      return notifyFullnessToDelegate();
    }
    overrideSizeAtPosition(p, newRecordSize);
    overrideFullNameAtPosition(p+sizeof(record_size_t), fullName);
    notifyChangeToDelegate(record);
    m_lastRecordRetrieved = record;
    m_lastRecordRetrievedPointer = p;
    #if ION_STORAGE_LOG
    logMessage("return None");
    logMessage("setFullNameOfRecord end");
    #endif
    return Record::ErrorStatus::None;
  }
  #if ION_STORAGE_LOG
  logMessage("return NotExist");
  logMessage("setFullNameOfRecord end");
  #endif
  return Record::ErrorStatus::RecordDoesNotExist;
}

Storage::Record::ErrorStatus Storage::setBaseNameWithExtensionOfRecord(Record record, const char *baseName, const char *extension, bool system) {
  #if ION_STORAGE_LOG
  logMessage("setBaseNameWithExtensionOfRecord start");
  logMessage(baseName);
  #endif
  setFullNameBufferWithPrefix(examPrefix, baseName);
  if (m_quarantine && !system) {
    baseName = m_fullNameBuffer;
  }
  if (isBaseNameWithExtensionTaken(baseName, extension, &record) || (!m_quarantine && !fullNameAuthorized(baseName))) {
    #if ION_STORAGE_LOG
    logMessage("Taken return");
    logMessage("setBaseNameWithExtensionOfRecord end");
    #endif
    return Record::ErrorStatus::NameTaken;
  }
  #if ION_STORAGE_LOG
  logMessage("After conditions");
  logMessage(baseName);
  #endif
  size_t nameSize = sizeOfBaseNameAndExtension(baseName, extension);
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    size_t previousNameSize = strlen(fullNameOfRecordStarting(p))+1;
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
      #if ION_STORAGE_LOG
      logMessage("notify return");
      logMessage("setBaseNameWithExtensionOfRecord end");
      #endif
      return notifyFullnessToDelegate();
    }
    overrideSizeAtPosition(p, newRecordSize);
    char * fullNamePosition = p + sizeof(record_size_t);
    overrideBaseNameWithExtensionAtPosition(fullNamePosition, baseName, extension);
    // Recompute the CRC32
    record = Record(fullNamePosition);
    notifyChangeToDelegate(record);
    m_lastRecordRetrieved = record;
    m_lastRecordRetrievedPointer = p;
    #if ION_STORAGE_LOG
    logMessage("return None");
    logMessage("setBaseNameWithExtensionOfRecord end");
    #endif
    return Record::ErrorStatus::None;
  }
  #if ION_STORAGE_LOG
  logMessage("return NotExist");
  logMessage("setBaseNameWithExtensionOfRecord end");
  #endif
  return Record::ErrorStatus::RecordDoesNotExist;
}

Storage::Record::Data Storage::valueOfRecord(const Record record) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    const char * fullName = fullNameOfRecordStarting(p);
    record_size_t size = sizeOfRecordStarting(p);
    const void * value = valueOfRecordStarting(p);
    return {.buffer = value, .size = size-strlen(fullName)-1-sizeof(record_size_t)};
  }
  return {.buffer= nullptr, .size= 0};
}

Storage::Record::ErrorStatus Storage::setValueOfRecord(Record record, Record::Data data) {
  char * p = pointerOfRecord(record);
  /* TODO: if data.buffer == p, assert that size hasn't change and do not do any
   * memcopy, but still notify the delegate. Beware of scripts and the accordion
   * routine.*/
  if (p != nullptr) {
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    const char * fullName = fullNameOfRecordStarting(p);
    size_t newRecordSize = sizeOfRecordWithFullName(fullName, data.size);
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+previousRecordSize, newRecordSize-previousRecordSize)) {
      return notifyFullnessToDelegate();
    }
    record_size_t fullNameSize = strlen(fullName)+1;
    overrideSizeAtPosition(p, newRecordSize);
    overrideValueAtPosition(p+sizeof(record_size_t)+fullNameSize, data.buffer, data.size);
    notifyChangeToDelegate(record);
    m_lastRecordRetrieved = record;
    m_lastRecordRetrievedPointer = p;
    return Record::ErrorStatus::None;
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

void Storage::destroyRecord(Record record) {
  if (record.isNull()) {
    return;
  }
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    slideBuffer(p+previousRecordSize, -previousRecordSize);
    notifyChangeToDelegate();
  }
}

char * Storage::pointerOfRecord(const Record record) const {
  if (record.isNull()) {
    return nullptr;
  }
  if (!m_lastRecordRetrieved.isNull() && record == m_lastRecordRetrieved) {
    assert(m_lastRecordRetrievedPointer != nullptr);
    return m_lastRecordRetrievedPointer;
  }
  for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      m_lastRecordRetrieved = record;
      m_lastRecordRetrievedPointer = p;
      return p;
    }
  }
  return nullptr;
}

Storage::record_size_t Storage::sizeOfRecordStarting(char * start) const {
  return StorageHelper::unalignedShort(start);
}

const char * Storage::fullNameOfRecordStarting(char * start) const {
  return start+sizeof(record_size_t);
}

const void * Storage::valueOfRecordStarting(char * start) const {
  char * currentChar = start+sizeof(record_size_t);
  size_t fullNameLength = strlen(currentChar);
  return currentChar+fullNameLength+1;
}

size_t Storage::overrideSizeAtPosition(char * position, record_size_t size) {
  StorageHelper::writeUnalignedShort(size, position);
  return sizeof(record_size_t);
}

size_t Storage::overrideFullNameAtPosition(char * position, const char * fullName) {
  return strlcpy(position, fullName, strlen(fullName)+1) + 1;
}

size_t Storage::overrideBaseNameWithExtensionAtPosition(char * position, const char * baseName, const char * extension) {
  size_t result = strlcpy(position, baseName, strlen(baseName)+1); // strlcpy copies the null terminating char
  assert(UTF8Decoder::CharSizeOfCodePoint(k_dotChar) == 1);
  *(position+result) = k_dotChar; // Replace the null terminating char with a dot
  result++;
  result += strlcpy(position+result, extension, strlen(extension)+1);
  return result+1;
}

size_t Storage::overrideValueAtPosition(char * position, const void * data, record_size_t size) {
  memcpy(position, data, size);
  return size;
}

bool Storage::isFullNameTaken(const char * fullName, const Record * recordToExclude) {
  Record r = Record(fullName);
  return isNameOfRecordTaken(r, recordToExclude);
}

bool Storage::isBaseNameWithExtensionTaken(const char * baseName, const char * extension, Record * recordToExclude) {
  Record r = Record(baseName, extension);
  return isNameOfRecordTaken(r, recordToExclude);
}

bool Storage::isNameOfRecordTaken(Record r, const Record * recordToExclude) {
  if (r == Record()) {
    /* If the CRC32 of fullName is 0, we want to refuse the name as it would
     * interfere with our escape case in the Record constructor, when the given
     * name is nullptr. */
    return true;
  }
  for (char * p : *this) {
    Record s(fullNameOfRecordStarting(p));
    if (recordToExclude && s == *recordToExclude) {
      continue;
    }
    if (s == r) {
      return true;
    }
  }
  return false;
}

bool Storage::FullNameCompliant(const char * fullName, bool withoutExtension) {
  // We check that there is one dot and one dot only.
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  if (*dotChar == 0) {
    return false;
  }
  if (*(UTF8Helper::CodePointSearch(dotChar+1, k_dotChar)) == 0) {
    return true;
  }
  return false;
}

bool Storage::FullNameHasExtension(const char * fullName, const char * extension, size_t extensionLength) {
  if (fullName == nullptr) {
    return false;
  }
  size_t fullNameLength = strlen(fullName);
  if (fullNameLength > extensionLength) {
    const char * ext = fullName + fullNameLength - extensionLength;
    if (UTF8Helper::PreviousCodePointIs(fullName, ext, k_dotChar) && strcmp(ext, extension) == 0) {
      return true;
    }
  }
  return false;
}

char * Storage::endBuffer() {
  char * currentBuffer = m_buffer;
  for (char * p : *this) {
    currentBuffer += sizeOfRecordStarting(p);
  }
  return currentBuffer;
}

size_t Storage::sizeOfBaseNameAndExtension(const char * baseName, const char * extension) const {
  // +1 for the dot and +1 for the null terminating char
  return strlen(baseName)+1+strlen(extension)+1;
}

size_t Storage::sizeOfRecordWithBaseNameAndExtension(const char * baseName, const char * extension, size_t dataSize) const {
  return sizeOfBaseNameAndExtension(baseName, extension) + dataSize + sizeof(record_size_t);
}

size_t Storage::sizeOfRecordWithFullName(const char * fullName, size_t dataSize) const {
  size_t nameSize = strlen(fullName)+1;
  return nameSize+dataSize+sizeof(record_size_t);
}

bool Storage::slideBuffer(char * position, int delta) {
  if (delta > (int)availableSize()) {
    return false;
  }
  memmove(position+delta, position, endBuffer()+sizeof(record_size_t)-position);
  return true;
}

Storage::Record Storage::privateRecordAndExtensionOfRecordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult, int baseNameLength) {
  setFullNameBufferWithPrefix(examPrefix, baseName);
  if (m_quarantine && !strstr(baseName, examPrefix)) {
    baseName = m_fullNameBuffer;
  }
  size_t nameLength = baseNameLength < 0 ? strlen(baseName) : baseNameLength;
  {
    const char * lastRetrievedRecordFullName = fullNameOfRecordStarting(m_lastRecordRetrievedPointer);
    if (m_lastRecordRetrievedPointer != nullptr && strncmp(baseName, lastRetrievedRecordFullName, nameLength) == 0) {
      for (size_t i = 0; i < numberOfExtensions; i++) {
        if (strcmp(lastRetrievedRecordFullName+nameLength+1 /*+1 to pass the dot*/, extensions[i]) == 0) {
          assert(UTF8Helper::CodePointIs(lastRetrievedRecordFullName + nameLength, '.'));
          if (extensionResult != nullptr) {
            *extensionResult = extensions[i];
          }
          return m_lastRecordRetrieved;
        }
      }
    }
  }
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (strncmp(baseName, currentName, nameLength) == 0) {
      for (size_t i = 0; i < numberOfExtensions; i++) {
        if (strcmp(currentName+nameLength+1 /*+1 to pass the dot*/, extensions[i]) == 0) {
          assert(UTF8Helper::CodePointIs(currentName + nameLength, '.'));
          if (extensionResult != nullptr) {
            *extensionResult = extensions[i];
          }
          return Record(currentName);
        }
      }
    }
  }
  if (extensionResult != nullptr) {
    *extensionResult = nullptr;
  }
  return Record();
}

void Storage::activateQuarantine() {
  m_quarantine = true;
  // TODO maybe do: check if there is enough space for the exam
}

bool Storage::strstr(const char * first, const char * second) {
  size_t fSize = strlen(first);
  size_t sSize = strlen(second);
  if (fSize == sSize) {
    return strcmp(first, second) == 0;
  }
  if (sSize > fSize) {
    const char * buf = first;
    first = second;
    second = buf;
    fSize = strlen(first);
    sSize = strlen(second);
  }
  bool result = false;
  int index = 0;
  for (int i = 0; (size_t)i < fSize; i++) {
    if (first[i] == second[index]) {
      index++;
      result = (size_t)index >= sSize;
    } else if (fSize - 1 - (size_t)i < sSize - 1 - (size_t)index) {
      break;
    } else {
      index = 0;
    }
    if (result) break;
  }
  return result;
}

void Storage::deactivateQuarantine() {
  assert(m_quarantine);
  int count = numberOfRecords(true);
  const char * buffer[count];
  int index = 0;
  #ifdef ION_STORAGE_LOG
  logMessage("deactivateQuarantine start");
  #endif
  
  for (char * p : *this) {
    const char * name = fullNameOfRecordStarting(p);
    if (strstr(name, examPrefix)) {
      buffer[index] = name;
      index ++;
    }
  }
  for (int i = 0; i <= index; i++) {
    Record r = Record(buffer[i]);
    const char * originalName = fullNameOfRecord(r, false);
    #ifdef ION_STORAGE_LOG
    logMessage(originalName);
    #endif
    int current = 0;
    char converted = current;
    setFullNameBufferWithPrefix(originalName, &converted);
    while (isFullNameTaken(m_fullNameBuffer)) {
      current ++;
      converted = current;
      setFullNameBufferWithPrefix(originalName, &converted);
    }
    setFullNameOfRecord(r, m_fullNameBuffer, true);
  }
  #ifdef ION_STORAGE_LOG
  logMessage("deactivateQuarantine end");
  #endif
  m_quarantine = false;
}

void Storage::setFullNameBufferWithPrefix(const char *prefix, const char *name) {
  assert(sizeof(prefix) + sizeof(name) <= k_fullNameMaxSize);
  memset(m_fullNameBuffer, 0, k_fullNameMaxSize);
  strlcat(m_fullNameBuffer, prefix, k_fullNameMaxSize);
  strlcat(m_fullNameBuffer, name, k_fullNameMaxSize);
  #ifdef ION_STORAGE_LOG
  logMessage("setFullNameBufferWithPrefix start");
  logMessage(name);
  logMessage(prefix);
  logMessage(m_fullNameBuffer);
  logMessage("setFullNameBufferWithPrefix end");
  #endif
}

bool Storage::fullNameAuthorized(const char *fullname) {
  return !Storage::strstr(fullname, examPrefix);
}

Storage::RecordIterator & Storage::RecordIterator::operator++() {
  assert(m_recordStart);
  record_size_t size = StorageHelper::unalignedShort(m_recordStart);
  char * nextRecord = m_recordStart+size;
  record_size_t newRecordSize = StorageHelper::unalignedShort(nextRecord);
  m_recordStart = (newRecordSize == 0 ? nullptr : nextRecord);
  return *this;
}

}
