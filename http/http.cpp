// http.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>  

#include <cpprest/json.h> 
#include <iostream>
#include <sstream>



using namespace web::http;
using namespace web::http::client;
using namespace web::json;

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace concurrency::streams;       // Asynchronous streams

//using namespace std;

// Creates an HTTP request and prints the length of the response stream.
pplx::task<void> HTTPStreamingAsync()
{
	http_client client(L"http://www.fourthcoffee.com");

	// Make the request and asynchronously process the response. 
	return client.request(methods::GET).then([](http_response response)
	{
		// Print the status code.
		std::wostringstream ss;
		ss << L"Server returned returned status code " << response.status_code() << L'.' << std::endl;
		std::wcout << ss.str();

		// TODO: Perform actions here reading from the response stream.
		auto bodyStream = response.body();

		// In this example, we print the length of the response to the console.
		ss.str(std::wstring());
		ss << L"Content length is " << response.headers().content_length() << L" bytes." << std::endl;
		std::wcout << ss.str();
	});

	/* Sample output:
	Server returned returned status code 200.
	Content length is 63803 bytes.
	*/
}

// Builds an HTTP request that uses custom header values.
pplx::task<void> HTTPRequestCustomHeadersAsync()
{
	http_client client(L"https://private-anon-575155ce3-sparkauthentication.apiary-proxy.com/api/v1/oauth/accesstoken");
	// Manually build up an HTTP request with header and request URI.
	http_request request(methods::POST);
	request.headers().add(L"Content-Type", L"application/x-www-form-urlencoded");
	request.headers().add(L"Authorization", L"Basic [WnByQWE3UDA1V01MWmFMNG53WFd0dU1FNlZldkIyN3Y6SUFaWFN6MWtIRnhWUzVWUA==]");

	request.set_body("grant_type=client_credentials");

	return client.request(request).then([](http_response response)
	{

		auto r = response.extract_json();

		
		auto bodyStream = response.body();
		// Print the status code.
		std::wostringstream ss;
		ss << L"Server returned returned status code " << response.status_code() << L"." << std::endl;
		std::wcout << ss.str();
	});

	/* Sample output:
	Server returned returned status code 200.
	*/
}

// Upload a file to an HTTP server.
pplx::task<void> UploadFileToHttpServerAsync()
{
	using concurrency::streams::file_stream;
	using concurrency::streams::basic_istream;

	// To run this example, you must have a file named myfile.txt in the current folder. 
	// Alternatively, you can use the following code to create a stream from a text string. 
	// std::string s("abcdefg");
	// auto ss = concurrency::streams::stringstream::open_istream(s); 

	// Open stream to file. 
	return file_stream<unsigned char>::open_istream(L"myfile.txt").then([](pplx::task<basic_istream<unsigned char>> previousTask)
	{
		try
		{
			auto fileStream = previousTask.get();

			// Make HTTP request with the file stream as the body.
			http_client client(L"http://www.fourthcoffee.com");
			return client.request(methods::PUT, L"myfile", fileStream).then([fileStream](pplx::task<http_response> previousTask)
			{
				fileStream.close();

				std::wostringstream ss;
				try
				{
					auto response = previousTask.get();
					ss << L"Server returned returned status code " << response.status_code() << L"." << std::endl;
				}
				catch (const http_exception& e)
				{
					ss << e.what() << std::endl;
				}
				std::wcout << ss.str();
			});
		}
		catch (const std::system_error& e)
		{
			std::wostringstream ss;
			ss << e.what() << std::endl;
			std::wcout << ss.str();

			// Return an empty task. 
			return pplx::task_from_result();
		}
	});

	/* Sample output:
	The request must be resent
	*/
}


pplx::task<void> sparkGuestToken(json::value& val)
{
	// TODO: To successfully use this example, you must perform the request  
	// against a server that provides JSON data.  
	// This example fails because the returned Content-Type is text/html and not application/json.
	http_client client(L"https://private-anon-575155ce3-sparkauthentication.apiary-proxy.com/api/v1/oauth/accesstoken");
	// Manually build up an HTTP request with header and request URI.
	http_request request(methods::POST);
	request.headers().add(L"Content-Type", L"application/x-www-form-urlencoded");
	request.headers().add(L"Authorization", L"Basic [WnByQWE3UDA1V01MWmFMNG53WFd0dU1FNlZldkIyN3Y6SUFaWFN6MWtIRnhWUzVWUA==]");

	request.set_body("grant_type=client_credentials");


	return client.request(request).then([&val](http_response response) -> pplx::task<json::value>
	{
		if (response.status_code() == status_codes::OK)
		{
			return response.extract_json();
		}

		// Handle error cases, for now return empty json value... 
		return pplx::task_from_result(json::value());
	})
		.then([&val](pplx::task<json::value> previousTask)
	{
		try
		{
			const json::value& v = previousTask.get();
			val = v;
			std::string string;
		//auto arrayJ = 	v.as_array();


		std::cout <<"print"<< std::endl;
	
			// Perform actions here to process the JSON value...
		}
		catch (const http_exception& e)
		{
			// Print error.
			//wostringstream ss;
			//ss << e.what() << endl;
		//	wcout << ss.str();
		}
	});

	/* Output:
	Content-Type must be application/json to extract (is: text/html)
	*/
}


