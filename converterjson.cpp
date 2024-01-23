//
// Created by marshtv on 29.12.2023.
//

#include "converterjson.h"
#include "nlohmann/json.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

// Функция извлекает содержимое файла JSON в словарь nlohmann
nlohmann::json getDicFromJsonFile(const std::string& _jsonFilePath) {
	// Открывает файл *.json для чтения
	std::ifstream inJsonFile(_jsonFilePath);

	// Проверка на открытие/существование файла *.json
	assert (inJsonFile.is_open());

	// Парсим содержимое JSON-файла в объявленный JSON-словарь
	nlohmann::json jsonDic = nlohmann::json::parse(inJsonFile);

	// Закрываем файл
	inJsonFile.close();

	// Возвращаем содержимое файла в виде JSON-словаря
	return jsonDic;
}

// Функция сохраняет содержимое словаря в файл JSON
void putDicToJsonFile(const std::string& _jsonFilePath, const nlohmann::json& _jsonDic) {
	// Открывает файл *.json для записи с нуля
	std::ofstream outJsonFile(_jsonFilePath, std::ios::trunc);

	// Проверяем открылся ли файл
	assert(outJsonFile.is_open());

	// Записываем содержимое JSON-словаря в файл *.json
	outJsonFile << _jsonDic;

	// Закрываем файл
	outJsonFile.close();
}

// Метод обновляет данные файла config.json
void ConverterJSON::UpdateConfigJsonFile(const std::vector<std::string>& _docs) {
	// Объявляем JSON-словарь
	nlohmann::json configOutJsonDic;
	// Формируем строку, имитирующую JSON-формат
	std::string configInStr = "{\n\t\"config\":{"
							  "\n\t\t\"name\":\"SkillboxSearchEngine\","
							  "\n\t\t\"version\":\"0.1\","
							  "\n\t\t\"max_responses\":5"
							  "\n\t},\n\t\"files\":[";
	for (int i = 0; i < _docs.size(); ++i) {
		if (i < 10)
			configInStr += ("\n\t\t\"../resources/file00" + std::to_string(i) + ".txt\"");
		else if (i > 9 && i < 100)
			configInStr += ("\n\t\t\"../resources/file0" + std::to_string(i) + ".txt\"");
		else if (i > 99 && i < 1000)
			configInStr += ("\n\t\t\"../resources/file" + std::to_string(i) + ".txt\"");
		if (i < _docs.size() - 1)
			configInStr += ",";
	}
	configInStr += "\n\t]\n}";

	// Парсим строку в словарь
	configOutJsonDic = nlohmann::json::parse(configInStr);

	// Сохраняем содержимое словаря в файл
	putDicToJsonFile("../config.json", configOutJsonDic);
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
	// Объявляем JSON-словарь, и присваиваем ему извлечённое содержимое файла "../config.json"
	nlohmann::json configJsonDic = getDicFromJsonFile("../config.json");

	// Создаёт вектор строк для сохранения конкретных строк
	std::vector<std::string> docFilePathList, docTextList;

	// В цикле добавляем в вектор нужные строки из ветки "files"
	for (const auto& it : configJsonDic["files"]) {
		std::ifstream docFile(it);
		std::string text;
		while(getline(docFile, text));
		docFile.close();
		docTextList.push_back(text);
	}

	// Возвращает вектор содержимого файлов
	return docTextList;
}

int ConverterJSON::GetResponsesLimit() {
	// Объявляем JSON-словарь, и присваиваем ему извлечённое содержимое файла "../config.json"
	nlohmann::json configJsonDic = getDicFromJsonFile("../config.json");

	// возвращаем значение искомого ключа словаря
	return configJsonDic["config"]["max_responses"];
}

void ConverterJSON::putRequests(const std::vector<std::string>& _requests) {
	//  Объявляем JSON-словарь
	nlohmann::json requestsInputJsonDic;

	// Формируем строку, имитирующую JSON-формат
	std::string requestsStr = "{\n\t\"requests\":[";
	for (int i = 0; i < _requests.size(); ++i) {
		requestsStr += "\n\t\t\"" + _requests[i] + "\"";
		if (i < _requests.size() - 1)
			requestsStr += ",";
	}
	requestsStr += "\n\t]\n}";

	// Парсим строку в словарь
	requestsInputJsonDic = nlohmann::json::parse(requestsStr);

	// Сохраняем содержимое словаря в файл
	putDicToJsonFile("../requests.json", requestsInputJsonDic);
}

std::vector<std::string> ConverterJSON::GetRequests() {
	// Объявляем JSON-словарь
	nlohmann::json requestsInJsonDic;

	// Объявляем вектор строк
	std::vector<std::string> requestsList;

	// Присваиваем словарю извлечённое содержимое файла "../requests.json"
	requestsInJsonDic = getDicFromJsonFile("../requests.json");

	// В цикле из JSON-словаря заполняем вектор значениями ключа "requests"
	for (const auto& it : requestsInJsonDic["requests"]) {
		requestsList.push_back(it);
	}

	// Возвращаем вектор запросов
	return requestsList;
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<size_t, float>>>& _answers) {
	// Формируем строку, имитирующую JSON-формат
	std::string answersStr = "{\n\t\"answers\":{\n\t\t";
	for (int i = 0; i < _answers.size(); ++i) {
		float count = 0;
		for (auto &it: _answers[i]) {
			if (it.second > count) {
				count += it.second;
				break;
			}
		}
		if (count == 0) {
			answersStr += "\"request00" + std::to_string(i) + "\":{\n\t\t\t" + R"("result":"false")";
		} else {
			answersStr += "\"request00" + std::to_string(i) + "\":{\n\t\t\t" + R"("result":"true",)" + "\n\t\t\t\"relevance\":{\n\t\t\t\t";
			for (int j = 0; j < _answers[i].size(); ++j) {
				answersStr += "\"docid\":" +
					std::to_string(_answers[i][j].first) + ",\"rank\":" +
					std::to_string(_answers[i][j].second);
				if (j < _answers[i].size() - 1)
					answersStr += ",\n\t\t\t\t";
			}
			answersStr += "\n\t\t\t}";
		}
		if (i < _answers.size() - 1) {
			answersStr += "\n\t\t},\n\t\t";
		} else
			answersStr += "\n\t\t}";
	}
	answersStr += "\n\t}\n}";

	// Парсим строку в виде JSON-словаря и сохраняем в файл "../answers.json"
	putDicToJsonFile("../answers.json", nlohmann::json::parse(answersStr));
}