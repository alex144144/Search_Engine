//
// Created by marshtv on 03.01.2024.
//

#include "invertedIndex.h"
#include "converterjson.h"
#include <filesystem>
#include <cassert>
#include <fstream>
#include <regex>
#include <thread>

// Метод удаления не нужных слов или символов
std::string InvertedIndex::replaceString(const std::string& subject,
						  const std::string &search) {
	std::regex e("(\\b(" + search + "))([^,. ]*)");
	return std::regex_replace(subject, e, "");
}

std::vector<std::string> InvertedIndex::convertTextToUniqWords(const std::string& _text) {
	// Текст не должен быть пустым
	assert(!_text.empty());

	// Регулярное выражение для выделения слов из строки
	std::regex regexWordRule{R"((\w+))"};

	// Вектор слов
	std::vector<std::string> words;

	// Временная строка инициализируется параметром функции,
	// чтобы оставить оригинальный текст без изменений.
	std::string textTemp = _text;

	// Стартовый строковый итератор регулярных выражений
	auto wordBegin = std::sregex_iterator(textTemp.begin(),
										  textTemp.end(),
										  regexWordRule);

	// Конечный строковый итератор регулярных выражений
	auto wordEnd = std::sregex_iterator();

	// В цикле добавляем выбранные слова в вектор
	for (auto &it = wordBegin; it != wordEnd; ++it) {
		words.push_back(it->str());
	}

	// Возвращаем значение вектора слов
	return words;
}

// Метод возвращает количество вхождения слова в строке
size_t getWordCountInString(const std::string& _text, const std::string& _regexStr) {
	std::string regexStr = "(\\b(" + _regexStr + "))([^,. ]*)";
	std::regex re(regexStr);
	auto wordBegin = std::sregex_iterator(_text.begin(), _text.end(), re);
	auto wordEnd = std::sregex_iterator();
	return std::distance(wordBegin, wordEnd);
}

// Метод удаления файлов из директории, исключая файл CMakeLists.txt
void eraseDBFiles(const std::string& _folder) {
	for (auto& dirEntry : std::filesystem::directory_iterator(_folder)) {
		if ((std::filesystem::path)dirEntry != std::filesystem::path(_folder + "\\CMakeLists.txt")){
			std::filesystem::remove(dirEntry);
		}
	}
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& _inputDocs) {
	assert(!_inputDocs.empty());

	this->docs.clear();
	this->docs.shrink_to_fit();
	this->freqDictionary.clear();
	eraseDBFiles("..\\resources");

	ConverterJSON converterJson;
	converterJson.UpdateConfigJsonFile(_inputDocs);

	std::vector<std::vector<std::string>> docsByWords;
	for (int i = 0; i < _inputDocs.size(); ++i) {
		std::string filePath;
		if (i < 10) {
			filePath = "../resources/file00" + std::to_string(i) + ".txt";
		} else if (i > 9 && i < 100) {
			filePath = "../resources/file0" + std::to_string(i) + ".txt";
		} else if (i > 99 && i < 1000) {
			filePath = "../resources/file" + std::to_string(i) + ".txt";
		}
		std::ofstream textFile(filePath);
		textFile << _inputDocs[i];
		textFile.close();
		docs.push_back(_inputDocs[i]);
		std::vector<std::string> words = convertTextToUniqWords(_inputDocs[i]);
		docsByWords.push_back(words);
	}

	for (auto& words : docsByWords) {
		for (auto& word : words) {
			std::thread threadWordCount([&]() {
				GetWordCount(word);
			});
			threadWordCount.join();
		}
	}
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& _word) {
	std::vector<Entry> tempEntryVector;
	assert(!_word.empty());

	freqDictionary_mutex.lock();
	for (auto& it : this->freqDictionary) {
		if (it.first == _word) {
			freqDictionary_mutex.unlock();
			return freqDictionary[_word];
		}
	}
	freqDictionary_mutex.unlock();
	docs_mutex.lock();
	for (int i = 0; i < this->docs.size(); ++i) {
		size_t countTemp = getWordCountInString(docs[i], _word);
		if (countTemp != 0) {
			tempEntryVector.emplace_back(static_cast<size_t>(i),
										 static_cast<size_t>(countTemp));
		}
	}
	docs_mutex.unlock();
	freqDictionary_mutex.lock();
	this->freqDictionary.emplace(_word, tempEntryVector);
	freqDictionary_mutex.unlock();

	return tempEntryVector;
}