pplx::task<void> sparkAccessToken(json::value& val,const utility::string_t& accessCode)
{
	// TODO: To successfully use this example, you must perform the request  
	// against a server that provides JSON data.  
	// This example fails because the returned Content-Type is text/html and not application/json.
	http_client client(L"https://private-anon-575155ce3-sparkauthentication.apiary-proxy.com/api/v1/oauth/accesstoken");
	// Manually build up an HTTP request with header and request URI.
	http_request request(methods::POST);
	request.headers().add(L"Content-Type", L"application/x-www-form-urlencoded");
	request.headers().add(L"Authorization", L"Basic [WnByQWE3UDA1V01MWmFMNG53WFd0dU1FNlZldkIyN3Y6SUFaWFN6MWtIRnhWUzVWUA==]");

	request.set_body(L"grant_type=authorization_code&"+accessCode + L"&response_type=code&redirect_uri=http://localhost/dev/SPARK/Sample-App-Auth-Drive-JS/callback.html");


	return client.request(request).then([&val](http_response response) -> pplx::task<json::value>
	{
		if (response.status_code() == status_codes::OK)
		{
			return response.extract_json();
		}

		// Handle error cases, for now return empty json value... 
		return pplx::task_from_result(json::value());
	})
		.then([&val](pplx::task<json::value> previousTask)
	{
		try
		{
			const json::value& v = previousTask.get();
			val = v;
			std::string string;
			//auto arrayJ = 	v.as_array();


			std::cout << "print" << std::endl;

			// Perform actions here to process the JSON value...
		}
		catch (const http_exception& e)
		{
			// Print error.
			//wostringstream ss;
			//ss << e.what() << endl;
			//	wcout << ss.str();
		}
	});

	/* Output:
	Content-Type must be application/json to extract (is: text/html)
	*/
}

void IterateJSONValue(const json::value& objIn)
{
	// Create a JSON object.
	json::value obj;
	obj[L"key1"] = json::value::boolean(false);
	obj[L"key2"] = json::value::number(44);
	obj[L"key3"] = json::value::number(43.6);
	obj[L"key4"] = json::value::string(U("str"));


// 	// Loop over each element in the object. 
// 	for (auto iter = obj.cbegin(); iter != obj.cend(); ++iter)
// 	{
// 		// Make sure to get the value as const reference otherwise you will end up copying 
// 		// the whole JSON value recursively which can be expensive if it is a nested object. 
// 		const json::value &str = iter->first;
// 		const json::value &v = iter->second;
// 
// 		// Perform actions here to process each string and value in the JSON object...
// 		std::wcout << L"String: " << str.as_string() << L", Value: " << v.to_string() << endl;
// 	}

	/* Output:
	String: key1, Value: false
	String: key2, Value: 44
	String: key3, Value: 43.6
	String: key4, Value: str
	*/
}

int _tmain(int argc, _TCHAR* argv[])
{

	web::json::value get;
	sparkAccessToken(get, L"hello").wait();

	std::wcout << L"Calling RequestJSONValueAsync..." << std::endl;
	sparkGuestToken(get).wait();

	std::wcout << L"Guest Json Received:" << std::endl;

	for (auto iter = get.as_object().cbegin(); iter != get.as_object().cend(); ++iter)
	{
		// Make sure to get the value as const reference otherwise you will end up copying 
		// the whole JSON value recursively which can be expensive if it is a nested object. 
		auto& str = iter->first;
		const json::value &v = iter->second;

		auto print = v.as_string();
		// Perform actions here to process each string and value in the JSON object...
		std::wcout << L"String: " << str << L", Value: " << v.as_string() << std::endl;
	}


	auto aCode = get[L"access_token"].as_string();
	//std::string a =static_cast<std::string>( get[L"access_token"]);
	std::wcout << aCode.c_str() << std::endl;

	web::json::value get2;
	sparkAccessToken(get2, aCode.c_str()).wait();




	if(get2.size()>0){
		std::wcout << L"Access json Received:" << std::endl;
	for (auto iter = get2.as_object().cbegin(); iter != get2.as_object().cend(); ++iter)
	{
		// Make sure to get the value as const reference otherwise you will end up copying 
		// the whole JSON value recursively which can be expensive if it is a nested object. 

			auto& str = iter->first;
			const json::value &v = iter->second;

			auto print = v.as_string();
			// Perform actions here to process each string and value in the JSON object...
			std::wcout << L"String: " << str << L", Value: " << v.as_string() << std::endl;


		}
	}






	auto fileStream = std::make_shared<ostream>();

	return 0;
}

