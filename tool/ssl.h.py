#!/usr/bin/env python3

import subprocess
import tempfile
import os
import http.client
import urllib.parse
import hashlib

CA_CERT_URL = "https://curl.se/ca/cacert-2022-07-19.pem"
CA_CERT_SHA256 = "6ed95025fba2aef0ce7b647607225745624497f876d74ef6ec22b26e73e9de77"

HEADER_FORMAT = "#include <bearssl.h>\n{}"

url = urllib.parse.urlparse(url = CA_CERT_URL)

print("Fetching data from %s" % CA_CERT_URL)

connection = http.client.HTTPSConnection(host = url.netloc, port = url.port)

connection.request(method = "GET", url = url.path)

response = connection.getresponse()
content = response.read()

connection.close()

assert hashlib.sha256(content).hexdigest() == CA_CERT_SHA256, "SHA 256 hash didn't match!"

with tempfile.TemporaryDirectory() as directory:
	filename = os.path.join(directory, "cacert.pem")
	
	with open(file = filename, mode = "wb") as file:
		file.write(content)
	
	header = subprocess.check_output(["brssl", "ta", "/storage/emulated/0/HttpCanary/certs/HttpCanary.pem", filename])
	content = HEADER_FORMAT.format(header.decode())

with open(file = "../src/ssl.h", mode = "w") as file:
	file.write(content)