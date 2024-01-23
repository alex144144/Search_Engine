//
// Created by marshtv on 29.12.2023.
//

#ifndef SEARCH_ENGINE_CONVERTERJSON_H
#define SEARCH_ENGINE_CONVERTERJSON_H

#pragma once

#include <vector>
#include <string>

class ConverterJSON {
public:
	ConverterJSON() = default;
	~ConverterJSON() = default;

	/**
	 * Метод обновления файла конфигурации config.json
	 * @param _docs - Список содержимого документов
	*/
	void UpdateConfigJsonFile(const std::vector<std::string>& _docs);

	/**
	* Метод получения содержимого JSON-файлов
	* @return Возвращает список с содержимым файлов перечисленных
	* в config.json
	*/
	std::vector<std::string> GetTextDocuments();

	/**
	* Метод считывает поле max_responses из файла config.json для определения предельного
	* количества ответов на один запрос
	* @return Возвращает int значение
	*/
	int GetResponsesLimit();

	/**
	* Метод сохраняет запросы в файл requests.json
	*/
	void putRequests(const std::vector<std::string>& _requests);

	/**
	* Метод получения запросов из файла requests.json
	* @return возвращает список запросов из файла requests.json
	*/
	std::vector<std::string> GetRequests();

	/**
	* Положить в файл answers.json результаты поисковых запросов
	*/
	void putAnswers(const std::vector<std::vector<std::pair<size_t, float>>>& _answers);
};

#endif //SEARCH_ENGINE_CONVERTERJSON_H
