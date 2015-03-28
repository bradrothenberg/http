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




int _tmain(int argc, _TCHAR* argv[])
{

	http_client client(L"https://private-anon-575155ce3-sparkauthentication.apiary-proxy.com/api/v1/oauth/accesstoken");
	// Manually build up an HTTP request with header and request URI.
	http_request request(methods::POST);
	request.headers().add(L"Content-Type", L"application/x-www-form-urlencoded");
	request.headers().add(L"Authorization", L"Basic [WnByQWE3UDA1V01MWmFMNG53WFd0dU1FNlZldkIyN3Y6SUFaWFN6MWtIRnhWUzVWUA==]");

	request.set_body("grant_type=client_credentials");

	
	client.request(request).then([](http_response response)
	{

		auto r = response.extract_json();

		auto bodyStream = response.body();


	
		auto fileStream = std::make_shared<ostream>();

		fstream::open_ostream(U("results.txt")).then([=](ostream outFile) {
			*fileStream = outFile;
			response.body().read_to_end(fileStream->streambuf());
		});

		

		// Print the status code.
		std::wostringstream ss;
		ss << L"Server returned returned status code " << response.status_code() << L"." << std::endl;
		std::wcout << ss.str();
	});




	std::wcout << L"Calling HTTPStreamingAsync..." << std::endl;
	HTTPStreamingAsync().wait();

	std::wcout << L"Calling HTTPRequestCustomHeadersAsync..." << std::endl;
	HTTPRequestCustomHeadersAsync().wait();

	std::wcout << L"Calling UploadFileToHttpServerAsync..." << std::endl;
	UploadFileToHttpServerAsync().wait();





	return 0;
}

