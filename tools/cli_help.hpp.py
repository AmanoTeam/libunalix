import argparse
import io

parser = argparse.ArgumentParser(
    prog = "unalix",
    description = None,
    allow_abbrev = False,
    epilog = "Note, options that take an argument require a equal sign. E.g. -u=URL"
)

parser.add_argument(
    "-v",
    "--version",
    action = "store_true",
    help = "show version number and exit"
)

parser.add_argument(
    "-u",
    "--url",
    required = True,
    help = "Specify a URL to remove tracking fields from"
)

parser.add_argument(
    "-s",
    "--unshort",
    action = "store_true",
    help = "Unshort URLs (HTTP requests will be made)"
)

parser.add_argument(
    "-l",
    "--launch-with-browser",
    action = "store_true",
    help = "Open URLs with user's default browser"
)

parser.add_argument(
    "--strict-errors",
    action = "store_true",
    help = "Fail and exit on the first detected error"
)

url_filtering = parser.add_argument_group(
    title = "URL filtering"
)

url_filtering.add_argument(
    "--skip-blocked",
    action = "store_true",
    help = "Ignore patterns when <completeProvider> is true"
)

url_filtering.add_argument(
    "--ignore-rules",
    action = "store_true",
    help = "Ignore <rules> patterns"
)

url_filtering.add_argument(
    "--ignore-raw",
    action = "store_true",
    help = "Ignore <rawRules> patterns"
)

url_filtering.add_argument(
    "--ignore-referral",
    action = "store_true",
    help = "Ignore <referralMarketing> patterns"
)

url_filtering.add_argument(
    "--ignore-exceptions",
    action = "store_true",
    help = "Ignore <exceptions> patterns"
)

url_filtering.add_argument(
    "--ignore-redirections",
    action = "store_true",
    help = "Ignore <redirections> patterns"
)

http_options = parser.add_argument_group(
    title = "HTTP options"
)

http_options.add_argument(
    "--timeout",
    type = int,
    help = "Maximum time in seconds that you allow unalix's socket operations to take"
)

http_options.add_argument(
    "--max-redirs",
    type = int,
    help = "Set maximum number of redirections to follow"
)

http_options.add_argument(
    "--user-agent",
    type = str,
    help = "Specify the User-Agent string to send to the HTTP server"
)

dns_options = parser.add_argument_group(
    title = "DNS options"
)

dns_options.add_argument(
    "--dns-server",
    type = str,
    help = "Set the DNS server to be used instead of the system default"
)

proxy_options = parser.add_argument_group(
    title = "Proxy options"
)

proxy_options.add_argument(
    "--proxy",
    type = str,
    help = "Use the specified proxy"
)

proxy_options.add_argument(
    "--proxy-user",
    type = str,
    help = "Specify the user name to use for proxy authentication"
)

proxy_options.add_argument(
    "--proxy-pass",
    type = str,
    help = "Specify the password to use for proxy authentication"
)

with io.StringIO() as file:
	parser.print_help(file = file)
	file.seek(0)
	description = file.read()

with open(file = "../src/cli_help.hpp", mode = "w") as file:
	file.write("/*\nAutomatically generated header. Use '../tools/cli_help.hpp.py' to modify/regenerate.\n*/\n\n")
	file.write("static const char cli_help[] = ")
	
	for line in description.strip().split(sep = "\n"):
		file.write('\n\t"%s\\n"' % line)
	file.write(";\n")
