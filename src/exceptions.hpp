#include <string>
#include <exception>

class UnalixException : public std::exception {
	private:
		std::string message;
		std::string url;
	
	protected:
		UnalixException() {}
		
		UnalixException(const std::string message) :
			message(message)
		{}
	
	public:
		
		const void set_message(const std::string value) {
			this -> message = value;
		}
		
		const std::string get_message() const {
			return this -> message;
		}
		
		const void set_url(const std::string value) {
			this -> url = value;
		}
		
		const std::string get_url() const {
			return this -> url;
		}
		
		const char* what() const throw () {
			return message.c_str();
		}
};

class GAIError : public UnalixException {
	
	public:
		GAIError(const std::string message) :
			UnalixException(message)
		{}
		
		GAIError() :
			UnalixException()
		{}
	
};

class SocketError : public UnalixException {
	
	public:
		SocketError(const std::string message) :
			UnalixException(message)
		{}
		
		SocketError() :
			UnalixException()
		{}
	
};

class ConnectError : public SocketError {
	
	public:
		ConnectError(const std::string message) :
			SocketError(message)
		{}
		
		ConnectError() :
			SocketError()
		{}
	
};

class SendError : public SocketError {
	
	public:
		SendError(const std::string message) :
			SocketError(message)
		{}
		
		SendError() :
			SocketError()
		{}
	
};

class RecvError : public SocketError {
	
	public:
		RecvError(const std::string message) :
			SocketError(message)
		{}
		
		RecvError() :
			SocketError()
		{}
	
};

class SSLError : public UnalixException {
	
	public:
		SSLError(const std::string message) :
			UnalixException(message)
		{}
		
		SSLError() :
			UnalixException()
		{}
	
};

class UnsupportedProtocolError : public UnalixException {
	
	public:
		UnsupportedProtocolError(const std::string message) :
			UnalixException(message)
		{}
		
		UnsupportedProtocolError() :
			UnalixException()
		{}
	
};

class RemoteProtocolError : public UnalixException {
	
	public:
		RemoteProtocolError(const std::string message) :
			UnalixException(message)
		{}
		
		RemoteProtocolError() :
			UnalixException()
		{}
	
};

class TooManyRedirectsError : public UnalixException {
	
	public:
		TooManyRedirectsError(const std::string message) :
			UnalixException(message)
		{}
		
		TooManyRedirectsError() :
			UnalixException()
		{}
	
};

class DNSError : public UnalixException {
	
	public:
		DNSError(const std::string message) :
			UnalixException(message)
		{}
		
		DNSError() :
			UnalixException()
		{}
	
};

class ValueError : public UnalixException {
	
	public:
		ValueError(const std::string message) :
			UnalixException(message)
		{}
		
		ValueError() :
			UnalixException()
		{}
	
};

class Socks5Error : public UnalixException {
	
	public:
		Socks5Error(const std::string message) :
			UnalixException(message)
		{}
		
		Socks5Error() :
			UnalixException()
		{}
	
};

class RulesetParserError : public UnalixException {
	
	public:
		RulesetParserError(const std::string message) :
			UnalixException(message)
		{}
		
		RulesetParserError() :
			UnalixException()
		{}
	
};

class SHA256Error : public UnalixException {
	
	public:
		SHA256Error(const std::string message) :
			UnalixException(message)
		{}
		
		SHA256Error() :
			UnalixException()
		{}
	
};

#pragma once